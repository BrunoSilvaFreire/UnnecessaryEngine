#include <cxxopts.hpp>
#include <chrono>
#include <unordered_map>

#include <unnecessary/serializer/jobs/generate_includes_job.h>
#include <unnecessary/serializer/jobs/generate_serializer_job.h>
#include <unnecessary/serializer/generation_plan.h>
#include <unnecessary/jobs/job_system_builder.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/file_jobs.h>
#include <unnecessary/jobs/misc/join_buffers_job.h>
#include <unnecessary/jobs/misc/job_visualization.h>
#include <unnecessary/misc/benchmark.h>
#include <unnecessary/misc/pretty_print.h>
#include <grapphs/dot.h>
#include "unnecessary/source_analysis/parse_plan.h"

static const char* const kFileArgName = "file";
static const char* const kRelativeToName = "relative_to";
static const char* const kGlobalIncludeName = "global_include";
static const char* const kNumWorkersName = "num_workers";

static const char* const kIncludeArgName = "include";
static const char* const kOutputDir = "output";

bool isSerializable(const std::shared_ptr<un::CXXComposite>& composite);

void process(
    const std::vector<std::string>& parsingIncludes,
    const std::vector<std::string>& globalIncludes,
    const std::string& file,
    const std::filesystem::path& output,
    const std::string& relativeTo
);

void writeJobToDot(std::unique_ptr<un::SimpleJobSystem>& jobSystem, const std::filesystem::path& jobsDot);

