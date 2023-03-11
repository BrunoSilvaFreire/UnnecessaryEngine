#include "field_writer.h"
#include "complex_writer.h"
#include "default_writer.h"
#include "delegate_writer.h"
#include "identifiable_writer.h"
#include "collection_writer.h"
#include "enum_writer.h"
#include <iostream>

namespace un {
    bool field_writer::is_optional(const cxx_field& field) {
        return field.find_attribute("un::serialize")->has_argument("optional");
    }

    void field_writer::add_missing_field_exception(std::stringstream& ss, const cxx_field& field) {
        add_missing_field_exception(ss, field.get_name());
    }

    void
    field_writer::add_missing_field_exception(std::stringstream& ss, const std::string& fieldName) {
        ss << "throw std::runtime_error(\"Unable to read field " << fieldName << "\");"
           << std::endl;
    }

    bool field_writer::try_serialize_primitive(
        std::stringstream& ss,
        const std::string& name,
        const cxx_type& primitiveCandidate
    ) {
        if (primitiveCandidate.get_name() != "std::string") {
            if (primitiveCandidate.get_kind() == complex) {
                return false;
            }
        }

        ss << "into.set<" << primitiveCandidate.get_name() << ">" << "(\"" << name << "\", value."
           << name << ");"
           << std::endl;
        return true;
    }

    bool field_writer::try_deserialize_primitive(
        std::stringstream& ss,
        const std::string& name,
        const cxx_type& primitiveCandidate
    ) {
        if (primitiveCandidate.get_name() != "std::string") {
            if (primitiveCandidate.get_kind() == complex) {
                return false;
            }
        }
        std::string typeName = primitiveCandidate.get_name();

        ss << "if (!from.try_get<" << typeName << ">"
           << "(\"" << name << "\", value." << name << ")) {" << std::endl;
        add_missing_field_exception(ss, name);
        ss << "}" << std::endl;
        return true;
    }

    std::shared_ptr<field_writer> writer_registry::get_writer(
        const cxx_field& field,
        const cxx_translation_unit& unit
    ) const {
        const auto& bestWriter = std::max_element(
            _writers.begin(), _writers.end(),
            [&](
                const std::shared_ptr<field_writer>& a,
                const std::shared_ptr<field_writer>& b
            ) {
                float first;
                float second;
                if (!a->accepts(field, unit, first)) {
                    return true;
                }
                if (!b->accepts(field, unit, second)) {
                    return false;
                }
                return first < second;
            }
        );
        if (bestWriter == _writers.end()) {
            throw std::runtime_error("No suitable writer found");
        }
        return *bestWriter;
    }

    writer_registry::writer_registry() : _writers() {
        add_writer<primitive_writer>();
        add_writer<complex_writer>();
        add_writer<delegate_writer>();
        add_writer<identifiable_vector_writer>();
        add_writer<collection_writer>();
        add_writer<enum_by_name_writer>();
    }
}
