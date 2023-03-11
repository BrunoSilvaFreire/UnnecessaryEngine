//
// Created by brunorbsf on 11/09/22.
//

#ifndef UNNECESSARYENGINE_MESSAGE_H
#define UNNECESSARYENGINE_MESSAGE_H

#include <ostream>
#include <vector>
#include <memory>
#include <unnecessary/def.h>
#include <unnecessary/logging.h>
#include <unnecessary/platform.h>

namespace un {
    class tree_message;

    class text_message;

    class map_message;

    class rich_message;

    class message {
    public:
        message() = default;

        message(const message&) = delete;

        virtual void write(std::ostream& stream) const = 0;

        virtual bool is_node();
    };

    class composite_message : public message {
    protected:
        std::vector<std::unique_ptr<message>> _children;

        virtual void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<message>& msg
        ) const {
        }

        template<typename TChild>
        TChild& add_child() {
            return dynamic_cast<TChild&>(*_children.emplace_back(std::make_unique<TChild>()));
        }

    public:
        bool is_node() override;

    public:
        composite_message() = default;

        void write(std::ostream& stream) const override;

        std::size_t num_children() const;

        text_message& text();

        tree_message& tree();

        map_message& map();
    };
}
#endif //UNNECESSARYENGINE_MESSAGE_H
