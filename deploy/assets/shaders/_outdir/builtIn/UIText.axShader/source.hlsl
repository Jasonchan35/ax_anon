         
              
                                              
   
 
                
                         
             
 
                                     
                                      
 
                   
                   
 
                  
                  
   
  
 
#include "../common/ax_render.hlsl"

float4x4 	matMvp;
float4		color;
Sampler2D(glyphTex, 0)

struct VS_Input {
	float4 pos : POSITION0;
	float4 color : COLOR0;
	float2 uv0  : TEXCOORD0;
};

struct PS_Input {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 uv0  : TEXCOORD0;
};

PS_Input vs_main(VS_Input i) {
	PS_Input o;
	o.pos = mul(i.pos, matMvp);
	o.color = color * i.color;
	o.uv0 = i.uv0;
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
	float4 gt = tex2D(glyphTex, i.uv0);
	float4 o = i.color * gt.r;
	return o;
}
