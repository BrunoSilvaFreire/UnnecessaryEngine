#ifndef UNNECESSARYENGINE_PACK_TEXTURE_JOB_H
#define UNNECESSARYENGINE_PACK_TEXTURE_JOB_H

#include <png++/png.hpp>
#include <packing_algorithm.h>
#include "unnecessary/memory/membuf.h"
#include <unnecessary/misc/indexing.h>
#include <unnecessary/jobs/parallel_for_job.h>

namespace un {
    template<typename TPixel = png::rgba_pixel>
    class pack_texture_job final : public parallel_for_job<job_worker> {
    private:
        std::shared_ptr<png::image<TPixel>> _source;
        std::shared_ptr<png::image<TPixel>> _destination;
        un::rect<u32> _rect;

    public:
        pack_texture_job(
            const std::shared_ptr<png::image<TPixel>>& source,
            const std::shared_ptr<png::image<TPixel>>& destination,
            un::rect<u32> rect
        ) : _source(source),
            _destination(destination),
            _rect(rect) {
        }

        UN_AGGRESSIVE_INLINE void operator()(std::size_t index, job_worker* worker) override {
            auto pos = indexing::position_of(index, static_cast<std::size_t>(_rect.get_width()));
            auto [x0, y0] = pos;

            const TPixel& pixel = _source->get_pixel(x0, y0);
            const auto& origin = _rect.get_origin();
            u32 x1 = origin.x + x0;
            u32 y1 = origin.y + y0;
            _destination->set_pixel(x1, y1, pixel);
        }
    };
}
#endif
