//
// Created by brunorbsf on 06/04/22.
//

#ifndef UNNECESSARYENGINE_SERIALIZED_STRING_H
#define UNNECESSARYENGINE_SERIALIZED_STRING_H

#include "serialized_primitive.h"

namespace un {
    class SerializedString : public SerializedPrimitive<std::string> {
    public:
        SerializedString(const std::string& value);

        un::SerializedType getType() override;
    };
};
#endif
