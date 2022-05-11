#include <utility>
#include <unnecessary/source_analysis/parser.h>
#include <unnecessary/misc/benchmark.h>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_class_template.hpp>
#include <cppast/visitor.hpp>


namespace un::parsing {
    const std::filesystem::path& ParsingOptions::getFile() const {
        return file;
    }

    ParsingOptions::ParsingOptions(
        std::filesystem::path file,
        std::vector<std::string> includes
    ) : file(std::move(file)), includes(std::move(includes)) { }

    const std::vector<std::string>& ParsingOptions::getIncludes() const {
        return includes;
    }

    Parser::Parser(const ParsingOptions& options) {
        auto work = std::filesystem::current_path();
        cppast::libclang_parser parser;
        cppast::libclang_compile_config config;
        std::string fileName = options.getFile().string();
        LOG(INFO) << "Parsing file: " << fileName;
        for (const std::string& item : options.getIncludes()) {
            LOG(INFO) << "include: " << item;
            config.add_include_dir(item);
        }
        cppast::cpp_entity_index index;
        un::Chronometer<> chronometer;
        result = parser.parse(index, fileName, config);
        auto ms = chronometer.stop();
        LOG(INFO) << "Parsing of file " << options.getFile().filename() << " took " << ms;

        if (result == nullptr) {
            LOG(FUCK) << "cppast returned null entity.";
            return;
        }
        const cppast::cpp_file& file = *result;
        std::set<std::string> alreadyParsed;
        cppast::visit(
            file,
            [](const cppast::cpp_entity& e) {
                switch (e.kind()) {
                    case cppast::cpp_entity_kind::class_t:
                        return cppast::is_definition(e);
                    case cppast::cpp_entity_kind::class_template_t:
                        return cppast::is_definition(e);
                    case cppast::cpp_entity_kind::namespace_t:
                        return true;
                    default:
                        return false;
                }
            },
            [&](const cppast::cpp_entity& e, cppast::visitor_info info) {
                if (alreadyParsed.contains(e.name())) {
                    return;
                }
                alreadyParsed.emplace(e.name());
                switch (e.kind()) {
                    case cppast::cpp_entity_kind::class_t:
                        parse_class(dynamic_cast<const cppast::cpp_class&>(e));
                        break;
                    case cppast::cpp_entity_kind::class_template_t:
                        parse_template(dynamic_cast<const cppast::cpp_class_template&>(e));
                        break;
                    default:
                        break;
                }
            }
        );
    }

