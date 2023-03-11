#include <unnecessary/sdf/generator/jobs/process_pixel_job.h>

namespace un {
    process_pixel_job::process_pixel_job(
        png::image<png::rgba_pixel>* input,
        png::image<png::gray_pixel>* output,
        float minDistance,
        float maxDistance
    )
        : _input(input),
          _output(output),
          _minDistance(minDistance),
          maxDistance(maxDistance) {
    }
}
