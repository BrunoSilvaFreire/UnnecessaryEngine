#include <unnecessary/rendering/render_group.h>

namespace un {
    bool material_batch::operator<(const material_batch& rhs) const {
        return _material < rhs._material;
    }

    bool material_batch::operator>(const material_batch& rhs) const {
        return rhs < *this;
    }

    bool material_batch::operator<=(const material_batch& rhs) const {
        return !(rhs < *this);
    }

    bool material_batch::operator>=(const material_batch& rhs) const {
        return !(*this < rhs);
    }

    const std::vector<Drawable>& geometry_batch::get_drawables() const {
        return drawables;
    }

    material_batch::material_batch(material* material) : _material(material) {
    }

    material* material_batch::get_material() const {
        return _material;
    }

    geometry_batch&
    material_batch::get_or_create_mesh_batch_for(const device_geometry* const geometry) {
        auto found = _geometryBatches.find(geometry);
        if (found != _geometryBatches.end()) {
            return found.operator*().second;
        }
        auto iterator = _geometryBatches.emplace(
            std::make_pair(
                geometry,
                geometry_batch()
            )
        ).first;
        return iterator->second;
    }

    const material_batch::geometry_map& material_batch::get_geometry_batches() const {
        return _geometryBatches;
    }

    material_batch& render_group::get_or_create_batch_for(material* material) {
        auto found = _materialBatches.find(material);
        if (found != _materialBatches.end()) {
            return const_cast<material_batch&>(found.operator*());
        }
        return const_cast<material_batch&>(
            _materialBatches.emplace(material).first.operator*()
        );
    }

    const std::set<material_batch>& render_group::get_material_batches() const {
        return _materialBatches;
    }

    void render_group::push(
        Drawable drawable,
        material* material,
        const device_geometry* const geometry
    ) {
        material_batch& materialBatch = get_or_create_batch_for(material);
        geometry_batch& geometryBatch = materialBatch.get_or_create_mesh_batch_for(geometry);
        geometryBatch.drawables.push_back(drawable);
    }
}
