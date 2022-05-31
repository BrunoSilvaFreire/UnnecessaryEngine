#include <unnecessary/serializer/jobs/generate_serializer_job.h>

#include <utility>

namespace un {
    void GenerateSerializerJob::operator()(un::JobWorker* worker) {
        std::stringstream ss;
        ss << "#ifndef UN_SERIALIZER_GENERATED_" << info.upper << std::endl;
        ss << "#define UN_SERIALIZER_GENERATED_" << info.upper << std::endl;
        std::shared_ptr<CXXComposite> asComp = std::dynamic_pointer_cast<un::CXXComposite>(toGenerate);
        if (asComp != nullptr) {
            generateCompositeSerializer(ss, asComp);
        }

        ss << "#endif" << std::endl;
        buffer->operator=(ss.str());
    }

    void GenerateSerializerJob::generateInfoComments(std::stringstream& ss, const std::shared_ptr<CXXComposite>& ptr) {
        ss << "// Serialization info: " << std::endl;
        for (const auto& field : ptr->getFields()) {
            auto att = field.findAttribute("un::serialize");
            if (att == nullptr) {
                continue;
            }
            const std::vector<un::CXXAttribute>& attributes = field.getAttributes();
            ss << "// Field: " << field.getName() << " (" << field.getType().getFullName() << ")" << std::endl;
            if (attributes.empty()) {
                ss << "// No attributes. " << std::endl;
            } else {
                ss << "// Attributes (" << attributes.size() << "):" << std::endl;
                for (const un::CXXAttribute& att : attributes) {
                    ss << "// " << att.getName();
                    const std::set<std::string>& args = att.getArguments();
                    if (!args.empty()) {
                        ss << ", args: " << un::join_strings(args.begin(), args.end());
                    }
                    ss << std::endl;
                }
            }
        }
    }

    void GenerateSerializerJob::generateCompositeSerializer(
        std::stringstream& ss,
        const std::shared_ptr<CXXComposite>& comp
    ) {
        generateInfoComments(ss, comp);
        ss << "namespace un {" << std::endl;
        // static serialization
        ss << "namespace serialization {" << std::endl;
        ss << "}" << std::endl;
        // static serialization

        ss << "class " << info.name << "Serializer final : public un::Serializer<" << info.fullName
           << "> {"
           << std::endl;
        ss << "inline virtual void serialize(const " << info.fullName
           << "& value, un::Serialized& into) override {"
           << std::endl;
        ss << "un::serialization::serialize(value, into);" << std::endl;
        ss << "}" << std::endl;

        ss << "inline virtual " << info.fullName << " deserialize(un::Serialized& from) override {"
           << std::endl;
        ss << "return un::serialization::deserialize<" << info.fullName << ">(from);" << std::endl;
        ss << "}" << std::endl;

        ss << "};" << std::endl;
        ss << "}" << std::endl;

    }
}

un::GenerateSerializerJob::GenerateSerializerJob(
    std::shared_ptr<un::Buffer> buffer,
    const std::shared_ptr<un::CXXDeclaration>& toGenerate,
    const un::CXXTranslationUnit* translationUnit
) : buffer(std::move(buffer)), toGenerate(toGenerate), translationUnit(translationUnit) {
    std::string name = toGenerate->getName();
    info.name = name;
    info.upper = un::upper(name);
    info.lower = un::lower(name);
    info.fullName = toGenerate->getFullName();
}
