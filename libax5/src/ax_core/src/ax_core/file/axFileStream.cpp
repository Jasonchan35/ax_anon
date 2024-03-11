//
//  FileStream.cpp
//  ax_core
//
//  Created by Jason Chan on 12/2/14.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#include <ax_core/file/axFileStream.h>
#include <ax_core/string/axUtfUtil.h>
#include <ax_core/time/axTimestamp.h>

#if 0
#pragma mark ================= Common ====================
#endif

axFileStream::~axFileStream() {
	close();
}

void axFileStream::_check_fd() const {
	if( ! isOpened() ) {
		throw axError_Undefined(AX_LOC);
	}
}

void axFileStream::openRead(axStrView filename) {
	open(filename, axFileMode::OpenExists, axFileAccess::Read, axFileShareMode::Read);
}

void axFileStream::openAppend(axStrView filename) {
	open(filename, axFileMode::OpenOrCreate, axFileAccess::ReadWrite, axFileShareMode::Read);
	setPosEnd(0);
}

void axFileStream::openWrite(axStrView filename, bool truncate) {
	open(filename, axFileMode::OpenOrCreate, axFileAccess::ReadWrite, axFileShareMode::Read);
	if (truncate) {
		setFileSize(0);
	}
}

void axFileStream::appendReadAllUtf8 ( axIString8& buf ) {
	_check_fd();

	axFileSize cur = getPos();
	axFileSize file_size = getFileSize();

	axInt size;
	if (!ax_try_safe_assign( size, file_size )) {
		throw axError_Undefined(AX_LOC);
	}

	if( cur > file_size ) throw axError_Undefined(AX_LOC);

    auto old_size  = buf.size();
	axInt data_size = 0;
	if (!ax_try_safe_assign(data_size, file_size - cur)) {
		throw axError_Undefined(AX_LOC);
	}

	try{
		buf.incSize(data_size);
		readBytes( reinterpret_cast<axByte*>(buf.data() + old_size), data_size );

	}catch(...) {
		buf.clear();
		throw;
	}
}

void axFileStream::appendReadAllText ( axIString & out_buf ) {
	#if AX_TYPE_CHAR_DEFINE == 'A'
		appendReadAllUtf8( out_buf );
	#else
		axTempString8 tmp;
		appendReadAllUtf8( tmp );
		out_buf.appendUtf( tmp );
	#endif
}

void axFileStream::writeText ( axStrView buf ) {
	axTempString8 buf8;
	buf8.setUtf(buf);
	writeUtf8(buf8);
}

void axFileStream::writeUtf8 ( const axStrView8& buf ) {
	writeBytes( buf.byteSpan() );
}

void axFileStream::appendReadAllBytes ( axIByteArray & buf ) {
	axFileSize cur = getPos();

	axFileSize	file_size = getFileSize();
	axInt size;
	if( !ax_try_safe_assign( size, file_size ) ) {
		throw axError_Undefined(AX_LOC);
	}
    if( cur > file_size ) throw axError_Undefined(AX_LOC);

	auto old_size  = buf.size();
	axInt data_size = 0;
	if (!ax_try_safe_assign(data_size, file_size - cur)) {
		throw axError_Undefined(AX_LOC);
	}

	buf.incSize(data_size);

	try{
		readBytes( reinterpret_cast<axByte*>(buf.data() + old_size), data_size );
	}catch(...){
		buf.clear();
		throw;
	}
}

#if 0
#pragma mark ================= Unix ====================
#endif
#if AX_OS_UNIX || AX_OS_MINGW || AX_OS_CYGWIN

#include <sys/stat.h>

axFileStream::axFileStream() {
	_fd = -1;
}

bool axFileStream::isOpened() const {
	return _fd != -1;
}

void axFileStream::close() {
	if( isOpened() ) {
		::close( _fd );
		_fd = -1;
	}
}

