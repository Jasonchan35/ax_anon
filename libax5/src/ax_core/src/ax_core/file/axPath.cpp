//
//  Path.cpp
//  ax_core
//
//  Created by Jason Chan on 12/2/14.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#include <ax_core/file/axPath.h>
#include <ax_core/file/axDirectory.h>
#include <ax_core/app/axApp.h>

const axStrView	axPath::kSeperators			= AX_STR("/\\");
const axChar	axPath::kUnixSeperator		= AX_CH('/');
const axChar	axPath::kWindowsSeperator	= AX_CH('\\');

#if AX_OS_WINDOWS
	const axChar	axPath::kNativeSeperator = axPath::kWindowsSeperator;
#else
	const axChar	axPath::kNativeSeperator = axPath::kUnixSeperator;
#endif

axPath::SplitResult axPath::split(axStrView path) {
	SplitResult o;

	auto p = path;
	auto s = p.splitByChar(':');
	if (s.second) {
		o.driver = s.first;
		p = s.second;
	}

	s = p.splitByCharsBack("\\/");
	if (s.second) {
		o.dir = s.first;
		p = s.second;
	}

	s = p.splitByCharBack('.');
	o.name = s.first;
	o.ext = s.second;

	return o;
}

axStrView axPath::dirname ( axStrView path ) {
	auto s = path.splitByCharsBack("\\/");
	return s.second ? s.first : axStrView();
}

axStrView axPath::basename ( axStrView path, bool withExtension ) {
	auto s = path.splitByCharsBack("\\/");
	auto f = s.second ? s.second : s.first;
	if (withExtension) return f;
	s = f.splitByCharBack('.');
	return s.first;
}

axStrView	axPath::extension ( axStrView path ) {
	//	remove dir first to avoid corner case like: "/aaa/bbb/ccc.here/eee"
	//	while should return "" instead or "here/eee"
	auto s = path.splitByCharsBack("\\/");
	auto f = s.second ? s.second : s.first;

	s = f.splitByCharBack('.');
	return s.second;
}

void axPath::unixPath(axIString& outPath, axStrView inPath) {
	outPath = inPath;
	outPath.replaceChars('\\', '/');
}

void axPath::windowsPath(axIString& outPath, axStrView inPath) {
	outPath = inPath;
	outPath.replaceChars('/', '\\');
}

void	axPath::changeExtension ( axIString& path, axStrView new_extension ) {
	axTempString tmp(path);
	path.set(dirname(tmp));
	if (path) {
		path.append("/");
	}
	path.append(basename(tmp, false));
	if (new_extension) {
		path.append('.', new_extension);
	}
}

bool axPath::isAbsolute(const axStrView& path) {
	if (path.size() < 1) return false;
	if (path[0] == '/') return true;

	if (path.size() < 2) return false;
	if (axCStr::isAlpha(path[0]) && path[1] == ':') return true;

	return false;
}

void axPath::makeFullPath(axIString& out_str, axStrView dir, axStrView path) {
	if (isAbsolute(path)) {
		getAbsolute(out_str, path); // normalize '.' or '..'
	}else{
		axString tmp;
		tmp.append(dir, '/', path);
		getAbsolute(out_str, tmp);
	}
}

void axPath::getAbsolute(axIString& out_str, axStrView path) {
	out_str.clear();

	if (!path) return;
	bool needSlash = false;
	if (isAbsolute(path)) {
		needSlash = (path[0] == '/'); //unix path need '/' at beginning

	}else{
		getCurrentDir(out_str);
		needSlash = true;
	}

	axStrView p = path;
	while (p) {
		auto s = p.splitByChars("\\/");
		if (s.first == ".") {
			//skip '.'
		}else if (!s.first) {
			//skip '/'
		}else if (s.first == "..") {
			auto idx = out_str.view().findCharsBack("\\/");
			if (idx < 0) {
				out_str.clear(); //no more parent folder
				return;
			}

			out_str.resize(idx);
		}else{
			if (needSlash) {
				out_str += '/';
			}
			out_str += s.first;
			needSlash = true;
		}
		p = s.second;
	}
}