    void Parser::parse_class(const cppast::cpp_class& clazz) {
        un::CXXComposite composite(clazz.name(), getNamespace(clazz));
        un::CXXAccessModifier modifier = un::CXXAccessModifier::eNone;
        cppast::visit(
            clazz,
            [](const cppast::cpp_entity& e) {
                switch (e.kind()) {
                    case cppast::cpp_entity_kind::member_variable_t :
                    case cppast::cpp_entity_kind::access_specifier_t:
                        return true;
                    default:
                        return false;
                }
            },
            [&](const cppast::cpp_entity& e, cppast::visitor_info info) {
                switch (e.kind()) {
                    case cppast::cpp_entity_kind::member_variable_t : {
                        parse_field(composite, modifier, e);
                        break;
                    }
                    case cppast::cpp_entity_kind::access_specifier_t: {
                        const auto& specifier = dynamic_cast<const cppast::cpp_access_specifier&>(e);
                        switch (specifier.access_specifier()) {
                            case cppast::cpp_public:
                                currentAccess = un::CXXAccessModifier::ePublic;
                                break;
                            case cppast::cpp_protected:
                                currentAccess = un::CXXAccessModifier::eProtected;
                                break;
                            case cppast::cpp_private:
                                currentAccess = un::CXXAccessModifier::ePrivate;
                                break;
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
        );
        translationUnit.addSymbol(std::make_shared<un::CXXComposite>(composite));
    }

    void Parser::parse_field(CXXComposite& composite, const CXXAccessModifier& modifier, const cppast::cpp_entity& e) {
        const auto& var = dynamic_cast<const cppast::cpp_member_variable&>(e);
        std::vector<CXXAttribute> attributes;
        const cppast::cpp_type& type = var.type();
        CXXType fieldType = toUnType(type);
        for (const auto& att : e.attributes()) {
            const auto& args = att.arguments();

            std::string name;
            if (att.scope()) {
                name.append(att.scope().value());
                name.append("::");
            }
            name.append(att.name());
            if (args) {
                std::vector<std::string> a;
                for (const auto& item : args.value()) {
                    a.emplace_back(item.spelling);
                }
                attributes.emplace_back(name, a);
            } else {
                attributes.emplace_back(name);
            }
        }
        CXXField field(modifier, var.name(), fieldType, attributes);
        composite.addField(std::move(field));
    }

    std::string Parser::getNamespace(const cppast::cpp_entity& entt) const {
        std::stack<std::string> segments;
        auto aNamespace = dynamic_cast<const cppast::cpp_namespace*>(&entt.parent().value());
        while (aNamespace != nullptr) {
            segments.push(aNamespace->name());
            aNamespace = dynamic_cast<const cppast::cpp_namespace*>(&aNamespace->parent().value());
        }
        std::vector<std::string> inOrder;
        while (!segments.empty()) {
            inOrder.push_back(segments.top());
            segments.pop();
        }
        return un::join_strings("::", inOrder.begin(), inOrder.end());
    }

    un::CXXType Parser::toUnType(const cppast::cpp_type& type) {
        std::string typeStr = to_string(type);
        un::CXXType fieldType;
        if (!translationUnit.searchType(typeStr, fieldType)) {
            CXXTypeKind tType;
            cppast::cpp_type_kind typeKind = type.kind();
            switch (typeKind) {
                case cppast::cpp_type_kind::builtin_t:
                    tType = toPrimitiveType(type);
                    break;
                default:
                    tType = eComplex;
                    break;
            }
            if (typeKind == cppast::cpp_type_kind::template_instantiation_t) {
                const auto& instantiation = dynamic_cast<const cppast::cpp_template_instantiation_type&>(type);
                fieldType = CXXType(instantiation.primary_template().name(), tType);
                if (instantiation.arguments_exposed()) {
                    const auto& optArgs = instantiation.arguments();
                    if (optArgs) {
                        for (const auto& item : optArgs.value()) {
                            const auto& optType = item.type();
                            if (!optType) {
                                LOG(WARN) << "Unable to find type of argument in " << typeStr;
                                continue;
                            }
                            const cppast::cpp_type& cppType = optType.value();
                            CXXType unType = toUnType(cppType);
                            fieldType.addTemplate(std::move(unType.getName()));
                        }
                    } else {
                        LOG(WARN) << typeStr << " is a template instantiation, but no args were found";
                    }
                } else {

                    const std::string& unexposedArguments = instantiation.unexposed_arguments();
                    std::vector<std::string> splitArgs = un::split_string(unexposedArguments, ",");
                    for (const auto& arg : splitArgs) {
                        std::string trimmed = un::trim_whitespace_prefix(arg);
                        fieldType.addTemplate(std::move(trimmed));
                    }
                }
            } else {
                fieldType = CXXType(typeStr, tType);
            }
            translationUnit.addType(fieldType);
        }
        return fieldType;
    }

    CXXTypeKind Parser::toPrimitiveType(const cppast::cpp_type& type) const {
        CXXTypeKind tType;
        const auto& builtIn = dynamic_cast<const cppast::cpp_builtin_type&>(type);
        switch (builtIn.builtin_type_kind()) {
            case cppast::cpp_bool:
                tType = eBool;
                break;
            case cppast::cpp_uchar:
                tType = eUnsignedInteger8;
                break;
            case cppast::cpp_ushort:
                tType = eUnsignedInteger16;
                break;
            case cppast::cpp_uint:
                tType = eUnsignedInteger32;
                break;
            case cppast::cpp_ulong:
                tType = eUnsignedInteger64;
                break;
            case cppast::cpp_ulonglong:
            case cppast::cpp_uint128:
                tType = eUnsignedInteger128;
                break;
            case cppast::cpp_schar:
                tType = eInteger8;
                break;
            case cppast::cpp_short:
                tType = eInteger16;
                break;
            case cppast::cpp_int:
                tType = eInteger32;
                break;
            case cppast::cpp_long:
                tType = eInteger64;
                break;
            case cppast::cpp_longlong:
            case cppast::cpp_int128:
                tType = eInteger128;
                break;
            case cppast::cpp_float:
                tType = eFloat;
                break;
            case cppast::cpp_double:
                tType = eDouble;
                break;
        };
        return tType;
    }

    void Parser::parse_template(const cppast::cpp_class_template& clazz) {
        for (const auto& item : clazz) {

        }

    }
}