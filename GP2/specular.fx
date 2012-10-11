float4x4 matWorld:WORLD<string UIWidget="None";>;
float4x4 matView:VIEW<string UIWidget="None";>;
float4x4 matProjection:PROJECTION<string UIWidget="None";>;

float4 ambientMaterial<string UIName="Ambient Material";string UIWidget="Color";>;
float4 ambientLightColour<string UIName="Ambient Light Colour";string UIWidget="Color";>;

float4 lightDirection:DIRECTION<string Object = "DirectionalLight";string UIWidget="None";>;

float4 diffuseMaterial<string UIName="Diffuse Material";string UIWidget="Color";>;
float4 diffuseLightColour:DIFFUSE<string UIName="Diffuse Light Colour";string UIWidget="None";>;

float4 specularMaterial<string UIName="Specular Material";string UIWidget="Color";>;
float4 specularLightColour<string UIName="Specular Light Colour";string UIWidget="Color";>;
float4 cameraPosition:POSITION<string Object="Perspective";>;

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 colour:COLOR;
	float3 normal:NORMAL;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 colour:COLOR;
	float3 normal:NORMAL;
	float4 cameraDirection:VIEWDIR;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output=(PS_INPUT)0;
	
	input.colour = output.colour;
		
	float4x4 matViewProjection = mul(matView,matProjection);
	float4x4 matWorldViewProjection = mul(matWorld,matViewProjection);
	
	float4 worldPos=mul(input.pos,matWorld);
	
	output.pos = mul(input.pos,matWorldViewProjection);
	
	output.normal=mul(input.normal,matWorld);
	
	output.cameraDirection=normalize(cameraPosition-worldPos);
	
	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
	float3 normal=normalize(input.normal);
	float4 lightDir=normalize(lightDirection);
	
	float diffuse=saturate(dot(normal,lightDir));
	
	float4 halfVec=normalize(lightDir+input.cameraDirection);
	float4 specular=pow(saturate(dot(normal,halfVec)),25);
		
	return (ambientMaterial*ambientLightColour)+(diffuseMaterial*diffuseLightColour*diffuse)+(specularMaterial*specularLightColour*specular);
}

RasterizerState DisableCulling
{
	CullMode = NONE;
};

technique10 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0,VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0,PS()));
		SetRasterizerState(DisableCulling);
	}
}