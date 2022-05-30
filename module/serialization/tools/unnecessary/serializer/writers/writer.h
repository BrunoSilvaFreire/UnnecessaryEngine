#ifndef UNNECESSARYENGINE_WRITER_H
#define UNNECESSARYENGINE_WRITER_H

#include <sstream>
#include <vector>
#include <algorithm>
#include <memory>
#include <unnecessary/source_analysis/structures.h>

namespace un {
    class WriterRegistry;

    class SerializationWriter {
    protected:
        static bool isOptional(const un::CXXField& field);

        static void addMissingFieldException(std::stringstream& ss, const un::CXXField& field);

        static void addMissingFieldException(std::stringstream& ss, const std::string& fieldName);

        static bool trySerializePrimitive(
            std::stringstream& ss,
            const std::string& name,
            const un::CXXType& primitiveCandidate
        );

        bool tryDeserializePrimitive(
            std::stringstream& ss,
            const std::string& name,
            const CXXType& primitiveCandidate
        );

    public:
        // The following priorities are *hints*, don't feel obliged to follow them
        static constexpr float kDefaultPriority = 0.0F;
        static constexpr float kBasicPriority = 1.0F;
        static constexpr float kSpecializedKindPriority = 1.5F;

        virtual bool accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) = 0;

        virtual void write_serializer(
            std::stringstream& ss,
            const CXXField& field,
            const un::CXXTranslationUnit& unit,
            const un::WriterRegistry& registry
        ) = 0;

        virtual void write_deserializer(
            std::stringstream& ss,
            const CXXField& field,
            const un::CXXTranslationUnit& unit,
            const un::WriterRegistry& registry
        ) = 0;

        virtual std::string name() = 0;
    };

    class WriterRegistry {
    private:
        std::vector<std::shared_ptr<un::SerializationWriter>> writers;
    public:
        WriterRegistry();

        std::shared_ptr<un::SerializationWriter> getWriter(
            const un::CXXField& field,
            const un::CXXTranslationUnit& unit
        ) const;

        template<typename TWriter>
        void addWriter() {
            std::shared_ptr<TWriter> val = std::make_shared<TWriter>();
            writers.emplace_back(std::move(std::static_pointer_cast<un::SerializationWriter>(val)));
        }
    };
}
#endif //UNNECESSARYENGINE_WRITER_H