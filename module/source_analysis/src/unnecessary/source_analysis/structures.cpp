#include <unnecessary/source_analysis/structures.h>

namespace un {

    std::string CXXMacroExpansion::getName() {
        return macro;
    }

    CXXMacroExpansion::CXXMacroExpansion(const std::string& macro, const std::string& value) : macro(macro),
                                                                                               value(value) { }

    CXXComposite::CXXComposite(std::string name, std::string ns) : fields(), name(std::move(name)), ns(ns) { }

    void CXXComposite::addField(CXXField&& field) {
        fields.emplace_back(std::move(field));
    }

    std::string CXXComposite::getName() {
        return name;
    }

    CXXField::CXXField(
        CXXAccessModifier accessModifier,
        std::string name,
        std::string type,
        std::vector<un::CXXMacroExpansion> expansions
    ) : accessModifier(accessModifier), name(std::move(name)), type(std::move(type)), macros(std::move(expansions)) { }
}