
#include "unnecessary/misc/aggregator.h"
#include <utility>
#include <unnecessary/graphics/rendering/frame_graph.h>
#include <unnecessary/graphics/rendering/attachment.h>

namespace un {

    Attachment::Attachment(
        const vk::AttachmentDescription& description,
        const vk::ClearValue& clear,
        vk::ImageLayout ownedImageLayout,
        vk::ImageUsageFlags ownedImageUsage,
        vk::ImageAspectFlags ownedImageFlags
    ) : description(description),
        clear(clear),
        ownedImageLayout(ownedImageLayout),
        ownedImageUsage(ownedImageUsage),
        ownedImageFlags(ownedImageFlags) {

    }

    const vk::AttachmentDescription& Attachment::getDescription() const {
        return description;
    }

    const vk::ClearValue& Attachment::getClear() const {
        return clear;
    }

    vk::ImageLayout Attachment::getOwnedImageLayout() const {
        return ownedImageLayout;
    }

    vk::ImageUsageFlags Attachment::getOwnedImageUsage() const {
        return ownedImageUsage;
    }

    vk::ImageAspectFlags Attachment::getOwnedImageFlags() const {
        return ownedImageFlags;
    }
};