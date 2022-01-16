#include <unnecessary/rendering/material.h>

namespace un {

    bool un::Material::operator<(const un::Material& rhs) const {
        return pipeline < rhs.pipeline;
    }

    bool un::Material::operator>(const un::Material& rhs) const {
        return rhs < *this;
    }

    bool un::Material::operator<=(const un::Material& rhs) const {
        return !(rhs < *this);
    }

    bool un::Material::operator>=(const un::Material& rhs) const {
        return !(*this < rhs);
    }

    int Material::getRenderOrder() const {
        return renderOrder;
    }

    un::Pipeline* Material::getPipeline() const {
        return pipeline;
    }
}