#include <sstream>
#include <unnecessary/serializer/generation.h>
#include <unnecessary/serializer/writers/default_writer.h>

namespace un {

    std::string getGeneratedIncludeName(const GenerationInfo& info);

    void serializeFields(
        const std::shared_ptr<un::CXXComposite>& composite, const GenerationInfo& info,
        WriterRegistry& registry, std::stringstream& ss, un::CXXTranslationUnit& unit
    );

    void generateSerializerInclude(
        const std::filesystem::path& includesOutput,
        const std::shared_ptr<un::CXXComposite>& composite,
        CXXTranslationUnit& unit,
        const GenerationInfo& info, const std::vector<std::string>& additionalIncludes
    ) {
        un::WriterRegistry registry;
        std::stringstream ss;
        ss << "#ifndef UN_SERIALIZER_GENERATED_" << info.upper << std::endl;
        ss << "#define UN_SERIALIZER_GENERATED_" << info.upper << std::endl;
        ss << "#include <unnecessary/serialization/serializer.h>" << std::endl;
//        ss << "#include <" << info.ownInclude << ">" << std::endl;
        for (const auto& item : additionalIncludes) {
            ss << "#include <" << item << ">" << std::endl;
        }
        ss << "// Serialization info: " << std::endl;
        for (const auto& field : composite->getFields()) {
            auto att = field.findAttribute("un::serialize");
            if (att == nullptr) {
                continue;
            }
            const std::vector<un::CXXAttribute>& attributes = field.getAttributes();
            ss << "// Field: " << field.getName() << std::endl;
            if (attributes.empty()) {
                ss << "// No attributes. " << std::endl;
            } else {
                ss << "// Attributes (" << attributes.size() << "):" << std::endl;
                for (const un::CXXAttribute& att : attributes) {
                    ss << "// " << att.getName();
                    const std::set<std::string>& args = att.getArguments();
                    if (!args.empty()) {
                        ss << ", args: " << un::join_strings(args.begin(), args.end());
                    }
                    ss << std::endl;
                }
            }
        }
        ss << "namespace un {" << std::endl;
        // static serialization
        ss << "namespace serialization {" << std::endl;
        serializeFields(composite, info, registry, ss, unit);
        ss << "}" << std::endl;
        // static serialization

        ss << "class " << info.name << "Serializer final : public un::Serializer<" << info.fullName
           << "> {"
           << std::endl;
        ss << "inline virtual void serialize(const " << info.fullName
           << "& value, un::Serialized& into) override {"
           << std::endl;
        ss << "un::serialization::serialize(value, into);" << std::endl;
        ss << "}" << std::endl;

        ss << "inline virtual " << info.fullName << " deserialize(un::Serialized& from) override {"
           << std::endl;
        ss << "return un::serialization::deserialize<" << info.fullName << ">(from);" << std::endl;
        ss << "}" << std::endl;

        ss << "};" << std::endl;
        ss << "}" << std::endl;

        ss << "#endif" << std::endl;
        auto includeName = getGeneratedIncludeName(info);
        std::filesystem::path includeFile = includesOutput / includeName;
        std::string p = includeFile.string();
        un::files::safe_write_file(includeFile, ss);
        LOG(INFO) << "Written " << info.fullName << " serializer include to " << includeFile;
    }

    void writeFieldInfo(
        std::stringstream& stream,
        const CXXField& field,
        const std::shared_ptr<SerializationWriter>& writer
    );

    void serializeFields(
        const std::shared_ptr<un::CXXComposite>& composite, const GenerationInfo& info,
        WriterRegistry& registry, std::stringstream& ss, un::CXXTranslationUnit& unit
    ) {
        std::stringstream fieldsSerialization;
        std::stringstream fieldsDeserialization;
        for (const auto& field : composite->getFields()) {
            auto att = field.findAttribute("un::serialize");
            if (att == nullptr) {
                continue;
            }
            const std::shared_ptr<SerializationWriter>& writer = registry.getWriter(field, unit);
            const std::string& fName = field.getName();
            const un::CXXType& fieldType = field.getType();
            LOG(INFO) << "Elected " << writer->name() << " for field " << fName << " of " << info.fullName
                      << " with type " << fieldType.getName() << " (kind: " << un::to_string(fieldType.getKind())
                      << ")";

            //Serialization
            fieldsSerialization << "// --- BEGIN FIELD SERIALIZATION: " << fName << std::endl;
            writeFieldInfo(fieldsSerialization, field, writer);
            writer->write_serializer(fieldsSerialization, field, unit, registry);
            fieldsSerialization << "// --- END FIELD SERIALIZATION: " << fName << std::endl;
            fieldsSerialization << std::endl;

            // Deserialization
            fieldsDeserialization << "// --- BEGIN FIELD DESERIALIZATION: " << fName << std::endl;
            writeFieldInfo(fieldsDeserialization, field, writer);
            writer->write_deserializer(fieldsDeserialization, field, unit, registry);
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

    void writeFieldInfo(
        std::stringstream& stream,
        const CXXField& field,
        const std::shared_ptr<SerializationWriter>& writer
    ) {
        stream << "// Access: " << to_string(field.getAccessModifier()) << std::endl;
        stream << "// Type: " << field.getType().getName() << std::endl;
        stream << "// Writer: " << writer->name() << std::endl;
    }


    bool isSerializable(const std::shared_ptr<un::CXXComposite>& composite) {
        const std::vector<CXXField>& field = composite->getFields();
        return std::any_of(
            field.begin(), field.end(), [](const un::CXXField& item) {
                return item.findAttribute("un::serialize") != nullptr;
            }
        );

    }

    std::string getGeneratedIncludeName(const GenerationInfo& info) { return info.lower + ".serializer.generated.h"; }

    std::string getGeneratedIncludeName(const std::string& typeName) {
        return un::lower(
            typeName
        ) + ".serializer.generated.h";
    }

}