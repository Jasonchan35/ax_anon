//
//  File.h
//  ax_core
//
//  Created by Jason Chan on 12/2/14.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#pragma once

#include "../string/axString.h"
#include "../container/axArrayPredefineTypes.h"
#include "../time/axTimestamp.h"

//--------------------

enum class axFileMode {
	CreateNew,
	OpenExists,
	OpenOrCreate,
};

enum class axFileAccess {
	Read,
	ReadWrite,
	WriteOnly,
};

enum class axFileShareMode {
	None,
	Read,
	Write,
	ReadWrite,
};

typedef	int64_t	axFileSize;


struct axFile : public axStaticClass {
	static	bool 		exists 					( axStrView filename );

	static	void		rename					( axStrView src_name, axStrView dst_name );

	static	void		remove					( axStrView filename );
	static	void		moveToRecycleBin		( axStrView filename );

	static	void		readBytes				( axStrView filename, axIByteArray& buf );
	static	void		readText				( axStrView filename, axIString&    buf );
	static	void		readUtf8				( axStrView filename, axIString8&   buf );

	static	void		appendReadBytes			( axStrView filename, axIByteArray& buf );
	static	void		appendReadText			( axStrView filename, axIString&    buf );
	static	void		appendReadUtf8			( axStrView filename, axIString8&   buf );

	static	void		writeBytes				( axStrView filename, axByteSpan buf );
	static	void		writeText				( axStrView filename, axStrView  text );
	static	void		writeUtf8				( axStrView filename, axStrView8 text );

	static	axChar		writeFile				( axStrView filename, axByteSpan buf,  bool createDir, bool logResult = true);
	static	axChar		writeFile				( axStrView filename, axStrView8 text, bool createDir, bool logResult = true);

	static	axChar		writeFileIfChanged		( axStrView filename, axByteSpan buf,  bool createDir, bool logResult = true, bool logNoChange = false);
	static	axChar		writeFileIfChanged		( axStrView filename, axStrView8 text, bool createDir, bool logResult = true, bool logNoChange = false);

	static	void		touch	(axStrView filename);
};
