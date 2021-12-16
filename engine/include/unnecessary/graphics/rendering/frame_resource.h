//
// Created by brunorbsf on 02/10/2021.
//

#ifndef UNNECESSARYENGINE_FRAME_RESOURCE_H
#define UNNECESSARYENGINE_FRAME_RESOURCE_H

#include <string>
#include <set>
#include <unnecessary/def.h>

namespace un {

    /**
     * Literally any kind of resource
     */
    class FrameResource {
    private:
        std::string name;
        /**
         * This stores a cache of passes that read and write to this.
         */
        std::set<u32> readPasses, writePasses;
    public:
        explicit FrameResource(std::string name);

        friend class RenderPassDescriptor;
    };

    class FrameResourceReference {
    private:
        u32 index;
        FrameResource* resource;
    public:
        FrameResourceReference(u32 index, FrameResource* resource);

        u32 getIndex() const;

        FrameResource* getResource() const;
    };

};
#endif //UNNECESSARYENGINE_FRAME_RESOURCE_H