void axFileStream::flush () {
	int b = ::fsync( _fd );
	if( b != 0 ) throw axError_Undefined(AX_LOC);
}

axUtcTime	axFileStream::lastAccessTime 	() {
	_check_fd();
	struct stat info;
	if( 0 != fstat( _fd, &info ) ) throw axError_Undefined(AX_LOC);
	axUtcTime t;

    #if AX_OS_MACOSX || AX_OS_IOS
    	t = axUtcTime_make(info.st_atimespec);
	#elif AX_OS_UNIX || AX_OS_MINGW || AX_OS_CYGWIN
        t = axUtcTime_make(info.st_atime);
    #endif
	return t;
}

axUtcTime	axFileStream::lastWriteTime	( ) {
	_check_fd();
	struct stat info;
	if( 0 != fstat( _fd, &info ) ) throw axError_Undefined(AX_LOC);
	axUtcTime t;

    #if AX_OS_MACOSX || AX_OS_IOS
        t = axUtcTime_make(info.st_mtimespec);
	#elif AX_OS_MINGW
        t = axUtcTime_make(info.st_mtime);
    #else
        t = axUtcTime_make(info.st_mtim);
    #endif

	return t;
}

void axFileStream::setLastWriteTime(const axUtcTime& t) {
	_check_fd();

	#if AX_OS_MACOSX || AX_OS_IOS
		auto s = ax_timeval_make(t);
		timeval arr[2] = {s,s};
		futimes(_fd, arr);
	#elif AX_OS_ANDROID
		auto s = ax_timespec_make(t);
		timespec arr[2] = {s,s};
		utimensat(_fd, nullptr, arr, 0);
	#else
		auto s = ax_timespec_make(t);
		timespec arr[2] = {s,s};
		futimens(_fd, arr);
	#endif
}

void axFileStream::setPos( axFileSize  n ) {
	_check_fd();
	off_t ret = lseek( _fd, n, SEEK_SET );
	if( ret == -1 ) throw axError_Undefined(AX_LOC);
}

void axFileStream::advPos( axFileSize  n ) {
	_check_fd();
	off_t ret = lseek( _fd, n, SEEK_CUR );
	if( ret == -1 ) throw axError_Undefined(AX_LOC);
}

void axFileStream::setPosEnd( axFileSize  n ) {
	_check_fd();
	off_t ret = lseek( _fd, n, SEEK_END );
	if( ret == -1 ) throw axError_Undefined(AX_LOC);
}

axFileSize axFileStream::getPos () {
	_check_fd();
	axFileSize n = 0;
	off_t ret = lseek( _fd, n, SEEK_SET );
	if( ret == -1 ) throw axError_Undefined(AX_LOC);
	return axFileSize( ret );
}

axFileSize axFileStream::getFileSize() {
	_check_fd();
	axFileSize out = 0;

	off_t curr, r;

	curr = lseek( _fd, 0, SEEK_CUR );	if( curr == -1 ) throw axError_Undefined(AX_LOC);
	r = lseek( _fd, 0, SEEK_END );		if( r == -1 ) throw axError_Undefined(AX_LOC);
	out = r;
	r = lseek( _fd, curr, SEEK_SET );	if( r == -1 ) throw axError_Undefined(AX_LOC);

	return axFileSize( out );
}

void axFileStream::setFileSize( axFileSize newSize ) {
	_check_fd();

	axFileSize	oldPos = getPos();

	off_t o = newSize;
	if( 0 != ftruncate( _fd, o ) ) throw axError_Undefined(AX_LOC);

	if( oldPos < newSize ) setPos( oldPos );
}

void axFileStream::readBytes ( axByte* buf, axInt sizeInBytes ) {
	_check_fd();

	if( sizeInBytes <= 0 ) return;
	auto ret = ::read( _fd, buf, (size_t)sizeInBytes );
	if( ret <= 0 ) throw axError_Undefined(AX_LOC);
}

