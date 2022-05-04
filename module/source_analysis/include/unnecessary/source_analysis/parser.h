//
// Created by bruno on 03/05/2022.
//

#ifndef UNNECESSARYENGINE_PARSER_H
#define UNNECESSARYENGINE_PARSER_H

#include <filesystem>
#include <functional>
#include <clang-c/Index.h>
#include <stack>
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
        un::CXXTranslationUnit translationUnit;
        un::CXXAccessModifier currentAccess = un::CXXAccessModifier::eNone;
        std::shared_ptr<un::CXXComposite> currentComposite;

        static CXChildVisitResult visitor_func(CXCursor first, CXCursor second, CXClientData data) {
            return reinterpret_cast<Parser*>(data)->visit(first, second);
        }

        std::string to_string(const CXString& cursor) {
            return std::string(clang_getCString(cursor));
        }

        std::string to_string(const CXCursor& cursor) {
            return to_string(clang_getCursorDisplayName(cursor));
        }

        CXChildVisitResult visit(CXCursor cursor, CXCursor parent) {
            CXCursorKind kind = clang_getCursorKind(cursor);
            CXCursorKind parentKind = clang_getCursorKind(parent);
            std::string nameStr = to_string(clang_getCursorSpelling(cursor));
            if (kind == CXCursor_MacroDefinition) {
                return CXChildVisit_Continue;
            }
            if (kind == CXCursor_ClassDecl) {
                currentAccess = un::CXXAccessModifier::eNone;
                currentComposite = std::make_shared<un::CXXComposite>(nameStr);
            }
            if (kind == CXCursor_CXXAccessSpecifier) {
                auto specifier = clang_getCXXAccessSpecifier(cursor);
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
                return CXChildVisit_Continue;
            }
            if (kind == CXCursor_FieldDecl) {

                CXType cType = clang_getCursorType(cursor);
                CXSourceLocation loc = clang_getCursorLocation(cursor);
                std::string semantic = to_string(clang_getCursorSemanticParent(cursor));
                std::string definition = to_string(clang_getCursorDefinition(cursor));
                std::string reference = to_string(clang_getCursorReferenced(cursor));
                std::string canonical = to_string(clang_getCanonicalCursor(cursor));
                std::string typedefName = to_string(clang_getTypeSpelling(clang_getElementType(cType)));
                std::string spelling = to_string(clang_getTypeSpelling(cType));
                un::CXXField field(currentAccess, nameStr, "unknownType");
                return CXChildVisit_Continue;
            }

            return CXChildVisit_Recurse;
        }

    public:

        Parser(const un::parsing::ParsingOptions& options);

        const CXXTranslationUnit& getTranslationUnit() const {
            return translationUnit;
        }
    };
}
#endif