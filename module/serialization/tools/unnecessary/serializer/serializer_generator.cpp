#include <cxxopts.hpp>
#include <chrono>
#include <unordered_map>
#include <unnecessary/serializer/jobs/parse_translation_unit_job.h>
#include <unnecessary/serializer/jobs/generate_serializer_job.h>
#include <unnecessary/serializer/generation_plan.h>
#include <unnecessary/jobs/job_system_builder.h>
#include <unnecessary/source_analysis/parser.h>
#include <unnecessary/source_analysis/parsing.h>
#include <unnecessary/jobs/worker_chain.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/load_file_job.h>
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

int main(int argc, char** args) {
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
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }

    un::JobSystemBuilder<un::SimpleJobSystem> builder;
    builder.withRecorder();
    auto jobSystem = builder.build();

    un::GenerationPlan plan(relativeTo);
    {
        un::JobChain<un::SimpleJobSystem> chain(jobSystem.get());
        for (const std::string& file : files) {
            std::filesystem::path path = file;
            std::string debugFileName;
            debugFileName += path.filename().string();
            debugFileName += ".info.txt";
            std::filesystem::path debugFile = output / debugFileName;
            un::files::ensure_directory_exists(debugFile.parent_path());
            chain.immediately<un::ParseTranslationUnitJob>(
                path,
                relativeTo,
                debugFileName,
                includes,
                &plan
            );
        }
        LOG(INFO) << "Dispatching " << chain.getNumJobs() << " jobs...";

    }
    jobSystem->join();
    plan.bake();
    un::GenerationPlan::IncludeGraphType& includeGraph = plan.getIncludeGraph();
    gpp::save_to_dot(
        includeGraph.getInnerGraph(),
        output / "includes.dot"
    );
    {
        std::vector<std::shared_ptr<un::Buffer>> buffers;
        {
            un::JobChain<un::SimpleJobSystem> chain(jobSystem.get());
            std::unordered_map<u32, std::set<un::JobHandle>> include2DeclarationsJobs;
            includeGraph.each_rlo(
                [&](u32 index) {
                    const un::GenerationFile* pGenerationFile = includeGraph[index];
                    const un::CXXTranslationUnit& translationUnit = pGenerationFile->getUnit();
                    std::vector<std::shared_ptr<un::CXXDeclaration>> symbols = translationUnit.collectSymbols<un::CXXDeclaration>();
                    for (const auto& item : symbols) {
                        const std::shared_ptr<un::Buffer>& buffer = buffers.emplace_back(std::make_shared<un::Buffer>());
                        un::JobHandle handle;
                        auto outPath = output / (pGenerationFile->getPath().filename().string() + ".generated.h");
                        LOG(INFO) << item->getFullName() << " serializer will be written to " << outPath;
                        chain.immediately<un::GenerateSerializerJob>(
                            &handle,
                            buffer,
                            item,
                            &translationUnit
                        ).after<un::WriteFileJob>(
                            handle,
                            outPath,
                            static_cast<std::ios::openmode>(std::ios::out | std::ios::trunc),
                            buffer.get()
                        );
                        include2DeclarationsJobs[index].emplace(handle);
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
        }
        jobSystem->join();
    }
    jobSystem->complete();
}

void process(
    const std::vector<std::string>& parsingIncludes,
    const std::vector<std::string>& globalIncludes,
    const std::string& file,
    const std::filesystem::path& output,
    const std::string& relativeTo
) {
    std::string ownInclude = std::filesystem::relative(file, relativeTo).string();
    std::filesystem::path includesOutput = output / "include";
    std::filesystem::path filePath = file;
    un::parsing::ParsingOptions parseOptions(file, parsingIncludes);
    parseOptions.setDebugFile(output / "cppast.info.txt");
    auto translationUnit = un::parsing::parse(parseOptions);
    std::vector<std::string> allGeneratedFilenames;
    for (const auto& composite : translationUnit.collectSymbols<un::CXXComposite>()) {
        LOG(INFO) << "Checking " << composite->getFullName();
        if (!un::isSerializable(composite)) {
            LOG(INFO) << composite->getFullName() << " is not serializable, skipping.";
            continue;
        }
        un::GenerationInfo info;
        std::vector<std::string> additionalIncludes;
        for (const auto& item : composite->getFields()) {
            const un::CXXType& type = item.getType();
            if (type.getKind() != un::eComplex) {
                continue;
            }
            std::shared_ptr<un::CXXComposite> other;
            std::string typeName = type.getName();
            if (!translationUnit.findSymbol<un::CXXComposite>(typeName, other)) {
//                    LOG(WARN) << "Unable to find composite type " << typeName << " for field " << item.getName()
//                              << ". Will not add extra include.";
                continue;
            }
            if (!un::isSerializable(other)) {
//                    LOG(WARN) << "Type " << typeName << " for field " << item.getName()
//                              << " isn't serializable. Will not add extra include.";
                continue;
            }
            std::string name = other->getName();
            auto lastName = name.find_last_of(':');
            if (lastName > 0) {
                name = name.substr(lastName + 1);
            }
            std::string include = un::getGeneratedIncludeName(name);
            additionalIncludes.emplace_back(include);
        }
        for (const std::string& include : globalIncludes) {
            additionalIncludes.emplace_back(include);
        }
        std::string name = composite->getName();
        info.name = name;
        info.upper = un::upper(name);
        info.lower = un::lower(name);
        info.fullName = composite->getFullName();
        info.ownInclude = ownInclude;
        LOG(INFO) << composite->getFullName() << " is suitable for serialization, proceeding...";
        generateSerializerInclude(includesOutput, composite, translationUnit, info, additionalIncludes);
        allGeneratedFilenames.emplace_back(info.lower + ".serializer.generated.h");
    }
    std::stringstream ss;
    ss << "// All generated serializers are included here" << std::endl;
    ss << "#pragma once" << std::endl;
    for (const std::string& file : allGeneratedFilenames) {
        ss << "#include <" << file << ">" << std::endl;
    }
    un::files::safe_write_file(
        includesOutput / (filePath.filename().replace_extension().string() + ".serializers.h"), ss
    );
}
