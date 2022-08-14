#include <unnecessary/serializer/generation.h>
#include <unnecessary/serializer/writers/default_writer.h>

namespace un {

    GenerationInfo::GenerationInfo(
        const CXXDeclaration& toGenerate
    ) : name(toGenerate.getName()),
        upper(un::upper(name)),
        lower(un::lower(name)),
        fullName(toGenerate.getFullName()) {
    }
}