//
// Created by brunorbsf on 29/05/22.
//

#ifndef UNNECESSARYENGINE_EXTENSION_H
#define UNNECESSARYENGINE_EXTENSION_H

#include <memory>
#include <vector>

namespace un {
    template<typename t_target>
    class extension {
    public:
        using target_type = t_target;

        virtual void apply(target_type& target) = 0;
    };

    template<typename t_extension>
    class extensible {
    private:
        std::vector<std::shared_ptr<t_extension>> _extensions;
    public:
        using extension_type = t_extension;

        void extend_with(const std::shared_ptr<t_extension>& extension) {
            extension->apply(*static_cast<typename extension_type::target_type*>(this));
            _extensions.push_back(extension);
        }

        template<typename TOfType>
        std::shared_ptr<TOfType> find_extension() {
            for (const auto& extension : _extensions) {
                const auto& casted = std::dynamic_pointer_cast<TOfType>(extension);
                if (casted != nullptr) {
                    return casted;
                }
            }
            return nullptr;
        }
    };
}
#endif
