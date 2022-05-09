#include <cxxopts.hpp>
#include "generation.h"

static const char* const kFileArgName = "file";
static const char* const kRelativeToName = "relative_to";
static const char* const kGlobalIncludeName = "global_include";

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
        (kGlobalIncludeName, "Specifies an include that will be added to all generated files",
         cxxopts::value<std::vector<std::string>>());
    options.add_options()
        (kRelativeToName, "Directory where to write the generated files",
         cxxopts::value<std::string>());
    options.parse_positional(kFileArgName);
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        auto includes = result[kIncludeArgName].as<std::vector<std::string>>();
        std::vector<std::string> globalIncludes;
        if (result.count(kGlobalIncludeName) > 0) {
            globalIncludes = result[kGlobalIncludeName].as<std::vector<std::string>>();
        }

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
            LOG(INFO) << "Generating " << composite->getFullName();
            un::GenerationInfo info;
            std::vector<std::string> additionalIncludes;
            for (const auto& item : composite->getFields()) {
                const un::CXXType& type = item.getType();
                if (type.getKind() != un::CXXTypeKind::eComplex) {
                    continue;
                }
                std::shared_ptr<un::CXXComposite> other;
                std::string typeName = type.getName();
                if (!translationUnit.findSymbol<un::CXXComposite>(typeName, other)) {
                    LOG(WARN) << "Unable to find type " << typeName << " for field " << item.getName();
                    continue;
                }
                if (!un::isSerializable(other)) {
                    LOG(WARN) << "Type " << typeName << " for field " << item.getName() << " isn't serializable";
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
            generateSerializerInclude(includesOutput, composite, info, additionalIncludes);
        }
    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
}
