#include <ax_core/file/axMemMap.h>
#include <ax_core/file/axFileStream.h>

axMemMap::~axMemMap() {
	close();
}

#if AX_OS_WINDOWS

void axMemMap::openFile(axStrView filename) {
	close();

	_file.openRead(filename);
	auto size = _file.getFileSize();
	if (size <= 0) return;

	_mapping = CreateFileMapping(_file.nativeFileDescriptor(), nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (!_mapping) {
		throw axError_Undefined(AX_LOC);
	}

	auto* data = (axUInt8*)MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0);
	if (!data) {
		throw axError_Undefined(AX_LOC);
	}

	_span.setPtr(data, size);
}

void axMemMap::close() {
	if (_span.data()) {
		UnmapViewOfFile(_span.data());
		_span.setPtr(nullptr, 0);
	}

	if (_mapping) {
		CloseHandle(_mapping);
		_mapping = nullptr;
	}

	_file.close();
}

#else

void axMemMap::openFile(axStrView filename) {
	close();
	_file.openRead(filename);
	auto size = _file.getFileSize();
	if (size <= 0) return;

	size_t nativeSize;
	ax_safe_assign(nativeSize, size);

	auto* data = (axUInt8*)mmap(0, nativeSize, PROT_READ, MAP_PRIVATE, _file.nativeFileDescriptor() , 0);
	if ( data == MAP_FAILED || data == NULL ) {
		throw axError_Undefined(AX_LOC);
	}

	_span.setPtr(data, size);
}

void axMemMap::close() {
	if (_span.data()) {
		size_t nativeSize;
		ax_safe_assign(nativeSize, _span.size());
		
		void* data = ax_const_cast(_span.data());
		munmap(data, nativeSize);
		_span.setPtr(nullptr, 0);
	}
	_file.close();
}

#endif