void axPath::getRelative(axIString& out_str, axStrView path, axStrView relativeTo) {
	out_str.clear();

	axString src;
	getAbsolute(src, path);

	axString to;
	getAbsolute(to, relativeTo);

	auto sv = src.view();
	auto tv = to.view();
	
	//unix path starts with /
	if (sv && sv[0] == '/') sv = sv.sliceFrom(1);
	if (tv && tv[0] == '/') tv = tv.sliceFrom(1);
	
	for(;;) {
		auto sp = sv.splitByChar('/');
		auto tp = tv.splitByChar('/');
		if(!sp.first || !tp.first) break;
		if (sp.first != tp.first) break;
		
		sv = sp.second;
		tv = tp.second;
	}

	for(;;) {
		auto tp = tv.splitByChar('/');
		if (!tp.first) break;
		out_str.append("../");
		tv = tp.second;
	}

	out_str.append(sv);
}

bool axPath::hasPrefix(axStrView path, axStrView prefix, bool ignoreCase) {
	if (path.startsWith(prefix, ignoreCase))
	{
		if (path.size() == prefix.size())
			return true; // exact same path

		auto ch = path[prefix.size()];
		if (ch == '/' || ch == '\\')
			return true; // match folder
	}
	return false;
}

class axPath_GlobHelper : public axNonCopyable {
public:
	void search(axIArray<axString>& outPaths, axStrView searchPath, bool needDir, bool needFile, bool needHidden) {
		_needDir    = needDir;
		_needFile   = needFile;
		_needHidden = needHidden;

		_outPaths = &outPaths;
		axTempString absPath;
		axPath::getAbsolute(absPath, searchPath);

		auto p = absPath.view();
		auto start = p.splitByChars("*?").first
					  .splitByCharBack('/').first;

		_curPath = start;
		auto remain = p.sliceFrom(start.size() + 1);
		_step(remain);
	}

private:
	void _step(axStrView path) {
		auto s = path.splitByChar('/');
		_step2(s.first, s.second);
	}

	void _step2(axStrView name, axStrView remain) {
		if (name.findChars("*?") < 0) {
			auto oldSize = _curPath.size();
			if (name) {
				_curPath += '/';
				_curPath += name;
			}

			if (!remain) {
				if ((_needFile && axFile::exists(_curPath))
				  ||(_needDir  && axDirectory::exists(_curPath)))
				{
					_outPaths->append(_curPath);
				}
			}else{
				_step(remain);
			}
			_curPath.resize(oldSize);
			return;
		}
		
		if (name == "**") {
			_step(remain);
		}
		
		if (!axDirectory::exists(_curPath)) return;

		axArray<axDirectory::Entry>	entries;
		axDirectory::getFileSystemEntries(entries, _curPath, false, nullptr);

		for (auto& entry : entries) {
			if (!_needHidden && entry.name.startsWith(".")) {
				continue;
			}

			if (name == "**") {
				auto oldSize = _curPath.size();
				if (name) {
					_curPath += '/';
					_curPath += entry.name;
				}
				_step2(name, remain);
				_curPath.resize(oldSize);

				continue;
			}

			if (entry.name.matchWildcard(name, true)) {
				_step2(entry.name, remain);
			}
		}
	}

	axIArray<axString>* _outPaths = nullptr;
	axString _curPath;
	bool _needDir;
	bool _needFile;
	bool _needHidden;
};

void axPath::glob(axIArray<axString>& outPaths, axStrView searchPath, bool needDir, bool needFiles, bool needHidden) {
	outPaths.clear();
	appendGlob(outPaths, searchPath, needDir, needFiles, needHidden);
}

