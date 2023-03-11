#include <unnecessary/misc/rich_messages/text_message.h>

namespace un {
    void text_message::write(std::ostream& stream) const {
        stream << _text.str();
    }

    text_message::text_message() : _text() {
        _text << termcolor::colorize;
    }
}
