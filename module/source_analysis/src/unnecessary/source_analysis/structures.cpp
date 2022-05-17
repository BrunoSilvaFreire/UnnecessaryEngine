#include <unnecessary/source_analysis/structures.h>

#include <utility>

namespace un {

    std::string CXXAttribute::getName() const {
        return name;
    }

    CXXAttribute::CXXAttribute(std::string macro, std::vector<std::string> argsSegments) : name(std::move(macro)),
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

    CXXComposite::CXXComposite(std::string name, std::string ns) : fields(), name(std::move(name)), ns(ns) { }

    void CXXComposite::addField(CXXField&& field) {
        fields.emplace_back(std::move(field));
    }

    std::string CXXComposite::getName() const {
        return name;
    }

    const std::vector<CXXField>& CXXComposite::getFields() const {
        return fields;
    }

    std::string CXXComposite::getFullName() const {
        return ns + "::" + name;
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
    ) : accessModifier(accessModifier), name(std::move(name)), type(std::move(type)),
        attributes(std::move(expansions)) { }

    const std::string& CXXField::getName() const {
        return name;
    }

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
    template<>
    bool CXXScope::findSymbol(std::string fullName, std::shared_ptr<un::CXXComposite>& out) const {
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
    CXXType::CXXType(std::string name, CXXTypeKind innerType) : name(std::move(name)), innerType(innerType) { }

    std::string CXXType::getName() const {
        return name;
    }

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

}