void axPath::appendGlob(axIArray<axString>& outPaths, axStrView searchPath, bool needDir, bool needFiles, bool needHidden) {
	auto index = searchPath.findChars("*?");
	if (index < 0) {
		outPaths.emplaceBack(searchPath);
		return;
	}

	axPath_GlobHelper helper;
	helper.search(outPaths, searchPath, needDir, needFiles, needHidden);
}

void axPath::append(axIString& folder, axStrView file) {
	if (folder.size() != 0) { // avoid "/file"
		auto c = folder.back();
		if (c != '/' && c != '\\') // avoid double '/'
			folder.append('/');
	}
	folder.append(file);
}

axStrView axPath::appResourcesDir() {
	struct Obj {
		Obj() {
			axTempString tmp(currentProcessDir());
		#if AX_OS_MACOSX
			tmp.append("/../Resources");
		#endif
			axPath::getAbsolute(path, tmp);
		}
		axTempString path;
	};

	static Obj obj;
	return obj.path;
}

axString axPath::currentDir() {
	axString tmp;
	getCurrentDir(tmp);
	return tmp;
}

axStrView axPath::currentProcessDir() {
	return dirname(currentProcessFile());
}

#if 0
#pragma mark ================= AX_OS_WINDOWS ====================
#endif
#if AX_OS_WINDOWS

#include <shlobj_core.h>

void axPath::getCurrentDir(axIString & path) {
	axStringW_<axPath::kMaxChar> w;
	w.resizeToLocalBufSize();
	auto n = GetCurrentDirectory((DWORD)w.size(), w.data());
	w.resize(n);
	path.setUtf(w);
	path.replaceChars('\\', '/');
}

void axPath::setCurrentDir(axStrView path){
	axTempStringW w;
	w.setUtf(path);
	SetCurrentDirectory(w.c_str());
}

class axPath_SHGetFolderPath {
public:
	axPath_SHGetFolderPath(int CSIDL) {
		wchar_t	p[MAX_PATH + 1];
		HRESULT ret = SHGetFolderPath(NULL, CSIDL | CSIDL_FLAG_DONT_VERIFY, NULL, SHGFP_TYPE_CURRENT, p);
		if( ret != S_OK ) throw axError_Undefined(AX_LOC);
		path.setUtf_c_str(p);
	}
	axTempString	path;
};

axStrView axPath::userAppDataDir	() { static axPath_SHGetFolderPath s(CSIDL_APPDATA);		return s.path; }
axStrView axPath::userDocumentDir	() { static axPath_SHGetFolderPath s(CSIDL_MYDOCUMENTS);	return s.path; }
axStrView axPath::userHomeDir		() { static axPath_SHGetFolderPath s(CSIDL_PROFILE);		return s.path; }
axStrView axPath::userDesktopDir	() { static axPath_SHGetFolderPath s(CSIDL_DESKTOP);		return s.path; }

