//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_ATTACHMENT_H
#define UNNECESSARYENGINE_ATTACHMENT_H

#include <vulkan/vulkan.hpp>

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
        vk::ImageAspectFlags ownedImageFlags;
    public:
        Attachment(
            const vk::AttachmentDescription& description,
            const vk::ClearValue& clear,
            vk::ImageLayout ownedImageLayout = vk::ImageLayout::eUndefined,
            vk::ImageUsageFlags ownedImageUsage = static_cast<vk::ImageUsageFlags>(0),
            vk::ImageAspectFlags ownedImageFlags = static_cast<vk::ImageAspectFlags>(0)
        );

        const vk::AttachmentDescription& getDescription() const;

        const vk::ClearValue& getClear() const;

        vk::ImageLayout getOwnedImageLayout() const;

        vk::ImageUsageFlags getOwnedImageUsage() const;

        vk::ImageAspectFlags getOwnedImageFlags() const;

    };

}
#endif
