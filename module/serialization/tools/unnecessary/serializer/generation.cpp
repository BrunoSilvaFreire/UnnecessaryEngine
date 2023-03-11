#include <unnecessary/serializer/generation.h>
#include "unnecessary/serializer/writers/fields/default_writer.h"

namespace un {
    generation_info::generation_info(
        const cxx_declaration& toGenerate
    ) : name(toGenerate.get_name()),
        fullName(toGenerate.get_full_name()),
        upper(un::upper(name)),
        lower(un::lower(name)) {
    }
}
