#include <cxxopts.hpp>
#include <unnecessary/sdf/generator/sdf_generator.h>

static const char* const kInputName = "input";
static const char* const kThreadsName = "threads";
static const char* const kMinName = "min";
static const char* const kMaxName = "max";

#ifndef UN_TESTING
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
        std::string imageFile = result[kInputName].as<std::string>();
        std::string outputFile = result["output"].as<std::string>();
        float min = result[kMinName].as<float>();
        float max = result[kMaxName].as<float>();
        nThreads = result[kThreadsName].as<int>();

        un::sdf::process_sdf(size, nThreads, imageFile, outputFile, min, max);
    } else {
        options.help();
        return 1;
    }
    return 0;
}
#endif
void un::sdf::process_sdf(
    int size,
    int nThreads,
    const std::string& imageFile,
    const std::string& outputFile,
    float min,
    float max
) {
    png::image<png::rgba_pixel> img(imageFile);
    un::SimpleJobSystem jobSystem(nThreads, true);
    png::image<png::gray_pixel> sdf(size, size);
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
    jobSystem.complete();
    sdf.write(outputFile);
}
