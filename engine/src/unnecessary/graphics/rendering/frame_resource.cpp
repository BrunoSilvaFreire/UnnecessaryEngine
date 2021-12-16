//
// Created by brunorbsf on 02/10/2021.
//

#include "unnecessary/graphics/rendering/rendering_pipeline.h"
#include "unnecessary/misc/aggregator.h"
#include "unnecessary/graphics/renderer.h"
#include "unnecessary/logging.h"
#include <utility>
#include "unnecessary/graphics/rendering/frame_graph.h"
#include "unnecessary/graphics/rendering/frame_resource.h"

namespace un {
    FrameResource::FrameResource(std::string name) : name(std::move(name)) {}
}