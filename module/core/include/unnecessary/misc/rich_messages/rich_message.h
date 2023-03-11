//
// Created by brunorbsf on 11/09/22.
//

#ifndef UNNECESSARYENGINE_RICH_MESSAGE_H
#define UNNECESSARYENGINE_RICH_MESSAGE_H

#include <unnecessary/misc/rich_messages/message.h>

namespace un {
    class rich_message : public composite_message {
    private:
        std::string _indent;
        std::string _header;
        std::string _level = INFO;

    public:
        explicit rich_message(std::string indent, std::string header);

        ~rich_message();

        void set_level(const std::string& level);

        void set_header(const std::string& header);

    protected:
        void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<message>& msg
        ) const override;
    };

    UN_AGGRESSIVE_INLINE rich_message message() {
        return rich_message("", "");
    }

    UN_AGGRESSIVE_INLINE rich_message

    message(const std::string& header) {
        return rich_message("", header);
    }
}

#endif //UNNECESSARYENGINE_RICH_MESSAGE_H
