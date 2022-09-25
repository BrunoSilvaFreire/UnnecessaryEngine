//
// Created by brunorbsf on 25/09/22.
//

#ifndef UNNECESSARYENGINE_STRUCTURE_SERIALIZATION_LOGIC_H
#define UNNECESSARYENGINE_STRUCTURE_SERIALIZATION_LOGIC_H

#include <unnecessary/serializer/generation.h>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/serializer/writers/fields/field_writer.h>

namespace un::serialization {
    void write_structure_serialization_info(std::stringstream& ss, const std::shared_ptr<CXXComposite>& ptr);

    void write_structure_fields_serialization_logic(
        un::WriterRegistry& writerRegistry,
        un::ptr<un::CXXTranslationUnit> translationUnit,
        const std::shared_ptr<un::CXXComposite>& composite,
        std::stringstream& ss
    ) {
        std::stringstream fieldsSerialization;
        std::stringstream fieldsDeserialization;
        for (const auto& field : composite->getFields()) {
            auto att = field.findAttribute("un::serialize");
            if (att == nullptr) {
                continue;
            }
            const std::shared_ptr<FieldWriter>& writer = writerRegistry.getWriter(field, *translationUnit);
            const std::string& fName = field.getName();
            const un::CXXType& fieldType = field.getType();
//            LOG(INFO) << "Elected " << writer->name() << " for field " << fName << " of " << info.fullName
//                      << " with type " << fieldType.getName() << " (kind: " << un::to_string(fieldType.getKind())
//                      << ")";

            //Serialization
            fieldsSerialization << "// --- BEGIN FIELD SERIALIZATION: " << fName << std::endl;
            writeFieldInfo(fieldsSerialization, field, writer);
            writer->write_serializer(fieldsSerialization, field, *translationUnit, writerRegistry);
            fieldsSerialization << "// --- END FIELD SERIALIZATION: " << fName << std::endl;
            fieldsSerialization << std::endl;

            // Deserialization
            fieldsDeserialization << "// --- BEGIN FIELD DESERIALIZATION: " << fName << std::endl;
            writeFieldInfo(fieldsDeserialization, field, writer);
            writer->write_deserializer(fieldsDeserialization, field, *translationUnit, writerRegistry);
            fieldsDeserialization << "// --- END FIELD DESERIALIZATION: " << fName << std::endl;
            fieldsDeserialization << std::endl;
        }
        ss << "template<>" << std::endl;
        ss << "inline void serialize_structure<" << info.fullName << ">" << "(const " << info.fullName
           << "& value, un::Serialized& into) {" << std::endl;
        ss << fieldsSerialization.str() << std::endl;
        ss << "}" << std::endl;

        ss << "template<>" << std::endl;
        ss << info.fullName << " deserialize_structure<" << info.fullName << ">(un::Serialized& from) {" << std::endl;
        ss << info.fullName << " value;" << std::endl;
        ss << fieldsDeserialization.str() << std::endl;
        ss << "return value;" << std::endl;
        ss << "}" << std::endl;
    }

    void write_structure_serialization_logic(
        std::stringstream& ss,
        const un::GenerationInfo& info,
        const std::shared_ptr<un::CXXComposite>& anComposite
    ) {
        write_structure_serialization_info(ss, anComposite);
        ss << "namespace un::serialization {" << std::endl;
        serializeFields(anComposite, ss);
        ss << "}" << std::endl;
    }

}


#endif //UNNECESSARYENGINE_STRUCTURE_SERIALIZATION_LOGIC_H
