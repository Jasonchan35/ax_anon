#pragma once

#include <libAnon/libAnon_config.h>

namespace Anon {

class DontSaveAttribute : public axAttribute {
	AX_RTTI_CLASS(DontSaveAttribute, axAttribute)
public:
};

class OutputAttribute : public axAttribute {
	AX_RTTI_CLASS(OutputAttribute, axAttribute)
public:
};

class SliderAttribute : public axAttribute {
	AX_RTTI_CLASS(SliderAttribute, axAttribute)
public:
	SliderAttribute(const double& min, const double& max) : _min(min), _max(max) {}

	const double& min() const { return _min; }
	const double& max() const { return _max; }

private:
	double	_min;
	double	_max;
};

}
