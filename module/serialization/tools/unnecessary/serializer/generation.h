//
// Created by brunorbsf on 08/05/22.
//

#ifndef UNNECESSARYENGINE_GENERATION_H
#define UNNECESSARYENGINE_GENERATION_H

#include <string>
#include <filesystem>
#include <unnecessary/strings.h>
#include <unnecessary/misc/files.h>
#include <unnecessary/source_analysis/parsing.h>

namespace un {

    struct GenerationInfo {
        std::string ownInclude;
        std::string name;
        std::string fullName;
        std::string upper;
        std::string lower;
    };

    std::string getGeneratedIncludeName(const GenerationInfo& info);

    std::string getGeneratedIncludeName(const std::string& typeName);

    void generateSerializerInclude(const std::filesystem::path& includesOutput,
                                   const std::shared_ptr<un::CXXComposite>& composite, const GenerationInfo& info,
                                   const std::vector<std::string>& additionalIncludes);

    bool isSerializable(const std::shared_ptr<un::CXXComposite>& composite);

}
#endif //UNNECESSARYENGINE_GENERATION_H
