#include <unnecessary/application.h>
#include <unnecessary/world.h>
#include <unnecessary/systems/transform.h>
#include <unnecessary/graphics/shader.h>
#include <unnecessary/graphics/pipeline_spec.h>

int main() {
    un::Application app("Demo", un::Version(0, 1, 0));
    un::World world(app);
    vk::Device &device = app.getRenderer().getVirtualDevice();
    un::ShaderStage frag("standart.frag", device);
    un::ShaderStage vert("standart.vert", device);
    un::Shader shader(device, {frag, vert});
    un::GraphicsPipelineSpecification specification;
    specification.vertexInput().
    specification.addStages(frag, vert);
    world.addSystem<un::TransformSystem>();
    entt::registry &registry = world.getRegistry();
    entt::entity cube = registry.create();
    registry.emplace<un::LocalToWorld>(cube);
    registry.emplace<un::Translation>(cube);
    app.execute();
}