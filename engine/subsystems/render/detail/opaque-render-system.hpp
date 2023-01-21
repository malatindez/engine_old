#pragma once
#include "render/model.hpp"
#include "render/shader-program.hpp"
#include "entt/entt.hpp"
#include "components/components.hpp"
#include "render/common.hpp"
namespace engine::core
{
    class Scene;
}
namespace engine::render
{
    class ModelSystem;
    namespace _light_detail
    {
        class LightRenderSystem;
    }
    namespace _opaque_detail
    {
        struct OpaquePerMaterial;
        class OpaqueRenderSystem;
    }
    struct OpaqueMaterial
    {
        ID3D11ShaderResourceView *albedo_map = nullptr;
        ID3D11ShaderResourceView *normal_map = nullptr;
        ID3D11ShaderResourceView *metalness_map = nullptr;
        ID3D11ShaderResourceView *roughness_map = nullptr;

        // Used only to discard pixels with opacity < 0.5
        ID3D11ShaderResourceView *opacity_map = nullptr;
        core::math::vec3 albedo_color;
        float metalness_value;
        float roughness_value;
        uint32_t texture_flags;
        bool reverse_normal_y = false;
        bool twosided = false;
        core::math::vec2 uv_multiplier{1};
        void UpdateTextureFlags();
        OpaqueMaterial() = default;
        void BindTextures() const;
        void Bind(direct3d::DynamicUniformBuffer<_opaque_detail::OpaquePerMaterial> &uniform_buffer) const;
        explicit OpaqueMaterial(Material const &material);
        void reset()
        {
            albedo_map = nullptr;
            normal_map = nullptr;
            metalness_map = nullptr;
            roughness_map = nullptr;
            opacity_map = nullptr;
            albedo_color = core::math::vec3{0.0f};
            metalness_value = 0.01f;
            roughness_value = 0.01f;
            texture_flags = 0;
            reverse_normal_y = false;
            twosided = false;
            uv_multiplier = core::math::vec2{1};
        }
    };
}
namespace std
{
    template <>
    struct hash<engine::render::OpaqueMaterial>
    {
        std::size_t operator()(engine::render::OpaqueMaterial const &material) const
        {
            size_t seed = 0;
            engine::utils::hash_combine(seed, material.albedo_map);
            engine::utils::hash_combine(seed, material.normal_map);
            engine::utils::hash_combine(seed, material.metalness_map);
            engine::utils::hash_combine(seed, material.roughness_map);
            engine::utils::hash_combine(seed, material.albedo_color);
            engine::utils::hash_combine(seed, material.metalness_value);
            engine::utils::hash_combine(seed, material.roughness_value);
            engine::utils::hash_combine(seed, material.texture_flags);
            engine::utils::hash_combine(seed, material.reverse_normal_y);
            engine::utils::hash_combine(seed, material.uv_multiplier);
            return seed;
        }
    };
}
namespace engine::components
{

    /**
     * @brief Component that contains the data for opaque instanced rendering
     */
    struct OpaqueComponent
    {
        /**
         * @brief Model id in the ModelSystem class
         */
        size_t model_id = std::numeric_limits<size_t>::max();
        /**
         * @brief Material hash to find the material in the system
         * These are used to update the instance and change it's materials
         * You need to call entt::registry.patch<OpaqueComponent>(entity) so the system would catch the change
         */
        std::vector<size_t> material_hashes = {};

    private:
        friend class _opaque_detail::OpaqueRenderSystem;
        
