#include <unnecessary/source_analysis/parser.h>

#include <utility>

namespace un::parsing {
    const std::filesystem::path& ParsingOptions::getFile() const {
        return file;
    }

    ParsingOptions::ParsingOptions(std::filesystem::path file, std::vector<std::string> includes)
        : file(std::move(file)), includes(includes) { }

    const std::vector<std::string>& ParsingOptions::getIncludes() const {
        return includes;
    }

    Parser::Parser(const ParsingOptions& options) {
        auto index = clang_createIndex(true, true);
        auto work = std::filesystem::current_path();
        std::string asString = options.getFile().string();
        CXTranslationUnit translationUnit;
        const char* cStr = asString.c_str();
        std::vector<const char*> args;
        args.emplace_back("clang++");
        args.emplace_back("-stdlib=libc++");
        std::vector<std::string> includeArgs;
        for (const std::string& include : options.getIncludes()) {
            includeArgs.emplace_back("-I" + include);
        }
        for (const auto& item : includeArgs){
            args.emplace_back(item.data());
        }
        auto err = clang_parseTranslationUnit2FullArgv(
            index,
            cStr,
            args.data(),
            args.size(),
            nullptr,
            0,
            CXTranslationUnit_SkipFunctionBodies |
            CXTranslationUnit_DetailedPreprocessingRecord |
            CXTranslationUnit_IncludeAttributedTypes |
            CXTranslationUnit_SingleFileParse |
            CXTranslationUnit_IgnoreNonErrorsFromIncludedFiles,
            &translationUnit
        );

        CXCursor rootCursor = clang_getTranslationUnitCursor(translationUnit);
        CXSourceRange range = clang_getCursorExtent(rootCursor);
//            CXToken* tokens;
//            std::uint32_t numTokens;
//            clang_tokenize(translationUnit, range, &tokens, &numTokens);
        clang_visitChildren(
            rootCursor,
            &un::parsing::Parser::visitor_func,
            this
        );

    }
}