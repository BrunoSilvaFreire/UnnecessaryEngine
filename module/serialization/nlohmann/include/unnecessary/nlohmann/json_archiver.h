#ifndef UNNECESSARYENGINE_JSON_ARCHIVER_H
#define UNNECESSARYENGINE_JSON_ARCHIVER_H

#include <unnecessary/archives/archiver.h>

namespace un {
    class json_archiver final : public archiver {
    public:
        byte_buffer write(const serialized& serialized) override;

        void read(const byte_buffer& buffer, serialized& into) override;
    };
}
#endif
