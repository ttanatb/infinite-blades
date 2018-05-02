//stores stuff for directional light
struct DirectionalLight
{
	float4 diffuseColor;
	float3 direction;
};

//point light
struct PointLight
{
	float4 diffuseColor;
	float3 position;
};

//struct for data  from vertex shader that gets travaled to pixel shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 worldPos		: WORLD_POS;
};

//buffer for light calculations
cbuffer lightData : register(b0)
{
	float4 ambientColor;
	DirectionalLight directionalLight;
	DirectionalLight directionalLight2;
	PointLight pointLight;
	float3 cameraPos;
};

//variables
Texture2D diffuseTexture	: register(t0);
SamplerState diffuseSampler	: register(s0);

//calculates diffuse light on texel from a directional light
float4 dirLightDiffuse(DirectionalLight dirLight, float3 normal) {
	return dirLight.diffuseColor * saturate(dot(-normalize(dirLight.direction), normal));
}

//calculates diffuse light on texel from a point light
float4 pointLightDiffuse(PointLight pointLight, float3 normal, float3 worldPos) {
	return pointLight.diffuseColor *
		saturate(dot(normalize(pointLight.position - worldPos), normal));
}

//calculates diffuse + spec light on texel from a point light
float4 pointLightSpec(PointLight pointLight, float3 normal, float3 worldPos, float3 camPos, float specExp) {
	float3 dirToLight = normalize(pointLight.position - worldPos);
	float spec = pow(saturate(dot(reflect(-dirToLight, normal), normalize(camPos - worldPos))), specExp);

	return (pointLight.diffuseColor * saturate(dot(dirToLight, normal))) + spec.xxxx;
}

//main
float4 main(VertexToPixel input) : SV_TARGET
{
	//normalize normal (after interpolation)
	input.normal = normalize(input.normal);

	return diffuseTexture.Sample(diffuseSampler, input.uv) *							  //texture
		(ambientColor +																  //ambient color
		dirLightDiffuse(directionalLight, input.normal) +							  //dir light 1
		dirLightDiffuse(directionalLight2, input.normal) +							  //dir light 2
		pointLightSpec(pointLight, input.normal, input.worldPos, cameraPos, 256));	  //point light
}

