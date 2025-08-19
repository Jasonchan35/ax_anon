#pragma once

namespace Anon { namespace Editor {

class EditorModuleBase : public axNonCopyable {
public:
	void create() { onCreate(); }
	void destroy() { onDestroy(); }

	virtual void onCreate()  {}
	virtual void onDestroy() {}
};

}} // namespace