        void instance_reset()
        {
            saved_model_id = std::numeric_limits<size_t>::max();
            saved_material_hashes = {};
            model_instance_id = std::numeric_limits<size_t>::max();
            mesh_instance_ids = {};
        }
        /**
         * @brief Global model id in the ModelSystem class just to compare if the changes were made
         */
        size_t saved_model_id = std::numeric_limits<size_t>::max();
        /**
         * @brief saved mesh hashes so we can check if something has changed
         */
        std::vector<size_t> saved_material_hashes = {};
        /**
         * @brief Local model id in the system
         */
        size_t model_instance_id = std::numeric_limits<size_t>::max();
        /**
         * @brief Local mesh ids in the system
         */
        std::vector<size_t> material_instance_ids = {};
    };
}
namespace engine::render::_opaque_detail
{
    struct OpaqueInstance
    {
        core::math::mat4 world_transform;
        uint32_t entity_id;
    };
    struct MaterialInstance
    {
        /// @brief Material to use
        OpaqueMaterial material;
        /// @brief Vector of the entity instances. It is used to get the transform component from the registry
        std::vector<entt::entity> instances;
    };
    struct MeshInstance
    {
        /// @brief Copy of the mesh to model matrix located in ModelMesh
        core::math::mat4 mesh_to_model;
        /// @brief Copy of the inverse of the mesh to model matrix located in ModelMesh
        core::math::mat4 inv_mesh_to_model;
        /// @brief Copy of the mesh range located in Model class
        MeshRange mesh_range;
        /// @brief Vector of the material instances
        std::vector<MaterialInstance> material_instances;
    };
    struct ModelInstance
    {
        /// @brief Pointer to the model in the ModelSystem class, just to mark that the model is used
        std::shared_ptr<Model> model;
        /// @brief ID3D11Buffer * copy from the Model class, just so it's stored locally
        direct3d::ImmutableVertexBuffer<Vertex> vertices;
        /// @brief ID3D11Buffer * copy from the Model class, just so it's stored locally
        direct3d::ImmutableIndexBuffer<uint32_t> indices;
        /// @brief Id of the model in the ModelSystem
        size_t model_id;
        /// @brief Vector of the mesh instances
        /// @note Each MeshInstance should correspond to the one on the same index in Model
        std::vector<MeshInstance> mesh_instances;
    };
    struct OpaquePerMaterial
    {
        core::math::vec3 albedo_color;
        float metalness;
        float roughness;
        uint32_t enabled_texture_flags;
        core::math::vec2 uv_multiplier;
    };
    struct OpaquePerDepthCubemap
    {
        std::array<core::math::mat4, 6> g_view_projection;
        uint32_t g_slice_offset;
        core::math::vec3 padding0;
    };
    struct OpaquePerDepthTexture
    {
        core::math::mat4 g_view_projection;
        uint32_t g_slice_offset;
        core::math::vec3 padding0;
    };

    auto constexpr opaque_vs_shader_path = "assets/shaders/opaque/opaque-vs.hlsl";
    auto constexpr opaque_ps_shader_path = "assets/shaders/opaque/opaque-ps.hlsl";
    auto constexpr opaque_vs_depth_only_shader_path = "assets/shaders/opaque/opaque-depth-only-vs.hlsl";
    auto constexpr opaque_gs_depth_only_cubemap_shader_path = "assets/shaders/opaque/opaque-depth-only-cubemap-gs.hlsl";
    auto constexpr opaque_gs_depth_only_texture_shader_path = "assets/shaders/opaque/opaque-depth-only-texture-gs.hlsl";
    auto constexpr opaque_ps_depth_only_shader_path = "assets/shaders/opaque/opaque-depth-only-ps.hlsl";

    /**
     * @brief These are helper structs that are used to properly fill changes and then update them all at once
     * It will help to update instance array by reusing existing data and we won't be updating unchanged entities
     */
    namespace _opaque_helpers
    {
        struct MaterialInstanceChanges
        {
            // self-explanatory
            std::vector<entt::entity> added_entities{};
            std::vector<entt::entity> removed_entities{};
            std::vector<entt::entity> updated_entities{};
        };
        struct MeshInstanceChange
        {
            // material changes corresponding to material id
            std::map<material_id, MaterialInstanceChanges> material_changes;
        };
        struct ModelInstanceChange
        {
            /**
             * @brief Mesh changes in the current model
             * @note guaranteed to be the same size as the amount of meshes in the model
             */
            std::vector<MeshInstanceChange> mesh_changes;
        };
    }
    // TODO:
    // update opaque, emissive and skybox rendering systems so we don't have to call AddInstance
    // We will use on_construct and on_destruct to change it
    // TODO:
    // We should store list of all available materials as an unordered map of opaque materials in opaque render system.
    // This way we can avoid creating new materials every time we add a new instance
    // TODO:
    // We should create OpaqueComponent and store the vector of size_t hashes of materials in it
    // and store model_id as well

    // We will probably need to have the private cache variable in it so we can delete instances of materials that aren't used

    // on_destruct / on_update / on_construct should update the instance tree using these materials
    // if hash differs -> update material, update cache, delete previous instance
    // if hash is the same -> do nothing

