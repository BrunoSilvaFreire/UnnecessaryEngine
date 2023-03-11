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
    class process_pixel_job final : public parallel_for_job<job_worker> {
    private:
        png::image<png::rgba_pixel>* _input;
        png::image<png::gray_pixel>* _output;
        float _minDistance;
        float _maxDistance;

        UN_AGGRESSIVE_INLINE glm::uvec2 output_to_input_coordinates(size_t x, size_t y) {
            size_t ow = _output->get_width();
            size_t oh = _output->get_height();
            float dw = static_cast<float>(x) / ow;
            float dh = static_cast<float>(y) / oh;
            size_t iw = _input->get_width();
            size_t ih = _input->get_height();
            size_t fx = iw * dw;
            size_t fy = ih * dh;
            return glm::uvec2(fx, fy);
        }

    public:
        process_pixel_job(
            png::image<png::rgba_pixel>* input,
            png::image<png::gray_pixel>* output,
            float minDistance,
            float maxDistance
        );

        void batch_started(size_t start, size_t end) override {
            LOG(INFO) << "Starting batch " << start << " -> " << end << " (" << (end - start + 1)
                      << " pixels)";
        }

        void batch_finished(size_t start, size_t end) override {
            LOG(INFO) << "Finished batch " << start << " -> " << end << " (" << (end - start + 1)
                      << " pixels)";
        }

        UN_AGGRESSIVE_INLINE void operator()(size_t index, job_worker* worker) override {
            int ow = static_cast<int>(_output->get_width());
            int oh = static_cast<int>(_output->get_height());
            int ox = static_cast<int>(index % ow);
            int oy = static_cast<int>(index / oh);
            glm::uvec2 inputCoords = output_to_input_coordinates(ox, oy);
            float fullDistance = compute_distance_of_pixel(inputCoords);
            float distance01 = inv_lerp(_minDistance, _maxDistance, fullDistance);
            auto pixel = lerp(
                std::numeric_limits<png::gray_pixel>::min(),
                std::numeric_limits<png::gray_pixel>::max(),
                1 - distance01
            );
            _output->set_pixel(ox, oy, pixel);
        }

        static UN_AGGRESSIVE_INLINE bool is_out_of_bounds(
            int nx,
            int ny,
            size_t width,
            size_t height
        ) {
            return nx < 0 || ny < 0 || nx >= width || ny >= height;
        }

        UN_AGGRESSIVE_INLINE float compute_distance_of_pixel(const glm::uvec2& inputCoords) {
            int iw = _input->get_width();
            int ih = _input->get_height();
            float outTexelX = 1.0F / static_cast<f32>(_output->get_width());
            float outTexelY = 1.0F / static_cast<f32>(_output->get_height());
            float d = _maxDistance;
            for (int dy = -iw; dy < iw; ++dy) {
                if (abs(dy) > _maxDistance) {
                    continue;
                }
                for (int dx = -iw; dx < iw; ++dx) {
                    if (abs(dx) > _maxDistance) {
                        continue;
                    }
                    int nx = inputCoords.x + dx;
                    int ny = inputCoords.y + dy;
                    if (is_out_of_bounds(nx, ny, iw, ih)) {
                        continue;
                    }
                    glm::uvec2 neighbor(nx, ny);
                    const png::rgba_pixel& pixel = _input->get_pixel(neighbor.x, neighbor.y);
                    if (pixel.alpha > 0) {
                        float candidate = distance(
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
