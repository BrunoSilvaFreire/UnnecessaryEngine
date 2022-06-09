#include <cxxopts.hpp>
#include <chrono>
#include <unordered_map>

#include <unnecessary/source_analysis/parser.h>
#include <unnecessary/source_analysis/parsing.h>
#include <unnecessary/serializer/jobs/parse_translation_unit_job.h>
#include <unnecessary/serializer/jobs/generate_includes_job.h>
#include <unnecessary/serializer/jobs/generate_serializer_job.h>
#include <unnecessary/serializer/generation_plan.h>
#include <unnecessary/jobs/job_system_builder.h>
#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/file_jobs.h>
#include <unnecessary/jobs/misc/join_buffers_job.h>
#include <unnecessary/jobs/misc/job_visualization.h>
#include <unnecessary/misc/benchmark.h>
#include <grapphs/dot.h>
#include "generation.h"

static const char* const kFileArgName = "file";
static const char* const kRelativeToName = "relative_to";
static const char* const kGlobalIncludeName = "global_include";

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
    un::Chronometer<> chronometer;
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
    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "unnecessary_serializer_generator: " << x.what() << '\n';
        std::cerr << "usage: unnecessary_serializer_generator [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }

    un::JobSystemBuilder<un::SimpleJobSystem> builder;
    builder.withRecorder();
    auto jobSystem = builder.build();
    un::GenerationPlan plan(relativeTo, output);
    {
        un::JobChain<un::SimpleJobSystem> chain(jobSystem.get());
        for (const std::string& file : files) {
            std::filesystem::path path = file;
            std::string debugFileName;
            debugFileName += path.filename().string();
            debugFileName += ".info.txt";
            std::filesystem::path debugFile = output / debugFileName;
            un::files::ensure_directory_exists(debugFile.parent_path());
            un::JobHandle jobHandle;
            chain.immediately<un::ParseTranslationUnitJob>(
                &jobHandle,
                plan.getIndex(),
                path,
                relativeTo,
                debugFileName,
                includes,
                &plan
            );
            chain.setName(
                jobHandle,
                std::string("Parse ").append(path.string())
            );
        }
        writeJobToDot(jobSystem, output / "parse_jobs.dot");
        LOG(INFO) << "Dispatching " << chain.getNumJobs() << " parsing jobs...";

    }
    jobSystem->join();
    plan.bake();
    un::GenerationPlan::IncludeGraphType& includeGraph = plan.getIncludeGraph();
    gpp::save_to_dot(includeGraph.getInnerGraph(), output / "includes.dot");
    {
        {
            un::JobChain<un::SimpleJobSystem> chain(jobSystem.get());
            std::unordered_map<u32, std::set<un::JobHandle>> include2DeclarationsJobs;
            includeGraph.each_rlo(
                [&](u32 index) {
                    const un::GenerationFile* pGenerationFile = includeGraph[index];
                    const un::CXXTranslationUnit& translationUnit = pGenerationFile->getUnit();
                    std::vector<std::shared_ptr<un::CXXDeclaration>> symbols = translationUnit.collectSymbols<un::CXXDeclaration>();
                    std::vector<std::shared_ptr<un::Buffer>> toJoin;
                    std::set<un::JobHandle> dependencies;
                    std::string fileName = pGenerationFile->getPath().filename().string() + ".generated.h";
                    auto outPath = pGenerationFile->getOutput();
                    LOG(INFO) << pGenerationFile->getPath().filename() << " serializer will be written to " << outPath;

                    std::shared_ptr<un::Buffer> includeBuf = std::make_shared<un::Buffer>();
                    un::JobHandle includeHandle;
                    const un::CXXTranslationUnit* pUnit = &translationUnit;
                    chain.immediately<un::GenerateIncludesJobs>(
                        &includeHandle,
                        index,
                        pUnit,
                        &plan,
                        includeBuf
                    );
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
                            pUnit
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
                },
                [&](u32 from, u32 to) {
                    /*
                    const auto& fromIt = include2DeclarationsJobs.find(from);
                    if (fromIt == include2DeclarationsJobs.end()) {
                        return;
                    }
                    const auto& toIt = include2DeclarationsJobs.find(to);
                    if (toIt == include2DeclarationsJobs.end()) {
                        return;
                    }
                    for (const auto& fromJob : fromIt->second) {
                        for (const auto& toJob : toIt->second) {
                            chain.after<un::JobWorker>(fromJob, toJob);
                        }
                    }
                    */
                }
            );
            writeJobToDot(jobSystem, output / "generate_jobs.dot");
            LOG(INFO) << "Dispatching " << chain.getNumJobs() << " generation jobs...";
        }
        jobSystem->join();
    }
    jobSystem->complete();
    auto ptr = jobSystem->findExtension<un::JobSystemRecorder<un::SimpleJobSystem>>();
    ptr->saveToFile(output / "recording.csv");
    LOG(INFO) << "Sources generated in " << chronometer.stop();
}

void writeJobToDot(std::unique_ptr<un::SimpleJobSystem>& jobSystem, const std::filesystem::path& jobsDot) {
    un::create_job_system_graph_writer(*jobSystem).save_to_dot(jobSystem->getJobGraph().getInnerGraph(), jobsDot);
}
