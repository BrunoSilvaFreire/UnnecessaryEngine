//
// Created by brunorbsf on 02/10/2021.
//

#ifndef UNNECESSARYENGINE_ATTACHMENT_H
#define UNNECESSARYENGINE_ATTACHMENT_H
namespace un {

    class Attachment {
    private:
        vk::AttachmentDescription description;
        vk::ClearValue clear;
        /**
         * The layout used to create the image for this attachment.
         * In case it's not borrowed.
         * @see un::FrameGraph
         */
        vk::ImageLayout ownedImageLayout;
        vk::ImageUsageFlags ownedImageUsage;
    public:
        Attachment(
            const vk::AttachmentDescription& description,
            const vk::ClearValue& clear,
            vk::ImageLayout ownedImageLayout = vk::ImageLayout::eUndefined,
            vk::ImageUsageFlags ownedImageUsage = static_cast<vk::ImageUsageFlags>(0)
        );

        const vk::AttachmentDescription& getDescription() const;

        const vk::ClearValue& getClear() const;

        vk::ImageLayout getOwnedImageLayout() const;

        vk::ImageUsageFlags getOwnedImageUsage() const;
    };

}

#include <set>
#include <string>
#include "unnecessary/def.h"
#include "unnecessary/graphics/buffers/command_buffer.h"
#include "unnecessary/logging.h"
#include "grapphs/adjacency_list.h"

#endif //UNNECESSARYENGINE_ATTACHMENT_H
