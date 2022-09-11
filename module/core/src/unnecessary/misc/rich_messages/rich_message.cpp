#include <unnecessary/misc/rich_messages/rich_message.h>

namespace un {

    RichMessage::RichMessage(
        std::string indent,
        std::string header
    ) : _indent(std::move(indent)),
        _header(std::move(header)) { }


    RichMessage::~RichMessage() {
        if (_header.empty()) {
            write(LOG(_level));
        } else {
            write(un::Log(_level, _header).stream());
        }
    }

    void RichMessage::prefix(
        std::ostream& stream,
        std::size_t i,
        std::size_t lineNbr,
        const std::unique_ptr<IMessage>& msg
    ) const {
        stream << _indent;
    }

    void RichMessage::setLevel(const std::string& level) {
        _level = level;
    }

    void RichMessage::setHeader(const std::string& header) {
        _header = header;
    }
}