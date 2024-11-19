//
//  Directory.cpp
//  ax_core
//
//  Created by Jason Chan on 12/2/14.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#include <ax_core/file/axDirectory.h>
#include <ax_core/file/axPath.h>
#include <ax_core/file/axFile.h>
#include <ax_core/log/axLog.h>
#include <ax_core/app/axApp.h>

#if 0
#pragma mark ================= Unix ====================
#endif
#if AX_OS_UNIX || AX_OS_CYGWIN || AX_OS_MINGW

#include <dirent.h>
#include <sys/stat.h>

bool	axDirectory::exists	( axStrView path ) {
	axTempString8 pathA;
	pathA.setUtf(path);

	struct stat s;
	if( 0 != ::stat( pathA.c_str(), &s ) ) return false;
	return ( s.st_mode & S_IFDIR ) != 0;
}

void	axDirectory_os_create	( axStrView path ) {
	axTempString8 pathA;
	pathA.setUtf(path);

	#if AX_OS_MINGW
		auto ret = ::mkdir( pathA.c_str() );
	#else
		auto ret = ::mkdir( pathA.c_str(), 0755 );
	#endif

	if( ret != 0 ) {
		if( errno == EEXIST ) return; //already exists
		throw axError_Undefined(AX_LOC);
	}
}

void	 axDirectory_os_remove ( axStrView path )    {
	axTempString8 pathA;
	pathA.setUtf(path);

	auto ret = ::rmdir( pathA.c_str() );
	if( ret != 0 ) {
		throw axError_Undefined(AX_LOC);
	}
}

void axDirectory::appendGetFileSystemEntries(axIArray<Entry> & result, axStrView path, bool subDir, FilterFunc filter) {
	axTempString8 pathA;
	pathA.setUtf(path);

	struct dirent*	r = nullptr;
	axDirectory::Entry e;

	auto h = ::opendir( pathA.c_str() );
	if( ! h ) {
		AX_DUMP_VAR( pathA );
		throw axError_Undefined(AX_LOC);
	}

	try{
		for(;;) {

			#if 1 // AX_OS_MINGW
				r = ::readdir( h );
			#else
				struct dirent	entry;
				if( 0 != ::readdir_r( h, &entry, &r ) ) {
					throw axError_Undefined(AX_LOC);
				}
			#endif

			if( ! r ) break;

			e.hidden = axCStr::hasPrefix( r->d_name, ".");

			if (axCStr::equals(r->d_name, "." )) continue;
			if (axCStr::equals(r->d_name, "..")) continue;

			#if AX_OS_MINGW || AX_OS_CYGWIN
				struct stat statbuf;
				axTempString8 tmpFullpath;
				tmpFullpath.set(path);
				tmpFullpath.append_c_str(r->d_name);
				if (stat(r->d_name, &statbuf) == -1) {
					throw axError_Undefined(AX_LOC);
				}
				e.isDir = S_ISDIR(statbuf.st_mode);
			#else
				e.isDir = ( r->d_type & DT_DIR );
			#endif
			e.name.setUtf_c_str(r->d_name);
			e.path.set(path);
			axPath::append(e.path, e.name);

			if (filter) {
				if (!filter(e)) continue;
			}

			result.append(ax_move(e));

			if(e.isDir && subDir) {
				appendGetFileSystemEntries( result, e.name, subDir, filter );
			}
		}
	}catch(...){
		::closedir( h );
		throw;
	}
}

#endif // Unix

#if 0
#pragma mark ================= AX_OS_WINDOWS ====================
#endif
#if AX_OS_WINDOWS

static void	axDirectory_os_remove	( axStrView path ) {
	axTempStringW pathW;
	pathW.setUtf(path);
	if (0 == ::RemoveDirectory(pathW.c_str())) {
		throw axError_Undefined(AX_LOC);
	}
}

