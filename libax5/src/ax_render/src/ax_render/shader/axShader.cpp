#include "../axRenderer.h"
#include "../axRenderContext.h"
#include "axShader.h"

void axShader::loadFile(axStrView filename) {
	_filename = filename;

	axTempString infoFilename;
	infoFilename.set(filename, "/info.json");

	axShaderInfo info;
	axJsonUtil::readFile(infoFilename, info);

	{
		auto n = info.passes.size();
		_passes.resize(n);
		for (axInt i=0; i<n; i++) {
			auto& d = _passes[i];
			d = axRenderer::s_instance()->createShaderPass();
			d->init(this, i, info.passes[i]);
		}
	}

	{
		auto n = info.props.size();
		_props.resize(n);
		for (axInt i=0; i<n; i++) {
			auto& d = _props[i];
			auto& s = info.props[i];

			d.index			= i;
			d.propType		= s.propType;
			d.displayName	= s.displayName;
		}
	}
}

void axShader::checkAcceptVertexDesc(VertexDesc meshDesc) {
	for (auto& p : _passes) {
		p->checkAcceptVertexDesc(meshDesc);
	}
}

axShader::Prop* axShader::findProp(axStrView name) {
	for (auto& p : _props) {
		if (p.name == name) return &p;
	}
	return nullptr;
}
