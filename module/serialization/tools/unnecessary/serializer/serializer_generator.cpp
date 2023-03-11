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

static const char* const k_file_arg_name = "file";
static const char* const k_relative_to_name = "relative_to";
static const char* const k_global_include_name = "global_include";
static const char* const k_num_workers_name = "num_workers";
static const char* const k_job_system_logger = "job_system_logger";
static const char* const k_include_arg_name = "include";
static const char* const k_output_dir = "output";

bool is_serializable(const std::shared_ptr<un::cxx_composite>& composite);

void process(
    const std::vector<std::string>& parsingIncludes,
    const std::vector<std::string>& globalIncludes,
    const std::string& file,
    const std::filesystem::path& output,
    const std::string& relativeTo
);

void
write_job_to_dot(
    std::unique_ptr<un::simple_job_system>& jobSystem,
    const std::filesystem::path& jobsDot
);

int main(int argc, char** args) {
    un::chronometer<> mainChronometer;
    LOG(INFO) << "Generating serialization.";
    cxxopts::Options options("unnecessary_serialization_generator");
    options.add_options("required")
               (
                   k_file_arg_name, "File to parse and generate",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   k_include_arg_name, "Adds an include directory to parsing",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   k_num_workers_name, "Number of thread workers to use",
                   cxxopts::value<std::size_t>()
               )
               (
                   k_output_dir, "Directory where to write the generated files",
                   cxxopts::value<std::string>()
               );
    options.add_options("optional")
               (
                   k_global_include_name,
                   "Specifies an include that will be added to all generated files",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   k_relative_to_name, "Directory where to write the generated files",
                   cxxopts::value<std::string>()
               )
               (k_job_system_logger, "Whether to log job system status", cxxopts::value<bool>());

    std::filesystem::path output;
    std::filesystem::path relativeTo;
    std::vector<std::string> files;
    std::vector<std::string> includes;
    un::job_system_builder<un::simple_job_system> builder;
    bool logJobSystem = false;
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        includes = result[k_include_arg_name].as<std::vector<std::string>>();
        std::vector<std::string> globalIncludes;
        if (result.count(k_global_include_name) > 0) {
            globalIncludes = result[k_global_include_name].as<std::vector<std::string>>();
        }
        std::size_t numSpecifiedFiles = result.count(k_file_arg_name);
        output = result[k_output_dir].as<std::string>();
        if (result.count(k_relative_to_name) > 0) {
            relativeTo = result[k_relative_to_name].as<std::string>();
        }
        else {
            relativeTo = std::filesystem::current_path();
        }
        std::filesystem::path filePath = "temp";
        files = result[k_file_arg_name].as<std::vector<std::string>>();
        std::for_each(
            files.begin(), files.end(),
            [](std::string& path) {
                path = un::to_string(std::filesystem::absolute(path));
            }
        );

        if (result.count(k_num_workers_name) > 0) {
            std::size_t numWorkers = result[k_num_workers_name].as<std::size_t>();
            builder.set_num_workers<un::job_worker>(numWorkers);
        }

        if (result.count(k_job_system_logger) > 0) {
            logJobSystem = true;
        }
    }
    catch (const cxxopts::OptionParseException& x) {
        std::cerr << "unnecessary_serializer_generator: " << x.what() << '\n';
        std::cerr << "usage: unnecessary_serializer_generator [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }

    {
        auto msg = un::message();
        msg.text() << "Serialization info:";
        auto& root = msg.map();

        auto& filesMsg = root.tree("files");
        for (const auto& file : files) {
            filesMsg.text() << file;
        }
        auto& includesMsg = root.tree("includes");
        for (const auto& file : includes) {
            includesMsg.text() << file;
        }
    }
    if (logJobSystem) {
        builder.with_logger();
    }
    auto jobSystem = builder.build();

    auto index = std::make_shared<cppast::cpp_entity_index>();

    std::vector<un::generation_file> genFiles;
    {
        un::parse_plan parsePlan(index);
        std::for_each(
            includes.begin(), includes.end(),
            [&](const auto& inc) {
                parsePlan.add_include(inc);
            }
        );
        std::for_each(
            files.begin(), files.end(),
            [&](const auto& item) {
                parsePlan.add_file(item);
            }
        );
        std::vector<un::cxx_translation_unit> units = parsePlan.parse(jobSystem.get());
        for (auto& item : units) {
            std::filesystem::path location = item.get_location();
            std::string fileName =
                location.filename().replace_extension().string() + ".serializer.generated.h";
            genFiles.emplace_back(
                location,
                output / fileName,
                std::move(item)
            );
        }
    }

    un::generation_plan plan(index, output);
    un::chronometer<> chronometer(false);

    {
        un::job_chain<un::simple_job_system> chain(jobSystem.get());
        std::unordered_map<u32, std::set<un::job_handle>> include2DeclarationsJobs;
        for (const auto& pGenerationFile : genFiles) {
            std::filesystem::path filePath = pGenerationFile.get_source();
            const un::cxx_translation_unit& pUnit = pGenerationFile.get_unit();
            std::vector<std::shared_ptr<un::cxx_declaration>> symbols = pUnit.collect_symbols<un::cxx_declaration>();
            std::vector<std::shared_ptr<un::byte_buffer>> toJoin;
            std::set<un::job_handle> dependencies;
            std::filesystem::path outPath = pGenerationFile.get_output();
            std::string fileName = outPath.filename().string();
            LOG(INFO) << filePath.filename() << " serializer will be written to "
                      << un::uri(outPath);

            auto includeBuf = std::make_shared<un::byte_buffer>();
            un::job_handle includeHandle;
            u32 index = 0;
            chain.immediately<un::generate_includes_jobs>(
                &includeHandle,
                index,
                &pUnit,
                &plan,
                includeBuf
            );
            std::stringstream name;
            name << "Generate includes for translation unit ("
                 << un::uri(pGenerationFile.get_source()) << ")";
            chain.set_name(includeHandle, name.str());
            toJoin.emplace_back(includeBuf);
            dependencies.emplace(includeHandle);

            for (const auto& item : symbols) {
                auto buffer = std::make_shared<un::byte_buffer>();
                toJoin.emplace_back(buffer);
                un::job_handle handle;
                chain.immediately<un::generate_serializer_job>(
                    &handle,
                    buffer,
                    item,
                    &pUnit
                );
                chain.set_name(
                    handle,
                    std::string("Generate ").append(item->get_full_name()).append(" serializer")
                );
                dependencies.emplace(handle);
                include2DeclarationsJobs[index].emplace(handle);
            }
            un::job_handle joinHandle;
            un::job_handle writeHandle;
            auto finalBuffer = std::make_shared<un::byte_buffer>();
            chain.immediately<un::join_buffers_job>(
                &joinHandle,
                toJoin,
                finalBuffer
            ).after<un::write_file_job>(
                joinHandle,
                &writeHandle,
                outPath,
                std::ios::out | std::ios::trunc,
                finalBuffer
            );
            chain.set_name(
                joinHandle,
                std::string("Join buffers for ").append(fileName)
            );
            chain.set_name(
                writeHandle,
                std::string("Write to file ").append(fileName)
            );
            for (const auto& generationHandle : dependencies) {
                chain.after<un::join_buffers_job>(generationHandle, joinHandle);
            }
        }
        write_job_to_dot(jobSystem, output / "generate_jobs.dot");
        LOG(INFO) << "Dispatching " << chain.get_num_jobs() << " generation jobs...";
    }
    jobSystem->complete();
    //    auto ptr = jobSystem->findExtension<un::JobSystemRecorder<un::simple_job_system>>();
    //    ptr->saveToFile(output / "recording.csv");
    LOG(INFO) << "Sources generated in " << mainChronometer.stop().count() << " ms";
}

void
write_job_to_dot(
    std::unique_ptr<un::simple_job_system>& jobSystem,
    const std::filesystem::path& jobsDot
) {
    create_job_system_graph_writer(*jobSystem).save_to_dot(
        jobSystem->get_job_graph()
                 .get_inner_graph(), jobsDot
    );
}
