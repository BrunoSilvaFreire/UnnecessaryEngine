#include <unnecessary/misc/rich_messages/message.h>
#include <unnecessary/misc/rich_messages/text_message.h>
#include <unnecessary/misc/rich_messages/tree_message.h>
#include <unnecessary/misc/rich_messages/map_message.h>

namespace un {
    void map_message::write(std::ostream& stream) const {
        std::size_t i = 0;
        for (const auto& [name, child] : _named) {
            std::stringstream sub;
            sub << termcolor::colorize;
            child->write(sub);
            auto lines = split_string(sub.str(), new_line());
            auto isNode = child->is_node();
            for (std::size_t j = 0; j < lines.size(); ++j) {
                const auto& line = lines[j];
                prefix(stream, name, i, j, child);
                if (!isNode) {
                    stream << YELLOW(line) << std::endl;
                }
                else {
                    stream << line << std::endl;
                }
            }
            i++;
        }
    }

    void map_message::prefix(
        std::ostream& stream,
        const std::string& name,
        std::size_t i,
        std::size_t lineNbr,
        const std::unique_ptr<message>& msg
    ) const {
        bool first = i == 0;
        std::size_t indentLength = name.size() + 3; // + 3 for suffix ": ┬"
        std::string rawIndent(indentLength, ' ');
        bool last = i == _named.size() - 1;
        bool isMidsection = lineNbr != 0;
        if (last && isMidsection) {
            stream << ' ' << rawIndent;
            return;
        }
        if (isMidsection) {
            stream << "│" << rawIndent;
            return;
        }
        if (i == 0) {
            if (last) {
                stream << "─";
            }
            else {
                stream << "┬";
            }
        }
        else if (last) {
            stream << "└";
        }
        else {
            stream << "├";
        }
        stream << ' ' << PURPLE(name) << ": ";
    }

    text_message& map_message::text(const std::string& name) {
        return add_child<text_message>(name);
    }

    tree_message& map_message::tree(const std::string& name) {
        return add_child<tree_message>(name);
    }

    bool map_message::is_node() {
        return true;
    }
}
