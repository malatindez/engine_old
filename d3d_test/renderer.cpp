#include "renderer.hpp"
using namespace engine;
using namespace core;
using namespace direct3d;
using namespace math;
using namespace ShaderCompiler;
void Renderer::OnGuiRender()
{
    ImGui::Begin("Cube render settings");
    const char *items[] = { "PointWrapSampler","PointClampSampler", "LinearWrapSampler","LinearClampSampler", "AnisotropicWrapSampler","AnisotropicClampSampler" };
    static const char *current_item = "PointWrapSampler";
    static int current_item_id = 0, previous_item_id = 0;
    if (ImGui::BeginCombo("Sampler", current_item, ImGuiComboFlags_NoArrowButton))
    {
        current_item_id = -1;
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (current_item == items[n]);
            if (ImGui::Selectable(items[n], is_selected))
            {
                current_item = items[n];
                current_item_id = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
                current_item_id = n;
            }
        }
        ImGui::EndCombo();
        if (current_item_id != previous_item_id)
        {
            previous_item_id = current_item_id;
            switch (current_item_id)
            {
            case 0:
                current_state = direct3d::states::point_wrap_sampler.as_default_wrapper();
                spdlog::info("PointWrapSampler is bound for cube.");
                break;
            case 1:
                current_state = direct3d::states::point_clamp_sampler.as_default_wrapper();
                spdlog::info("PointClampSampler is bound for cube.");
                break;
            case 2:
                current_state = direct3d::states::linear_wrap_sampler.as_default_wrapper();
                spdlog::info("LinearWrapSampler is bound for cube.");
                break;
            case 3:
                current_state = direct3d::states::linear_clamp_sampler.as_default_wrapper();
                spdlog::info("LinearClampSampler is bound for cube.");
                break;
            case 4:
                current_state = direct3d::states::anisotropic_wrap_sampler.as_default_wrapper();
                spdlog::info("AnisotropicWrapSampler is bound for cube.");
                break;
            case 5:
                current_state = direct3d::states::anisotropic_clamp_sampler.as_default_wrapper();
                spdlog::info("AnisotropicClampSampler is bound for cube.");
                break;
            default:
                current_state = direct3d::states::point_wrap_sampler.as_default_wrapper();
                spdlog::info("PointWrapSampler is bound for cube.");
                break;
            }

        }
    }
    ImGui::End();
}
Renderer::Renderer(ivec2 const &screen_resolution) : screen_resolution(screen_resolution)
{
    current_state = direct3d::states::point_wrap_sampler.as_default_wrapper();
    spdlog::info("PointWrapSampler is bound for cube.");


    std::vector<D3D11_INPUT_ELEMENT_DESC> d3d_input_desc{
     {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
     {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    auto path = std::filesystem::current_path();
    auto vs = core::ShaderManager::instance()->CompileVertexShader(path / "assets/shaders/test-cube/test-cube.hlsl");
    auto ps = core::ShaderManager::instance()->CompilePixelShader(path / "assets/shaders/test-cube/test-cube.hlsl");
    auto il = std::make_shared<render::InputLayout>(vs->blob(), d3d_input_desc);
    test_shader.SetVertexShader(vs).SetPixelShader(ps).SetInputLayout(il);
}

void Renderer::DrawTestCube()
{

    test_shader.Bind();
    direct3d::api::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    direct3d::api::devcon4->RSSetState(direct3d::states::cull_back);
    direct3d::api::devcon4->PSSetSamplers(0, 1, &current_state.ptr());
    direct3d::api::devcon4->OMSetDepthStencilState(direct3d::states::geq_depth, 0);
    direct3d::api::devcon4->OMSetBlendState(nullptr, nullptr, 0xffffffff); // use default blend mode (i.e. disable)

    DynamicUniformBuffer<math::mat4> transform_buffer;
    transform_buffer.Bind(direct3d::ShaderType::VertexShader, 1);

    for (auto entity : Engine::scene()->registry.view<components::TransformComponent, components::ModelInstanceComponent, TestCubeComponent>())
    {
        auto model_component = Engine::scene()->registry.get<components::ModelInstanceComponent>(entity);
        auto &model_instance = render::ModelSystem::GetModelInstance(model_component.kInstanceId);
        auto &model = render::ModelSystem::GetModel(model_instance.model_id);
        model.vertices.Bind();
        model.indices.Bind();
        auto transform = Engine::scene()->registry.get<components::TransformComponent>(entity);
        for (auto const &mesh_instance : model_instance.mesh_instances)
        {
            auto &mesh = model.meshes[mesh_instance.mesh_id];
            transform_buffer.Update(mesh.mesh_to_model * transform.model);
            direct3d::api::devcon4->PSSetShaderResources(0, 1, &model.materials[mesh_instance.material_id].texture);
            direct3d::api::devcon4->DrawIndexed(mesh.mesh_range.index_num, mesh.mesh_range.index_offset, 0);
        }
    }
}
void Renderer::OnRender()
{
    auto const &camera = Engine::scene()->main_camera->camera();
    per_frame.view = camera.view;
    per_frame.projection = camera.projection;
    per_frame.view_projection = camera.view_projection;
    per_frame.inv_view = camera.inv_view;
    per_frame.inv_projection = camera.inv_projection;
    per_frame.inv_view_projection = camera.inv_view_projection;
    per_frame.screen_resolution = vec2{ screen_resolution };
    per_frame.mouse_position = InputLayer::instance()->mouse_position();
    per_frame_buffer.Update(per_frame);
    per_frame_buffer.Bind(direct3d::ShaderType::VertexShader, 0);
    render::ModelSystem::instance()->Render();

    DrawTestCube();


    for (auto entity : Engine::scene()->registry.view<components::SkyboxComponent>())
    {
        SkyboxManager::RenderSkybox(Engine::scene()->registry.get<components::SkyboxComponent>(entity));
    }

}