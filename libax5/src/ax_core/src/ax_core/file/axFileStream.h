//
//  FileStream.h
//  ax_core
//
//  Created by Jason Chan on 12/2/14.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#pragma once

#include "axFile.h"

class axFileStream : public axNonCopyable {
public:
	void		openRead			( axStrView filename);
	void		openAppend			( axStrView filename);
	void		openWrite			( axStrView filename, bool truncate);

	void		open				( axStrView filename, axFileMode mode, axFileAccess access, axFileShareMode share );

	axUtcTime	lastAccessTime 		();
	axUtcTime	lastWriteTime		();
	void		setLastWriteTime	(const axUtcTime& t);

	void		close				();
	bool		isOpened			() const;

	void		lock				( bool exclusive );
	bool		trylock				( bool exclusive );
	void		unlock				();

	void		setPos				( axFileSize n );
	void		advPos				( axFileSize n ); //!< advance position
	void		setPosEnd			( axFileSize n ); //!< set position from end

	axFileSize	getPos				();
	axFileSize	getFileSize			();
	void		setFileSize			( axFileSize n );

//------
	void		readBytes			( axByte* buf, axInt byte_size );
	void		writeBytes			( axByteSpan buf );

	void		writeText			( axStrView s );
	void		writeUtf8			( const axStrView8& s );

	void		readAllBytes		( axIByteArray& out_buf )	{ out_buf.clear(); appendReadAllBytes(out_buf); }
	void		appendReadAllBytes	( axIByteArray& out_buf );

	void		readAllText			( axIString& out_buf )		{ out_buf.clear(); appendReadAllText(out_buf); }
	void		appendReadAllText	( axIString& out_buf );

	void		readAllUtf8			( axIString8 & out_buf )	{ out_buf.clear(); appendReadAllUtf8(out_buf); }
	void		appendReadAllUtf8	( axIString8 & out_buf );

	void		flush			();

	axStrView	filename		() const { return _filename; }

	axFileStream();
	~axFileStream();

#if AX_OS_WINDOWS
public:
	HANDLE		nativeFileDescriptor()	{ return _fd; }
private:
	bool		_os_lock	( DWORD flags );
	HANDLE		_fd;
#endif

#if AX_OS_UNIX || AX_OS_MINGW || AX_OS_CYGWIN
public:
	int			nativeFileDescriptor()	{ return _fd; }
private:
	void		_os_open	( axStrView filename, int access_flag );
	bool		_os_lock	( int flags );
	int			_fd;
#endif

private:
	axString	_filename;
	void		_check_fd	() const;
};
