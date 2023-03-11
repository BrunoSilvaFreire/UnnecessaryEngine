//
// Created by bruno on 20/01/2022.
//

#ifndef UNNECESSARYENGINE_ATTACHMENT_H
#define UNNECESSARYENGINE_ATTACHMENT_H

#include <vulkan/vulkan.hpp>
#include <string>

namespace un {
    class attachment {
    private:
        std::string _name;
        vk::AttachmentDescription _description;
        vk::ClearValue _clear;
        /**
         * The layout inUse to create the image for this attachment.
         * In case it's not borrowed.
         * @see un::FrameGraph
         */
        vk::ImageLayout _ownedImageLayout;
        vk::ImageUsageFlags _ownedImageUsage;
        vk::ImageAspectFlags _ownedImageFlags;

    public:
        attachment(
            const vk::AttachmentDescription& description,
            const vk::ClearValue& clear,
            vk::ImageLayout ownedImageLayout = vk::ImageLayout::eUndefined,
            vk::ImageUsageFlags ownedImageUsage = static_cast<vk::ImageUsageFlags>(0),
            vk::ImageAspectFlags ownedImageFlags = static_cast<vk::ImageAspectFlags>(0)
        );

        const std::string& get_name() const;

        void set_name(const std::string& name);

        const vk::AttachmentDescription& get_description() const;

        const vk::ClearValue& get_clear() const;

        vk::ImageLayout get_owned_image_layout() const;

        vk::ImageUsageFlags get_owned_image_usage() const;

        vk::ImageAspectFlags get_owned_image_flags() const;
    };
}
#endif
