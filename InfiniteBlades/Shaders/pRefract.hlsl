cbuffer externalData : register(b0)
{
	matrix view;
	float3 CameraPosition;
};


struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
	noperspective float2 screenUV		: TEXCOORD1;
};

// textures
Texture2D ScenePixels		: register(t0);
Texture2D NormalMap			: register(t1);
SamplerState BasicSampler	: register(s0);
SamplerState RefractSampler	: register(s1);


// Entery for pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Normalize interpolated normals
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Unpack normal
	float3 normalFromTexture = NormalMap.Sample(BasicSampler, input.uv).xyz * 2 - 1;

	// TBN matrix for tangent-to-world-space
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);
	
	// Overwrite the existing normal
	input.normal = normalize(mul(normalFromTexture, TBN));
	
	// Variations
	float indexOfRefraction = 0.9f;
	float refractAdjust = 0.1f;
	
	// Calculate world-space refract amount
	float3 dirToPixel = normalize(input.worldPos - CameraPosition);
	float3 refractDir = refract(dirToPixel, input.normal, indexOfRefraction);
	
	// Get the refraction XY direction in VIEW SPACE (relative to the camera)
	float2 refractUV = mul(float4(refractDir, 0.0f), view).xy * refractAdjust;
	refractUV.x *= -1.0f;
	
	// Sample render target and return
	return ScenePixels.Sample(RefractSampler, input.screenUV + refractUV);
}