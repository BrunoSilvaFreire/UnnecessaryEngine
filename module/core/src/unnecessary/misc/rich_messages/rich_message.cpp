#include <unnecessary/misc/rich_messages/rich_message.h>

namespace un {
    rich_message::rich_message(
        std::string indent,
        std::string header
    ) : _indent(std::move(indent)),
        _header(std::move(header)) {
    }

    rich_message::~rich_message() {
        if (_header.empty()) {
            write(LOG(_level));
        }
        else {
            write(log(_level, _header).stream());
        }
    }

    void rich_message::prefix(
        std::ostream& stream,
        std::size_t i,
        std::size_t lineNbr,
        const std::unique_ptr<message>& msg
    ) const {
        stream << _indent;
    }

    void rich_message::set_level(const std::string& level) {
        _level = level;
    }

    void rich_message::set_header(const std::string& header) {
        _header = header;
    }
}
