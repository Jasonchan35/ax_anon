//
//  File.cpp
//  ax_core
//
//  Created by Jason Chan on 12/2/14.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#include <ax_core/file/axFile.h>
#include <ax_core/file/axFileStream.h>
#include <ax_core/file/axMemMap.h>
#include <ax_core/file/axDirectory.h>
#include <ax_core/file/axPath.h>

void axFile::readText( axStrView filename, axIString & buf ) {
	buf.clear();
	appendReadText(filename, buf);
}

void axFile::readUtf8( axStrView filename, axIString8 & buf ) {
	buf.clear();
	appendReadUtf8(filename, buf);
}

void axFile::readBytes(axStrView filename, axIByteArray& buf) {
	buf.clear();
	appendReadBytes(filename, buf);
}

void axFile::appendReadText ( axStrView filename, axIString & buf ) {
	axFileStream	s;
	s.openRead( filename );
	s.appendReadAllText( buf );
}

void axFile::appendReadBytes(axStrView filename, axIByteArray& buf) {
	axFileStream	s;
	s.openRead( filename );
	s.appendReadAllBytes( buf );
}

void axFile::appendReadUtf8 ( axStrView filename, axIString8 & buf ) {
	axFileStream	s;
	s.openRead( filename );
	s.appendReadAllUtf8( buf );
}

void axFile::writeBytes(axStrView filename, axByteSpan buf) {
	axFileStream	s;
	s.openWrite( filename, true );
	s.writeBytes( buf );
}

void axFile::writeText		( axStrView filename, axStrView text ) {
	axFileStream	s;
	s.openWrite( filename, true );
	s.writeText( text );
}

void axFile::writeUtf8		( axStrView filename, axStrView8 text ) {
	axFileStream	s;
	s.openWrite( filename, true );
	s.writeUtf8( text );
}

axChar axFile::writeFile(axStrView filename, axByteSpan buf, bool createDir, bool logResult) {
	axChar op = '+';

	axString absPath;
	axPath::getAbsolute(absPath, filename);

	if (axFile::exists(absPath)) {
		op = 'U';
	}

	if (logResult) {
		AX_LOG("[{?}] {?}, size={?}", op, absPath, buf.sizeInBytes());
	}

	if (createDir) {
		if (auto dir = axPath::dirname(absPath)) {
			axDirectory::create(dir, true);
		}
	}

	axFile::writeBytes(absPath, buf);
	return op;
}

axChar axFile::writeFile(axStrView filename, axStrView8 text, bool createDir, bool logResult) {
	return writeFile(filename, text.byteSpan(), createDir, logResult);
}

axChar axFile::writeFileIfChanged(axStrView filename, axByteSpan buf, bool createDir, bool logResult, bool logNoChange) {
	axChar op = '+';

	axString absPath;
	axPath::getAbsolute(absPath, filename);

	if (axFile::exists(absPath)) {
		axMemMap map(absPath);
		if (map.span() == buf) {
			op = '=';
		}else{
			op = 'U';
		}
	}

	if (logResult) {
		if (op != '=' || logNoChange) {
			AX_LOG("[{?}] {?}, size={?}", op, absPath, buf.sizeInBytes());
		}
	}

	if (op == '=') return op;
	axFile::writeFile(absPath, buf, createDir, false);
	return op;
}

axChar axFile::writeFileIfChanged(axStrView filename, axStrView8 text, bool createDir, bool logResult, bool logNoChange) {
	return writeFileIfChanged(filename, text.byteSpan(), createDir, logResult, logNoChange);
}

void axFile::touch(axStrView filename) {
	axFileStream f;
	f.openWrite(filename, false);

	axUtcTime t;
	t.setToNow();
	f.setLastWriteTime(t);
}

#if 0
#pragma mark --------- AX_OS_UNIX || AX_OS_MINGW || AX_OS_CYGWIN ---------
#endif
#if AX_OS_UNIX || AX_OS_MINGW || AX_OS_CYGWIN

#include <dirent.h>
#include <sys/stat.h>

void axFile::remove	( axStrView filename ) {
	auto filename8 = axTempString8::s_convert(filename);
	auto ret = ::remove( filename8.c_str() );
	if( ret != 0 ) {
		throw axError_Undefined(AX_LOC);
	}
}

bool axFile::exists ( axStrView filename ) {
	auto filename8 = axTempString8::s_convert(filename);
	struct stat s;
	if( 0 != ::stat( filename8.c_str(), &s ) ) return false;
	return ( s.st_mode & S_IFDIR ) == 0;
}

void axFile::rename	( axStrView src_name, axStrView dst_name ) {
	auto src = axTempString8::s_convert(src_name);
	auto dst = axTempString8::s_convert(dst_name);
	auto ret = ::rename( src.c_str(), dst.c_str() );
	if( ret != 0 ) {
		throw axError_Undefined(AX_LOC);
	}
}

#endif

#if 0
#pragma mark --------- AX_OS_WINDOWS ---------
#endif
#if AX_OS_WINDOWS

#include <Shellapi.h>

void axFile::remove	( axStrView filename ) {
	axTempStringW filenameW;
	filenameW.setUtf(filename);
	if( ! ::DeleteFile( filenameW.c_str()) ) {
		throw axError_Undefined(AX_LOC);
	}
}

void axFile::moveToRecycleBin	( axStrView filename ) {
	axTempStringW filenameW;
	filenameW.setUtf(filename);
	filenameW.append(L'\0');	//double 0 end is needed by WinAPI

	SHFILEOPSTRUCT op = {};
	op.hwnd		= NULL;
	op.wFunc	= FO_DELETE;
	op.pFrom	= filenameW.c_str();
	op.pTo		= NULL;
	op.fFlags	= FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

	if( 0 != SHFileOperation( &op ) ) throw axError_Undefined(AX_LOC);
}

bool axFile::exists ( axStrView filename ) {
	axTempStringW filenameW;
	filenameW.setUtf(filename);
	DWORD dwAttrib = ::GetFileAttributes(filenameW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void axFile::rename	( axStrView src_name, axStrView dst_name ) {
	axTempStringW src;
	src.setUtf(src_name);
	axTempStringW dst;
	dst.setUtf(dst_name);
	if( 0 != ::_wrename( src.c_str(), dst.c_str() ) ) {
		throw axError_Undefined(AX_LOC);
	}
}

#endif
