#include "structure_serialization_logic.h"

namespace un::serialization {

    void write_structure_serialization_info(std::stringstream& ss, const std::shared_ptr<CXXComposite>& ptr) {
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
};