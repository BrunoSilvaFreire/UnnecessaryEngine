#include <unnecessary/misc/rich_messages/tree_message.h>

namespace un {
    void tree_message::prefix(
        std::ostream& stream,
        std::size_t i,
        std::size_t lineNbr,
        const std::unique_ptr<message>& msg
    ) const {
        bool last = i == _children.size() - 1;
        bool isMidsection = lineNbr != 0;
        if (last && isMidsection) {
            stream << "    ";
            return;
        }
        if (isMidsection) {
            stream << "│   ";
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
        stream << '[' << GREEN(i) << ']';
        if (!msg->is_node()) {
            stream << ' ';
        }
    }
}
