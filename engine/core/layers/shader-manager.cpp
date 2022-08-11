#include "shader-manager.hpp"
namespace engine::core
{
    std::shared_ptr<ShaderManager> ShaderManager::instance_ = nullptr;
        void ShaderManager::OnEvent(events::Event &e)
        {
            std::unordered_set<std::shared_ptr<render::Shader>> compiled_shaders_;
            if (e.type() == events::EventType::FilesChanged)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                auto &fce = static_cast<events::FilesChangedEvent &>(e);
                for (auto const &path : fce.files_changed())
                {
                    if (dependent_shaders_map_.find(path) == dependent_shaders_map_.end()) [[unlikely]]
                    {
                        continue;
                    }
                    ShaderCompileOutput out;
                    for (auto const &shader : dependent_shaders_map_.at(path))
                    {
                        if (compiled_shaders_.find(shader) != compiled_shaders_.end()) [[unlikely]]
                        {
                            continue;
                        }
                            if (dependent_shaders_map_.find(path) == dependent_shaders_map_.end()) [[unlikely]]
                            {
                                dependent_shaders_map_[path] = std::unordered_set<std::shared_ptr<render::Shader>>{};
                            }
                        std::ifstream o(path);
                        o.close();
                        try
                        {
                            ShaderCompiler::CompileShader(shader_inputs_[shader], out);
                        }
                        catch (ShaderCompiler::CompilerException e)
                        {
                            spdlog::warn(e.what());
                            continue;
                        }
                        shader->Recreate(out.blob);
                        for (auto &path : out.dependent_files)
                        {
                            dependent_shaders_map_[path].emplace(shader);
                            watcher_.AddPathToWatch(path);
                        }
                    }
                }
            }
        }
}