#ifndef UNNECESSARYENGINE_JSON_ARCHIVER_H
#define UNNECESSARYENGINE_JSON_ARCHIVER_H

#include <unnecessary/archives/archiver.h>

namespace un {
    class JsonArchiver final : public Archiver {
    public:
        un::Buffer write(const Serialized& serialized) override;
        void read(const Buffer& buffer, Serialized& into) override;
    };
}
#endif
