#ifndef UNNECESSARYENGINE_MATERIAL_H
#define UNNECESSARYENGINE_MATERIAL_H

#include <unnecessary/rendering/pipelines/pipeline.h>

namespace un {

    class material {
    private:
        int _renderOrder;
        un::pipeline* _pipeline;
    public:
        bool operator<(const material& rhs) const;

        bool operator>(const material& rhs) const;

        bool operator<=(const material& rhs) const;

        bool operator>=(const material& rhs) const;

        int get_render_order() const;

        pipeline* get_pipeline() const;
    };
}
#endif
