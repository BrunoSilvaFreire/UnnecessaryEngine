#ifndef UNNECESSARYENGINE_PACK_JOB_H
#define UNNECESSARYENGINE_PACK_JOB_H

#include <png++/png.hpp>
#include <packing_algorithm.h>
#include "unnecessary/memory/membuf.h"
#include <unnecessary/misc/indexing.h>
#include <unnecessary/jobs/parallel_for_job.h>

namespace un {
    template<typename TPixel = png::rgba_pixel>
    class PackTextureJob final : public un::ParallelForJob<un::JobWorker> {
    private:
        std::shared_ptr<png::image<TPixel>> source;
        std::shared_ptr<png::image<TPixel>> destination;
        std::size_t destX, destY;
        std::size_t sourceWidth;
    public:
        PackTextureJob(
            const std::shared_ptr<png::image<TPixel>>& source,
            const std::shared_ptr<png::image<TPixel>>& destination,
            size_t destX,
            size_t destY
        ) : source(source),
            destination(destination),
            destX(destX),
            destY(destY),
            sourceWidth(source->get_width()) {

        }

    public:
        UN_AGGRESSIVE_INLINE void operator()(size_t index, JobWorker* worker) override {
            auto [x0, y0] = un::indexing::position_of(index, sourceWidth);
            const TPixel& pixel = source->get_pixel(x0, y0);
            std::size_t x1 = destX + x0;
            std::size_t y1 = destY + y0;
            destination->set_pixel(x1, y1, pixel);
        }
    };

}
#endif
