#include <cxxopts.hpp>
#include <unnecessary/sdf/generator/sdf_generator.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>

static const char* const k_input_name = "input";
static const char* const k_threads_name = "threads";
static const char* const k_min_name = "min";
static const char* const k_max_name = "max";

#ifndef UN_TESTING

int main(int argc, char** argv) {
    cxxopts::Options options(
        "unnecessary_sdf_generator",
        "Signed Distance Field generator."
    );
    int size = 64;
    int nThreads = std::thread::hardware_concurrency();
    options.add_options()
               (k_input_name, "RichInput file", cxxopts::value<std::string>())
               ("output", "PNG output", cxxopts::value<std::string>())
               ("size", "Generate CPP", cxxopts::value<int>(size))
               (k_min_name, "Minimum SDF distance", cxxopts::value<float>())
               (k_max_name, "Maximum SDF distance", cxxopts::value<float>())
               (k_threads_name, "Number of threads", cxxopts::value<int>(nThreads));
    options.parse_positional(k_input_name);
    const auto& result = options.parse(argc, argv);
    size_t a = result.count(k_input_name);
    if (a > 0) {
        auto imageFile = result[k_input_name].as<std::string>();
        auto outputFile = result["output"].as<std::string>();
        float min = result[k_min_name].as<float>();
        float max = result[k_max_name].as<float>();
        nThreads = result[k_threads_name].as<int>();
        un::sdf::process_sdf(size, nThreads, imageFile, outputFile, min, max);
    }
    else {
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
    std::filesystem::path inputPath(imageFile);
    auto extension = inputPath.extension();

    png::image<png::rgba_pixel> img(imageFile);
    png::image<png::gray_pixel> sdf(size, size);
    LOG(INFO) << "Creating image of size " << size << "x" << size << "(" << size * size
              << " pixels)";
    {
        simple_job_system jobSystem(nThreads, false);
        job_system_recorder<simple_job_system> profiler(&jobSystem);
        process_pixel_job job(
            &img,
            &sdf,
            min,
            max
        );
        {
            job_chain<simple_job_system> chain(&jobSystem);

            process_pixel_job::parallelize(
                &job,
                chain,
                size * size,
                128
            );
        }
        chronometer<std::chrono::milliseconds> chronometer;
        jobSystem.start();
        jobSystem.complete();
        std::chrono::milliseconds duration = chronometer.stop();
        LOG(INFO) << "Took " << duration.count() << "ms.";
        profiler.save_to_file(std::filesystem::current_path() / "sdf_generator.csv");
    }
    LOG(INFO) << "Saving image to '" << outputFile << "'";
    sdf.write(outputFile);
}
