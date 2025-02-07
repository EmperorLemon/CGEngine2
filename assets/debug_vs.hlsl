// debug_vs.hlsl
struct VS_IN
{
	float4 aPos : POSITION;
    float4 aColor : COLOR;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUT VSMain(VS_IN input)
{
	VS_OUT output;
	
	output.position = input.aPos;
    output.color = input.aColor;
	
	return output;
}