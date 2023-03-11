#include <unnecessary/misc/rich_messages/message.h>
#include <unnecessary/misc/rich_messages/text_message.h>
#include <unnecessary/misc/rich_messages/tree_message.h>
#include <unnecessary/misc/rich_messages/map_message.h>

namespace un {
    void composite_message::write(std::ostream& stream) const {
        for (std::size_t i = 0; i < _children.size(); ++i) {
            const auto& child = _children[i];
            std::stringstream sub;
            sub << termcolor::colorize;
            child->write(sub);
            auto lines = split_string(sub.str(), new_line());
            for (std::size_t j = 0; j < lines.size(); ++j) {
                const auto& line = lines[j];
                prefix(stream, i, j, child);
                stream << line << std::endl;
            }
        }
    }

    std::size_t composite_message::num_children() const {
        return _children.size();
    }

    text_message& composite_message::text() {
        return add_child<text_message>();
    }

    tree_message& composite_message::tree() {
        return add_child<tree_message>();
    }

    map_message& composite_message::map() {
        return add_child<map_message>();
    }

    bool composite_message::is_node() {
        return true;
    }

    bool message::is_node() {
        return false;
    }
}
