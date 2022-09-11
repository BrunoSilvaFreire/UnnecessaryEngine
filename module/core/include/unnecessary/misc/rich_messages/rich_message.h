//
// Created by brunorbsf on 11/09/22.
//

#ifndef UNNECESSARYENGINE_RICH_MESSAGE_H
#define UNNECESSARYENGINE_RICH_MESSAGE_H

#include <unnecessary/misc/rich_messages/message.h>

namespace un {

    class RichMessage : public CompositeMessage {
    private:
        std::string _indent;
        std::string _header;
        std::string _level = INFO;
    public:
        explicit RichMessage(std::string indent, std::string header);

        ~RichMessage();

        void setLevel(const std::string& level);

        void setHeader(const std::string& header);

    protected:
        void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const override;
    };

    UN_AGGRESSIVE_INLINE un::RichMessage message() {
        return un::RichMessage("", "");
    }

    UN_AGGRESSIVE_INLINE un::RichMessage

    message(const std::string& header) {
        return un::RichMessage("", header);
    }
}

#endif //UNNECESSARYENGINE_RICH_MESSAGE_H
