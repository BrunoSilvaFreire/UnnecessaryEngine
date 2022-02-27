//
// Created by bruno on 21/12/2021.
//

#ifndef UNNECESSARYENGINE_RENDER_GROUP_H
#define UNNECESSARYENGINE_RENDER_GROUP_H

#include <unordered_map>
#include <unnecessary/rendering/material.h>
#include <unnecessary/rendering/geometry.h>
#include <set>


namespace un {
    class Drawable {
    };

    class GeometryBatch {
    private:
        std::vector<un::Drawable> drawables;
    public:
        friend class RenderGroup;

        const std::vector<un::Drawable>& getDrawables() const;
    };

    class MaterialBatch {
    public:
        typedef std::unordered_map<const un::DeviceGeometry*, un::GeometryBatch> GeometryMap;

    private:
        un::Material* material;
        GeometryMap geometryBatches;

        un::GeometryBatch& getOrCreateMeshBatchFor(const un::DeviceGeometry* const geometry);

    public:
        MaterialBatch(Material* material);

        bool operator<(const MaterialBatch& rhs) const;

        bool operator>(const MaterialBatch& rhs) const;

        bool operator<=(const MaterialBatch& rhs) const;

        bool operator>=(const MaterialBatch& rhs) const;

        Material* getMaterial() const;

        const GeometryMap& getGeometryBatches() const;

        friend class RenderGroup;
    };

    class RenderGroup {
    private:
        std::set<un::MaterialBatch> materialBatches;

        un::MaterialBatch& getOrCreateBatchFor(un::Material* material);

    public:
        const std::set<un::MaterialBatch>& getMaterialBatches() const;

        void push(
            un::Drawable drawable,
            un::Material* material,
            const un::DeviceGeometry* const geometry
        );
    };

}
#endif //UNNECESSARYENGINE_RENDER_GROUP_H
