struct DirectionalLight
{
	float4 diffuseColor;
	float3 direction;
	//float pad;
};

struct PointLight
{
	float4 diffuseColor;
	float3 position;
	//float pad;
};

cbuffer lightData : register(b0)
{
	float4 ambientColor;
	DirectionalLight directionalLight;
	DirectionalLight directionalLight2;
	PointLight pointLight;
	float3 cameraPos;
};

Texture2D diffuseTexture	: register(t0);
SamplerState basicSampler	: register(s0);
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 worldPos		: WORLD_POS;
};

float4 calcDirLightDiffuse(DirectionalLight dirLight, float3 normal) {
	return dirLight.diffuseColor * saturate(dot(-normalize(dirLight.direction), normal));
}

float4 calcPointLightDiffuse(PointLight pointLight, float3 normal, float3 worldPos) {
	return pointLight.diffuseColor *
		saturate(dot(normalize(pointLight.position - worldPos), normal));
}

float4 calcPointLightDiffuseSpec(PointLight pointLight, float3 normal, float3 worldPos, float3 camPos, float specExp) {
	float3 dirToLight = normalize(pointLight.position - worldPos);
	float spec = pow(saturate(dot(reflect(-dirToLight, normal), normalize(camPos - worldPos))), specExp);

	return (pointLight.diffuseColor * saturate(dot(dirToLight, normal))) + spec.xxxx;
}
// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	float dirToPointLight = normalize(pointLight.position - input.worldPos);
	
	return surfaceColor * (ambientColor +
		calcDirLightDiffuse(directionalLight, input.normal) +
		calcDirLightDiffuse(directionalLight2, input.normal) +
		calcPointLightDiffuseSpec(pointLight, input.normal, input.worldPos, cameraPos, 256));
}