axStrView axPath::currentProcessFile() {
	struct Obj {
		Obj() {
			wchar_t tmp[MAX_PATH + 1];
			DWORD n = ::GetModuleFileName(nullptr, tmp, MAX_PATH);
			if (n == 0) throw axError_Undefined(AX_LOC);
			tmp[n] = 0;
			path.setUtf(axStrView_make(tmp, n));
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

axStrView axPath::tempDir() {
	struct Obj {
		Obj() {
			wchar_t	tmp[MAX_PATH+1];
			tmp[MAX_PATH] = 0;
			DWORD n = ::GetTempPath(MAX_PATH, tmp);
			if (n == 0) throw axError_Undefined(AX_LOC);
			path.setUtf(axStrView_make(tmp, n));
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

#endif // AX_OS_WINDOWS


#if 0
#pragma mark ================= iOS / Mac ====================
#endif

#if AX_OS_IOS || AX_OS_MACOSX

class axPath_NSSearchPath {
public:
	axPath_NSSearchPath(NSSearchPathDirectory d) {
		NSArray *tmp = NSSearchPathForDirectoriesInDomains(d, NSUserDomainMask, YES);
		if (tmp == nil) throw axError_Undefined(AX_LOC);
		if ([tmp count] <= 0) throw axError_Undefined(AX_LOC);
		NSString *docu = [tmp objectAtIndex:0];
		if (docu == nil) throw axError_Undefined(AX_LOC);

		path.set(axStrView_make(docu));
	}

	axTempString path;
};

axStrView axPath::userAppDataDir	() { static axPath_NSSearchPath s(NSApplicationSupportDirectory);	return s.path; }
axStrView axPath::userDocumentDir	() { static axPath_NSSearchPath s(NSDocumentDirectory);				return s.path; }
axStrView axPath::userDesktopDir	() { static axPath_NSSearchPath s(NSDesktopDirectory);				return s.path; }

axStrView axPath::userHomeDir		() {
	struct Obj {
		Obj() {
			path.set(axStrView_make(NSHomeDirectory()));
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

axStrView axPath::currentProcessFile() {
	struct Obj {
		Obj() {
			NSProcessInfo* info = [NSProcessInfo processInfo];
			NSArray* args = [info arguments];
			if ([args count ] == 0) throw axError_Undefined(AX_LOC);

			auto s = (NSString*)[args objectAtIndex:0];
			path.set(axStrView_make(s));
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

#endif // iOS / Mac


#if 0
#pragma mark ================= Linux ====================
#endif

#if AX_OS_LINUX

#include <linux/limits.h>

axStrView axPath::userAppDataDir() {
	struct Obj {
		Obj() {
			path = userHomeDir();
			if (auto* app = axApp::s_instance()) {
				path.append("/", app->appName());
			} else {
				path.append("/", axPath::basename(currentProcessFile(), false));
			}
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

axStrView axPath::userDocumentDir() {
	struct Obj {
		Obj() {
			path = userHomeDir();
			path.append("/Documents");
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

axStrView axPath::userDesktopDir() {
	struct Obj {
		Obj() {
			path = userHomeDir();
			path.append("/Desktop");
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

axStrView axPath::userHomeDir() {
	struct Obj {
		Obj() {
			struct passwd	*p = ::getpwuid( ::getuid() );
			if (!p) throw axError_Undefined(AX_LOC);
			path.setUtf_c_str(p->pw_dir);
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

axStrView axPath::currentProcessFile() {
	struct Obj {
		Obj() {
			char buf[PATH_MAX + 1];
			ssize_t ret = readlink("/proc/self/exe", buf, PATH_MAX);
			if (ret == -1) throw axError_Undefined(AX_LOC);

			// if the /proc/self/exe symlink has been dropped by the kernel for
			// some reason, then readlink() could also return success but
			// "(deleted)" as link destination...
			if (axCStr::compare(buf, "(deleted)") == 0) {
				 throw axError_Undefined(AX_LOC);
			}

			buf[ret] = 0;
			path.setUtf_c_str(buf);
		}
		axTempString path;
	};
	static Obj obj;
	return obj.path;
}

#endif //axOS_Linux

#if 0
#pragma mark ================= Unix ====================
#endif
#if AX_OS_UNIX || AX_OS_CYGWIN || AX_OS_MINGW

void axPath::getCurrentDir(axIString& path) {
	path.clear();
	char  tmp[ axPath::kMaxChar + 1 ];
	if( ! ::getcwd( tmp, axPath::kMaxChar ) ) {
		throw axError_Undefined(AX_LOC);
	}
	tmp[ axPath::kMaxChar ] = 0;
	path.setUtf_c_str(tmp);
}

void axPath::setCurrentDir(axStrView path) {
	axTempString8 pathA;
	pathA.setUtf(path);
	::chdir(pathA.c_str());
}

axStrView axPath::tempDir() {
	return "/tmp";
}


#endif // Unix
