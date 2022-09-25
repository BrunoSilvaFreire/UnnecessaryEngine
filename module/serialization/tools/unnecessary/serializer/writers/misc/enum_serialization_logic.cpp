//
// Created by brunorbsf on 25/09/22.
//

#include "enum_serialization_logic.h"

namespace un::serialization {
    void write_enum_serialization_logic(
        std::stringstream& ss,
        const GenerationInfo& info,
        const std::shared_ptr<un::CXXEnum>& anEnum
    ) {

        ss << "// BEGIN ENUM SERIALIZATION " << std::endl;
        ss << "template<>" << std::endl;
        ss << "void serialize_inline<" << info.fullName << ">(const std::string& key, const "
           << info.fullName
           << "& value, un::Serialized& into) {"
           << std::endl;

        ss << "switch (value) {" << std::endl;
        const std::vector<CXXEnumValue>& enumValues = anEnum->getValues();
        for (const auto& enumValue : enumValues) {
            const auto& valueName = enumValue.getFullName();
            ss << "case " << info.fullName << "::" << valueName << ":" << std::endl;
            ss << "into.set<std::string>(key, \"" << valueName << "\");" << std::endl;
            ss << "break;" << std::endl;
        }
        ss << "}" << std::endl;


        ss << "}" << std::endl;
        ss << "// END ENUM SERIALIZATION " << std::endl;
    }

    void write_enum_deserialization_logic(
        std::stringstream& ss,
        const GenerationInfo& info,
        const std::shared_ptr<un::CXXEnum>& anEnum
    ) {


        ss << "// BEGIN ENUM DESERIALIZATION " << std::endl;

        ss << "template<>" << std::endl;
        ss << info.fullName << " deserialize_inline<" << info.fullName
                            << ">(const std::string& key, un::Serialized& from) {"
                            << std::endl;

        ss << info.fullName << " value;" << std::endl;
        ss << "static std::unordered_map<std::string, " << info.fullName
                            << "> kSerializationLookUpTable = {" << std::endl;

        const std::vector<CXXEnumValue>& enumValues = anEnum->getValues();
        std::size_t last = enumValues.size();
        for (std::size_t i = 0; i < last; ++i) {
            const auto& enumValue = enumValues[i];
            ss << "std::make_pair<std::string, " << info.fullName << ">(\"" << enumValue.getFullName()
                                << "\", " << info.fullName << "::" << enumValue.getFullName() << ")";
            if (i != last - 1) {
                ss << ",";
            }
            ss << std::endl;
        }
        ss << "};" << std::endl;

        ss << "return value;" << std::endl;
        ss << "}" << std::endl;

        ss << "// END ENUM DESERIALIZATION " << std::endl;
    }

    void write_enum_serialization_logic_pair(
        std::stringstream& ss,
        const GenerationInfo& info,
        const std::shared_ptr<CXXEnum>& anEnum
    ) {
        ss << "namespace un::serialization {" << std::endl;
        un::serialization::write_enum_serialization_logic(ss, info, anEnum);
        ss << std::endl;
        un::serialization::write_enum_deserialization_logic(ss, info, anEnum);
        ss << std::endl;
        ss << "} " << std::endl;
    }
}