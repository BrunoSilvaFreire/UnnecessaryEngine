//
// Created by brunorbsf on 29/05/22.
//

#ifndef UNNECESSARYENGINE_EXTENSION_H
#define UNNECESSARYENGINE_EXTENSION_H

#include <memory>
#include <vector>

namespace un {
    template<typename TTarget>
    class Extension {
    public:
        using TargetType = TTarget;

        virtual void apply(TargetType& target) = 0;
    };

    template<typename TExtension>
    class Extensible {
    private:
        std::vector<std::shared_ptr<TExtension>> extensions;
    public:
        using ExtensionType = TExtension;

        void extendWith(const std::shared_ptr<TExtension>& extension) {
            extension->apply(*static_cast<typename ExtensionType::TargetType*>(this));
            extensions.push_back(extension);
        }

        template<typename TOfType>
        std::shared_ptr<TOfType> findExtension() {
            for (const auto& extension : extensions) {
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