void axFileStream::writeBytes( axByteSpan buf ) {
	_check_fd();
	if( buf.size() <= 0 ) return;
	auto ret = ::write( _fd, buf.data(), (size_t)buf.sizeInBytes() );
	if( ret <= 0 ) throw axError_Undefined(AX_LOC);
}

//----- open file
void axFileStream::_os_open( axStrView filename, int access_flag ) {
	close();
#if axOS_Linux
	access_flag |= O_LARGEFILE;
#endif
	auto filenameA = axTempString8::s_convert(filename);

	#if AX_OS_MINGW
		mode_t mode = S_IRUSR | S_IWUSR;
	#else
		mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	#endif

	_fd = ::open( filenameA.c_str(), access_flag, mode );
	if( _fd == -1 ) {
		AX_LOG("open file error {?}: filename=[{?}]", errno, filename);
		switch( errno ) {
			case EACCES: throw axError_Undefined(AX_LOC);
			case EEXIST: throw axError_Undefined(AX_LOC);
			case ENFILE: throw axError_Undefined(AX_LOC);
			case EMFILE: throw axError_Undefined(AX_LOC);
			case ENOENT: throw axError_Undefined(AX_LOC);
			default:
				AX_ASSERT(false);
				throw axError_Undefined(AX_LOC);
				break;
		}
	}
}

void axFileStream::open ( axStrView filename, axFileMode mode, axFileAccess access, axFileShareMode share ) {
	_filename = filename;
	int access_flag = 0;

	switch( mode ) {
		case axFileMode::CreateNew: 	access_flag |= O_CREAT | O_EXCL;	break;
		case axFileMode::OpenExists:	break;
		case axFileMode::OpenOrCreate: 	access_flag |= O_CREAT;				break;
	}

	switch( access ) {
		case axFileAccess::Read:		access_flag |= O_RDONLY;	break;
		case axFileAccess::ReadWrite:	access_flag |= O_RDWR;		break;
		case axFileAccess::WriteOnly:	access_flag |= O_WRONLY;	break;
	}

	_os_open( filename, access_flag );
}

bool	axFileStream::_os_lock( int flags ) {
	_check_fd();

	int rc;
	/* keep trying if flock() gets interrupted (by a signal) */
	while ((rc = flock( _fd, flags)) < 0 && errno == EINTR)
		continue;

	if (rc == -1) return false;
	return true;
}

void	axFileStream::lock( bool exclusive ) {
	if( exclusive ) {
		if( ! _os_lock( LOCK_EX ) ) throw axError_Undefined(AX_LOC);
	}else{
		if( ! _os_lock( LOCK_SH ) ) throw axError_Undefined(AX_LOC);
	}
}

bool	axFileStream::trylock( bool exclusive ) {
	if( exclusive ) {
		return _os_lock( LOCK_EX | LOCK_NB );
	}else{
		return _os_lock( LOCK_SH | LOCK_NB );
	}
}

void	axFileStream::unlock() {
	_os_lock( LOCK_UN );
}

#endif //axOS_Unix

#if 0
#pragma mark ================= Windows ====================
#endif
#if AX_OS_WINDOWS

axFileStream::axFileStream() {
	_fd = INVALID_HANDLE_VALUE;
}

bool axFileStream::isOpened() const {
	return _fd != INVALID_HANDLE_VALUE;
}

void axFileStream::close() {
	if( isOpened() ) {
		BOOL ret = ::CloseHandle( _fd );
		AX_UNUSED(ret);
		assert( ret );
		_fd = INVALID_HANDLE_VALUE;
	}
}

void axFileStream::flush () {
	BOOL b = FlushFileBuffers( _fd );
	if( ! b ) throw axError_Undefined(AX_LOC);
}

