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

    CXXSymbol::~CXXSymbol() {

    }

    CXXType::CXXType(std::string name, CXXTypeKind innerType) : name(std::move(name)), innerType(innerType) { }

    std::string CXXType::getName() const {
        return name;
    }

    CXXTypeKind CXXType::getKind() const {
        return innerType;
    }
}