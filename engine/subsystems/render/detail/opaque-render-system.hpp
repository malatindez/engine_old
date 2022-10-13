#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "entt/entt.hpp"
#include "components/components.hpp"
namespace engine::render
{
    class ModelSystem;
    namespace _opaque_detail
    {
        struct OpaquePerMaterial;
    }
    struct OpaqueMaterial
    {
        ID3D11ShaderResourceView *ambient = nullptr;
        ID3D11ShaderResourceView *albedo_map = nullptr;
        ID3D11ShaderResourceView *normal_map = nullptr;
        ID3D11ShaderResourceView *shininess_map = nullptr;
        ID3D11ShaderResourceView *metalness_map = nullptr;
        ID3D11ShaderResourceView *roughness_map = nullptr;
        ID3D11ShaderResourceView *ambient_occlusion_map = nullptr;
        ID3D11ShaderResourceView *reflection_map = nullptr;
        // opacity ?
        core::math::vec3 ambient_color;
        core::math::vec3 albedo_color;
        core::math::vec3 reflective_color;
        float shininess_value;
        float metalness_value;
        float roughness_value;
        float reflectance_value;
        uint32_t texture_flags;
        bool reverse_normal_y = false;
        bool normal_map_srgb = false;
        core::math::vec2 uv_multiplier{ 1 };
        void UpdateTextureFlags();
        OpaqueMaterial() = default;
        void Bind(direct3d::DynamicUniformBuffer<_opaque_detail::OpaquePerMaterial> &uniform_buffer) const;
        explicit OpaqueMaterial(Material const &material);
        void reset()
        {
            ambient = nullptr;
            albedo_map = nullptr;
            normal_map = nullptr;
            shininess_map = nullptr;
            metalness_map = nullptr;
            roughness_map = nullptr;
            ambient_occlusion_map = nullptr;
            reflection_map = nullptr;
            ambient_color = core::math::vec3{ 0.0f };
            albedo_color = core::math::vec3{ 0.0f };
            reflective_color = core::math::vec3{ 0.0f };
            shininess_value = 0.0f;
            metalness_value = 0.0f;
            roughness_value = 0.0f;
            reflectance_value = 0.0f;
            texture_flags = 0;
            reverse_normal_y = false;
            normal_map_srgb = false;
            uv_multiplier = core::math::vec2{ 1 };
        }
    };
}
namespace std {
    template <>
    struct hash<engine::render::OpaqueMaterial>
    {
        std::size_t operator()(engine::render::OpaqueMaterial const &material) const
        {
            size_t seed = 0;
            engine::utils::hash_combine(seed, material.ambient);
            engine::utils::hash_combine(seed, material.albedo_map);
            engine::utils::hash_combine(seed, material.normal_map);
            engine::utils::hash_combine(seed, material.shininess_map);
            engine::utils::hash_combine(seed, material.metalness_map);
            engine::utils::hash_combine(seed, material.roughness_map);
            engine::utils::hash_combine(seed, material.ambient_occlusion_map);
            engine::utils::hash_combine(seed, material.reflection_map);
            engine::utils::hash_combine(seed, material.ambient_color);
            engine::utils::hash_combine(seed, material.albedo_color);
            engine::utils::hash_combine(seed, material.reflective_color);
            engine::utils::hash_combine(seed, material.shininess_value);
            engine::utils::hash_combine(seed, material.metalness_value);
            engine::utils::hash_combine(seed, material.roughness_value);
            engine::utils::hash_combine(seed, material.reflectance_value);
            engine::utils::hash_combine(seed, material.texture_flags);
            engine::utils::hash_combine(seed, material.reverse_normal_y);
            engine::utils::hash_combine(seed, material.normal_map_srgb);
            engine::utils::hash_combine(seed, material.uv_multiplier);
            return seed;
        }
    };
}
namespace engine::components
{
    struct OpaqueComponent
    {
        uint64_t model_id;
    };
}
namespace engine::render::_opaque_detail
{
    constexpr uint32_t kOpaqueShaderMaxPointLights = 32;
    constexpr uint32_t kOpaqueShaderMaxSpotLights = 32;
    constexpr uint32_t kOpaqueShaderMaxDirectionalLights = 4;
    struct OpaqueInstance
    {
        core::math::mat4 world_transform;
    };
    struct MaterialInstance
    {
        OpaqueMaterial material;
        std::vector<entt::entity> instances;
    };
    struct MeshInstance
    {
        std::vector<MaterialInstance> material_instances;
    };
    struct ModelInstance
    {
        Model &model;
        uint64_t model_id;
        // Each MeshInstance should correspond the one on the same index in model
        std::vector<MeshInstance> mesh_instances;
    };
    struct OpaquePointLight
    {
        core::math::vec3 color;
        float padding;
        core::math::vec3 position;
        float radius;
    };
    struct OpaqueSpotLight
    {
        core::math::vec3 color;
        float padding0;
        core::math::vec3 direction;
        float padding1;
        core::math::vec3 position;
        float cut_off;
    };
    struct OpaqueDirectionalLight
    {
        core::math::vec3 color;
        float padding;
        core::math::vec3 direction;
        float solid_angle;
    };
    struct OpaquePerFrame
    {
        std::array<OpaquePointLight, kOpaqueShaderMaxPointLights> point_lights;
        std::array<OpaqueSpotLight, kOpaqueShaderMaxSpotLights> spot_lights;
        std::array<OpaqueDirectionalLight, kOpaqueShaderMaxDirectionalLights> directional_lights;
        uint32_t num_point_lights;
        uint32_t num_spot_lights;
        uint32_t num_directional_lights;
    };
    struct OpaquePerMaterial
    {
        core::math::vec3 ambient_color;
        float shininess;
        core::math::vec3 albedo_color;
        float metalness;
        core::math::vec3 reflective_color;
        float roughness;
        float reflectance;
        uint32_t enabled_texture_flags;
        core::math::vec2 uv_multiplier;
    };

    auto constexpr opaque_vs_shader_path = "assets/shaders/opaque/opaque-vs.hlsl";
    auto constexpr opaque_ps_shader_path = "assets/shaders/opaque/opaque-ps.hlsl";
    // This class can only be used as a member of ModelSystem
    class OpaqueRenderSystem
    {
    public:
        ModelInstance *GetInstancePtr(uint64_t model_id);
    private:
        friend class ::engine::render::ModelSystem;

        OpaqueRenderSystem();
        void Render(entt::registry &registry);

        ModelInstance &GetInstance(uint64_t model_id);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials);

        void OnInstancesUpdated(entt::registry &registry);
        std::vector<ModelInstance> model_instances_;

        GraphicsShaderProgram opaque_shader_;
        direct3d::DynamicUniformBuffer<OpaquePerFrame> opaque_per_frame_buffer_;
        direct3d::DynamicUniformBuffer<OpaquePerMaterial> opaque_per_material_buffer_;
        direct3d::DynamicUniformBuffer<OpaqueInstance> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<OpaqueInstance> instance_buffer_;
    };
} // namespace engine::render::_opaque_detail