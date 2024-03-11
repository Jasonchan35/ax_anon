         
              
                                              
   
 
                
                         
             
 
                                     
                                      
 
                   
                   
 
                  
                  
   
  
 
#include "../common/ax_render.hlsl"

float4x4 	matMvp;
float4		color;
Sampler2D(glyphTex, 0)

float2 invViewportSize;

struct VS_Input {
	float4 pos : POSITION0;
	float4 color : COLOR0;
	float2 uv0  : TEXCOORD0;
	float2 uv1  : TEXCOORD1;
};

struct PS_Input {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 glyphUv : TEXCOORD0;
};

PS_Input vs_main(VS_Input i) {
	PS_Input o;

	i.pos.xy -= i.uv1.xy;
	o.pos = mul(i.pos, matMvp);
	o.pos.xy += i.uv1.xy * invViewportSize * o.pos.w * float2(2,-2);

	o.color = color * i.color;
	o.glyphUv = i.uv0;
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
	float4 t = tex2D(glyphTex, i.glyphUv);
	float4 o = i.color * t.r;
	return o;
}
