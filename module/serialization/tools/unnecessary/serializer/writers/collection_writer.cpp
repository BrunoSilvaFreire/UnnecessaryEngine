#include <unnecessary/serializer/writers/collection_writer.h>

namespace un {

    bool CollectionWriter::accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) {
        return field.getType().getName() == "std::vector";
    }

    void
    CollectionWriter::write_serializer(std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
                                       const un::WriterRegistry& registry) {

    }

    void CollectionWriter::write_deserializer(std::stringstream& ss, const CXXField& field,
                                              const un::CXXTranslationUnit& unit,
                                              const un::WriterRegistry& registry) {

    }

    std::string CollectionWriter::name() {
        return std::string();
    }
}
