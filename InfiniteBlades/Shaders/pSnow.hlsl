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
	float3 tangent		: TANGENT;
	float3 color		: COLOR;
};

//buffer for light calculations
cbuffer lightData : register(b0)
{
	float4 ambientColor;
	DirectionalLight directionalLight;
	DirectionalLight directionalLight2;
	PointLight pointLight;

	float3 cameraPos;
	float transparentStrength;
};

// Texture-related variables
Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState diffuseSampler : register(s0);
SamplerState normalSampler	: register(s1);
TextureCube  skyTexture		: register(t2);


//calculates normal based on normal map
float3 recalculateNormals(float3 normal, float3 tangent, float2 uv) {
	float3 normalFromMap = normalTexture.Sample(normalSampler, uv).rgb;
	normalFromMap = normalFromMap * 2 - 1;

	float3 N = normal;
	float3 T = normalize(tangent - N * dot(tangent, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	return normalize(mul(normalFromMap, TBN));
}

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


float4 main(VertexToPixel input) : SV_TARGET
{
	//normalize normal (after interpolation)
	input.normal = normalize(input.normal);
input.tangent = normalize(input.tangent);

//alpha value 
float blendStr = 1.0f - transparentStrength;

float4 surfaceColor = diffuseTexture.Sample(diffuseSampler, input.uv);
surfaceColor.a = surfaceColor.a * blendStr;

// Adjust the normal from the map and simply use the results
input.normal = recalculateNormals(input.normal, input.tangent, input.uv);
// Direction to the camera from the current pixel
float3 dirToCamera = normalize(cameraPos - input.worldPos);

//reflections 
float4 reflection = skyTexture.Sample(
	normalSampler,
	reflect(-dirToCamera, input.normal));

//lights in the scene 
float4 lights = (ambientColor +													  //ambient color
	dirLightDiffuse(directionalLight, input.normal) +							  //dir light 1
	dirLightDiffuse(directionalLight2, input.normal));							  //dir light 2
	//pointLightSpec(pointLight, input.normal, input.worldPos, cameraPos, 256));    //point light

return reflection * surfaceColor * lights + float4(input.color, 0.f);
}

