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

    class CXXSymbol {
    public:
        virtual ~CXXSymbol();
    };

    class CXXNamed {
    public:
        virtual std::string getName() const = 0;
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
        ePointer,
        eUnsignedFlag = 1 << (sizeof(CXXTypeKind) - 1), // Last bit indicates if isSigned
        eUnsignedInteger8 = eInteger8 | eUnsignedFlag,
        eUnsignedInteger16 = eInteger16 | eUnsignedFlag,
        eUnsignedInteger32 = eInteger32 | eUnsignedFlag,
        eUnsignedInteger64 = eInteger64 | eUnsignedFlag,
        eUnsignedInteger128 = eInteger128 | eUnsignedFlag,
    };

    class CXXType : CXXSymbol, CXXNamed {
    private:
        std::string name;
        un::CXXTypeKind innerType;
    public:
        CXXType(std::string name, CXXTypeKind innerType);

        CXXType() = default;

        std::string getName() const override;

        CXXTypeKind getKind() const;
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
        CXXAttribute(std::string name, std::vector<std::string> argsSegments);

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
        std::string name;
        un::CXXType type;
    public:
        CXXField(
            CXXAccessModifier accessModifier,
            std::string name, un::CXXType type,
            std::vector<un::CXXAttribute> expansions
        );

        const std::string& getName() const;

        const std::vector<un::CXXAttribute>& getAttributes() const;

        const un::CXXAttribute& getAttribute(std::string name) const {
            for (const auto& att : attributes) {
                if (att.getName() == name) {
                    return att;
                }
            }
            std::stringstream ss;
            ss << "Unable to find attribute " << name;
            throw std::runtime_error(ss.str());
        }

        const un::CXXAttribute* findAttribute(std::string name) const {
            for (const auto& att : attributes) {
                if (att.getName() == name) {
                    return &att;
                }
            }
            return nullptr;
        }

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
        bool findSymbol(std::string type, std::shared_ptr<TSymbol>& out) const {
            for (const auto& item : symbols) {
                std::shared_ptr<TSymbol> ptr = std::dynamic_pointer_cast<TSymbol>(item);
                if (ptr == nullptr) {
                    continue;
                }
                std::shared_ptr<un::CXXNamed> named = std::dynamic_pointer_cast<un::CXXNamed>(item);
                if (named == nullptr) {
                    continue;
                }
                if (named->getName() == type) {
                    out = ptr;
                    return true;
                }
            }
            return false;
        }

        template<typename TSymbol>
        std::vector<std::shared_ptr<TSymbol>> collectSymbols() {
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


    class CXXComposite : public CXXSymbol, public CXXNamed {
    private:
        std::string name;
        std::string ns;
        std::vector<CXXField> fields;
    public:
        explicit CXXComposite(std::string name, std::string ns);

        void addField(CXXField&& field);

        const std::vector<CXXField>& getFields() const;

        std::string getName() const override;

        std::string getFullName() const;;
    };

    class CXXTranslationUnit : public CXXScope {
    private:
        std::unordered_map<std::string, un::CXXType> _typeIndex;
    public:
        bool searchType(std::string type, un::CXXType& out) {
            return un::maps::search(_typeIndex, type, out);
        }


        void addType(std::string key, const un::CXXType& type) {
            _typeIndex.emplace(key, type);

        }

        void addType(const un::CXXType& out) {
            addType(out.getName(), out);
        }
    };

}
#endif