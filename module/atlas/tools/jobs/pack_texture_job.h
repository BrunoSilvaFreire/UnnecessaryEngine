#ifndef UNNECESSARYENGINE_PACK_JOB_H
#define UNNECESSARYENGINE_PACK_JOB_H

#include <png++/png.hpp>
#include <packing_algorithm.h>
#include "unnecessary/memory/membuf.h"
#include <unnecessary/misc/indexing.h>
#include <unnecessary/jobs/parallel_for_job.h>

namespace un {
    template<typename TPixel = png::rgba_pixel>
    class PackTextureJob final : public un::ParallelForJob<JobWorker> {
    private:
        std::shared_ptr<png::image<TPixel>> source;
        std::shared_ptr<png::image<TPixel>> destination;
        un::Rect<u32> rect;
    public:
        PackTextureJob(
            const std::shared_ptr<png::image<TPixel>>& source,
            const std::shared_ptr<png::image<TPixel>>& destination,
            un::Rect<u32> rect
        ) : source(source),
            destination(destination),
            rect(rect) {

        }

        UN_AGGRESSIVE_INLINE void operator()(std::size_t index, JobWorker* worker) override {
            auto pos = un::indexing::position_of(index, static_cast<std::size_t>(rect.getWidth()));
            auto[x0, y0] = pos;

            const TPixel& pixel = source->get_pixel(x0, y0);
            const auto& origin = rect.getOrigin();
            u32 x1 = origin.x + x0;
            u32 y1 = origin.y + y0;
            destination->set_pixel(x1, y1, pixel);
        }
    };

}
#endif
