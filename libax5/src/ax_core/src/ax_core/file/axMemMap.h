#pragma once

#include <ax_core/container/axSpan.h>
#include <ax_core/file/axFileStream.h>

class axMemMap : public axNonCopyable {
public:
	axMemMap() = default;
	axMemMap(axStrView filename) { openFile(filename); }

	~axMemMap();

	void openFile(axStrView filename);
	void close();

	const axByte*	data()	{ return _span.data(); }
		axInt		size()	{ return _span.size(); }
		axInt		sizeInBytes() { return _span.sizeInBytes(); }

	axByteSpan		span() { return _span; }
	operator axByteSpan()  { return _span; }

	axStrView8		strView8() { return axStrView8_make(_span); }

	axStrView filename() const { return _file.filename(); }

private:

#if AX_OS_WINDOWS
	axFileStream	_file;
	HANDLE	_mapping = nullptr;
#else
	axFileStream	_file;

#endif
	axByteSpan _span;
};

class axMutMemMapping {
public:

};