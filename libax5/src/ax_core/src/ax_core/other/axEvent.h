#pragma once

#include <ax_core/container/axDList.h>
#include <ax_core/pointer/axWeak.h>
#include <ax_core/other/axDelegate.h>

template<bool ENTER_ONCE, class FuncSig>
class axEventBase;

template<bool ENTER_ONCE, class... ARGS>
class axEventBase<ENTER_ONCE, void (ARGS...)> : public axNonCopyable {
public:
	const static bool kEnterOnce = ENTER_ONCE;

						void bind(void(*func)(ARGS...))								{ _bind(func); }
	template<class OBJ>	void bind(OBJ* obj, void(OBJ::*func)(ARGS...))				{ _bind(obj, func); }
	template<class OBJ>	void bind(OBJ* obj, axFunc<void (ARGS...)> func)			{ _bind(obj, func); }

	template<class OBJ>	void weakBind(axSPtr<OBJ>& obj, void(OBJ::*func)(ARGS...))	{ _weakBind(obj, func); }

	//bind object RAW pointer 
	template<class OBJ>	void unsafeBind(OBJ* obj, void(OBJ::*func)(ARGS...))		{ _unsafeBind(obj, func); }

	//-------------------
						void unbindAll()											{ _unbindAll(); }
						void unbind(void(*func)(ARGS...))							{ _unbind(func); }
	template<class OBJ>	void unbind(axSPtr<OBJ>& obj, void(OBJ::*func)(ARGS...))	{ _unbind(obj.ptr(), func); }
	template<class OBJ>	void unbind(axLink<OBJ>& obj, void(OBJ::*func)(ARGS...))	{ _unbind(obj.ptr(), func); }
	template<class OBJ>	void unbind(OBJ* obj, void(OBJ::*func)(ARGS...))			{ _unbind(obj, func); }
	template<class OBJ>	void unbindByObject(axSPtr<OBJ>& obj)						{ _unbindByObject(obj.ptr()); }
	template<class OBJ>	void unbindByObject(axLink<OBJ> obj)						{ _unbindByObject(obj.ptr()); }
	template<class OBJ> void unbindByObject(OBJ* obj)								{ _unbindByObject(obj); }

	AX_INLINE void emit(ARGS... args)			{ _emit(ax_forward(args)...); }
	AX_INLINE bool valid() const				{ return _nodes.size() != 0; }
	AX_INLINE explicit operator bool () const	{ return valid(); }

private:
	using Delegate = axDelegateBase<ENTER_ONCE, void (ARGS...)>;

	struct Node : public axDListNode<Node> {
		Delegate dg;
	};

	void _bind(void(*func)(ARGS...)) {
		_nodes.append(ax_new Node())->dg.bind(func);
	}

	template<class OBJ>
	void _bind(OBJ* obj, void(OBJ::*func)(ARGS...)) {
		if (!obj) return;
		_nodes.append(ax_new Node())->dg.bind(obj, func);
	}

	template<class OBJ>
	void _bind(OBJ* obj, axFunc<void (ARGS...)> func) {
		if (!obj) return;
		_nodes.append(ax_new Node())->dg.bind(obj, func);
	}

	template<class OBJ>
	void _weakBind(axSPtr<OBJ>& obj, void(OBJ::*func)(ARGS...)) {
		if (!obj) return;
		_nodes.append(ax_new Node)->dg.weakBind(obj, func);
	}

	void _unbindAll() {
		_nodes.clear();
	}

	template<class OBJ>
	void _unsafeBind(OBJ* obj, void(OBJ::*func)(ARGS...)) {
		if (!obj) return;
		_nodes.append(ax_new Node())->dg.unsafeBind(obj, func);
	}

	void _unbind(void(*func)(ARGS...)) {
		auto* p = _nodes.head();
		while (p) {
			auto* next = p->listNode_next;
			if (p->dg.hasBoundTo(func)) {
				_nodes.remove(p);
				ax_delete(p);
			}
			p = next;
		}
	}

	template<class OBJ>
	void _unbind(OBJ* obj, void(OBJ::*func)(ARGS...)) {
		auto* p = _nodes.head();
		while (p) {
			auto* next = p->listNode_next;
			if (p->dg.hasBoundTo(obj, func)) {
				_nodes.remove(p);
				ax_delete(p);
			}
			p = next;
		}
	}

	template<class OBJ>
	void _unbindByObject(OBJ* obj) {
		auto* p = _nodes.head();
		while (p) {
			auto* next = p->listNode_next;
			if (p->dg.hasBoundToObject(obj)) {
				_nodes.remove(p);
				ax_delete(p);
			}
			p = next;
		}
	}

	void _emit(ARGS... args) {
		AX_IF_CONSTEXPR (!kEnterOnce) {
			_internalEmit(ax_forward(args)...);
		} else if (auto re = _enterOnce.enter()) {
			_internalEmit(ax_forward(args)...);
		}
	}

	void _internalEmit(ARGS... args) {	
		auto* p = _nodes.head();
		while (p) {
			auto* next = p->listNode_next;
			bool valid = true;
			p->dg.emitValid(valid, ax_forward(args)...);
			if (!valid) {
				_nodes.remove(p);
				ax_delete(p);
			}
			p = next;
		}
	}

	axDList<Node> _nodes;
	axEnterOnce _enterOnce;
};

template<class FuncSig> using axEvent			= axEventBase<true,  FuncSig>;
template<class FuncSig> using axReentrantEvent	= axEventBase<false, FuncSig>;

