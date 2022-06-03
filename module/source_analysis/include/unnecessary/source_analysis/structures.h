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

    class CXXNamed {
    public:
        virtual std::string getName() const = 0;
    };

    class CXXSymbol : public un::CXXNamed {
    private:
        std::string name;
    public:
        CXXSymbol() = default;

        explicit CXXSymbol(std::string name);

        virtual ~CXXSymbol();

        std::string getName() const override;

        virtual std::string getFullName() const;
    };

    enum CXXTypeKind : u16 {
        eInteger8,
        eInteger16,
        eInteger32,
        eInteger64,
        eInteger128,
        eFloat,
        eDouble,
        eBool,
        eComplex,
        eEnum,
        ePointer,
        eUnsignedFlag = 1 << 15, // Last bit indicates if isSigned
        eUnsignedInteger8 = eInteger8 | eUnsignedFlag,
        eUnsignedInteger16 = eInteger16 | eUnsignedFlag,
        eUnsignedInteger32 = eInteger32 | eUnsignedFlag,
        eUnsignedInteger64 = eInteger64 | eUnsignedFlag,
        eUnsignedInteger128 = eInteger128 | eUnsignedFlag,
    };

    template<>
    std::string to_string(const un::CXXTypeKind& kind);

    class CXXType : public un::CXXSymbol {
    private:
        un::CXXTypeKind innerType;
        std::vector<std::string> templateTypes;
    public:
        CXXType() = default;

        CXXType(const std::string& name, CXXTypeKind innerType);

        CXXTypeKind getKind() const;

        const std::vector<std::string>& getTemplateTypes() const;

        void addTemplate(std::string&& type);
    };

    enum CXXAccessModifier {
        eNone,
        ePublic,
        eProtected,
        ePrivate
    };

    class CXXAttribute : CXXNamed {
    private:
        std::string name;
        std::set<std::string> arguments;
    public:
        CXXAttribute(std::string name, const std::vector<std::string>& argsSegments);

        CXXAttribute(std::string name);

        std::string getName() const override;

        const std::set<std::string>& getArguments() const;

        bool hasArgument(const std::string& arg) const;

        std::string getArgumentValue(const std::string& arg) const;
    };

    class CXXField : public CXXSymbol {
    private:
        std::vector<un::CXXAttribute> attributes;
        CXXAccessModifier accessModifier;
        un::CXXType type;
    public:
        CXXField(
            CXXAccessModifier accessModifier,
            std::string name, un::CXXType type,
            std::vector<un::CXXAttribute> expansions
        );

        const std::vector<un::CXXAttribute>& getAttributes() const;

        const un::CXXAttribute& getAttribute(const std::string& name) const;

        const un::CXXAttribute* findAttribute(const std::string& name) const;

        bool hasAttribute(const std::string& name) const;

        CXXAccessModifier getAccessModifier() const;

        const un::CXXType& getType() const;
    };

    class CXXScope {
    private:
        std::vector<std::shared_ptr<CXXSymbol>> symbols;
    public:
        void addSymbol(const std::shared_ptr<CXXSymbol>& symbol) {
            symbols.emplace_back(symbol);
        }

        template<typename TSymbol>
        bool findSymbol(const std::string& fullName, std::shared_ptr<TSymbol>& out) const {
            for (const auto& item : symbols) {
                std::shared_ptr<TSymbol> ptr = std::dynamic_pointer_cast<TSymbol>(item);
                if (ptr == nullptr) {
                    continue;
                }
                std::shared_ptr<un::CXXNamed> named = std::dynamic_pointer_cast<un::CXXNamed>(item);
                if (named == nullptr) {
                    continue;
                }
                if (named->getName() == fullName) {
                    out = ptr;
                    return true;
                }
            }
            return false;
        }


        template<typename TSymbol>
        inline bool findSymbol(const un::CXXType& type, std::shared_ptr<TSymbol>& out) const {
            return findSymbol<TSymbol>(type.getName(), out);
        }

        template<typename TSymbol>
        std::vector<std::shared_ptr<TSymbol>> collectSymbols() const {
            std::vector<std::shared_ptr<TSymbol>> view;
            for (const auto& item : symbols) {
                std::shared_ptr<TSymbol> ptr = std::dynamic_pointer_cast<TSymbol>(item);
                if (ptr != nullptr) {
                    view.push_back(std::move(ptr));
                }
            }
            return view;
        }
    };

    class CXXDeclaration : public CXXSymbol {
    private:
        std::string ns;
    public:
        CXXDeclaration() = default;

        CXXDeclaration(const std::string& name, std::string ns);

        std::string getFullName() const override;
    };

    class CXXEnumValue : CXXSymbol {
    private:
        std::size_t value;
    public:
        CXXEnumValue(const std::string& name, size_t value);
    };

    class CXXEnum : public CXXDeclaration {
    private:
        std::vector<CXXEnumValue> values;
    public:
        CXXEnum(
            const std::string& name,
            const std::string& ns,
            std::vector<CXXEnumValue> values
        );
    };

    class CXXComposite : public CXXDeclaration {
    private:
        std::vector<CXXField> fields;
    public:
        explicit CXXComposite(const std::string& name, std::string ns);

        void addField(CXXField&& field);

        const std::vector<CXXField>& getFields() const;;

    };

    template<>
    bool CXXScope::findSymbol(const std::string& fullName, std::shared_ptr<un::CXXComposite>& out) const;

    class CXXTranslationUnit : public CXXScope {
    private:
        std::vector<std::string> includes;
        std::string selfInclude;
        std::filesystem::path location;
        std::unordered_map<std::string, un::CXXType> _typeIndex;
    public:
        explicit CXXTranslationUnit(
            std::filesystem::path location,
            std::string selfInclude
        );

        void addInclude(const std::string& include);

        const std::vector<std::string>& getIncludes() const;

        bool searchType(const std::string& type, un::CXXType& out);

        void addType(const std::string& key, const un::CXXType& type);

        void addType(const un::CXXType& out);

        const std::string& getSelfInclude() const;

        const std::filesystem::path& getLocation() const;
    };

}
#endif