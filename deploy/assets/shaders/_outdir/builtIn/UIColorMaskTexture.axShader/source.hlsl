         
              
                                          
                                   
                                   
                                   
                                   
   
 
                
                         
             
 
                                     
                                      
 
                   
                   
 
                  
                  
   
  
 
#include "../common/ax_render.hlsl"

float4x4	matMvp;
Sampler2D(mainTex0, 0)
float4		color0;
float4		color1;
float4		color2;
float4		color3;

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
	o.color = i.color;
	o.uv0 = i.uv0;
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
	float4 t0 = tex2D(mainTex0, i.uv0);
	float4 o = t0.r * color0 
			 + t0.g * color1
			 + t0.b * color2
			 + t0.a * color3;

	return o * i.color;
}
