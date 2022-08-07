//
// Created by bruno on 18/05/2022.
//

#include "enum_writer.h"

namespace un {

    bool EnumWriter::accepts(const CXXField& field, const CXXTranslationUnit& unit, float& outPriority) {
        outPriority = 1.5F;
        return field.getType().getKind() == un::CXXTypeKind::eEnum;
    }

    void EnumWriter::write_serializer(
        std::stringstream& ss,
        const CXXField& field,
        const CXXTranslationUnit& unit,
        const WriterRegistry& registry
    ) {
        std::shared_ptr<un::CXXEnum> anEnum;
        if (!unit.findSymbol(field.getType(), anEnum)) {
            return;
        }
        ss << "namespace un {" << std::endl;
        // static serialization
        ss << "namespace serialization {" << std::endl;
        ss << "template<>" << std::endl;
        ss << "inline void serialize<" << anEnum->getFullName() << ">"
           << "(const " << anEnum->getFullName() << "& value, un::Serialized& into) {" << std::endl;
        ss << "switch (value) {" << std::endl;
        for (const auto& item : anEnum->getValues()) {
            ss << "case " << anEnum->getFullName() << "::" << item.getName() << ":" << std::endl;
            ss << "into.set<std::string>(" << anEnum->getFullName() << "::" << item.getName() << ":" << std::endl;
            ss << "return;" << std::endl;
        }
        ss << "}" << std::endl;
        ss << "}" << std::endl;

    }

    void EnumWriter::write_deserializer(
        std::stringstream& ss,
        const CXXField& field,
        const CXXTranslationUnit& unit,
        const WriterRegistry& registry
    ) {

    }

    std::string EnumWriter::name() {
        return "EnumWriter";
    }
}