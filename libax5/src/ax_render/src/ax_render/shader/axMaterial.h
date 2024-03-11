#pragma once

#include "axMaterialPass.h"

/*

+--------------------+     +-------------+
| Material           |  +->|  Shader     |
+--------------------+  |  +-------------+
| .Shader            |--+  |  .Pass[]    |
| .Pass[]            |     |             |
+--------------------+     +-------------+

+--------------------+     +------------+
| Material Pass      |  +->| ShaderPass |
+--------------------+  |  +------------+
| .Shader RenderPass |--+  |  .VsStage  |
| .UniformBuffer     |     |  .PsStage  |
| .Texture Samplers  |     +------------+
+--------------------+

*/

class axRenderDrawCall;

class axMaterial : public axRenderResource {
	AX_RTTI_CLASS(axMaterial, axRenderResource)
public:

	using PropType			= axShaderPropType;

	template<class T>
	void setParam(axNameId name, T v) {
		for (auto& pass : _passes) {
			pass->setParam(name, v);
		}
	}

	void setShader(Shader* shader);
	Shader* shader() { return _shader.ptr(); }

	void onUpdateGpuResource(axRenderRequest& req);

protected:
	axMaterial() = default;

	axArray< axSPtr<axMaterialPass> >	_passes;

	virtual void onSetShader(Shader* newShader) {}

private:
	axSPtr<Shader>		_shader;
};

class axMaterial_BlinnPhong : public axMaterial {
	using Names = axRenderNames;
public:
	void set_matObjectToWorld(const Mat4& v)	{ setParam(Names::matObjectToWorld(), v); }
	void set_ambient	(const axColorRGBf& v)	{ setParam(Names::ambient(),	v); }
	void set_diffuse	(const axColorRGBf& v)	{ setParam(Names::diffuse(),	v); }
	void set_specular	(const axColorRGBf& v)	{ setParam(Names::specular(),	v); }
	void set_shininess	(float v)				{ setParam(Names::shininess(),	v); }

	static axMaterial_BlinnPhong* s_cast(axMaterial* p) {
		AX_STATIC_ASSERT(sizeof(axMaterial_BlinnPhong) == sizeof(axMaterial));
		return static_cast<axMaterial_BlinnPhong*>(p);
	}
};

