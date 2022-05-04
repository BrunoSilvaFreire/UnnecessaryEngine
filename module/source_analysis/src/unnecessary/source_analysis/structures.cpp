#include <unnecessary/source_analysis/structures.h>

namespace un {

    std::string CXXMacroExpansion::getName() {
        return macro;
    }

    CXXMacroExpansion::CXXMacroExpansion(const std::string& macro, const std::string& value) : macro(macro),
                                                                                               value(value) { }

    CXXComposite::CXXComposite(std::string name) : fields(), name(std::move(name)) { }

    void CXXComposite::addField(CXXField&& field) {
        fields.emplace_back(std::move(field));
    }

    std::string CXXComposite::getName() {
        return name;
    }

    CXXField::CXXField(
        CXXAccessModifier accessModifier,
        const std::string& name,
        const std::string& type
    ) : accessModifier(accessModifier), name(name), type(type), macros() { }
}