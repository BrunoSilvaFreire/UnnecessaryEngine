#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/sdf/generator/jobs/process_pixel_job.h>
#include <cxxopts.hpp>
#include <png++/image.hpp>

static const char* const kInputName = "input";
static const char* const kThreadsName = "threads";
static const char* const kMinName = "min";
static const char* const kMaxName = "max";

int main(int argc, char** argv) {
    cxxopts::Options options(
        "unnecessary_sdf_generator",
        "Signed Distance Field generator."
    );
    int size = 64;
    int nThreads = std::thread::hardware_concurrency();
    options.add_options()
               (kInputName, "PNG input", cxxopts::value<std::string>())
               ("output", "PNG output", cxxopts::value<std::string>())
               ("size", "Generate CPP", cxxopts::value<int>(size))
               (kMinName, "Minimum SDF distance", cxxopts::value<float>())
               (kMaxName, "Maximum SDF distance", cxxopts::value<float>())
               (kThreadsName, "Number of threads", cxxopts::value<int>(nThreads));
    options.parse_positional(kInputName);
    const auto& result = options.parse(argc, argv);
    size_t a = result.count(kInputName);
    if (a > 0) {
        std::string shaderFile = result[kInputName].as<std::string>();
        png::image<png::rgba_pixel> img(shaderFile);
        nThreads = result[kThreadsName].as<int>();
        un::SimpleJobSystem jobSystem(nThreads, true);
        png::image<png::gray_pixel> sdf(size, size);
        float min = result[kMinName].as<float>();
        float max = result[kMaxName].as<float>();
        un::ProcessPixelJob job(
            &img,
            &sdf,
            min,
            max
        );
        {
            un::JobChain<un::SimpleJobSystem> chain(&jobSystem);

            un::ProcessPixelJob::parallelize(
                &job,
                chain,
                size * size,
                64
            );

        }
        jobSystem.finish(true);
        std::string outputFile = result["output"].as<std::string>();
        sdf.write(outputFile);
    } else {
        options.help();
        return 1;
    }
    return 0;
}