//
// Created by bruno on 09/05/2022.
//

#include "delegate_writer.h"

namespace un {
    bool DelegateWriter::accepts(const CXXField& field, float& outPriority) {
        outPriority = 50;
        const CXXAttribute& att = field.getAttribute("un::serialize");
        return att.hasArgument("serializer") && att.hasArgument("deserializer");
    }

    void DelegateWriter::write_serializer(std::stringstream& ss, const un::CXXField& field) {
        const CXXAttribute& att = field.getAttribute("un::serialize");
        std::string serializer = att.getArgumentValue("serializer");
        ss << serializer << "(value." << field.getName() << ", into);" << std::endl;
    }

    void DelegateWriter::write_deserializer(std::stringstream& ss, const un::CXXField& field) {
        const CXXAttribute& att = field.getAttribute("un::serialize");
        std::string deserializer = att.getArgumentValue("deserializer");
        ss << "value." << field.getName() << " = " << deserializer << "(from);" << std::endl;
    }

    std::string DelegateWriter::name() {
        return "DelegateWriter";
    }

}