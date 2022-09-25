#include <unnecessary/serializer/jobs/generate_serializer_job.h>

#include <utility>

namespace un {
    void GenerateSerializerJob::operator()(un::JobWorker* worker) {
        std::stringstream ss;
        ss << "#ifndef UN_SERIALIZER_GENERATED_" << info.upper << std::endl;
        ss << "#define UN_SERIALIZER_GENERATED_" << info.upper << std::endl;
        std::shared_ptr<CXXComposite> asComp = std::dynamic_pointer_cast<un::CXXComposite>(toGenerate);
        if (asComp != nullptr) {
            generateCompositeSerializer(ss, asComp);
        }
        std::shared_ptr<CXXEnum> asEnum = std::dynamic_pointer_cast<un::CXXEnum>(toGenerate);
        if (asEnum != nullptr) {
            generateEnumSerializer(ss, asEnum);
        }
        ss << "#endif" << std::endl;
        buffer->operator=(ss.str());
    }

    void GenerateSerializerJob::generateInfoComments(std::stringstream& ss, const std::shared_ptr<CXXComposite>& ptr) {
        ss << "// Serialization info: " << std::endl;
        for (const auto& field : ptr->getFields()) {
            auto attribute = field.findAttribute("un::serialize");
            if (attribute == nullptr) {
                continue;
            }
            const std::vector<un::CXXAttribute>& attributes = field.getAttributes();
            ss << "// Field: " << field.getName() << " (" << field.getType().getFullName() << ")" << std::endl;
            if (attributes.empty()) {
                ss << "//     No attributes. " << std::endl;
            } else {
                ss << "//     Attributes (" << attributes.size() << "):" << std::endl;
                for (const un::CXXAttribute& att : attributes) {
                    ss << "//         " << att.getName();
                    const std::set<std::string>& args = att.getArguments();
                    if (!args.empty()) {
                        ss << ", args: " << un::join_strings(args.begin(), args.end());
                    }
                    ss << std::endl;
                }
            }
        }
    }

    void GenerateSerializerJob::generateCompositeSerializer(
        std::stringstream& ss,
        const std::shared_ptr<CXXComposite>& comp
    ) {
        generateInfoComments(ss, comp);
        ss << "namespace un::serialization {" << std::endl;
        serializeFields(comp, ss);
        ss << "}" << std::endl;
        // static serialization
    }

    GenerateSerializerJob::GenerateSerializerJob(
        std::shared_ptr<un::Buffer> buffer,
        const std::shared_ptr<un::CXXDeclaration>& toGenerate,
        const un::CXXTranslationUnit* translationUnit
    ) : buffer(std::move(buffer)),
        toGenerate(toGenerate),
        translationUnit(translationUnit),
        info(*toGenerate) {

    }

    void GenerateSerializerJob::writeFieldInfo(
        std::stringstream& stream,
        const un::CXXField& field,
        const std::shared_ptr<SerializationWriter>& writer
    ) {
        stream << "// Access: " << to_string(field.getAccessModifier()) << std::endl;
        stream << "// Type: " << field.getType().getName() << std::endl;
        stream << "// Writer: " << writer->name() << std::endl;
    }

    void GenerateSerializerJob::generateEnumSerializer(std::stringstream& ss, const std::shared_ptr<CXXEnum>& anEnum) {
        std::stringstream enumSerialization;
        std::stringstream enumDeserialization;
        ss << "namespace un::serialization {" << std::endl;

        enumSerialization << "// BEGIN ENUM SERIALIZATION " << std::endl;
        enumSerialization << "template<>" << std::endl;
        enumSerialization << "void serialize_inline<" << info.fullName << ">(const std::string& key, const "
                          << info.fullName
                          << "& value, un::Serialized& into) {"
                          << std::endl;

        enumSerialization << "switch (value) {" << std::endl;
        const std::vector<CXXEnumValue>& enumValues = anEnum->getValues();
        for (const auto& item : enumValues) {
            enumSerialization << "case " << info.fullName << "::" << item.getFullName() << ":" << std::endl;
            enumSerialization << "into.set<std::string>(key, \"" << item.getFullName() << "\");" << std::endl;
            enumSerialization << "break;" << std::endl;
        }
        enumSerialization << "}" << std::endl;
        enumSerialization << "}" << std::endl;
        enumSerialization << "// END ENUM SERIALIZATION " << std::endl;


        enumDeserialization << "// BEGIN ENUM DESERIALIZATION " << std::endl;
        enumDeserialization << "template<>" << std::endl;
        enumDeserialization << info.fullName << " deserialize_inline<" << info.fullName
                            << ">(const std::string& key, un::Serialized& from) {"
                            << std::endl;

        enumDeserialization << info.fullName << " value;" << std::endl;
        enumDeserialization << "static std::unordered_map<std::string, " << info.fullName
                            << "> kSerializationLookUpTable = {" << std::endl;
        std::size_t last = enumValues.size();
        for (std::size_t i = 0; i < last; ++i) {
            const auto& enumValue = enumValues[i];
            enumDeserialization << "std::make_pair<std::string, " << info.fullName << ">(\"" << enumValue.getFullName()
                                << "\", " << info.fullName << "::" << enumValue.getFullName() << ")";
            if (i != last - 1) {
                enumDeserialization << ",";
            }
            enumDeserialization << std::endl;

        }
        enumDeserialization << "};" << std::endl;
        enumDeserialization << "return value;" << std::endl;
        enumDeserialization << "}" << std::endl;
        enumDeserialization << "// END ENUM DESERIALIZATION " << std::endl;

        ss << enumSerialization.str();
        ss << std::endl;
        ss << enumDeserialization.str();
        ss << std::endl;
        ss << "} " << std::endl;

    }

    void GenerateSerializerJob::addStaticRediction(std::stringstream& ss) const {
        ss << "class " << info.name << "Serializer final : public un::Serializer<" << info.fullName
           << "> {"
           << std::endl;


        // Serialize method
        ss << "inline virtual void serialize(const " << info.fullName
           << "& value, un::Serialized& into) override {"
           << std::endl;
        ss << "un::serialization::serialize(value, into);" << std::endl;
        ss << "}" << std::endl;

        // Deserialize method
        ss << "inline virtual " << info.fullName << " deserialize(un::Serialized& from) override {"
           << std::endl;
        ss << "return un::serialization::deserialize_structure<" << info.fullName << ">(from);" << std::endl;
        ss << "}" << std::endl;

        ss << "};" << std::endl;
    }

    void
    GenerateSerializerJob::serializeFields(const std::shared_ptr<un::CXXComposite>& composite, std::stringstream& ss) {
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

}
