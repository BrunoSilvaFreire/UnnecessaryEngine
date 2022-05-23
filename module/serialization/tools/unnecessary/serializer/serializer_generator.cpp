#include <cxxopts.hpp>
#include <chrono>
#include "generation.h"

static const char* const kFileArgName = "file";
static const char* const kRelativeToName = "relative_to";
static const char* const kGlobalIncludeName = "global_include";

static const char* const kIncludeArgName = "include";
static const char* const kOutputDir = "output";

bool isSerializable(const std::shared_ptr<un::CXXComposite>& composite);

void process(
    const std::vector<std::string>& parsingIncludes,
    const std::vector<std::string>& globalIncludes,
    const std::string& file,
    const std::filesystem::path& output,
    const std::string& relativeTo
);

int main(int argc, char** args) {
    cxxopts::Options options("unnecessary_serialization_generator");
    options.add_options("required")
               (
                   kFileArgName, "File to parse and generate",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   kIncludeArgName, "Adds an include directory to parsing",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   kOutputDir, "Directory where to write the generated files",
                   cxxopts::value<std::string>()
               );
    options.add_options("optional")
               (
                   kGlobalIncludeName, "Specifies an include that will be added to all generated files",
                   cxxopts::value<std::vector<std::string>>()
               )
               (
                   kRelativeToName, "Directory where to write the generated files",
                   cxxopts::value<std::string>()
               );
    try {
        cxxopts::ParseResult result = options.parse(argc, args);
        auto includes = result[kIncludeArgName].as<std::vector<std::string>>();
        std::vector<std::string> globalIncludes;
        if (result.count(kGlobalIncludeName) > 0) {
            globalIncludes = result[kGlobalIncludeName].as<std::vector<std::string>>();
        }

        std::size_t numSpecifiedFiles = result.count(kFileArgName);

        std::filesystem::path output = result[kOutputDir].as<std::string>();
        std::string relativeTo;
        if (result.count(kRelativeToName) > 0) {
            relativeTo = result[kRelativeToName].as<std::string>();
        } else {
            relativeTo = std::filesystem::current_path().string();
        }
        if (numSpecifiedFiles == 1) {
            const auto& file = result[kFileArgName].as<std::string>();
            process(includes, globalIncludes, file, output, relativeTo);
        } else {
            std::filesystem::path filePath = "temp";

            auto epoch = std::chrono::system_clock::now().time_since_epoch().count();
            filePath /= (std::string("multiple_files_") + std::to_string(epoch) + ".h");
            LOG(INFO) << "Generating unity file @ " << std::filesystem::absolute(filePath);
            {
                un::files::ensure_directory_exists(filePath.parent_path());
                std::ofstream tempAllIncludes(filePath);
                const auto& files = result[kFileArgName].as<std::vector<std::string>>();
                for (const auto& item : files) {
                    std::filesystem::path p = item;
                    std::filesystem::path relative = std::filesystem::relative(item, relativeTo);
                    tempAllIncludes << "#include <" << relative.string() << ">" << std::endl;
                }
            }
            process(includes, globalIncludes, filePath.string(), output, relativeTo);
        }
    } catch (const cxxopts::OptionParseException& x) {
        std::cerr << "dog: " << x.what() << '\n';
        std::cerr << "usage: dog [options] <input_file> ...\n";
        return EXIT_FAILURE;
    }
}

void process(
    const std::vector<std::string>& parsingIncludes,
    const std::vector<std::string>& globalIncludes,
    const std::string& file,
    const std::filesystem::path& output,
    const std::string& relativeTo
) {
    std::string ownInclude = std::filesystem::relative(file, relativeTo).string();
    std::filesystem::path includesOutput = output / "include";
    std::filesystem::path filePath = file;
    un::parsing::ParsingOptions parseOptions(file, parsingIncludes);
    parseOptions.setDebugFile(output / "cppast.info.txt");
    auto translationUnit = un::parsing::parse(parseOptions);
    std::vector<std::string> allGeneratedFilenames;
    for (const auto& composite : translationUnit.collectSymbols<un::CXXComposite>()) {
        LOG(INFO) << "Checking " << composite->getFullName();
        if (!un::isSerializable(composite)) {
            LOG(INFO) << composite->getFullName() << " is not serializable, skipping.";
            continue;
        }
        un::GenerationInfo info;
        std::vector<std::string> additionalIncludes;
        for (const auto& item : composite->getFields()) {
            const un::CXXType& type = item.getType();
            if (type.getKind() != un::eComplex) {
                continue;
            }
            std::shared_ptr<un::CXXComposite> other;
            std::string typeName = type.getName();
            if (!translationUnit.findSymbol<un::CXXComposite>(typeName, other)) {
//                    LOG(WARN) << "Unable to find composite type " << typeName << " for field " << item.getName()
//                              << ". Will not add extra include.";
                continue;
            }
            if (!un::isSerializable(other)) {
//                    LOG(WARN) << "Type " << typeName << " for field " << item.getName()
//                              << " isn't serializable. Will not add extra include.";
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
        LOG(INFO) << composite->getFullName() << " is suitable for serialization, proceeding...";
        generateSerializerInclude(includesOutput, composite, translationUnit, info, additionalIncludes);
        allGeneratedFilenames.emplace_back(info.lower + ".serializer.generated.h");
    }
    std::stringstream ss;
    ss << "// All generated serializers are included here" << std::endl;
    ss << "#pragma once" << std::endl;
    for (const std::string& file : allGeneratedFilenames) {
        ss << "#include <" << file << ">" << std::endl;
    }
    un::files::safe_write_file(
        includesOutput / (filePath.filename().replace_extension().string() + ".serializers.h"), ss
    );
}
