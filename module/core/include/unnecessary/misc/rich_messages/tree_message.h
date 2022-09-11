//
// Created by brunorbsf on 11/09/22.
//

#ifndef UNNECESSARYENGINE_TREE_MESSAGE_H
#define UNNECESSARYENGINE_TREE_MESSAGE_H

#include <unnecessary/misc/rich_messages/message.h>

namespace un {

    class TreeMessage final : public CompositeMessage {
    protected:
        void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const override;
    };
}
#endif