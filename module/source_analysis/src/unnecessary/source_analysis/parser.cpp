#include <unnecessary/source_analysis/parser.h>
#include <utility>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/AST/AST.h>

namespace un::parsing {
    const std::filesystem::path& ParsingOptions::getFile() const {
        return file;
    }

    ParsingOptions::ParsingOptions(
        std::filesystem::path file,
        std::vector<std::string> includes
    ) : file(std::move(file)), includes(std::move(includes)) { }

    const std::vector<std::string>& ParsingOptions::getIncludes() const {
        return includes;
    }

    Parser::Parser(const ParsingOptions& options) {
        auto index = clang_createIndex(true, true);
        auto work = std::filesystem::current_path();
        std::string asString = options.getFile().string();
        const char* cStr = asString.c_str();
        std::unique_ptr<std::vector<const char*>> Args(new std::vector<const char*>());
        std::vector<const char*> args;
        args.emplace_back("clang++");
        args.emplace_back("-std=c++20");
        // Detailed preprocessing
        args.push_back("-Xclang");
        args.push_back("-detailed-preprocessing-record");
        std::vector<std::string> includeArgs;
        for (const std::string& include : options.getIncludes()) {
            includeArgs.emplace_back("-I" + include);
        }
        auto SkipFunctionBodies = clang::SkipFunctionBodiesScope::PreambleAndMainFile;
        auto CaptureDiagnostics = clang::CaptureDiagsKind::AllWithoutNonErrorsFromIncludes;

        for (const auto& item : includeArgs) {
            args.emplace_back(item.data());
        }
        std::shared_ptr<clang::PCHContainerOperations> pchContainerOpts;
        std::shared_ptr<clang::CompilerInvocation> invocation;
        clang::FileSystemOptions fsOptions;
        clang::FileManager fileManager(fsOptions);
        llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> dEngine;
        clang::ASTUnit::LoadFromCompilerInvocation(
            invocation,
            pchContainerOpts,
            dEngine,
            &fileManager
        );

    }

    void Parser::setSpecifier(CX_CXXAccessSpecifier specifier) {
        switch (specifier) {
            case CX_CXXInvalidAccessSpecifier:
                break;
            case CX_CXXPublic:
                currentAccess = un::CXXAccessModifier::ePublic;
                break;
            case CX_CXXProtected:
                currentAccess = un::CXXAccessModifier::eProtected;
                break;
            case CX_CXXPrivate:
                currentAccess = un::CXXAccessModifier::ePrivate;
                break;
        }
    }


    std::string Parser::to_string(const CXString& cursor) {
        return std::string(clang_getCString(cursor));
    }
/*
    CXChildVisitResult Parser::visitor_func(CXCursor first, CXCursor second, CXClientData data) {
        return reinterpret_cast<Parser*>(data)->visit(first, second);
    }*/

    std::string Parser::to_string(const CXCursor& cursor) {
        return to_string(clang_getCursorDisplayName(cursor));
    }

    void Parser::addField(CXCursor cursor, std::string nameStr) {
        CXType cType = clang_getCursorType(cursor);
        CXSourceLocation loc = clang_getCursorLocation(cursor);
        std::string type = to_string(clang_getTypeSpelling(cType));
        un::CXXField field(currentAccess, nameStr, type, macros);
        currentComposite->addField(std::move(field));
        macros.clear();
    }
}