//
// Created by bruno on 03/05/2022.
//

#ifndef UNNECESSARYENGINE_DUMMY_H
#define UNNECESSARYENGINE_DUMMY_H
#define SERIALIZE(...)

#include <unnecessary/def.h>
#include <string>

namespace un {
    class ClassB {

        SERIALIZE()
        std::string name;

        SERIALIZE()
        std::string alias;
    };

    class MyStruct : public ClassB {
    private:

        SERIALIZE()
        std::string strValue;
    public:
        void doFoo() {

        }
    };

    class SerializedObject {
    private:
        SERIALIZE()
        int value;
        SERIALIZE()
        u32 uValue;
        SERIALIZE()
        f32 fValue;
        SERIALIZE()
        f64 dValue;
        SERIALIZE()
        double doubleValue;
        SERIALIZE()
        MyStruct structValue;
    };
}
#endif
