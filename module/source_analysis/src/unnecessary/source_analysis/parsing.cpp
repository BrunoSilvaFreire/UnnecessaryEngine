#include <clang-c/Index.h>
#include <unnecessary/source_analysis/parsing.h>

namespace un::parsing {
    un::CXXTranslationUnit parse(const un::parsing::ParsingOptions& options) {
       un::parsing::Parser parser(options);
        return parser.getTranslationUnit();
    }
}