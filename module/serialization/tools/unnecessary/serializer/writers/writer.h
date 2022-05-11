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

    public:
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
