#ifndef UNNECESSARYENGINE_STRUCTURES_H
#define UNNECESSARYENGINE_STRUCTURES_H

#include <string>
#include <vector>
#include <memory>

namespace un {

    class CXXSymbol {

    };

    class CXXNamed {
    public:
        virtual std::string getName() = 0;
    };

    enum CXXAccessModifier {
        eNone,
        ePublic,
        eProtected,
        ePrivate
    };

    class CXXMacroExpansion : CXXNamed {
    private:
        std::string macro;
        std::string value;
    public:
        CXXMacroExpansion(const std::string& macro, const std::string& value);

        std::string getName() override;
    };

    class CXXField : public CXXSymbol {
    private:
        std::vector<un::CXXMacroExpansion> macros;
        CXXAccessModifier accessModifier;
        std::string name;
        std::string type;
    public:
        CXXField(CXXAccessModifier accessModifier, const std::string& name, const std::string& type);
    };

    class CXXScope {
    private:
        std::vector<std::shared_ptr<CXXSymbol>> symbols;
    public:
        void addSymbol(const std::shared_ptr<CXXSymbol>& symbol) {
            symbols.emplace_back(symbol);
        }
    };


    class CXXComposite : public CXXSymbol, public CXXNamed {
    private:
        std::string name;
        std::vector<CXXField> fields;
    public:
        explicit CXXComposite(std::string name);

        void addField(CXXField&& field);

        std::string getName() override;
    };

    class CXXNamespace : public CXXScope {
    private:
        std::string value;
    public:
    };

    class CXXTranslationUnit : public CXXScope {

    };

}
#endif