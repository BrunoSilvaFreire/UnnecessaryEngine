//
// Created by bruno on 26/05/2022.
//

#ifndef UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H
#define UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H

#include <unnecessary/memory/membuf.h>
#include <unnecessary/jobs/simple_jobs.h>
#include <unnecessary/source_analysis/structures.h>
#include <unnecessary/serializer/writers/writer.h>
#include <unnecessary/serializer/generation.h>

namespace un {
    class GenerateSerializerJob : public un::SimpleJob {
    private:
        un::GenerationInfo info;
        std::shared_ptr<un::Buffer> buffer;
        std::shared_ptr<un::CXXDeclaration> toGenerate;
        const un::CXXTranslationUnit* translationUnit;
        un::WriterRegistry writerRegistry;

        void generateInfoComments(std::stringstream& ss, const std::shared_ptr<CXXComposite>& ptr);

        void writeFieldInfo(
            std::stringstream& stream,
            const CXXField& field,
            const std::shared_ptr<SerializationWriter>& writer
        );

        void serializeFields(
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
                const std::shared_ptr<SerializationWriter>& writer = writerRegistry.getWriter(field, *translationUnit);
                const std::string& fName = field.getName();
                const un::CXXType& fieldType = field.getType();
                LOG(INFO) << "Elected " << writer->name() << " for field " << fName << " of " << info.fullName
                          << " with type " << fieldType.getName() << " (kind: " << un::to_string(fieldType.getKind())
                          << ")";

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
            ss << "inline void serialize<" << info.fullName << ">" << "(const " << info.fullName
               << "& value, un::Serialized& into) {" << std::endl;
            ss << fieldsSerialization.str() << std::endl;
            ss << "}" << std::endl;

            ss << "template<>" << std::endl;
            ss << info.fullName << " deserialize<" << info.fullName << ">(un::Serialized& from) {" << std::endl;
            ss << info.fullName << " value;" << std::endl;
            ss << fieldsDeserialization.str() << std::endl;
            ss << "return value;" << std::endl;
            ss << "}" << std::endl;
        }

        void generateCompositeSerializer(
            std::stringstream& ss,
            const std::shared_ptr<CXXComposite>& comp
        );

    public:

        GenerateSerializerJob(
            std::shared_ptr<un::Buffer> buffer,
            const std::shared_ptr<un::CXXDeclaration>& toGenerate,
            const un::CXXTranslationUnit* translationUnit
        );

        void operator()(WorkerType* worker) override;
    };


};


#endif //UNNECESSARYENGINE_GENERATE_SERIALIZER_JOB_H
