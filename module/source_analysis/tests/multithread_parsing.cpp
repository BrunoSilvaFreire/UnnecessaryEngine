#include <unnecessary/jobs/job_system_builder.h>
#include <unnecessary/source_analysis/parse_plan.h>
#include <gtest/gtest.h>

TEST(source_mt, parse_sample) {
    un::job_system_builder<un::simple_job_system> builder;
    builder.with_logger();
    builder.with_recorder();
    auto jobSystem = builder.build();
    un::parse_arguments args;
    args.addInclude("C:/Users/bruno/CLionProjects/UnnecessaryEngine/module/core/include");
    un::parse_plan plan(args, jobSystem.get());
    cppast::cpp_entity_index index;
    std::filesystem::path p = std::filesystem::absolute("sample.h");
    const auto& file = plan(p, index);
    LOG(INFO) << "Parsed " << file->name();
}
