         
              
                       
   
 
                
                 
       
  
 
#include "../common/ax_render.hlsl"

/*
struct Triangles
{
	float3 pos;
	half4  color;
};
*/
static const int sizeofStruct = 20;

//RWStructuredBuffer<Triangles> BufferOut : register(u0);
RWByteAddressBuffer BufferOut : register(u0); // DX11 cannot use RWStructuredBuffer if output use as VertexBuffer

uint2 toColorRGBAh(float4 color) {
	uint4 tmp;
	tmp.r = f32tof16(color.r);
	tmp.g = f32tof16(color.g);
	tmp.b = f32tof16(color.b);
	tmp.a = f32tof16(color.a);

	uint2 o;
	o.x = tmp.r | tmp.g << 16;
	o.y = tmp.b | tmp.a << 16;

	return o;
}

float cellSize;

int2 gridSize;

void writeOutput(int x, int y, float3 pos)
{
	int index = (x + y * gridSize.x);
//	BufferOut[index].pos = pos;
//	BufferOut[index].color = half4(1,1,0,1);

	const int addr = index * sizeofStruct;
	BufferOut.Store3(addr, asuint(pos));

	float4 color = float4(1, 0, 1, 1);
	BufferOut.Store2(addr + 12, toColorRGBAh(color));
}

static const float3 triVertex[6] = {
	float3(0, 0, 0),
	float3(0, 1, 0),
	float3(1, 0, 0),
	float3(1, 0, 0),
	float3(0, 1, 0),
	float3(1, 1, 0)
};

struct CS_Input {
	uint3 t  : SV_DispatchThreadID;
	uint3 g  : SV_GroupID;
	uint3 gt : SV_GroupThreadID;
};

// axNumThreads2d
[numthreads(6, 1, 1)] 
void cs_main(CS_Input i) {
	float3 v = triVertex[i.gt.x];
	float3 pos  = (v + i.g) * cellSize;
	writeOutput(i.t.x, i.t.y * 6, pos);
}
