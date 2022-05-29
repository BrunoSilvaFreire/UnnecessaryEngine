#include <unnecessary/serializer/jobs/generate_serializer_job.h>

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
            ss << "// Field: " << field.getName() << std::endl;
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
    }
}

un::GenerateSerializerJob::GenerateSerializerJob(
    const std::shared_ptr<un::Buffer>& buffer,
    const std::shared_ptr<un::CXXDeclaration>& toGenerate,
    const un::CXXTranslationUnit* translationUnit
) : buffer(buffer), toGenerate(toGenerate), translationUnit(translationUnit) { }
