#include <utility>
#include <unnecessary/source_analysis/structures.h>


namespace un {

    std::string CXXAttribute::getName() const {
        return name;
    }

    CXXAttribute::CXXAttribute(
        std::string macro,
        const std::vector<std::string>& argsSegments
    ) : name(std::move(macro)),
        arguments() {
        std::string current;
        for (const auto& item : argsSegments) {
            if (item == ",") {
                arguments.emplace(current);
                current = std::string();
            } else {
                current.append(item);
            }
        }
        if (!current.empty()) {
            arguments.emplace(current);
        }
    }

    CXXAttribute::CXXAttribute(std::string name) : name(std::move(name)), arguments() {

    }

    const std::set<std::string>& CXXAttribute::getArguments() const {
        return arguments;
    }

    bool CXXAttribute::hasArgument(const std::string& arg) const {
        for (const std::string& argument : arguments) {
            auto eqIt = argument.find('=');
            if (eqIt == std::string::npos) {
                if (argument == arg) {
                    return true;
                }
            } else {
                std::string argKey = argument.substr(0, eqIt);
                if (argKey == arg) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string CXXAttribute::getArgumentValue(const std::string& arg) const {
        for (const std::string& argument : arguments) {
            auto eqIt = argument.find('=');
            if (eqIt == std::string::npos) {
                if (argument == arg) {
                    return std::string();
                }
            } else {
                std::string argKey = argument.substr(0, eqIt);
                if (argKey == arg) {
                    return argument.substr(eqIt + 1);
                }
            }
        }
        return std::string();
    }

    std::string un::CXXDeclaration::getFullName() const {
        if (!ns.empty()) {
            return ns + "::" + getName();
        }
        return CXXSymbol::getFullName();
    }

    CXXDeclaration::CXXDeclaration(const std::string& name, std::string ns) : CXXSymbol(name), ns(std::move(ns)) { }


    CXXComposite::CXXComposite(const std::string& name, std::string ns) : CXXDeclaration(name, std::move(ns)),
                                                                          fields() { }

    void CXXComposite::addField(CXXField&& field) {
        fields.emplace_back(std::move(field));
    }

    const std::vector<CXXField>& CXXComposite::getFields() const {
        return fields;
    }

    const std::vector<un::CXXAttribute>& CXXField::getAttributes() const {
        return attributes;
    }

    CXXAccessModifier CXXField::getAccessModifier() const {
        return accessModifier;
    }

    const un::CXXType& CXXField::getType() const {
        return type;
    }

    CXXField::CXXField(
        CXXAccessModifier accessModifier,
        std::string name,
        un::CXXType type,
        std::vector<un::CXXAttribute> expansions
    ) : un::CXXSymbol(std::move(name)),
        accessModifier(accessModifier), type(std::move(type)),
        attributes(std::move(expansions)) { }

    bool CXXField::hasAttribute(const std::string& name) const {
        return findAttribute(name) != nullptr;
    }

    const un::CXXAttribute* CXXField::findAttribute(const std::string& name) const {
        for (const auto& att : attributes) {
            if (att.getName() == name) {
                return &att;
            }
        }
        return nullptr;
    }

    const un::CXXAttribute& CXXField::getAttribute(const std::string& name) const {
        for (const auto& att : attributes) {
            if (att.getName() == name) {
                return att;
            }
        }
        std::stringstream ss;
        ss << "Unable to find attribute " << name;
        throw std::runtime_error(ss.str());
    }

    CXXSymbol::~CXXSymbol() {

    }

    CXXSymbol::CXXSymbol(std::string name) : name(std::move(name)) { }

    std::string un::CXXSymbol::getName() const {
        return name;
    }

    std::string un::CXXSymbol::getFullName() const {
        return name;
    }

    template<>
    bool CXXScope::findSymbol(const std::string& fullName, std::shared_ptr<un::CXXComposite>& out) const {
        for (const auto& item : symbols) {
            std::shared_ptr<un::CXXComposite> ptr = std::dynamic_pointer_cast<un::CXXComposite>(item);
            if (ptr == nullptr) {
                continue;
            }
            if (ptr->getFullName() == fullName) {
                out = ptr;
                return true;
            }
        }
        return false;
    }

    CXXType::CXXType(
        const std::string& name,
        CXXTypeKind innerType
    ) : CXXSymbol(name), innerType(innerType) { }


    CXXTypeKind CXXType::getKind() const {
        return innerType;
    }

    void CXXType::addTemplate(std::string&& type) {
        templateTypes.push_back(std::move(type));
    }

    const std::vector<std::string>& CXXType::getTemplateTypes() const {
        return templateTypes;
    }

    template<>
    std::string to_string(const CXXTypeKind& kind) {
        switch (kind) {
            case eInteger8:
                return "i8";
            case eInteger16:
                return "i16";
            case eInteger32:
                return "i32";
            case eInteger64:
                return "i64";
            case eInteger128:
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
            case eFloat:
                return "f32";
            case eDouble:
                return "f64";
            case eBool:
                return "bool";
            case eComplex:
                return "complex";
            case ePointer:
                return "pointer";
        }
        return "unknown_kind";
    }

    template<>
    std::string to_string(const CXXAccessModifier& value) {
        switch (value) {
            case un::CXXAccessModifier::eNone:
                return "none";
            case un::CXXAccessModifier::ePublic:
                return "public";
            case un::CXXAccessModifier::eProtected:
                return "protected";
            case un::CXXAccessModifier::ePrivate:
                return "private";
        }
    }

    CXXEnum::CXXEnum(
        const std::string& name,
        const std::string& ns,
        std::vector<CXXEnumValue> values
    ) : CXXDeclaration(name, ns), values(values) { }

    const std::vector<CXXEnumValue>& CXXEnum::getValues() const {
        return values;
    }

    CXXEnumValue::CXXEnumValue(const std::string& name, size_t value) : CXXSymbol(name), value(value) { }

    size_t CXXEnumValue::getValue() const {
        return value;
    }

    void CXXTranslationUnit::addInclude(const std::string& include) {
        includes.emplace_back(include);
    }

    bool CXXTranslationUnit::searchType(const std::string& type, CXXType& out) {
        return un::maps::search(_typeIndex, type, out);
    }

    void CXXTranslationUnit::addType(const std::string& key, const CXXType& type) {
        _typeIndex.emplace(key, type);
    }

    void CXXTranslationUnit::addType(const CXXType& out) {
        addType(out.getName(), out);
    }

    const std::vector<std::string>& CXXTranslationUnit::getIncludes() const {
        return includes;
    }

    CXXTranslationUnit::CXXTranslationUnit(
        std::filesystem::path location,
        std::string selfInclude
    ) : selfInclude(std::move(selfInclude)), location(std::move(location)) { }

    const std::string& CXXTranslationUnit::getSelfInclude() const {
        return selfInclude;
    }

    const std::filesystem::path& CXXTranslationUnit::getLocation() const {
        return location;
    }
}