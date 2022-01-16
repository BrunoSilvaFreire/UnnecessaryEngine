#ifndef UNNECESSARYENGINE_DOT_H
#define UNNECESSARYENGINE_DOT_H

#include <string>
#in
namespace un {
    namespace graphs {

        std::string toDot() const {
            std::stringstream dot;
            dot << "digraph {" << std::endl;
            for (auto[vertexPtr, index] : DependencyGraph<VertexType>::all_vertices()) {
                dot << index << "[label =\"#" << index << ": "
                    << un::to_string(*vertexPtr)
                    << "\" fontname=\"monospace\"" <<
                    vertex_properties(vertexPtr) <<
                    "];" << std::endl;
            }
            for (auto[vertex, index] : DependencyGraph<VertexType>::all_vertices()) {
                for (auto[otherIndex, edge] : DependencyGraph<VertexType>::edges_from(
                    index
                )) {
                    if (edge == un::DependencyType::eUses) {
                        dot << index << "->" << otherIndex << ";" << std::endl;
                    }
                }
            }
            dot << "}";
            return dot.str();
        }

        void saveToDot(const std::filesystem::path& toSaveInto) const {
            std::string content = toDot();
            std::ofstream stream(toSaveInto.string());
            stream << content;
            stream.close();
        }
    }
}
#endif
