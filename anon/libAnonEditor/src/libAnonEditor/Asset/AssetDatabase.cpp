#include "AssetDatabase.h"

namespace Anon{ namespace Editor {

AssetDatabaseEntry::AssetDatabaseEntry(AssetDatabase* db, Entry* parent, axStrView name, bool isDir) 
	: _name(name)
	, _isDir(isDir)
	, _database(db)
{
	if (parent) {
		parent->_children.append(this);

		if (parent->_path.size()) {
			_path.set(parent->_path, "/", name);
		} else {
			_path = name;
		}
	}
}

void AssetDatabaseEntry::reload() {
	if (_isDir) {
		axArray<axDirectoryEntry, 16> fileEntries;
		axTempString fullpath(_database->rootDir(), "/", _path);

		try {
			axDirectory::getFileSystemEntries(fileEntries, fullpath, false, nullptr);
		} catch(axError&) {
			AX_LOG_WARNING("cannot load directory {?}", fullpath);
			return;
		}

		for (auto& e : fileEntries) {
			auto* c = ax_new Entry(_database, this, e.name, e.isDir);
			c->reload();
		}
	}
}

void AssetDatabase::create(axStrView rootDir) {
	_rootDir = rootDir;
	_root.ref(ax_new Entry(this, nullptr, "", true));
	_root->reload();
}

void AssetDatabase::reload() {
	if (_root) {
		_root->reload();
	}
}

}} // namespace
