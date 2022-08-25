#pragma once
#include "render/shader-program.hpp"

namespace engine::core
{
    struct ShaderMacro
    {
        std::string name;
        std::string definition;
    };

    struct ShaderCompileInput
    {
        enum CompileFlags
        {
            FlagNone = 0,
            FlagDebug = 1 << 0,
            FlagDisableOptimization = 1 << 1,
        };
        direct3d::ShaderType type = direct3d::ShaderType::None;
        std::filesystem::path source_file;
        std::string entrypoint = "";
        std::vector<ShaderMacro> macros;
#if defined(_DEBUG)
        uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#else
        uint32_t flags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif
    };

    struct ShaderCompileOutput
    {
        render::ShaderBlob blob;
        std::vector<std::filesystem::path> dependent_files;
    };

    namespace ShaderCompiler
    {
        struct CompilerException : public std::runtime_error
        {
            using std::runtime_error::runtime_error;
        };
        void CompileShader(ShaderCompileInput const &input, ShaderCompileOutput &output);
        void GetBlobFromCompiledShader(std::filesystem::path const &filename, render::ShaderBlob &blob);
    }
}