axUtcTime	axFileStream::lastAccessTime 	() {
	_check_fd();
	BY_HANDLE_FILE_INFORMATION info;
	if( ! ::GetFileInformationByHandle( _fd, &info ) ) throw axError_Undefined(AX_LOC);
	auto t = axUtcTime_make(info.ftLastAccessTime);
	return t;
}

axUtcTime axFileStream::lastWriteTime	() {
	_check_fd();

	BY_HANDLE_FILE_INFORMATION info;
	if( ! ::GetFileInformationByHandle( _fd, &info ) ) throw axError_Undefined(AX_LOC);
	auto t = axUtcTime_make(info.ftLastWriteTime);
	return t;
}

void axFileStream::setLastWriteTime(const axUtcTime& t) {
	_check_fd();
	auto ft = axFILETIME_make(t);
	SetFileTime(_fd, nullptr, nullptr, &ft);
}

axFileSize axFileStream::getPos() {
	_check_fd();

	axFileSize n;
	LONG low, hi = 0;
	low = SetFilePointer( _fd, 0, &hi, FILE_CURRENT );
	int64_t	i64 = low | ( (int64_t)hi << 32 );

	if (!ax_try_safe_assign( n, i64 )) {
		throw axError_Undefined(AX_LOC);
	}
	return n;
}

void axFileStream::setPos( axFileSize n ) {
	_check_fd();

	int64_t	i64;
	if (!ax_try_safe_assign( i64, n )) {
		throw axError_Undefined(AX_LOC);
	}
	LONG hi = i64 >> 32;
	SetFilePointer( _fd, (LONG)i64, &hi, FILE_BEGIN );
}

void axFileStream::advPos( axFileSize n ) {
	_check_fd();

	int64_t	i64;
	if ( !ax_try_safe_assign( i64, n )) {
		throw axError_Undefined(AX_LOC);
	}
	LONG hi = i64 >> 32;
	SetFilePointer( _fd, (LONG)i64, &hi, FILE_CURRENT );
}

void axFileStream::setPosEnd( axFileSize n ) {
	_check_fd();

	int64_t	i64;
	if (!ax_try_safe_assign( i64, n )) {
		throw axError_Undefined(AX_LOC);
	}
	LONG hi = i64 >> 32;
	SetFilePointer( _fd, (LONG)i64, &hi, FILE_END );
}

axFileSize axFileStream::getFileSize() {
	_check_fd();

	axFileSize n;

	DWORD high = 0;
	DWORD low = ::GetFileSize( _fd, &high );
	if( low == INVALID_FILE_SIZE ) throw axError_Undefined(AX_LOC);

    uint64_t tmp = ((uint64_t)high) << 32 | low;
	if (!ax_try_safe_assign( n, tmp )) {
		throw axError_Undefined(AX_LOC);
	}
	return n;
}

void	axFileStream::setFileSize		( axFileSize newSize ) {
	_check_fd();

	axFileSize	oldPos = getPos();
	setPos( newSize );
	SetEndOfFile( _fd );

	if( oldPos < newSize ) setPos( oldPos );
}

void axFileStream::readBytes( axByte *buf, axInt sizeInBytes ) {
	_check_fd();

	if( sizeInBytes == 0 ) return;
	DWORD	n;
	if ( !ax_try_safe_assign( n, sizeInBytes ) ) {
		throw axError_Undefined(AX_LOC);
	}
	DWORD	result = 0;
	BOOL ret = ::ReadFile( _fd, buf, n, &result, nullptr );
	if( !ret ) {
		DWORD e = GetLastError();
//		ax_log_win32_error("FileStream read file", e);
		switch( e ) {
			case ERROR_LOCK_VIOLATION: throw axError_Undefined(AX_LOC);
		}
		throw axError_Undefined(AX_LOC);
	}
}