static void	axDirectory_os_create	( axStrView path ) {
	axTempStringW pathW;
	pathW.setUtf(path);
	if (0 == ::CreateDirectory(pathW.c_str(), nullptr)) {
		throw axError_Undefined(AX_LOC);
	}
}

bool	axDirectory::exists		( axStrView path ) {
	axTempStringW pathW;
	pathW.setUtf(path);
	DWORD dwAttrib = ::GetFileAttributes(pathW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

class axDirectory_Win32Handle : public axNonCopyable {
public:
	axDirectory_Win32Handle(HANDLE h = INVALID_HANDLE_VALUE) : _h(h) {}
	~axDirectory_Win32Handle() { unref(); }

	void set(HANDLE h) { unref(); _h = h; }
	void unref() { if(isValid()) {::FindClose(_h); _h = INVALID_HANDLE_VALUE; } }

	HANDLE	get() { return _h; }
	operator HANDLE() { return _h; }

	bool isValid() const { return _h != INVALID_HANDLE_VALUE; }

private:
	HANDLE _h;
};

void axDirectory::appendGetFileSystemEntries( axIArray<Entry> & result, axStrView path, bool subDir, FilterFunc filter) {
	axTempStringW pathW;
	pathW.format("{0}/*", path);

	WIN32_FIND_DATA data;
	axDirectory_Win32Handle h(::FindFirstFile(pathW.c_str(), &data));
	if (!h.isValid()) {
		throw axError_Undefined(AX_LOC);
	}

	axDirectory::Entry e;

	do{
		auto filename = axStrView_c_str(data.cFileName);
		if (filename == L"." || filename == L"..") continue;

		e.name.setUtf(filename);
		e.path.set(path);
		axPath::append(e.path, e.name);
		
		e.isDir  = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
		e.hidden = (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? true : false;

		if(e.isDir && subDir) {
			appendGetFileSystemEntries( result, e.name, subDir, filter );
		}

		if (filter) {
			if (!filter(e)) continue;
		}

		result.append(ax_move(e));

	}while(::FindNextFile(h, &data));

	auto err = ::GetLastError();
	if (err != ERROR_NO_MORE_FILES) {
		throw axError_Undefined(AX_LOC);
	}
}

#endif // windows

void axDirectory::getFileSystemEntries	( axIArray<Entry> & result, axStrView path, bool subDir, axDirectory::FilterFunc filter ) {
	result.clear();
	appendGetFileSystemEntries( result, path, subDir, filter);
}

void axDirectory::getFiles				( axIArray<Entry> & result, axStrView path, bool subDir, bool includeHiddenFile )	{
	result.clear();
	appendGetFileSystemEntries( result, path, subDir, [&](Entry& entry){
		if (!includeHiddenFile && entry.hidden) return false;
		return !entry.isDir;
	});
}

void axDirectory::getDirectories		( axIArray<Entry> & result, axStrView path, bool subDir, bool includeHiddenFile ) {
	result.clear();
	appendGetFileSystemEntries( result, path, subDir, [&](Entry& entry){
		if (!includeHiddenFile && entry.hidden) return false;
		return entry.isDir;
	});
}

void	axDirectory::removeIfExists	( axStrView path, bool subDir ) {
	if( axDirectory::exists( path ) ) {
		axDirectory::remove( path, subDir );
	}
}

void	axDirectory::remove ( axStrView path, bool subDir ) {
	if( ! subDir ) return axDirectory_os_remove( path );

	axArray<Entry, 32> list;

	getDirectories( list, path, false, true );
	for( auto& p : list.each() ) {
		axDirectory::remove( p.name, subDir );
	}

	getFiles( list, path, false, true );
	for( auto& p : list.each() ) {
		axFile::remove( p.name );
	}

	axDirectory_os_remove( path );
}

void axDirectory::create( axStrView path, bool subDir ) {
	if (exists(path)) return;

	if (subDir) {
		if (auto parent = axPath::dirname(path)) {
			create(parent, subDir);
		}
	}

	axDirectory_os_create(path);
}

