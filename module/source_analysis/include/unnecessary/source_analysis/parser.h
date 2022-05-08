//
// Created by bruno on 03/05/2022.
//

#ifndef UNNECESSARYENGINE_PARSER_H
#define UNNECESSARYENGINE_PARSER_H

#include <filesystem>
#include <functional>
#include <stack>
#include <clang/AST/AST.h>
#include <clang/Frontend/ASTUnit.h>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/logging.h>

namespace un::parsing {
    class ParsingOptions {
    private:
        std::filesystem::path file;
        std::vector<std::string> includes;

    public:
        ParsingOptions(std::filesystem::path file, std::vector<std::string> includes);

        const std::filesystem::path& getFile() const;

        const std::vector<std::string>& getIncludes() const;
    };

    class Parser {
    private:
        // Clang stuff
        std::unique_ptr<clang::ASTUnit> clangUnit;
        // Parsing stuff

        un::CXXTranslationUnit translationUnit;
        un::CXXAccessModifier currentAccess = un::CXXAccessModifier::eNone;
        std::shared_ptr<un::CXXComposite> currentComposite;
        std::vector<un::CXXMacroExpansion> macros;

        bool check_equal(CXToken* first, CXToken* second);

        static CXChildVisitResult visitor_func(CXCursor first, CXCursor second, CXClientData data);

        std::string to_string(const CXString& cursor);

        std::string to_string(const CXCursor& cursor);

        void setSpecifier(CX_CXXAccessSpecifier specifier);
/*
        std::string getCurrentNamespace() const {
            return un::join_strings("::", namespaces.begin(), namespaces.end());

        }

        CXChildVisitResult visit(CXCursor cursor, CXCursor parent) {
            CXCursorKind kind = clang_getCursorKind(cursor);
            CXCursorKind parentKind = clang_getCursorKind(parent);
            auto loc = clang_getCursorLocation(cursor);
            if (clang_Location_isInSystemHeader(loc)) {
                return CXChildVisit_Continue;
            }
            std::string nameStr = to_string(clang_getCursorSpelling(cursor));
            LOG(INFO) << "Found " << nameStr << " (" << to_string(clang_getCursorKindSpelling(kind)) << ").";
            switch (kind) {
                case CXCursor_MacroDefinition:
                    return CXChildVisit_Continue;
                case CXCursor_Namespace:
                case CXCursor_NamespaceRef:
                case CXCursor_NamespaceAlias:
                    namespaces.push_back(nameStr);

                    return CXChildVisit_Recurse;
                case CXCursor_ClassDecl:
                    currentAccess = un::CXXAccessModifier::eNone;

                    currentComposite = std::make_shared<un::CXXComposite>(nameStr, getCurrentNamespace());
                    return CXChildVisit_Continue;
                case CXCursor_CXXAccessSpecifier:
                    setSpecifier(clang_getCXXAccessSpecifier(cursor));
                    return CXChildVisit_Continue;
                case CXCursor_FieldDecl:
                    addField(cursor, nameStr);
                    return CXChildVisit_Continue;

                default:
                    return CXChildVisit_Recurse;

            }

            return CXChildVisit_Recurse;
        }*/

        void addField(CXCursor cursor, std::string nameStr);

    public:

        Parser(const un::parsing::ParsingOptions& options);

        const CXXTranslationUnit& getTranslationUnit() const {
            return translationUnit;
        }
    };
}
#endif