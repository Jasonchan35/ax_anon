//
//  Path.h
//  ax_core
//
//  Created by Jason Chan on 12/2/14.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#pragma once

#include "../string/axString.h"

struct axPath : public axStaticClass {
	enum { kMaxChar = 512 };

	static	const axStrView		kSeperators;		// both '/' and '\\'
	static	const axChar		kUnixSeperator;		// '/'
	static	const axChar		kWindowsSeperator;	// '\\'
	static	const axChar		kNativeSeperator;

	struct SplitResult {
		axStrView driver;
		axStrView dir;
		axStrView name;
		axStrView ext;
	};

	static	SplitResult	split			(axStrView path);

	// e.g. changeExtension( "a.txt", ".doc" );
	static	void		changeExtension	(axIString& out_value, axStrView ext);

	static	axStrView	dirname			(axStrView path);
	static	axStrView	basename		(axStrView path, bool withExtension);
	static	axStrView	extension		(axStrView path);

	static	void		unixPath		(axIString& outPath, axStrView inPath);
	static	void		windowsPath		(axIString& outPath, axStrView inPath);

	static	bool		isAbsolute		(const axStrView& path);

	static	void		makeFullPath	(axIString& out_str, axStrView dir, axStrView path);
	static	void		getAbsolute		(axIString& out_str, axStrView path);
	static	void		getRelative		(axIString& out_str, axStrView path, axStrView relativeTo);

	static	bool		hasPrefix		(axStrView path, axStrView prefix, bool ignoreCase = true);

	static	void		append			(axIString& folder, axStrView file);

	static	void		glob			(axIArray<axString>& outPaths, axStrView searchPath, bool needDir=false, bool needFiles=true, bool needHidden=false);
	static	void		appendGlob		(axIArray<axString>& outPaths, axStrView searchPath, bool needDir=false, bool needFiles=true, bool needHidden=false);

	static	void		getCurrentDir	(axIString & path);
	static	void		setCurrentDir	(axStrView path);

	static	axString	currentDir		();
	
	static	axStrView	userAppDataDir		();
	static	axStrView	userDocumentDir		();
	static	axStrView	userHomeDir			();
	static	axStrView	userDesktopDir		();
	static	axStrView	appResourcesDir		();
	static	axStrView	currentProcessDir	();
	static	axStrView	currentProcessFile	();
	static	axStrView	tempDir				();
};
