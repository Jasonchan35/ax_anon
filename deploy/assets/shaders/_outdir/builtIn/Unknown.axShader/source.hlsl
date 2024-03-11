         
              
   
 
                
                         
             
 
                                     
                                      
 
                         
                   
 
                   
                   
 
                  
                  
   
  
 
#include "../common/ax_render.hlsl"

float4x4 matMvp;

struct VS_Input {
	float4 pos : POSITION0;
};

struct PS_Input {
	float4 pos : SV_POSITION;
};

PS_Input vs_main(VS_Input i) {
	PS_Input o;
	o.pos = mul(i.pos, matMvp);
	return o;
}

float4 ps_main(PS_Input i) : SV_Target {
	return float4(1,0,1,1);
}
