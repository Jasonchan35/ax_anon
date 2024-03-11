         
              
                 
                 
                  
                     
   
 
                
                         
               
                   
 
                                     
                                      
 
                 
 
                  
                  
   
  
 
#include "../common/ax_render.hlsl"

float4x4	matMvp;
float4x4	matObjectToWorld;

float3		cameraPos;
float3		lightPos;
float		lightPower;
float3		lightColor;

float3		ambient;
float3		diffuse;
float3		specular;
float		shininess;

struct VS_Input {
	float4 pos : POSITION0;
	float4 color : COLOR0;
	float3 normal : NORMAL0;
};

struct PS_Input {
	float4 pos 		: SV_POSITION;
	float3 worldPos : POSITION0;
	float4 color 	: COLOR0;
	float3 normal 	: NORMAL0;
};

PS_Input vs_main(VS_Input i) {
	PS_Input o;
	o.pos 		= mul(i.pos, matMvp);
	o.worldPos 	= mul(i.pos, matObjectToWorld).xyz;
	o.color 	= i.color;
	o.normal 	= i.normal;
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
	float3 normal   = normalize(i.normal);
	float3 lightDir = i.worldPos - lightPos;
	float  lightDistance = length(lightDir);
	lightDir = normalize(lightDir);

	float lambertian = max(dot(normal, -lightDir), 0);
	float specularPower = 0;

	if (lambertian > 0) {
		float3 viewDir = normalize(cameraPos - i.worldPos);

		if (1) { // blinn-phong
			float3 halfDir = normalize(viewDir - lightDir);
			float specAngle = saturate(dot(halfDir, normal));
			specularPower = pow(specAngle, shininess);

		} else { // phong
			float3 reflectDir = reflect(-lightDir, normal);
			float specAngle = max(dot(reflectDir, viewDir), 0);
			specularPower = pow(specAngle, shininess / 4.0);
		}
	}

	float3 outLightColor = lightColor * lightPower / (lightDistance * lightDistance);

	float3 outAmbient  = ambient;
	float3 outDiffuse  = diffuse  * lambertian    * outLightColor;
	float3 outSpecular = specular * specularPower * outLightColor;

	float4 outColor = float4(outAmbient + outDiffuse + outSpecular, 1) * i.color;

	outColor = axColor_Linear_to_sRGB(outColor);
	return outColor;
}
