/*
#ifndef UNNECESSARYENGINE_FREETYPE_ADAPTOR_H
#define UNNECESSARYENGINE_FREETYPE_ADAPTOR_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <filesystem>

namespace un {
    class SDFAdaptor {
        virtual float distanceToClosestPixel(glm::vec2 point) = 0;
    };

    class FreeTypeFaceAdaptor {
    private:
        FT_Face face;
    public:
        static std::vector<un::FreeTypeFaceAdaptor> load_faces(
            const FT_Library& library,
            const std::filesystem::path& file
        ) {
            for (uint32_t i = 0; i < std::numeric_limits<uint32_t>::max(); ++i) {
                FT_Face face;
                FT_Error error = FT_New_Face(
                    library,
                    reinterpret_cast<const char*>(file.c_str()),
                    i,
                    &face
                );

                if (error != FT_Err_Ok) {
                    break;
                }
                un::FreeTypeFaceAdaptor a(library);
            }
        }

        FreeTypeFaceAdaptor(
            FT_Face face
        ) {
            for (uint32_t i = 0; i < std::numeric_limits<uint32_t>::max(); ++i) {
                FT_Error error = FT_New_Face(
                    face,
                    reinterpret_cast<const char*>(file.c_str()),
                    i,
                    &face
                );

                if (error != FT_Err_Ok) {
                    break;
                }
                faces[i] = face;
            }
        }
    };
}


#endif //UNNECESSARYENGINE_FREETYPE_ADAPTOR_H
*/
