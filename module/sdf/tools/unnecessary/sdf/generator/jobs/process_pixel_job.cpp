//
// Created by bruno on 17/01/2022.
//

#include <cstddef>
#include <unnecessary/sdf/generator/jobs/process_pixel_job.h>
#include <unnecessary/misc/math.h>

namespace un {

    ProcessPixelJob::ProcessPixelJob(
        png::image<png::rgba_pixel>* input,
        png::image<png::gray_pixel>* output,
        float minDistance,
        float maxDistance
    )
        : input(input),
          output(output),
          minDistance(minDistance),
          maxDistance(maxDistance) {}

    void ProcessPixelJob::operator()(size_t index, un::JobWorker* worker) {
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
//        LOG(INFO) << "Pixel " << index << " (" << ox << ", " << oy << ") processed @ "
//                  << (int) pixel
//                  << " (dist: " << distance01 << ", " << fullDistance << " @ hex: "
//                  << std::hex << (int) pixel
//                  << std::dec
//                  << ")";
    }

    float ProcessPixelJob::computeDistanceOfPixel(const glm::uvec2& inputCoords) {
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

    glm::uvec2 ProcessPixelJob::outputToInputCoordinates(
        size_t x,
        size_t y
    ) {
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

    bool ProcessPixelJob::isOutOfBounds(int nx, int ny, size_t width, size_t height) {
        return nx < 0 || ny < 0 || nx >= width || ny >= height;
    }

    void ProcessPixelJob::batchStarted(size_t start, size_t end) {
        LOG(INFO) << "Starting batch " << start << " -> " << end << " (" << (end - start + 1)
                  << " pixels)";
    }

    void ProcessPixelJob::batchFinished(size_t start, size_t end) {
        LOG(INFO) << "Finished batch " << start << " -> " << end << " (" << (end - start + 1)
                  << " pixels)";
    }

}