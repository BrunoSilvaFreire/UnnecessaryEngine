#ifndef UNNECESSARYENGINE_FIELD_WRITER_H
#define UNNECESSARYENGINE_FIELD_WRITER_H

#include <sstream>
#include <vector>
#include <algorithm>
#include <memory>
#include "unnecessary/source_analysis/structures.h"

namespace un {
    class writer_registry;

    class field_writer {
    protected:
        static bool is_optional(const cxx_field& field);

        static void add_missing_field_exception(std::stringstream& ss, const cxx_field& field);

        static void
        add_missing_field_exception(std::stringstream& ss, const std::string& fieldName);

        static bool try_serialize_primitive(
            std::stringstream& ss,
            const std::string& name,
            const cxx_type& primitiveCandidate
        );

        static bool try_deserialize_primitive(
            std::stringstream& ss,
            const std::string& name,
            const cxx_type& primitiveCandidate
        );

    public:
        // The following priorities are *hints*, don't feel obliged to follow them
        static constexpr float k_default_priority = 0.0F;
        static constexpr float k_basic_priority = 1.0F;
        static constexpr float k_specialized_kind_priority = 1.5F;

        virtual bool
        accepts(const cxx_field& field, const cxx_translation_unit& unit, float& outPriority) = 0;

        virtual void write_serializer(
            std::stringstream& ss,
            const cxx_field& field,
            const cxx_translation_unit& unit,
            const writer_registry& registry
        ) = 0;

        virtual void write_deserializer(
            std::stringstream& ss,
            const cxx_field& field,
            const cxx_translation_unit& unit,
            const writer_registry& registry
        ) = 0;

        virtual std::string name() = 0;
    };

    class writer_registry {
    private:
        std::vector<std::shared_ptr<field_writer>> _writers;

    public:
        writer_registry();

        std::shared_ptr<field_writer> get_writer(
            const cxx_field& field,
            const cxx_translation_unit& unit
        ) const;

        template<typename TWriter>
        void add_writer() {
            std::shared_ptr<TWriter> val = std::make_shared<TWriter>();
            _writers.emplace_back(std::move(std::static_pointer_cast<field_writer>(val)));
        }
    };
}
#endif //UNNECESSARYENGINE_FIELD_WRITER_H
