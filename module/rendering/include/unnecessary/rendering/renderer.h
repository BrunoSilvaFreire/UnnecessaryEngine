#ifndef UNNECESSARYENGINE_RENDERER_H
#define UNNECESSARYENGINE_RENDERER_H
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <unnecessary/version.h>
#include <unnecessary/logging.h>
#include <unnecessary/rendering/rendering_device.h>
#include <unnecessary/rendering/swapchain.h>
#include <unnecessary/rendering/window.h>
#include <unnecessary/rendering/render_graph.h>
#include <unnecessary/rendering/rendering_pipeline.h>
#include <unnecessary/rendering/debug/vulkan_debugger.h>

#ifdef UN_VULKAN_DEBUG

#include <unnecessary/rendering/debug/vulkan_debugger.h>

#endif
#ifdef DEBUG

#include <grapphs/dot.h>

#endif
namespace un {
    class renderer {
    private:
        window* _window;
        vk::Instance _vulkan;
        rendering_device _device;
#ifdef UN_VULKAN_DEBUG
        vulkan_debugger _debugger;
#endif
        swap_chain _swapChain;
        render_graph _graph;

    public:
        renderer(window* window, std::string string, version version);

        void use_pipeline(rendering_pipeline* pipeline) {
            pipeline->setup(*this, _graph);
            _graph.bake(*this);
            auto vkPass = _graph.get_vulkan_pass();
            dependency_graph<std::unique_ptr<render_pass>>::inner_graph& innerGraph = _graph.get_inner_graph();
            for (auto [index, passPtr] : innerGraph.all_vertices()) {
                (*passPtr)->on_vulkan_pass_created(vkPass, *this);
            }
#ifdef DEBUG
            auto cwd = std::filesystem::current_path();
            auto output = cwd / "render_graph.dot";
            LOG(INFO) << "Printing pipeline render graph to " << output;
            save_to_dot(innerGraph, output);
#endif
        }

        window* get_window() const {
            return _window;
        }

        const vk::Instance& get_vulkan() const {
            return _vulkan;
        }

        vk::Device get_virtual_device() const {
            return _device.get_virtual_device();
        }

        vk::PhysicalDevice get_physical_device() const {
            return _device.get_physical_device();
        }

        const rendering_device& get_device() const;

        const render_graph& get_render_graph() const {
            return _graph;
        }

#if DEBUG

        const vulkan_debugger& get_debugger() const;

#endif

        const swap_chain& get_swap_chain() const;

        swap_chain& get_swap_chain();

        template<typename ValueType>
        UN_AGGRESSIVE_INLINE void tag(
            ValueType value,
            const std::string& tagName
        ) const {
#ifdef DEBUG
            _debugger.tag<ValueType>(value, tagName);
#endif
        }
    };
}
#endif
