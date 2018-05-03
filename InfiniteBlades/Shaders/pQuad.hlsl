// Pixel shader input
struct VertToPix
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

// Textures and sampler
Texture2D Pixels		: register(t0);
SamplerState Sampler	: register(s0);


// Start of pixel shader
float4 main(VertToPix input) : SV_TARGET
{
	return Pixels.Sample(Sampler, input.uv);
}