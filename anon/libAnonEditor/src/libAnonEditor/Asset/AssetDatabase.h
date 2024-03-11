#pragma once

namespace Anon{ namespace Editor {

class AssetDatabase;

class AssetDatabaseEntry : public AnonObject, public axDListNode<AssetDatabaseEntry> {
	AX_RTTI_CLASS(AssetDatabaseEntry, AnonObject)
public:
	using Entry = AssetDatabaseEntry;

	AssetDatabaseEntry(AssetDatabase* db, Entry* parent, axStrView name, bool isDir);

	axStrView name() const { return _name; }
	void reload();
	void setPath(axStrView path) { _path = path; }
	axDList<Entry>::Enumerator	children()	{ return _children.each(); }

private:
	axString		_name;
	axString		_path;
	bool			_isDir = false;
	axDList<Entry>	_children;
	AssetDatabase*	_database = nullptr;
};

class AssetDatabase : public AnonObject {
	AX_RTTI_CLASS(AssetDatabase, AnonObject)
public:
	using Entry = AssetDatabaseEntry;

	void create(axStrView rootDir);

	void reload();

	Entry* root() { return _root.ptr(); }

	axStrView rootDir() { return _rootDir; }

private:
	void _loadDir(Entry* entry, axStrView filePath);

	axUPtr<Entry> _root;
	axString	_rootDir;
};

}} //namespace