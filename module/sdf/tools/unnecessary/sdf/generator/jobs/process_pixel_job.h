//
// Created by bruno on 17/01/2022.
//

#ifndef UNNECESSARYENGINE_PROCESS_PIXEL_JOB_H
#define UNNECESSARYENGINE_PROCESS_PIXEL_JOB_H

#include <unnecessary/jobs/parallel_for_job.h>
#include <png++/image.hpp>
#include <glm/glm.hpp>
#include <cstddef>
#include <unnecessary/misc/math.h>

namespace un {

    class ProcessPixelJob : public un::ParallelForJob<un::JobWorker> {
    private:
        png::image<png::rgba_pixel>* input;
        png::image<png::gray_pixel>* output;
        float minDistance;
        float maxDistance;

        UN_AGGRESSIVE_INLINE glm::uvec2 outputToInputCoordinates(size_t x, size_t y) {
            size_t ow = output->get_width();
            size_t oh = output->get_height();
            float dw = static_cast<float>(x) / ow;
            float dh = static_cast<float>(y) / oh;
            size_t iw = input->get_width();
            size_t ih = input->get_height();
            size_t fx = static_cast<size_t>(iw * dw);
            size_t fy = static_cast<size_t>(ih * dh);
            return glm::uvec2(fx, fy);
        }


    public:
        ProcessPixelJob(
            png::image<png::rgba_pixel>* input,
            png::image<png::gray_pixel>* output,
            float minDistance,
            float maxDistance
        );

        UN_AGGRESSIVE_INLINE void batchStarted(size_t start, size_t end) override {
            LOG(INFO) << "Starting batch " << start << " -> " << end << " (" << (end - start + 1)
                      << " pixels)";
        }


        UN_AGGRESSIVE_INLINE void batchFinished(size_t start, size_t end) override {
            LOG(INFO) << "Finished batch " << start << " -> " << end << " (" << (end - start + 1)
                      << " pixels)";
        }

        UN_AGGRESSIVE_INLINE void operator()(size_t index, JobWorker* worker) override {
            int ow = output->get_width();
            int oh = output->get_height();
            int ox = index % ow;
            int oy = index / oh;
            glm::uvec2 inputCoords = outputToInputCoordinates(ox, oy);
            float fullDistance = computeDistanceOfPixel(inputCoords);
            float distance01 = un::inv_lerp(minDistance, maxDistance, fullDistance);
            png::gray_pixel pixel = un::lerp(
                std::numeric_limits<png::gray_pixel>::min(),
                std::numeric_limits<png::gray_pixel>::max(),
                1 - distance01
            );
            output->set_pixel(ox, oy, pixel);
            LOG(INFO) << "Pixel " << index << " (" << ox << ", " << oy << ") processed @ "
                      << (int) pixel
                      << " (dist: " << distance01 << ", " << fullDistance << " @ hex: "
                      << std::hex << (int) pixel
                      << std::dec
                      << ")";
        }

        static UN_AGGRESSIVE_INLINE bool isOutOfBounds(
            int nx,
            int ny,
            size_t width,
            size_t height
        ) {
            return nx < 0 || ny < 0 || nx >= width || ny >= height;
        }

        UN_AGGRESSIVE_INLINE float computeDistanceOfPixel(const glm::uvec2& inputCoords) {
            int iw = input->get_width();
            int ih = input->get_height();
            float d = maxDistance;
            for (int dy = -iw; dy < iw; ++dy) {
                if (abs(dy) > maxDistance) {
                    continue;
                }
                for (int dx = -iw; dx < iw; ++dx) {
                    if (abs(dx) > maxDistance) {
                        continue;
                    }
                    int nx = inputCoords.x + dx;
                    int ny = inputCoords.y + dy;
                    if (isOutOfBounds(nx, ny, iw, ih)) {
                        continue;
                    }
                    glm::uvec2 neighbor(nx, ny);
                    const png::rgba_pixel& pixel = input->get_pixel(neighbor.x, neighbor.y);
                    if (pixel.alpha > 0) {
                        float candidate = glm::distance(
                            glm::vec2(inputCoords.x, inputCoords.y),
                            glm::vec2(neighbor.x, neighbor.y)
                        );
                        if (candidate < d) {
                            d = candidate;
                        }
                    }
                }
            }
            return d;
        }
    };
}
#endif
