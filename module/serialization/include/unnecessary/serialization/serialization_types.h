#ifndef UNNECESSARYENGINE_SERIALIZATION_TYPES_H
#define UNNECESSARYENGINE_SERIALIZATION_TYPES_H
namespace un {
    enum SerializedType {
        eInteger8,
        eInteger16,
        eInteger32,
        eInteger64,
        eFloat,
        eDouble,
        eString,
        eComplex,
        eArray,
        eBinary
    };

    class SerializedNode {
    public:
        virtual SerializedType getType() = 0;

        template<typename TNode>
        TNode& as() {
            auto node = dynamic_cast<TNode*>(this);
            if (node == nullptr){
                throw std::runtime_error("Invalid cast.");
            }
            return *node;
        }
    };
}


#endif
