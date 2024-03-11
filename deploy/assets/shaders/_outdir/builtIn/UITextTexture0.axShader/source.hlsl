         
              
                                              
   
 
                
                         
             
 
                                     
                                      
 
                   
                   
 
                  
                  
   
  
 
#include "../common/ax_render.hlsl"

float4x4	matMvp;
float4		color;
Sampler2D(glyphTex, 0)

struct VS_Input {
	float4 pos : POSITION0;
	float4 color : COLOR0;
	float2 uv  : TEXCOORD0;
};

struct PS_Input {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 uv  : TEXCOORD0;
};

PS_Input vs_main(VS_Input i) {
	PS_Input o;
	o.pos = mul(i.pos, matMvp);
	o.color = color * i.color;
	o.uv = i.uv;
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
	float4 t = tex2D(glyphTex, i.uv);
	float4 o = i.color * t.r;
	return o;
}