int main(int argc, char** args) {
    un::Chronometer<> mainChronometer;
    LOG(INFO) << "Generating serialization.";
    cxxopts::Options options("unnecessary_serialization_generator");
    options.add_options("required")
               (
                   kFileArgName, "File to parse and generate",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   kIncludeArgName, "Adds an include directory to parsing",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   kNumWorkersName, "Number of thread workers to use",
                   cxxopts::value<std::size_t>()
               )
               (
                   kOutputDir, "Directory where to write the generated files",
                   cxxopts::value<std::string>()
               );
    options.add_options("optional")
               (
                   kGlobalIncludeName, "Specifies an include that will be added to all generated files",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   kRelativeToName, "Directory where to write the generated files",
                   cxxopts::value<std::string>()
               );

    std::filesystem::path output;
    std::filesystem::path relativeTo;
    std::vector<std::string> files;
    std::vector<std::string> includes;
    un::JobSystemBuilder<un::SimpleJobSystem> builder;
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        includes = result[kIncludeArgName].as<std::vector<std::string>>();
        std::vector<std::string> globalIncludes;
        if (result.count(kGlobalIncludeName) > 0) {
            globalIncludes = result[kGlobalIncludeName].as<std::vector<std::string>>();
        }
        std::size_t numSpecifiedFiles = result.count(kFileArgName);
        output = result[kOutputDir].as<std::string>();
        if (result.count(kRelativeToName) > 0) {
            relativeTo = result[kRelativeToName].as<std::string>();
        } else {
            relativeTo = std::filesystem::current_path();
        }
        std::filesystem::path filePath = "temp";
        files = result[kFileArgName].as<std::vector<std::string>>();
        std::for_each(
            files.begin(), files.end(),
            [](std::string& path) {
                path = un::to_string(std::filesystem::absolute(path));
            }
        );
        if (result.count(kNumWorkersName) > 0) {
            std::size_t numWorkers = result[kNumWorkersName].as<std::size_t>();
            builder.setNumWorkers<un::JobWorker>(numWorkers);
        }

    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "unnecessary_serializer_generator: " << x.what() << '\n';
        std::cerr << "usage: unnecessary_serializer_generator [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
    LOG(INFO) << un::prettify("files", files);
    LOG(INFO) << un::prettify("includes", includes);
    builder.withRecorder();
    auto jobSystem = builder.build();

    auto index = std::make_shared<cppast::cpp_entity_index>();

    std::vector<un::GenerationFile> genFiles;
    {
        un::ParsePlan parsePlan(index);
        std::for_each(
            includes.begin(), includes.end(),
            [&](const auto& inc) {
                parsePlan.addInclude(inc);
            }
        );
        std::for_each(
            files.begin(), files.end(),
            [&](const auto& item) {
                parsePlan.addFile(item);
            }
        );
        std::vector<un::CXXTranslationUnit> units = parsePlan.parse(jobSystem.get());
        for (auto& item : units) {
            std::filesystem::path location = item.getLocation();
            std::string fileName = location.filename().replace_extension().string() + ".serializer.generated.h";
            genFiles.emplace_back(
                location,
                output / fileName,
                std::move(item)
            );
        }
    }

    un::GenerationPlan plan(index, output);
    un::Chronometer<> chronometer(false);

    {
        un::JobChain<un::SimpleJobSystem> chain(jobSystem.get());
        std::unordered_map<u32, std::set<un::JobHandle>> include2DeclarationsJobs;
        for (const auto& pGenerationFile : genFiles) {
            std::filesystem::path filePath = pGenerationFile.getSource();
            const un::CXXTranslationUnit& pUnit = pGenerationFile.getUnit();
            std::vector<std::shared_ptr<un::CXXDeclaration>> symbols = pUnit.collectSymbols<un::CXXDeclaration>();
            std::vector<std::shared_ptr<un::Buffer>> toJoin;
            std::set<un::JobHandle> dependencies;
            std::filesystem::path outPath = pGenerationFile.getOutput();
            std::string fileName = outPath.filename();
            LOG(INFO) << filePath.filename() << " serializer will be written to "
                      << un::prettify(outPath);

            std::shared_ptr<un::Buffer> includeBuf = std::make_shared<un::Buffer>();
            un::JobHandle includeHandle;
            u32 index = 0;
            chain.immediately<un::GenerateIncludesJobs>(
                &includeHandle,
                index,
                &pUnit,
                &plan,
                includeBuf
            );
            chain.setName(includeHandle, "Generate includes for translation unit");
            toJoin.emplace_back(includeBuf);
            dependencies.emplace(includeHandle);

            for (const auto& item : symbols) {
                std::shared_ptr<un::Buffer> buffer = std::make_shared<un::Buffer>();
                toJoin.emplace_back(buffer);
                un::JobHandle handle;
                chain.immediately<un::GenerateSerializerJob>(
                    &handle,
                    buffer,
                    item,
                    &pUnit
                );
                chain.setName(
                    handle,
                    std::string("Generate ").append(item->getFullName()).append(" serializer")
                );
                dependencies.emplace(handle);
                include2DeclarationsJobs[index].emplace(handle);
            }
            un::JobHandle joinHandle;
            un::JobHandle writeHandle;
            auto finalBuffer = std::make_shared<un::Buffer>();
            chain.immediately<un::JoinBuffersJob>(
                &joinHandle,
                toJoin,
                finalBuffer
            ).after<un::WriteFileJob>(
                joinHandle,
                &writeHandle,
                outPath,
                static_cast<std::ios::openmode>(std::ios::out | std::ios::trunc),
                finalBuffer
            );
            chain.setName(
                joinHandle,
                std::string("Join buffers for ").append(fileName)
            );
            chain.setName(
                writeHandle,
                std::string("Write to file ").append(fileName)
            );
            for (const auto& generationHandle : dependencies) {
                chain.after<un::JoinBuffersJob>(generationHandle, joinHandle);
            }
        }
        writeJobToDot(jobSystem, output / "generate_jobs.dot");
        LOG(INFO) << "Dispatching " << chain.getNumJobs() << " generation jobs...";
    }
    jobSystem->complete();
    auto ptr = jobSystem->findExtension<un::JobSystemRecorder<un::SimpleJobSystem>>();
    ptr->saveToFile(output / "recording.csv");
    LOG(INFO) << "Sources generated in " << mainChronometer.stop().count() << " ms";
}

void writeJobToDot(std::unique_ptr<un::SimpleJobSystem>& jobSystem, const std::filesystem::path& jobsDot) {
    un::create_job_system_graph_writer(*jobSystem).save_to_dot(jobSystem->getJobGraph().getInnerGraph(), jobsDot);
}
