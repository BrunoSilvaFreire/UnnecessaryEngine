#ifndef UNNECESSARYENGINE_SERIALIZED_PRIMITIVE_H
#define UNNECESSARYENGINE_SERIALIZED_PRIMITIVE_H

#include "unnecessary/def.h"
#include "serialization_types.h"

namespace un {
    template<typename TValue>
    class SerializedPrimitive : public SerializedNode {
    private:
        TValue value;
    public:
        explicit SerializedPrimitive(TValue value) : value(value) { }

        explicit operator TValue() const {
            return value;
        }
    };

    class SerializedByte : public SerializedPrimitive<u8> {
    public:
        SerializedByte(u8 value);

        un::SerializedType getType() override;
    };

    class SerializedShort : public SerializedPrimitive<u16> {
    public:
        SerializedShort(u16 value);

        un::SerializedType getType() override;
    };

    class SerializedInteger : public SerializedPrimitive<u32> {
    public:
        SerializedInteger(u32 value);

        un::SerializedType getType() override;
    };

    class SerializedLong : public SerializedPrimitive<u64> {
    public:
        SerializedLong(u64 value);

        un::SerializedType getType() override;
    };

    class SerializedFloat : public SerializedPrimitive<float> {
    public:
        SerializedFloat(float value);

        SerializedType getType() override;
    };

    class SerializedDouble : public SerializedPrimitive<double> {
    public:
        SerializedDouble(double value);

    private:
        SerializedType getType() override;
    };
}
#endif
