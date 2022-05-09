#include <cxxopts.hpp>
#include "generation.h"

static const char* const kFileArgName = "file";
static const char* const kRelativeToName = "relative_to";

static const char* const kIncludeArgName = "include";
static const char* const kOutputDir = "output";

bool isSerializable(const std::shared_ptr<un::CXXComposite>& composite);

int main(int argc, char** args) {
    cxxopts::Options options("unnecessary_serialization_generator");
    options.add_options()
        (kFileArgName, "File to parse and generate",
         cxxopts::value<std::string>());
    options.add_options()
        (kIncludeArgName, "Adds an include directory to parsing",
         cxxopts::value<std::vector<std::string>>());
    options.add_options()
        (kOutputDir, "Directory where to write the generated files",
         cxxopts::value<std::string>());
    options.add_options()
        (kRelativeToName, "Directory where to write the generated files",
         cxxopts::value<std::string>());
    options.parse_positional(kFileArgName);
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        auto includes = result[kIncludeArgName].as<std::vector<std::string>>();
        auto file = result[kFileArgName].as<std::string>();
        std::string ownInclude;
        std::string relativeTo;
        if (result.count(kRelativeToName) > 0) {
            relativeTo = result[kRelativeToName].as<std::string>();

        } else {
            relativeTo = std::filesystem::current_path().string();
        }
        ownInclude = std::filesystem::relative(file, relativeTo).string();
        std::filesystem::path output = result[kOutputDir].as<std::string>();
        std::filesystem::path includesOutput = output / "include";
        un::parsing::ParsingOptions parseOptions(file, includes);
        auto translationUnit = un::parsing::parse(parseOptions);
        for (const auto& composite : translationUnit.collectSymbols<un::CXXComposite>()) {
            if (!un::isSerializable(composite)) {
                continue;
            }
            un::GenerationInfo info;
            std::vector<std::string> additionalIncludes;
            for (const auto& item : composite->getFields()) {
                const un::CXXType& type = item.getType();
                if (type.getInnerType() != un::CXXTypeKind::eComplex) {
                    continue;
                }
                un::CXXType other;
                if (translationUnit.searchType(type.getName(), other)) {
                    std::string name = other.getName();
                    auto lastName = name.find_last_of(':');
                    if (lastName > 0) {
                        name = name.substr(lastName + 1);
                    }
                    std::string include = un::getGeneratedIncludeName(name);
                    additionalIncludes.emplace_back(include);
                }
            }
            std::string name = composite->getName();
            info.name = name;
            info.upper = un::upper(name);
            info.lower = un::lower(name);
            info.fullName = composite->getFullName();
            info.ownInclude = ownInclude;
            generateSerializerInclude(includesOutput, composite, info, additionalIncludes);
        }
    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
}