void axFileStream::writeBytes(axByteSpan buf) {
	_check_fd();

	if( buf.size() <= 0 ) return;

	DWORD	n;
	if ( !ax_try_safe_assign( n, buf.sizeInBytes() ) ) {
		throw axError_Undefined(AX_LOC);
	}
	DWORD	result = 0;
	BOOL ret = ::WriteFile( _fd, buf.data(), n, &result, nullptr );
	if( !ret ) {
		throw axError_Undefined(AX_LOC);
	}
}

void axFileStream::open	( axStrView filename, axFileMode mode, axFileAccess access, axFileShareMode share ) {
	_filename = filename;
	close();

	DWORD	create_flag	= 0;
	DWORD	access_flag = 0;
	DWORD	share_flag	= 0;

	switch( mode ) {
		case axFileMode::CreateNew: 		create_flag |= CREATE_NEW;		break;
		case axFileMode::OpenExists:		create_flag |= OPEN_EXISTING;	break;
		case axFileMode::OpenOrCreate:	    create_flag |= OPEN_ALWAYS;		break;
	}

	switch( access ) {
		case axFileAccess::Read:			access_flag |= GENERIC_READ;					break;
		case axFileAccess::ReadWrite:		access_flag |= GENERIC_READ | GENERIC_WRITE;	break;
		case axFileAccess::WriteOnly:		access_flag |= GENERIC_WRITE;					break;
	}

	switch( share ) {
	    case axFileShareMode::None: break;
		case axFileShareMode::Read:		    share_flag |= FILE_SHARE_READ;						break;
		case axFileShareMode::ReadWrite:	share_flag |= FILE_SHARE_READ | FILE_SHARE_WRITE;	break;
		case axFileShareMode::Write:		share_flag |= FILE_SHARE_WRITE;						break;
	}

	axTempStringW filenameW;
	filenameW.setUtf(filename);

	_fd = ::CreateFile( filenameW.c_str(), access_flag, share_flag, nullptr, create_flag, FILE_ATTRIBUTE_NORMAL, nullptr );
	if( _fd == INVALID_HANDLE_VALUE ) {
		DWORD err = GetLastError();
//		ax_log_win32_error( "File_open", err );
		AX_LOG("open file error {?}: filename=[{?}]", errno, filename);
		switch( err ) {
			case ERROR_FILE_NOT_FOUND:		throw axError_Undefined(AX_LOC);
			case ERROR_PATH_NOT_FOUND:		throw axError_Undefined(AX_LOC);
			case ERROR_FILE_EXISTS:			throw axError_Undefined(AX_LOC);
			case ERROR_ALREADY_EXISTS:		throw axError_Undefined(AX_LOC);
			case ERROR_ACCESS_DENIED:		throw axError_Undefined(AX_LOC);
			case ERROR_SHARING_VIOLATION:	throw axError_Undefined(AX_LOC);
		}
		throw axError_Undefined(AX_LOC);
	}
}

bool axFileStream::_os_lock( DWORD flags ) {
	_check_fd();

    OVERLAPPED offset = {};
	const DWORD len = 0xffffffff;
	return 0 != ::LockFileEx( _fd, flags, 0, len, len, &offset );
}

void axFileStream::lock( bool exclusive ) {
	if( exclusive ) {
		if( ! _os_lock( LOCKFILE_EXCLUSIVE_LOCK ) ) throw axError_Undefined(AX_LOC);
	}else{
		if( ! _os_lock( 0 ) ) throw axError_Undefined(AX_LOC);
	}
}

bool axFileStream::trylock( bool exclusive ) {
	if( exclusive ) {
	    return _os_lock( LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY );
	}else{
	    return _os_lock( LOCKFILE_FAIL_IMMEDIATELY );
	}
}

void axFileStream::unlock() {
	_check_fd();

    DWORD len = 0xffffffff;
	OVERLAPPED offset = {};
	if ( ! ::UnlockFileEx( _fd, 0, len, len, &offset ) ) {
		throw axError_Undefined(AX_LOC);
	}
}

#endif //AX_OS_WINDOWS
