#include <utility>
#include <unnecessary/serializer/jobs/generate_serializer_job.h>
#include <unnecessary/serializer/writers/misc/enum_serialization_logic.h>

namespace un {
    void generate_serializer_job::operator()(job_worker* worker) {
        std::stringstream ss;
        ss << "#ifndef UN_SERIALIZER_GENERATED_" << _info.upper << std::endl;
        ss << "#define UN_SERIALIZER_GENERATED_" << _info.upper << std::endl;
        std::shared_ptr<cxx_composite> asComp = std::dynamic_pointer_cast<cxx_composite>(_toGenerate);
        if (asComp != nullptr) {
            generate_composite_serializer(ss, asComp);
        }
        std::shared_ptr<cxx_enum> asEnum = std::dynamic_pointer_cast<cxx_enum>(_toGenerate);
        if (asEnum != nullptr) {
            serialization::write_enum_serialization_logic_pair(ss, _info, asEnum);
        }
        ss << "#endif" << std::endl;
        _buffer->operator=(ss.str());
    }

    void
    generate_serializer_job::generate_info_comments(
        std::stringstream& ss, const std::shared_ptr<
        cxx_composite
    >& ptr
    ) {
        ss << "// Serialization info: " << std::endl;
        for (const auto& field : ptr->get_fields()) {
            auto attribute = field.find_attribute("un::serialize");
            if (attribute == nullptr) {
                continue;
            }
            const std::vector<cxx_attribute>& attributes = field.get_attributes();
            ss << "// Field: " << field.get_name() << " (" << field.get_type().get_full_name()
               << ")" << std::endl;
            if (attributes.empty()) {
                ss << "//     No attributes. " << std::endl;
            }
            else {
                ss << "//     Attributes (" << attributes.size() << "):" << std::endl;
                for (const cxx_attribute& att : attributes) {
                    ss << "//         " << att.get_name();
                    const std::set<std::string>& args = att.get_arguments();
                    if (!args.empty()) {
                        ss << ", args: " << join_strings(args.begin(), args.end());
                    }
                    ss << std::endl;
                }
            }
        }
    }

    void generate_serializer_job::generate_composite_serializer(
        std::stringstream& ss,
        const std::shared_ptr<cxx_composite>& comp
    ) {
        generate_info_comments(ss, comp);
        ss << "namespace un::serialization {" << std::endl;
        serialize_fields(comp, ss);
        ss << "}" << std::endl;
        // static serialization
    }

    generate_serializer_job::generate_serializer_job(
        std::shared_ptr<byte_buffer> buffer,
        const std::shared_ptr<cxx_declaration>& toGenerate,
        const cxx_translation_unit* translationUnit
    ) : _info(*toGenerate),
        _buffer(std::move(buffer)),
        _toGenerate(toGenerate),
        _translationUnit(translationUnit) {
    }

    void generate_serializer_job::write_field_info(
        std::stringstream& stream,
        const cxx_field& field,
        const std::shared_ptr<field_writer>& writer
    ) {
        stream << "// Access: " << to_string(field.get_access_modifier()) << std::endl;
        stream << "// Type: " << field.get_type().get_name() << std::endl;
        stream << "// Writer: " << writer->name() << std::endl;
    }

    void generate_serializer_job::serialize_fields(
        const std::shared_ptr<cxx_composite>& composite,
        std::stringstream& ss
    ) {
        std::stringstream fieldsSerialization;
        std::stringstream fieldsDeserialization;
        for (const auto& field : composite->get_fields()) {
            auto att = field.find_attribute("un::serialize");
            if (att == nullptr) {
                continue;
            }
            const std::shared_ptr<field_writer>& writer = _writerRegistry.get_writer(
                field,
                *_translationUnit
            );
            const std::string& fName = field.get_name();
            const cxx_type& fieldType = field.get_type();
            //            LOG(INFO) << "Elected " << writer->name() << " for field " << fName << " of " << info.fullName
            //                      << " with type " << fieldType.getName() << " (kind: " << un::to_string(fieldType.getKind())
            //                      << ")";

            //Serialization
            fieldsSerialization << "// --- BEGIN FIELD SERIALIZATION: " << fName << std::endl;
            write_field_info(fieldsSerialization, field, writer);
            writer->write_serializer(
                fieldsSerialization,
                field,
                *_translationUnit,
                _writerRegistry
            );
            fieldsSerialization << "// --- END FIELD SERIALIZATION: " << fName << std::endl;
            fieldsSerialization << std::endl;

            // Deserialization
            fieldsDeserialization << "// --- BEGIN FIELD DESERIALIZATION: " << fName << std::endl;
            write_field_info(fieldsDeserialization, field, writer);
            writer->write_deserializer(
                fieldsDeserialization,
                field,
                *_translationUnit,
                _writerRegistry
            );
            fieldsDeserialization << "// --- END FIELD DESERIALIZATION: " << fName << std::endl;
            fieldsDeserialization << std::endl;
        }
        ss << "template<>" << std::endl;
        ss << "inline void serialize_structure<" << _info.fullName << ">" << "(const "
           << _info.fullName
           << "& value, un::Serialized& into) {" << std::endl;
        ss << fieldsSerialization.str() << std::endl;
        ss << "}" << std::endl;

        ss << "template<>" << std::endl;
        ss << _info.fullName << " deserialize_structure<" << _info.fullName
           << ">(un::Serialized& from) {" << std::endl;
        ss << _info.fullName << " value;" << std::endl;
        ss << fieldsDeserialization.str() << std::endl;
        ss << "return value;" << std::endl;
        ss << "}" << std::endl;
    }
}
