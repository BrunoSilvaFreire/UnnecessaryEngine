#include <utility>
#include <fstream>
#include <unnecessary/misc/benchmark.h>
#include <unnecessary/source_analysis/parser.h>
#include <unnecessary/source_analysis/unnecessary_logger.h>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_preprocessor.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_entity_kind.hpp>
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
    ) : file(std::move(file)),
        includes(std::move(includes)),
        debugFile() { }

    const std::vector<std::string>& ParsingOptions::getIncludes() const {
        return includes;
    }

    const std::filesystem::path& ParsingOptions::getDebugFile() const {
        return debugFile;
    }

    void ParsingOptions::setDebugFile(const std::filesystem::path& debugFile) {
        ParsingOptions::debugFile = debugFile;
    }

    std::string ParsingOptions::getSelfInclude() const {
        if (selfInclude.empty()) {
            return std::filesystem::absolute(file).string();
        }
        return selfInclude;
    }

    void ParsingOptions::setSelfInclude(const std::string& selfInclude) {
        ParsingOptions::selfInclude = selfInclude;
    }


    const std::shared_ptr<cppast::cpp_entity_index>& ParsingOptions::getIndexToUse() const {
        return indexToUse;
    }

    void ParsingOptions::setIndexToUse(const std::shared_ptr<cppast::cpp_entity_index>& indexToUse) {
        ParsingOptions::indexToUse = indexToUse;
    }

    Parser::Parser(
        const ParsingOptions& options
    ) : translationUnit(options.getFile(), options.getSelfInclude()),
        index(getIndex(options)) {
        auto work = std::filesystem::current_path();
        un::UnnecessaryLogger logger;
        cppast::libclang_parser parser(type_safe::ref(logger));
        cppast::libclang_compile_config config;
        config.set_flags(cppast::cpp_standard::cpp_20);
        std::string fileName = options.getFile().string();
        for (const std::string& item : options.getIncludes()) {
            config.add_include_dir(item);
        }
        un::Chronometer<> chronometer;
        result = parser.parse(*index, fileName, config);
        auto ms = chronometer.stop();
        LOG(INFO) << "Parsing of file " << options.getFile().filename() << " took " << ms.count() << " ms.";

        if (result == nullptr) {
            LOG(FUCK) << "cppast returned null entity.";
            return;
        }
        const cppast::cpp_file& file = *result;
        const std::filesystem::path& debugFile = options.getDebugFile();
        if (!debugFile.empty()) {
            write_ast(file, debugFile);
        }
        parse_entity(file);
    }

    void Parser::parse_entity(const cppast::cpp_entity& file) {
        cppast::visit(
            file,
            [](const cppast::cpp_entity& e) {
                switch (e.kind()) {
                    case cppast::cpp_entity_kind::class_t:
                    case cppast::cpp_entity_kind::class_template_t:
                    case cppast::cpp_entity_kind::enum_t:
                        return cppast::is_definition(e);
                    case cppast::cpp_entity_kind::include_directive_t: {
                        const auto& directive = dynamic_cast<const cppast::cpp_include_directive&>(e);
//                        return directive.full_path().ends_with(".h");
                        return true;
                    }
                    case cppast::cpp_entity_kind::file_t:
                    case cppast::cpp_entity_kind::namespace_t:
                        return true;
                    default:
                        return false;
                }
            },
            [this](const cppast::cpp_entity& e, cppast::visitor_info info) {
                visit(e);
            }
        );
    }

    void Parser::visit(const cppast::cpp_entity& e) {
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
            case cppast::cpp_entity_kind::enum_t:
                parse_enum(dynamic_cast<const cppast::cpp_enum&>(e));
                break;
            case cppast::cpp_entity_kind::include_directive_t: {
                const auto& directive = dynamic_cast<const cppast::cpp_include_directive&>(e);
                translationUnit.addInclude(directive.name());
                break;
            }
            default:
                break;
        }
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
        std::string typeStr = cppast::to_string(type);
        un::CXXType fieldType;
        if (!translationUnit.searchType(typeStr, fieldType)) {
            cppast::cpp_type_kind typeKind = type.kind();
            CXXTypeKind tType = toUnTypeKind(type);
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

    CXXTypeKind Parser::toUnTypeKind(const cppast::cpp_type& type) const {
        switch (type.kind()) {
            case cppast::cpp_type_kind::builtin_t:
                return toPrimitiveType(type);
            default:
                break;
        }
        return un::CXXTypeKind::eComplex;
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

    void Parser::parse_enum(const cppast::cpp_enum& anEnum) {
        std::vector<un::CXXEnumValue> values;
        std::size_t i = 0;
        for (const cppast::cpp_enum_value& item : anEnum) {
            auto v = item.value();
            if (v.has_value()) {
                continue; //TODO: Parse manually specified this
            }
            values.emplace_back(item.name(), i);
            i++;
        }
        auto parsed = std::make_shared<un::CXXEnum>(
            anEnum.name(),
            getNamespace(anEnum),
            values
        );
        translationUnit.addSymbol(parsed);
    }

    void Parser::write_ast(const cppast::cpp_file& file, const std::filesystem::path& path) {
        std::ofstream os(path);
        std::string prefix;
        // visit each entity in the file
        cppast::visit(
            file, [&](const cppast::cpp_entity& e, cppast::visitor_info info) {
                if (info.event == cppast::visitor_info::container_entity_exit) {
                    prefix.pop_back();
                } else {
                    write_ast_node(os, prefix, e);
                    if (info.event == cppast::visitor_info::container_entity_enter) {
                        prefix += "\t";
                    }
                }
            }
        );
        os.close();
    }

    std::string to_string(cppast::cpp_type_kind kind) {
        switch (kind) {
            case cppast::cpp_type_kind::builtin_t:
                return "builtin";
            case cppast::cpp_type_kind::user_defined_t:
                return "user_defined";
            case cppast::cpp_type_kind::auto_t:
                return "auto";
            case cppast::cpp_type_kind::decltype_t:
                return "decltype";
            case cppast::cpp_type_kind::decltype_auto_t:
                return "decltype_auto";
            case cppast::cpp_type_kind::cv_qualified_t:
                return "cv_qualified";
            case cppast::cpp_type_kind::pointer_t:
                return "pointer";
            case cppast::cpp_type_kind::reference_t:
                return "reference";
            case cppast::cpp_type_kind::array_t:
                return "array";
            case cppast::cpp_type_kind::function_t:
                return "function";
            case cppast::cpp_type_kind::member_function_t:
                return "member_function";
            case cppast::cpp_type_kind::member_object_t:
                return "member_object";
            case cppast::cpp_type_kind::template_parameter_t:
                return "template_parameter";
            case cppast::cpp_type_kind::template_instantiation_t:
                return "template_instantiation";
            case cppast::cpp_type_kind::dependent_t:
                return "dependent";
            case cppast::cpp_type_kind::unexposed_t:
                return "unexposed";
        }
    }

    void Parser::write_ast_node(std::ofstream& os, const std::string& prefix, const cppast::cpp_entity& e) const {
        os << prefix << "'" << e.name() << "' - " << cppast::to_string(e.kind());
        switch (e.kind()) {
            case cppast::cpp_entity_kind::member_variable_t: {
                const cppast::cpp_member_variable& variable = dynamic_cast<const cppast::cpp_member_variable&>(e);
                const cppast::cpp_type& fieldType = variable.type();
                os << " (" << cppast::to_string(fieldType) << ", " <<
                   to_string(fieldType.kind());
//                if (fieldType.kind() == cppast::cpp_type_kind::user_defined_t) {
//                    const auto& definedType = dynamic_cast<const cppast::cpp_user_defined_type&>(fieldType);
//                    const cppast::cpp_type_ref& entityRef = definedType.entity();
//                    auto found = entityRef.get(*index);
//
//                    os << ", found: " << found.size();
//                }
                os << ")";
            }
            default:
                break;
        }
        os << std::endl;
    }

    std::shared_ptr<cppast::cpp_entity_index> Parser::getIndex(const ParsingOptions& options) {
        std::shared_ptr<cppast::cpp_entity_index> toUse = options.getIndexToUse();
        if (toUse == nullptr) {
            return std::make_shared<cppast::cpp_entity_index>();
        }
        return toUse;
    }
}