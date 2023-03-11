//
// Created by brunorbsf on 25/09/22.
//

#include "enum_serialization_logic.h"

namespace un::serialization {
    void write_enum_serialization_logic(
        std::stringstream& ss,
        const generation_info& info,
        const std::shared_ptr<cxx_enum>& anEnum
    ) {
        ss << "// BEGIN ENUM SERIALIZATION " << std::endl;
        ss << "template<>" << std::endl;
        ss << "void serialize_inline<" << info.fullName << ">(const std::string& key, const "
           << info.fullName
           << "& value, un::Serialized& into) {"
           << std::endl;

        ss << "switch (value) {" << std::endl;
        const std::vector<cxx_enum_value>& enumValues = anEnum->get_values();
        for (const auto& enumValue : enumValues) {
            const auto& valueName = enumValue.get_full_name();
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
        const generation_info& info,
        const std::shared_ptr<cxx_enum>& anEnum
    ) {
        ss << "// BEGIN ENUM DESERIALIZATION " << std::endl;

        ss << "template<>" << std::endl;
        ss << info.fullName << " deserialize_inline<" << info.fullName
           << ">(const std::string& key, un::Serialized& from) {"
           << std::endl;

        ss << "static const std::unordered_map<std::string, " << info.fullName
           << "> kSerializationLookUpTable = {" << std::endl;

        const std::vector<cxx_enum_value>& enumValues = anEnum->get_values();
        std::size_t last = enumValues.size();
        for (std::size_t i = 0; i < last; ++i) {
            const auto& enumValue = enumValues[i];
            ss << "std::make_pair<std::string, " << info.fullName << ">(\""
               << enumValue.get_full_name()
               << "\", " << info.fullName << "::" << enumValue.get_full_name() << ")";
            if (i != last - 1) {
                ss << ",";
            }
            ss << std::endl;
        }
        ss << "};" << std::endl;
        ss << "std::string serializedName;" << std::endl;
        ss << "if (from.try_get(key, serializedName)) {" << std::endl;
        ss << "auto found = kSerializationLookUpTable.find(serializedName);" << std::endl;
        ss << "if (found != kSerializationLookUpTable.end()) {" << std::endl;
        ss << "return found->second;" << std::endl;
        ss << "}" << std::endl;
        ss << "}" << std::endl;

        ss << "throw std::exception(\"Invalid enum found\");" << std::endl;
        ss << "}" << std::endl;

        ss << "// END ENUM DESERIALIZATION " << std::endl;
    }

    void write_enum_serialization_logic_pair(
        std::stringstream& ss,
        const generation_info& info,
        const std::shared_ptr<cxx_enum>& anEnum
    ) {
        ss << "namespace un::serialization {" << std::endl;
        write_enum_serialization_logic(ss, info, anEnum);
        ss << std::endl;
        write_enum_deserialization_logic(ss, info, anEnum);
        ss << std::endl;
        ss << "} " << std::endl;
    }
}
