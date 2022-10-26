#pragma once
#include "subsystems/render/detail/opaque-render-system.hpp"
#include "subsystems/render/detail/emissive-render-system.hpp"
#include "components/components.hpp"
#include "core/math.hpp"
#include "subsystems/render/model-system.hpp"

namespace engine::render
{
    class Renderer
    {
    public:
        Renderer() = default;
        Renderer(Renderer const &) = delete;
        Renderer &operator= (Renderer const &) = delete;
        Renderer(Renderer &&) = delete;
        Renderer &operator= (Renderer &&) = delete;
        virtual ~Renderer() = default;

        [[nodiscard]] auto &opaque_render_system() const noexcept { return opaque_render_system_; }
        [[nodiscard]] auto &emissive_render_system() const noexcept { return emissive_render_system_; }
        [[nodiscard]] auto &opaque_render_system() noexcept { return opaque_render_system_; }
        [[nodiscard]] auto &emissive_render_system() noexcept { return emissive_render_system_; }

        inline void AddOpaqueInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
        {
            opaque_render_system_.AddInstance(model_id, registry, entity);
        }

        inline void AddOpaqueInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials)
        {
            opaque_render_system_.AddInstance(model_id, registry, entity, materials);
        }

        inline void AddEmissiveInstance(uint64_t model_id, entt::registry &registry, entt::entity entity)
        {
            emissive_render_system_.AddInstance(model_id, registry, entity);
        }

        inline void AddEmissiveInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<EmissiveMaterial> const &materials)
        {
            emissive_render_system_.AddInstance(model_id, registry, entity, materials);
        }

        void Render(entt::registry &registry)
        {
            opaque_render_system_.Render(registry);
            emissive_render_system_.Render();
        }

        void OnInstancesUpdated(entt::registry &registry)
        {
            opaque_render_system_.OnInstancesUpdated(registry);
            emissive_render_system_.OnInstancesUpdated(registry);
        }

    private:
        _opaque_detail::OpaqueRenderSystem opaque_render_system_;
        _emissive_detail::EmissiveRenderSystem emissive_render_system_;
    };
}