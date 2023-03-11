#ifndef UNNECESSARYENGINE_STRUCTURES_H
#define UNNECESSARYENGINE_STRUCTURES_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <unnecessary/def.h>
#include <unnecessary/algorithms/maps.h>

namespace un {
    class cxx_named {
    public:
        virtual std::string get_name() const = 0;
    };

    class cxx_symbol : public cxx_named {
    private:
        std::string _name;

    public:
        cxx_symbol() = default;

        explicit cxx_symbol(std::string name);

        virtual ~cxx_symbol();

        std::string get_name() const override;

        virtual std::string get_full_name() const;
    };

    enum cxx_type_kind : u16 {
        integer8,
        integer16,
        integer32,
        integer64,
        integer128,
        float_single,
        float_double,
        eBool,
        complex,
        eEnum,
        ePointer,
        eUnsignedFlag = 1 << 15,
        // Last bit indicates if isSigned
        eUnsignedInteger8 = integer8 | eUnsignedFlag,
        eUnsignedInteger16 = integer16 | eUnsignedFlag,
        eUnsignedInteger32 = integer32 | eUnsignedFlag,
        eUnsignedInteger64 = integer64 | eUnsignedFlag,
        eUnsignedInteger128 = integer128 | eUnsignedFlag,
    };

    template<>
    std::string to_string(const cxx_type_kind& kind);

    class cxx_type : public cxx_symbol {
    private:
        cxx_type_kind _innerType;
        std::vector<std::string> _templateTypes;

    public:
        cxx_type() = default;

        cxx_type(const std::string& name, cxx_type_kind innerType);

        cxx_type_kind get_kind() const;

        const std::vector<std::string>& get_template_types() const;

        void add_template(std::string&& type);
    };

    enum cxx_access_modifier {
        eNone,
        ePublic,
        eProtected,
        ePrivate
    };

    template<>
    std::string to_string(const cxx_access_modifier& value);

    class cxx_attribute : cxx_named {
    private:
        std::string _name;
        std::set<std::string> _arguments;

    public:
        cxx_attribute(std::string name, const std::vector<std::string>& argsSegments);

        cxx_attribute(std::string name);

        std::string get_name() const override;

        const std::set<std::string>& get_arguments() const;

        bool has_argument(const std::string& arg) const;

        std::string get_argument_value(const std::string& arg) const;
    };

    class cxx_field : public cxx_symbol {
    private:
        std::vector<cxx_attribute> _attributes;
        cxx_access_modifier _accessModifier;
        cxx_type _type;

    public:
        cxx_field(
            cxx_access_modifier accessModifier,
            std::string name, cxx_type type,
            std::vector<cxx_attribute> expansions
        );

        const std::vector<cxx_attribute>& get_attributes() const;

        const cxx_attribute& get_attribute(const std::string& name) const;

        const cxx_attribute* find_attribute(const std::string& name) const;

        bool has_attribute(const std::string& name) const;

        cxx_access_modifier get_access_modifier() const;

        const cxx_type& get_type() const;
    };

    class cxx_scope {
    private:
        std::vector<std::shared_ptr<cxx_symbol>> _symbols;

    public:
        void addSymbol(const std::shared_ptr<cxx_symbol>& symbol) {
            _symbols.emplace_back(symbol);
        }

        template<typename t_symbol>
        bool
        find_symbol(const std::string& fullName, std::shared_ptr<un::cxx_composite>& out) const {
            for (const auto& item : _symbols) {
                std::shared_ptr<t_symbol> ptr = std::dynamic_pointer_cast<t_symbol>(item);
                if (ptr == nullptr) {
                    continue;
                }
                std::shared_ptr<cxx_named> named = std::dynamic_pointer_cast<cxx_named>(item);
                if (named == nullptr) {
                    continue;
                }
                if (named->get_name() == fullName) {
                    out = ptr;
                    return true;
                }
            }
            return false;
        }

        template<typename t_symbol>
        bool find_symbol(const cxx_type& type, std::shared_ptr<t_symbol>& out) const {
            return find_symbol<t_symbol>(type.get_name(), out);
        }

        template<typename t_symbol>
        std::vector<std::shared_ptr<t_symbol>> collect_symbols() const {
            std::vector<std::shared_ptr<t_symbol>> view;
            for (const auto& item : _symbols) {
                std::shared_ptr<t_symbol> ptr = std::dynamic_pointer_cast<t_symbol>(item);
                if (ptr != nullptr) {
                    view.push_back(std::move(ptr));
                }
            }
            return view;
        }
    };

    class cxx_declaration : public cxx_symbol {
    private:
        std::string _namespace;

    public:
        cxx_declaration() = default;

        cxx_declaration(const std::string& name, std::string ns);

        std::string get_full_name() const override;
    };

    class cxx_enum_value : public cxx_symbol {
    private:
        std::size_t _value;

    public:
        cxx_enum_value(const std::string& name, size_t value);

        size_t get_value() const;
    };

    class cxx_enum : public cxx_declaration {
    private:
        std::vector<cxx_enum_value> _values;

    public:
        cxx_enum(
            const std::string& name,
            const std::string& ns,
            std::vector<cxx_enum_value> values
        );

        const std::vector<cxx_enum_value>& get_values() const;
    };

    class cxx_composite : public cxx_declaration {
    private:
        std::vector<cxx_field> _fields;

    public:
        explicit cxx_composite(const std::string& name, std::string ns);

        void add_field(cxx_field&& field);

        const std::vector<cxx_field>& get_fields() const;;
    };

    template<>
    bool
    cxx_scope::find_symbol(const std::string& fullName, std::shared_ptr<cxx_composite>& out) const;

    class cxx_translation_unit : public cxx_scope {
    private:
        std::vector<std::string> _includes;
        std::string _selfInclude;
        std::filesystem::path _location;
        std::unordered_map<std::string, cxx_type> _typeIndex;

    public:
        explicit cxx_translation_unit(
            std::filesystem::path location,
            std::string selfInclude
        );

        void add_include(const std::string& include);

        const std::vector<std::string>& get_includes() const;

        bool search_type(const std::string& type, cxx_type& out);

        void add_type(const std::string& key, const cxx_type& type);

        void add_type(const cxx_type& out);

        const std::string& get_self_include() const;

        const std::filesystem::path& get_location() const;
    };
}
#endif
