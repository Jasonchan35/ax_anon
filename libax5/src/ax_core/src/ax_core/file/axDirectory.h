//
//  Directory.h
//  ax_core
//
//  Created by Jason Chan on 12/2/14.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#pragma once

#include "../container/axArray.h"
#include "../string/axString.h"

class axDirectoryEntry {
public:
	axString name;
	axString path;
	bool isDir  = false;
	bool hidden = false;
};


class axDirectory : axStaticClass {
public:
	using Entry = axDirectoryEntry;

	typedef axFunc<bool (Entry& entry)> FilterFunc;

	static	void	getFileSystemEntries	(axIArray<Entry> & result, axStrView path, bool subDir, FilterFunc filter);
	static	void	getFiles				(axIArray<Entry> & result, axStrView path, bool subDir, bool includeHiddenFile);
	static	void	getDirectories			(axIArray<Entry> & result, axStrView path, bool subDir, bool includeHiddenFile);

	static	bool	exists					(axStrView path);
	static	void	create					(axStrView path, bool subDir = true);
	static	void	remove					(axStrView path, bool subDir);
	static	void	removeIfExists			(axStrView path, bool subDir);

private:
	static	void	appendGetFileSystemEntries	(axIArray<Entry> & result, axStrView path, bool subDir, FilterFunc filter);
};
