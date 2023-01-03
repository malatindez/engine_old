cbuffer PerModel : register(b1) { row_major matrix g_mesh_to_model; }
struct VS_OUTPUT {
  float4 pos : SV_POSITION;
};

struct VS_INPUT {
  float3 pos : POSITION;
  float2 texcoord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float3 bitangent : BITANGENT;
  float4 RowX : ROWX;
  float4 RowY : ROWY;
  float4 RowZ : ROWZ;
  float4 RowW : ROWW;
  float time_begin : TIME_BEGIN;
  float lifetime : LIFETIME;
};

VS_OUTPUT vs_main(VS_INPUT input) {
  VS_OUTPUT output;
  float4x4 world_transform = float4x4(input.RowX, input.RowY, input.RowZ, input.RowW);
  world_transform = mul(g_mesh_to_model, world_transform);
  output.pos = mul(float4(input.pos, 1), world_transform);
  return output;
}