#include <unnecessary/rendering/material.h>

namespace un {
    bool material::operator<(const material& rhs) const {
        return _pipeline < rhs._pipeline;
    }

    bool material::operator>(const material& rhs) const {
        return rhs < *this;
    }

    bool material::operator<=(const material& rhs) const {
        return !(rhs < *this);
    }

    bool material::operator>=(const material& rhs) const {
        return !(*this < rhs);
    }

    int material::get_render_order() const {
        return _renderOrder;
    }

    pipeline* material::get_pipeline() const {
        return _pipeline;
    }
}
