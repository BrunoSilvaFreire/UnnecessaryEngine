#include <unnecessary/jobs/job_system_builder.h>
#include <unnecessary/source_analysis/parse_plan.h>
#include <gtest/gtest.h>


TEST(source_mt, parse_sample) {
    un::JobSystemBuilder<un::SimpleJobSystem> builder;
    builder.withLogger();
    builder.withRecorder();
    auto jobSystem = builder.build();
    un::ParseArguments args;
    args.addInclude("C:/Users/bruno/CLionProjects/UnnecessaryEngine/module/core/include");
    un::ParsePlan plan(args, jobSystem.get());
    cppast::cpp_entity_index index;
    std::filesystem::path p = std::filesystem::absolute("sample.h");
    const auto& file = plan(p, index);
    LOG(INFO) << "Parsed " << file->name();
}