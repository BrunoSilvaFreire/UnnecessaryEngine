#ifndef UNNECESSARYENGINE_PARSING_H
#define UNNECESSARYENGINE_PARSING_H

#include <filesystem>
#include <unnecessary/source_analysis/parser.h>

namespace un::parsing {



    un::CXXTranslationUnit parse(const un::parsing::ParsingOptions& options);
}
#endif
