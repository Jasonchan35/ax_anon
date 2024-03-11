#include "testing.hlsl"

#define Sampler2D(T, reg) SamplerState _axSamplerState_##T : register(s##reg); Texture2D T : register(t##reg);
#define tex2D(T, UV) T.Sample(_axSamplerState_##T, UV);

//CUSTOM ATTR (float4)
#define CUSTOM0 TEXCOORD10
#define CUSTOM1 TEXCOORD11
#define CUSTOM2 TEXCOORD12
#define CUSTOM3 TEXCOORD13

float3 axColor_Linear_to_sRGB_approximation(float3 x) { return pow(x, 1 / 2.2); }
float3 axColor_sRGB_to_Linear_approximation(float3 x) { return pow(x, 2.2); }

float3 axColor_sRGB_to_Linear_accurate(float3 x) { return x < 0.04045 ? x / 12.92 : pow( (x + 0.055) / 1.055, 2.4 ); }
float3 axColor_Linear_to_sRGB_accurate(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.055 * pow(x, 1.0 / 2.4) - 0.055; }

// These functions avoid pow() to efficiently approximate sRGB with an error < 0.4%.
float3 axColor_Linear_to_sRGB_fast(float3 x) { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
float3 axColor_sRGB_to_Linear_fast(float3 x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }


float3 axColor_Linear_to_sRGB(float3 x) { return axColor_Linear_to_sRGB_fast(x); }
float3 axColor_sRGB_to_Linear(float3 x) { return axColor_sRGB_to_Linear_fast(x); }

float4 axColor_Linear_to_sRGB(float4 x) { return float4(axColor_Linear_to_sRGB_fast(x.rgb), x.a); }
float4 axColor_sRGB_to_Linear(float4 x) { return float4(axColor_sRGB_to_Linear_fast(x.rgb), x.a); }

#define axNumThreads1d [numthreads(64, 1, 1)]	// NV=32 AMD=64 Intel=8, so 64 is multiple for all
#define axNumThreads2d [numthreads( 8, 8, 1)] 	// in 2 dimension 8 x 8 = 64
#define axNumThreads3d [numthreads( 4, 4, 4)]	// in 3 dimension 4 x 4 x 4 = 64
