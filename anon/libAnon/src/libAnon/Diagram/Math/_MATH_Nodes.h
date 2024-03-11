#pragma once

/*

#include "../DiaNode.h"

namespace Anon {

DIA_NODE()
class MATH_MakeVec3f : public DiaNode {
	#include "MATH_MakeVec3f._declare.h"
public:
	DIA_INPUT (Vec3f output);
	DIA_OUTPUT(float x);
	DIA_OUTPUT(float y);
	DIA_OUTPUT(float z);

	void onCompute() {
		output.setValue(Vec3f(	x.value(), 
								y.value(), 
								z.value()));
	}
};

DIA_NODE()
class MATH_BreakVec3f : public DiaNode {
	#include "MATH_BreakVec3f._declare.h"
public:

	DIA_OUTPUT(Vec3f input);
	DIA_INPUT (float x);
	DIA_INPUT (float y);
	DIA_INPUT (float z);

	void onCompute() {
		auto v = input.value();
		x.setValue(v.x);
		y.setValue(v.y);
		z.setValue(v.z);
	}
};

} //namespace
*/