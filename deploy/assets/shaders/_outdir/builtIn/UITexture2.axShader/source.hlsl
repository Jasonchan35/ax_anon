         
              
                                          
                                          
   
 
                
                         
             
 
                                     
                                      
 
                   
                   
 
                  
                  
   
  
 
#include "../common/ax_render.hlsl"

float4x4 	matMvp;
float4		color;
Sampler2D(mainTex0, 0)
Sampler2D(mainTex1, 1)

struct VS_Input {
	float4 pos : POSITION0;
	float4 color : COLOR0;
	float2 uv0  : TEXCOORD0;
	float2 uv1  : TEXCOORD1;
};

struct PS_Input {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 uv0  : TEXCOORD0;
	float2 uv1  : TEXCOORD1;
};

PS_Input vs_main(VS_Input i) {
	PS_Input o;
	o.pos = mul(i.pos, matMvp);
	o.color = color * i.color;
	o.uv0 = i.uv0;
	o.uv1 = i.uv1;
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
	float4 t0 = tex2D(mainTex0, i.uv0);
	float4 t1 = tex2D(mainTex1, i.uv1);
	float4 o = i.color * t0 * t1;
	return o;
}
