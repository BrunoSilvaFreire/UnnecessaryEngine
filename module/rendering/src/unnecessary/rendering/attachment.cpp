#include <utility>
#include <unnecessary/rendering/attachment.h>

namespace un {
    attachment::attachment(
        const vk::AttachmentDescription& description,
        const vk::ClearValue& clear,
        vk::ImageLayout ownedImageLayout,
        vk::ImageUsageFlags ownedImageUsage,
        vk::ImageAspectFlags ownedImageFlags
    ) : _description(description),
        _clear(clear),
        _ownedImageLayout(ownedImageLayout),
        _ownedImageUsage(ownedImageUsage),
        _ownedImageFlags(ownedImageFlags) {
    }

    const vk::AttachmentDescription& attachment::get_description() const {
        return _description;
    }

    const vk::ClearValue& attachment::get_clear() const {
        return _clear;
    }

    vk::ImageLayout attachment::get_owned_image_layout() const {
        return _ownedImageLayout;
    }

    vk::ImageUsageFlags attachment::get_owned_image_usage() const {
        return _ownedImageUsage;
    }

    vk::ImageAspectFlags attachment::get_owned_image_flags() const {
        return _ownedImageFlags;
    }

    const std::string& attachment::get_name() const {
        return _name;
    }

    void attachment::set_name(const std::string& name) {
        _name = name;
    }
};
