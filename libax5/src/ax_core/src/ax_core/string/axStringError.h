#pragma once

#include "axStrView.h"

class axStringError : public axError {};
class axStringError_Utf							: public axStringError {};
class axStringError_InputStringOverlapped		: public axStringError {};
class axStringError_InvalidParamIndexInFormat	: public axStringError {};
class axStringError_OutputStringCannotBeParam	: public axStringError {};
class axStringError_FormatNumberHandler			: public axStringError {};
class axStringError_FormatCurlyBracesMismatch	: public axStringError {
public:
	axStringError_FormatCurlyBracesMismatch(axInt bracePos_) 
		: bracePos(bracePos_)
	{}
	axInt bracePos;
};