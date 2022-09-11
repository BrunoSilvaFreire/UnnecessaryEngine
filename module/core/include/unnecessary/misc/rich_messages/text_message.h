//
// Created by brunorbsf on 11/09/22.
//

#ifndef UNNECESSARYENGINE_TEXT_MESSAGE_H
#define UNNECESSARYENGINE_TEXT_MESSAGE_H

#include <unnecessary/misc/rich_messages/message.h>

namespace un {

    class TextMessage final : public IMessage {
    private:
        std::stringstream _text;
    public:
        TextMessage();

        void write(std::ostream& stream) const override;

        template<typename TAny>
        std::ostream& operator<<(TAny any) {
            return TextMessage::_text << any;
        }
    };
}
#endif