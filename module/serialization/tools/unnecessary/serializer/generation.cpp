#include "generation.h"

namespace un {

    void serializeFields(const std::shared_ptr<un::CXXComposite>& composite, std::stringstream& serializerSource) {
        for (const auto& field : composite->getFields()) {
            auto att = field.findAttribute("un::serialize");
            if (att == nullptr) {
                continue;
            }
            const un::CXXType& type = field.getType();
            switch (type.getInnerType()) {
                case un::eComplex:
                    serializerSource << "into.set<" << type.getName() << ">(\"" << field.getName()
                                     << "\", un::serialization::serialize(value."
                                     << field.getName() << "));" << std::endl;
                    break;
                default: {
                    serializerSource << "into.set<" << type.getName() << ">(\"" << field.getName() << "\", value."
                                     << field.getName() << ");" << std::endl;
                }
                    break;
            }
        }
    }

    std::string getGeneratedIncludeName(const GenerationInfo& info);

    void generateSerializerInclude(const std::filesystem::path& includesOutput,
                                   const std::shared_ptr<un::CXXComposite>& composite, const GenerationInfo& info,
                                   const std::vector<std::string>& additionalIncludes) {
        std::stringstream serializerSource;
        serializerSource << "#ifndef UN_SERIALIZER_GENERATED_" << info.upper << std::endl;
        serializerSource << "#define UN_SERIALIZER_GENERATED_" << info.upper << std::endl;
        serializerSource << "#include <unnecessary/serialization/serializer.h>" << std::endl;
        serializerSource << "#include <" << info.ownInclude << ">" << std::endl;
        for (const auto& item : additionalIncludes) {
            serializerSource << "#include <" << item << ">" << std::endl;
        }
        serializerSource << "namespace un {" << std::endl;
        // static serialization
        serializerSource << "namespace serialization {" << std::endl;

        serializerSource << "template<>" << std::endl;
        serializerSource << "inline void serialize<" << info.fullName << ">"
                         << "(const " << info.fullName << "& value, un::Serialized& into) {"
                         << std::endl;
        serializeFields(composite, serializerSource);
        serializerSource << "}" << std::endl;

        serializerSource << "template<>" << std::endl;
        serializerSource << info.fullName << " deserialize<" << info.fullName << ">(un::Serialized& from) {"
                         << std::endl;
        serializerSource << info.fullName << " value{};" << std::endl;
        for (const auto& field : composite->getFields()) {
            auto att = field.findAttribute("un::serialize");
            if (att == nullptr) {
                continue;
            }
            bool optional = att->hasArgument("optional");
            const un::CXXType& type = field.getType();
            const std::string& typeName = type.getName();
            const std::string& fName = field.getName();
            switch (type.getInnerType()) {
                case un::CXXTypeKind::eComplex:
                    serializerSource << "un::Serialized serialized_" << fName << ";" << std::endl;
                    if (optional) {
                        serializerSource << "if (from.try_get(\"" << fName << "\", serialized_"
                                         << fName << ")) {" << std::endl;
                        serializerSource << "value." << fName << " = un::serialization::deserialize<"
                                         << typeName << ">(serialized_" << fName << ");" << std::endl;
                        serializerSource << "}" << std::endl;

                    } else {
                        serializerSource << "if (!from.try_get(\"" << fName << "\", serialized_"
                                         << fName << ")) {" << std::endl;
                        serializerSource << "throw std::runtime_error(\"Unable to read field " << fName << "\");"
                                         << std::endl;
                        serializerSource << "}" << std::endl;
                        serializerSource << "value." << fName << " = un::serialization::deserialize<"
                                         << typeName << ">(serialized_" << fName << ");" << std::endl;
                    }

                    break;
                default: {
                    if (optional) {
                        serializerSource << "from.try_get<" << typeName << ">(\"" << fName
                                         << "\", value."
                                         << fName << ");" << std::endl;
                    } else {
                        serializerSource << "if (!from.try_get<" << typeName << ">(\"" << fName
                                         << "\", value."
                                         << fName << ")) {" << std::endl;
                        serializerSource << "throw std::runtime_error(\"Unable to read field " << fName << "\");"
                                         << std::endl;
                        serializerSource << "}" << std::endl;
                    }
                }
                    break;
            }
        }
        serializerSource << "return value;" << std::endl;
        serializerSource << "}" << std::endl;

        serializerSource << "}" << std::endl;
        // static serialization

        serializerSource << "class " << info.name << "Serializer final : public un::Serializer<" << info.fullName
                         << "> {"
                         << std::endl;
        serializerSource << "inline virtual void serialize(const " << info.fullName
                         << "& value, un::Serialized& into) override {"
                         << std::endl;
        serializerSource << "un::serialization::serialize(value, into);" << std::endl;
        serializerSource << "}" << std::endl;

        serializerSource << "inline virtual " << info.fullName << " deserialize(un::Serialized& from) override {"
                         << std::endl;
        serializerSource << "return un::serialization::deserialize<" << info.fullName << ">(from);" << std::endl;
        serializerSource << "}" << std::endl;

        serializerSource << "};" << std::endl;
        serializerSource << "}" << std::endl;

        serializerSource << "#endif" << std::endl;
        auto includeName = getGeneratedIncludeName(info);
        std::filesystem::path includeFile = includesOutput / includeName;
        std::string p = includeFile.string();
        un::files::safe_write_file(includeFile, serializerSource);
        LOG(INFO) << "Written " << info.fullName << " serializer include to " << includeFile;
    }


    bool isSerializable(const std::shared_ptr<un::CXXComposite>& composite) {
        const std::vector<CXXField>& field = composite->getFields();
        return std::any_of(field.begin(), field.end(), [](const un::CXXField& item) {
            return item.findAttribute("un::serialize") != nullptr;
        });

    }

    std::string getGeneratedIncludeName(const GenerationInfo& info) { return info.lower + ".serializer.generated.h"; }

    std::string un::getGeneratedIncludeName(const std::string& typeName) {
        return un::lower(typeName
        ) + ".serializer.generated.h";
    }

}