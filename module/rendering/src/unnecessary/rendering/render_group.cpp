#include <unnecessary/rendering/render_group.h>

namespace un {

    bool MaterialBatch::operator<(const MaterialBatch& rhs) const {
        return material < rhs.material;
    }

    bool MaterialBatch::operator>(const MaterialBatch& rhs) const {
        return rhs < *this;
    }

    bool MaterialBatch::operator<=(const MaterialBatch& rhs) const {
        return !(rhs < *this);
    }

    bool MaterialBatch::operator>=(const MaterialBatch& rhs) const {
        return !(*this < rhs);
    }

    const std::vector<un::Drawable>& GeometryBatch::getDrawables() const {
        return drawables;
    }

    MaterialBatch::MaterialBatch(Material* material) : material(material) { }

    Material* MaterialBatch::getMaterial() const {
        return material;
    }

    un::GeometryBatch&
    MaterialBatch::getOrCreateMeshBatchFor(const un::DeviceGeometry* const geometry) {
        auto found = geometryBatches.find(geometry);
        if (found != geometryBatches.end()) {
            return found.operator*().second;
        }
        auto iterator = geometryBatches.emplace(
            std::make_pair(
                geometry,
                un::GeometryBatch()
            )
        ).first;
        return iterator->second;
    }

    const MaterialBatch::GeometryMap& MaterialBatch::getGeometryBatches() const {
        return geometryBatches;
    }

    un::MaterialBatch& RenderGroup::getOrCreateBatchFor(un::Material* material) {
        auto found = materialBatches.find(material);
        if (found != materialBatches.end()) {
            return const_cast<MaterialBatch&>(found.operator*());
        }
        return const_cast<MaterialBatch&>(
            materialBatches.emplace(material).first.operator*()
        );
    }

    const std::set<un::MaterialBatch>& RenderGroup::getMaterialBatches() const {
        return materialBatches;
    }

    void RenderGroup::push(
        un::Drawable drawable,
        un::Material* material,
        const un::DeviceGeometry* const geometry
    ) {
        un::MaterialBatch& materialBatch = getOrCreateBatchFor(material);
        un::GeometryBatch& geometryBatch = materialBatch.getOrCreateMeshBatchFor(geometry);
        geometryBatch.drawables.push_back(drawable);
    }
}