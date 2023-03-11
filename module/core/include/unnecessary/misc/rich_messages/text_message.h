//
// Created by brunorbsf on 11/09/22.
//

#ifndef UNNECESSARYENGINE_TEXT_MESSAGE_H
#define UNNECESSARYENGINE_TEXT_MESSAGE_H

#include <unnecessary/misc/rich_messages/message.h>

namespace un {
    class text_message final : public message {
    private:
        std::stringstream _text;

    public:
        text_message();

        void write(std::ostream& stream) const override;

        template<typename TAny>
        std::ostream& operator<<(TAny any) {
            return _text << any;
        }
    };
}
#endif
