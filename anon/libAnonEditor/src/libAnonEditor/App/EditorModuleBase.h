#pragma once

namespace Anon { namespace Editor {

class EditorModuleBase : public axNonCopyable {
public:
	void create() { onCreate(); }
	void destory() { onDestroy(); }

	virtual void onCreate()  {}
	virtual void onDestroy() {}
};

}} // namespace