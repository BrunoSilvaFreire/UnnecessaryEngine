#include <unnecessary/sdf/generator/jobs/process_pixel_job.h>

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
          maxDistance(maxDistance) { }

}