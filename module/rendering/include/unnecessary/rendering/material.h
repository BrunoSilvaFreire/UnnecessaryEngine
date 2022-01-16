#ifndef UNNECESSARYENGINE_MATERIAL_H
#define UNNECESSARYENGINE_MATERIAL_H

#include <unnecessary/rendering/pipelines/pipeline.h>

namespace un {

    class Material {
    private:
        int renderOrder;
        un::Pipeline* pipeline;
    public:
        bool operator<(const Material& rhs) const;

        bool operator>(const Material& rhs) const;

        bool operator<=(const Material& rhs) const;

        bool operator>=(const Material& rhs) const;

        int getRenderOrder() const;

        Pipeline* getPipeline() const;
    };
}
#endif
