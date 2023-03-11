//
// Created by brunorbsf on 06/04/22.
//

#ifndef UNNECESSARYENGINE_SERIALIZED_STRING_H
#define UNNECESSARYENGINE_SERIALIZED_STRING_H

#include "serialized_primitive.h"

namespace un {
    class serialized_string : public serialized_primitive<std::string> {
    public:
        serialized_string(const std::string& value);

        serialized_type get_type() override;
    };
};
#endif
