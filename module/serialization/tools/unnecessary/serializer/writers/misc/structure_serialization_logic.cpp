#include "structure_serialization_logic.h"

namespace un::serialization {
    void
    write_structure_serialization_info(
        std::stringstream& ss,
        const std::shared_ptr<cxx_composite>& ptr
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
};
