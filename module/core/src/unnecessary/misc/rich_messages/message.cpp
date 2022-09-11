#include <unnecessary/misc/rich_messages/message.h>
#include <unnecessary/misc/rich_messages/text_message.h>
#include <unnecessary/misc/rich_messages/tree_message.h>
#include <unnecessary/misc/rich_messages/map_message.h>

namespace un {

    void CompositeMessage::write(std::ostream& stream) const {
        for (std::size_t i = 0; i < _children.size(); ++i) {
            const auto& child = _children[i];
            std::stringstream sub;
            sub << termcolor::colorize;
            child->write(sub);
            auto lines = un::split_string(sub.str(), un::new_line());
            for (std::size_t j = 0; j < lines.size(); ++j) {
                const auto& line = lines[j];
                prefix(stream, i, j, child);
                stream << line << std::endl;
            }
        }
    }

    std::size_t CompositeMessage::num_children() const {
        return _children.size();
    }

    un::TextMessage& CompositeMessage::text() {
        return add_child<un::TextMessage>();
    }

    un::TreeMessage& CompositeMessage::tree() {
        return add_child<un::TreeMessage>();
    }

    un::MapMessage& CompositeMessage::map() {
        return add_child<un::MapMessage>();
    }

    bool CompositeMessage::is_node() {
        return true;
    }


    bool IMessage::is_node() {
        return false;
    }
}