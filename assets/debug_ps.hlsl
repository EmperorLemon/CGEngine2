// debug_ps.hlsl
struct PS_IN
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 PSMain(PS_IN input) : SV_TARGET
{
    return input.color;
}