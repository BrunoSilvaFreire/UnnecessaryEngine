#include <unnecessary/source_analysis/parsing.h>
#include <cxxopts.hpp>

static const char* const kFileArgName = "file";
static const char* const kIncludeArgName = "include";

int main(int argc, char** args) {
    cxxopts::Options options("unnecessary_serialization_generator");
    options.add_options()
        (kFileArgName, "File to parse and generate",
         cxxopts::value<std::string>());
    options.add_options()
        (kIncludeArgName, "Adds an include directory to parsing",
         cxxopts::value<std::vector<std::string>>());
    options.parse_positional(kFileArgName);
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        auto includes = result[kIncludeArgName].as<std::vector<std::string>>();
        auto file = result[kFileArgName].as<std::string>();
        un::parsing::ParsingOptions parseOptions(file, includes);
        auto translationUnit = un::parsing::parse(parseOptions);
    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
}