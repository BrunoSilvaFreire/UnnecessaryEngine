
#include <cxxopts.hpp>
#include <png++/png.hpp>
#include "packer_entry.h"
#include "packer.h"
#include <algorithms/heuristics.h>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/jobs/misc/load_file_job.h>
#include <unnecessary/jobs/recorder/job_system_recorder.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/memory/buffer_stream.h>
#include <algorithms/max_rectangles_algorithm.h>
#include <jobs/pack_texture_job.h>
#include <grapphs/dot.h>
#include <sstream>
#include <istream>

#ifndef UN_TESTING

int main(int argc, char* args[]) {
    cxxopts::Options options("UnnecessaryPacker");
    options.add_options()
        ("input_files", "Input file(s) to concatenate", cxxopts::value<std::vector<std::string>>());
    options.parse_positional("input_files");
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        auto files = result["input_files"].as<std::vector<std::string>>();
        un::packer::pack(files);
    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
}

#endif

namespace un::packer {

}