//
// Created by bruno on 09/05/2022.
//

#include "delegate_writer.h"

namespace un {
    bool DelegateWriter::accepts(const CXXField& field, const un::CXXTranslationUnit& unit, float& outPriority) {
        outPriority = 50;
        const CXXAttribute& att = field.getAttribute("un::serialize");
        return att.hasArgument("serializer") && att.hasArgument("deserializer");
    }

    void
    DelegateWriter::write_serializer(std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
                                     const un::WriterRegistry& registry) {
        const CXXAttribute& att = field.getAttribute("un::serialize");
        std::string serializer = att.getArgumentValue("serializer");
        ss << serializer << "(value." << field.getName() << ", into);" << std::endl;
    }

    void
    DelegateWriter::write_deserializer(std::stringstream& ss, const CXXField& field, const un::CXXTranslationUnit& unit,
                                       const un::WriterRegistry& registry) {
        const CXXAttribute& att = field.getAttribute("un::serialize");
        std::string deserializer = att.getArgumentValue("deserializer");
        ss << "un::Serialized serialized_" << field.getName() << ";" << std::endl;
        ss << "if (from.try_get(\"" << field.getName() << "\", serialized_" << field.getName() << ")) {" << std::endl;
        ss << "value." << field.getName() << " = " << deserializer << "(serialized_" << field.getName() << ");"
           << std::endl;
        ss << "}" << std::endl;

    }

    std::string DelegateWriter::name() {
        return "DelegateWriter";
    }

}