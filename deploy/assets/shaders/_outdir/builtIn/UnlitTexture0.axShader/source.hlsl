         
              
   
 
                
                         
             
                   
 
                                     
                                      
 
                 
 
                  
                  
   
  
 
#include "../common/ax_render.hlsl"

float4x4	matMvp;
float4		color;

struct VS_Input {
	float4 pos : POSITION0;
	float4 color : COLOR0;
};

struct PS_Input {
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
};

PS_Input vs_main(VS_Input i) {
	PS_Input o;
	o.pos = mul(i.pos, matMvp);
	o.color = color * i.color;
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
	return i.color;
}
