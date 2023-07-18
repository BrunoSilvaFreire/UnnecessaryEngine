#include <utility>
#include <unnecessary/source_analysis/structures.h>

namespace un {
    std::string cxx_attribute::get_name() const {
        return _name;
    }

    cxx_attribute::cxx_attribute(
        std::string macro,
        const std::vector<std::string>& argsSegments
    ) : _name(std::move(macro)),
        _arguments() {
        std::string current;
        for (const auto& item : argsSegments) {
            if (item == ",") {
                _arguments.emplace(current);
                current = std::string();
            }
            else {
                current.append(item);
            }
        }
        if (!current.empty()) {
            _arguments.emplace(current);
        }
    }

    cxx_attribute::cxx_attribute(std::string name) : _name(std::move(name)), _arguments() {
    }

    const std::set<std::string>& cxx_attribute::get_arguments() const {
        return _arguments;
    }

    bool cxx_attribute::has_argument(const std::string& arg) const {
        for (const std::string& argument : _arguments) {
            auto eqIt = argument.find('=');
            if (eqIt == std::string::npos) {
                if (argument == arg) {
                    return true;
                }
            }
            else {
                std::string argKey = argument.substr(0, eqIt);
                if (argKey == arg) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string cxx_attribute::get_argument_value(const std::string& arg) const {
        for (const std::string& argument : _arguments) {
            auto eqIt = argument.find('=');
            if (eqIt == std::string::npos) {
                if (argument == arg) {
                    return std::string();
                }
            }
            else {
                std::string argKey = argument.substr(0, eqIt);
                if (argKey == arg) {
                    return argument.substr(eqIt + 1);
                }
            }
        }
        return std::string();
    }

    std::string cxx_declaration::get_full_name() const {
        if (!_namespace.empty()) {
            return _namespace + "::" + get_name();
        }
        return cxx_symbol::get_full_name();
    }

    cxx_declaration::cxx_declaration(const std::string& name, std::string ns)
        : cxx_symbol(name), _namespace(std::move(ns)) {
    }

    cxx_composite::cxx_composite(const std::string& name, std::string ns)
        : cxx_declaration(name, std::move(ns)),
          _fields() {
    }

    void cxx_composite::add_field(cxx_field&& field) {
        _fields.emplace_back(std::move(field));
    }

    const std::vector<cxx_field>& cxx_composite::get_fields() const {
        return _fields;
    }

    const std::vector<cxx_attribute>& cxx_field::get_attributes() const {
        return _attributes;
    }

    cxx_access_modifier cxx_field::get_access_modifier() const {
        return _accessModifier;
    }

    const cxx_type& cxx_field::get_type() const {
        return _type;
    }

    cxx_field::cxx_field(
        cxx_access_modifier accessModifier,
        std::string name,
        cxx_type type,
        std::vector<cxx_attribute> expansions
    ) : cxx_symbol(std::move(name)),
        _attributes(std::move(expansions)), _accessModifier(accessModifier),
        _type(std::move(type)) {
    }

    bool cxx_field::has_attribute(const std::string& name) const {
        return find_attribute(name) != nullptr;
    }

    const cxx_attribute* cxx_field::find_attribute(const std::string& name) const {
        for (const auto& att : _attributes) {
            if (att.get_name() == name) {
                return &att;
            }
        }
        return nullptr;
    }

    const cxx_attribute& cxx_field::get_attribute(const std::string& name) const {
        for (const auto& att : _attributes) {
            if (att.get_name() == name) {
                return att;
            }
        }
        std::stringstream ss;
        ss << "Unable to find attribute " << name;
        throw std::runtime_error(ss.str());
    }

    cxx_symbol::~cxx_symbol() {
    }

    cxx_symbol::cxx_symbol(std::string name) : _name(std::move(name)) {
    }

    std::string cxx_symbol::get_name() const {
        return _name;
    }

    std::string cxx_symbol::get_full_name() const {
        return _name;
    }

    template<>
    bool cxx_scope::find_symbol(
        const std::string& fullName,
        std::shared_ptr<cxx_composite>& out
    ) const {
        for (const auto& item : _symbols) {
            std::shared_ptr<cxx_composite> ptr = std::dynamic_pointer_cast<cxx_composite>(item);
            if (ptr == nullptr) {
                continue;
            }
            if (ptr->get_full_name() == fullName) {
                out = ptr;
                return true;
            }
        }
        return false;
    }

    cxx_type::cxx_type(
        const std::string& name,
        cxx_type_kind innerType
    ) : cxx_symbol(name), _innerType(innerType) {
    }

    cxx_type_kind cxx_type::get_kind() const {
        return _innerType;
    }

    void cxx_type::add_template(std::string&& type) {
        _templateTypes.push_back(std::move(type));
    }

    const std::vector<std::string>& cxx_type::get_template_types() const {
        return _templateTypes;
    }

    template<>
    std::string to_string(const cxx_type_kind& kind) {
        switch (kind) {
            case integer8:
                return "i8";
            case integer16:
                return "i16";
            case integer32:
                return "i32";
            case integer64:
                return "i64";
            case integer128:
                return "i128";
            case eUnsignedInteger8:
                return "u8";
            case eUnsignedInteger16:
                return "u16";
            case eUnsignedInteger32:
                return "u32";
            case eUnsignedInteger64:
                return "u64";
            case eUnsignedInteger128:
                return "u128";
            case float_single:
                return "f32";
            case float_double:
                return "f64";
            case eBool:
                return "bool";
            case complex:
                return "complex";
            case ePointer:
                return "pointer";
        }
        return "unknown_kind";
    }

    template<>
    std::string to_string(const cxx_access_modifier& value) {
        switch (value) {
            case eNone:
                return "none";
            case ePublic:
                return "public";
            case eProtected:
                return "protected";
            case ePrivate:
                return "private";
        }
    }

    cxx_enum::cxx_enum(
        const std::string& name,
        const std::string& ns,
        std::vector<cxx_enum_value> values
    ) : cxx_declaration(name, ns), _values(std::move(values)) {
    }

    const std::vector<cxx_enum_value>& cxx_enum::get_values() const {
        return _values;
    }

    cxx_enum_value::cxx_enum_value(const std::string& name, size_t value)
        : cxx_symbol(name), _value(value) {
    }

    size_t cxx_enum_value::get_value() const {
        return _value;
    }

    void cxx_translation_unit::add_include(const std::string& include) {
        _includes.emplace_back(include);
    }

    bool cxx_translation_unit::search_type(const std::string& type, cxx_type& out) {
        return maps::search(_typeIndex, type, out);
    }

    void cxx_translation_unit::add_type(const std::string& key, const cxx_type& type) {
        _typeIndex.emplace(key, type);
    }

    void cxx_translation_unit::add_type(const cxx_type& out) {
        add_type(out.get_name(), out);
    }

    const std::vector<std::string>& cxx_translation_unit::get_includes() const {
        return _includes;
    }

    cxx_translation_unit::cxx_translation_unit(
        std::filesystem::path location,
        std::string selfInclude
    ) : _selfInclude(std::move(selfInclude)), _location(std::move(location)) {
    }

    const std::string& cxx_translation_unit::get_self_include() const {
        return _selfInclude;
    }

    const std::filesystem::path& cxx_translation_unit::get_location() const {
        return _location;
    }
}
