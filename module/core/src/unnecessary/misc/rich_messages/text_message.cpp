#include <unnecessary/misc/rich_messages/text_message.h>

namespace un {
    void TextMessage::write(std::ostream& stream) const {
        stream << _text.str();
    }

    TextMessage::TextMessage() : _text() {
        _text << termcolor::colorize;
    }
}