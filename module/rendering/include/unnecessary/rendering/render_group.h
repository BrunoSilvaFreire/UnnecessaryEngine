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
    class drawable {
    };

    class geometry_batch {
    private:
        std::vector<drawable> _drawables;

    public:
        friend class render_group;

        const std::vector<drawable>& get_drawables() const;
    };

    class material_batch {
    public:
        using geometry_map = std::unordered_map<const device_geometry*, geometry_batch>;

    private:
        material* _material;
        geometry_map _geometryBatches;

        geometry_batch& get_or_create_mesh_batch_for(const device_geometry* const geometry);

    public:
        material_batch(material* material);

        bool operator<(const material_batch& rhs) const;

        bool operator>(const material_batch& rhs) const;

        bool operator<=(const material_batch& rhs) const;

        bool operator>=(const material_batch& rhs) const;

        material* get_material() const;

        const geometry_map& get_geometry_batches() const;

        friend class render_group;
    };

    class render_group {
    private:
        std::set<material_batch> _materialBatches;

        material_batch& get_or_create_batch_for(material* material);

    public:
        const std::set<material_batch>& get_material_batches() const;

        void push(
            drawable drawable,
            material* material,
            const device_geometry* geometry
        );
    };
}
#endif //UNNECESSARYENGINE_RENDER_GROUP_H