    // TODO:
    // Change the system the way that on_destruct / on_construct will set the dirty flag
    // and add data that should be updated so we can update instances right before the frame if needed

    class OpaqueRenderSystem final : public RenderPass
    {
    public:
        ModelInstance *GetInstancePtr(uint64_t model_id);
        OpaqueRenderSystem();
        void OnRender(core::Scene *scene);
        void RenderDepthOnly(std::vector<OpaquePerDepthCubemap> const &cubemaps, core::Scene *scene);
        void RenderDepthOnly(std::vector<OpaquePerDepthTexture> const &textures, core::Scene *scene);

        ModelInstance &GetInstance(uint64_t model_id);
        /// @todo Remove these functions and call registry.patch<OpaqueComponent>(entity) so the observer will catch the change
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity);
        void AddInstance(uint64_t model_id, entt::registry &registry, entt::entity entity, std::vector<OpaqueMaterial> const &materials);

        void Update(core::Scene *scene);

        void LinkToRegistry(entt::registry &registry);

        size_t AddMaterial(OpaqueMaterial &&material)
        {
            size_t rv = std::hash<OpaqueMaterial>()(material);
            materials.insert(std::move(material))
            return rv;
        }

    private:
        using ModelInstanceId = size_t;
        void UpdateInstances(entt::registry &registry);

        /// @brief Observes the changes of the opaque components in the registry
        /// @param registry scene's registry, the same that is linked by LinkToRegistry
        /// @todo Maybe it would be better to store registry here, in OpaqueRenderSystem, if it is already linked, but i'm not sure for now.
        void ObserveOpaqueChanges(entt::registry &registry);

        /// @brief Observes the changes of the transform components in the registry
        /// @param registry scene's registry, the same that is linked by LinkToRegistry
        void ObserveTransformChanges(entt::registry &registry);

        /// @brief Processes the changes written to the model_instance_changes_ member
        /// @param registry scene's registry, the same that is linked by LinkToRegistry
        void ProcessTreeChanges(entt::registry &registry);
        /**
         * @brief This method is used to update the change tree and set the entities to remove
         * @param component Component with saved data to change 
         * @param entity entity to push
         */
        void CacheRemoveInstance(components::OpaqueComponent &component, entt::entity entity);
        /**
         * @brief This method is used to update the change tree and set the entities to add
         * @param component Component with saved data to change 
         * @param entity entity to push
         */
        void CacheAddInstance(components::OpaqueComponent &component, entt::entity entity);
        /**
         * @brief This method is used to update the change tree and set the entities to update
         * @param component Component with saved data to change 
         * @param entity entity to push
         */
        void CacheUpdateInstance(components::OpaqueComponent &component, entt::entity entity);
        // TODO:
        void on_attach(entt::registry &, entt::entity) {}
        void on_destroy(entt::registry &, entt::entity) {}

        bool is_instance_update_scheduled_ = false;
        std::vector<OpaqueMaterial> materials;
        std::vector<ModelInstance> model_instances_;

        std::map<ModelInstanceId, _opaque_helpers::ModelInstanceChange> model_instance_changes_;

        GraphicsShaderProgram opaque_cubemap_shader_;
        GraphicsShaderProgram opaque_texture_shader_;
        direct3d::DynamicUniformBuffer<OpaquePerDepthCubemap> opaque_per_cubemap_buffer_;
        direct3d::DynamicUniformBuffer<OpaquePerDepthTexture> opaque_per_texture_buffer_;

        GraphicsShaderProgram opaque_shader_;
        direct3d::DynamicUniformBuffer<OpaquePerMaterial> opaque_per_material_buffer_;
        direct3d::DynamicUniformBuffer<core::math::mat4> mesh_to_model_buffer_;
        direct3d::DynamicVertexBuffer<OpaqueInstance> instance_buffer_;

        std::unordered_set<OpaqueMaterial> materials;

        /// @brief Observes components where the opaque component data has changed
        entt::observer opaque_component_update_observer_;
        
        /// @brief Observes components where the transform component has changed
        /// This way we don't need to reconstruct instances, just to update them
        entt::observer transform_component_update_observer_;
    };
} // namespace engine::render::_opaque_detail
#include "opaque-render-system.inl"