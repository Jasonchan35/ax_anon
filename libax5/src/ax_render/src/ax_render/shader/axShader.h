#pragma once

#include "axShaderPass.h"

class axShader : public axRenderResource {
public:
	using Pass		= axShaderPass;
	using PropType	= axShaderPropType;
	using Info		= axShaderInfo;

	struct Prop {
		PropType	propType;
		axString	name;
		axString	displayName;
		axInt		index = -1;
	};

	void loadFile(axStrView filename);

	const axIArray<Prop>& props() { return _props; }

	void checkAcceptVertexDesc(VertexDesc meshDesc);

	Pass* findPass(axInt i) { return _passes.inBound(i) ? _passes[i].ptr() : nullptr; }
	axInt passCount() const { return _passes.size(); }

	Prop* findProp(axStrView name);

	axStrView filename() const { return _filename; }

protected:
	axShader() = default;

private:
	axArray<Prop>			_props;
	axString				_filename;
	axArray<axSPtr<Pass>>	_passes;
};

