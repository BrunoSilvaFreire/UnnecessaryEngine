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
    class TreeMessage;

    class TextMessage;

    class MapMessage;

    class RichMessage;

    class IMessage {
    public:
        IMessage() = default;

        IMessage(const IMessage&) = delete;

        virtual void write(std::ostream& stream) const = 0;

        virtual bool is_node();
    };


    class CompositeMessage : public IMessage {
    protected:
        std::vector<std::unique_ptr<IMessage>> _children;

        virtual void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const { }

        template<typename TChild>
        TChild& add_child() {
            return dynamic_cast<TChild&>(*_children.emplace_back(std::make_unique<TChild>()));
        }

    public:
        bool is_node() override;

    public:
        CompositeMessage() = default;

        void write(std::ostream& stream) const override;

        std::size_t num_children() const;

        un::TextMessage& text();

        un::TreeMessage& tree();

        un::MapMessage& map();
    };

}
#endif //UNNECESSARYENGINE_MESSAGE_H
