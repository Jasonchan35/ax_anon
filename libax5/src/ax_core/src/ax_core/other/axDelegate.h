#pragma once
#include "../pointer/axWeak.h"

template<bool ENTER_ONCE, class FuncSig>
class axDelegateBase;

template<bool ENTER_ONCE, class RETURN_TYPE, class... ARGS>
class axDelegateBase<ENTER_ONCE, RETURN_TYPE (ARGS...)> : public axNonCopyable {
public:
	const static bool kEnterOnce = ENTER_ONCE;

	using ReturnType = RETURN_TYPE;

	axDelegateBase() = default;
	axDelegateBase(axDelegateBase && r) { operator=(ax_move(r)); }

	~axDelegateBase() { unbindAll(); }
						// global or class static function
						void bind(ReturnType(*func)(ARGS...))								{ _bind(func); }
	template<class OBJ> void bind(OBJ* obj, ReturnType(OBJ::*func)(ARGS...))				{ _bind(obj, func); }
	template<class OBJ>	void bind(OBJ* obj, axFunc<RETURN_TYPE (ARGS...)> func)				{ _bind(obj, func); }

	template<class OBJ>	void weakBind(axSPtr<OBJ>& obj, ReturnType(OBJ::*func)(ARGS...))	{ _weakBind(obj, func); }

						// for lambda, the local variable capture could be problematic for callback happen in different time
						void unsafeBind(axFunc<RETURN_TYPE (ARGS...)> func)					{ _unsafeBind(func); }
	template<class OBJ>	void unsafeBind(OBJ* obj, ReturnType(OBJ::*func)(ARGS...))			{ _unsafeBind(obj, func); }

						bool hasBoundTo(ReturnType(*func)(ARGS...))							{ return _hasBoundTo(func); }
	template<class OBJ>	bool hasBoundTo(OBJ* obj, ReturnType(OBJ::*func)(ARGS...))			{ return _hasBoundTo(obj, func); }
	template<class OBJ>	bool hasBoundToObject(OBJ* obj)										{ return _hasBoundToObject(obj); }

						void unbindAll() { _unbindAll(); }

	AX_INLINE RETURN_TYPE emit(ARGS... args)							{ bool v = false; return emitValid(v, ax_forward(args)...); }
	AX_INLINE RETURN_TYPE emitValid(bool& outValid, ARGS... args)		{ return _emitValid(outValid, ax_forward(args)...); }

	AX_INLINE bool valid() const { return _functor ? _functor->valid() : false; }
	AX_INLINE explicit operator bool () const { return valid(); }

	void operator=(axDelegateBase && r) {
		unbindAll();
		if (r._functor) {
			r._functor->moveTo(*this);
		}
	}

private:
	AX_INLINE RETURN_TYPE _emitValid(bool& outValid, ARGS... args) {
		outValid = false;
		if (_functor) {
			AX_IF_CONSTEXPR(!kEnterOnce) {
				return _functor->emit(outValid, ax_forward(args)...);
			} else if (auto re = _enterOnce.enter()) {
				return _functor->emit(outValid, ax_forward(args)...);
			}
		}
		return RETURN_TYPE();
	}

	enum class Type {
		Unknown,
		StaticFunc,
		LambdaFunc,
		UnsafeMemberFunc,
		LinkMemberFunc,
		LinkLambdaFunc,
		WeakMemberFunc,
	};

	struct Functor {
		Functor(Type type) : _type(type) {}
		virtual ~Functor() {}
		virtual bool valid() const = 0;
		virtual RETURN_TYPE emit(bool& outValid, ARGS... args) = 0;
		virtual bool hasBoundToObject(void* p) = 0;

		virtual void moveTo(axDelegateBase& de) = 0;

		Type _type = Type::Unknown;
	};

	struct StaticFunc : public Functor {
		using Func = ReturnType(*)(ARGS...);
		Func	_func = nullptr;

		StaticFunc(Func func)
			: Functor(Type::StaticFunc)
			, _func(func) 
		{
			AX_STATIC_ASSERT(sizeof(StaticFunc) <= kFunctorlocalBufSize);
		}

		virtual bool valid() const override { return true; }
		virtual bool hasBoundToObject(void* p) override { return false; }

		virtual RETURN_TYPE emit(bool& outValid, ARGS... args) override {
			outValid = true;
			return _func(ax_forward(args)...); 
		}
		virtual void moveTo(axDelegateBase& de) override { de.bind(_func); }
	};

	struct UnsafeLambdaFunc : public Functor {
		using Func = axFunc< ReturnType (ARGS...) >;
		Func _func;

		UnsafeLambdaFunc(Func func) : Functor(Type::LambdaFunc), _func(func) {}
		virtual bool valid() const override { return true; }
		virtual bool hasBoundToObject(void* p) override { return false; }

		virtual RETURN_TYPE emit(bool& outValid, ARGS... args) override {
			outValid = true;
			return _func(ax_forward(args)...); 
		}
		virtual void moveTo(axDelegateBase& de) override { de.unsageBind(_func); }
	};

	template<class OBJ>
	struct UnsafeMemberFunc : public Functor {
		using Func = ReturnType(OBJ::*)(ARGS...);

		OBJ* _obj;
		Func _func = nullptr;

		UnsafeMemberFunc(OBJ* obj, Func func) : Functor(Type::UnsafeMemberFunc), _obj(obj), _func(func) {
			AX_STATIC_ASSERT(sizeof(UnsafeMemberFunc) <= kFunctorlocalBufSize);
		}

		virtual bool valid() const override { return _obj != nullptr; }
		virtual bool hasBoundToObject(void* p) override { return _obj == p; }

		virtual RETURN_TYPE emit(bool& outValid, ARGS... args) override { 
			if (!_obj) return RETURN_TYPE();
			outValid = true;
			return (_obj->*_func)(ax_forward(args)...); 
		}
		virtual void moveTo(axDelegateBase& de) override { if (_obj) de.bind(_obj, _func); }
	};

	template<class OBJ>
	struct LinkMemberFunc : public Functor {
		using Func = ReturnType(OBJ::*)(ARGS...);

		axLink<OBJ> _obj;
		Func _func = nullptr;

		LinkMemberFunc(OBJ* obj, Func func) : Functor(Type::LinkMemberFunc), _obj(obj), _func(func) {
			AX_STATIC_ASSERT(sizeof(LinkMemberFunc) <= kFunctorlocalBufSize);
		}

		virtual bool valid() const override { return _obj.ptr() != nullptr; }
		virtual bool hasBoundToObject(void* p) override { return _obj == p; }

		virtual RETURN_TYPE emit(bool& outValid, ARGS... args) override { 
			if (!_obj) return RETURN_TYPE();
			outValid = true;
			return (_obj->*_func)(ax_forward(args)...); 
		}
		virtual void moveTo(axDelegateBase& de) override { if (_obj) de.bind(_obj.ptr(), _func); }
	};

	template<class OBJ>
	struct LinkLambdaFunc : public Functor {
		using Func = axFunc< ReturnType (ARGS...) >;
		axLink<OBJ> _obj;
		Func _func;

		LinkLambdaFunc(OBJ* obj, Func func) : Functor(Type::LinkLambdaFunc), _obj(obj), _func(func) {}

		virtual bool valid() const override { return _obj.ptr() != nullptr; }
		virtual bool hasBoundToObject(void* p) override { return _obj == p; }

		virtual RETURN_TYPE emit(bool& outValid, ARGS... args) override {
			if (!_obj) return RETURN_TYPE();
			outValid = true;
			return _func(ax_forward(args)...); 
		}
		virtual void moveTo(axDelegateBase& de) override { if (_obj) de.bind(_obj.ptr(), _func); }
	};

	template<class OBJ>
	struct WeakMemberFunc : public Functor {
		using Func = ReturnType(OBJ::*)(ARGS...);

		axWeak<OBJ> _weak;
		Func _func = nullptr;

		WeakMemberFunc(OBJ* obj, Func func) : Functor(Type::WeakMemberFunc), _weak(obj), _func(func) {
			AX_STATIC_ASSERT(sizeof(WeakMemberFunc) <= kFunctorlocalBufSize);
		}

		axSPtr<      OBJ> obj()			{ return _weak.getSPtr(); }
		axSPtr<const OBJ> obj() const	{ return _weak.getSPtr(); }

		virtual bool valid() const override { auto sp = obj(); return sp.ptr(); }
		virtual bool hasBoundToObject(void* p) override { auto sp = obj(); return sp.ptr() == p; }

		virtual RETURN_TYPE emit(bool& outValid, ARGS... args) override { 
			auto o = obj();
			if (!o) return RETURN_TYPE();
			outValid = true;
			return (o->*_func)(ax_forward(args)...); 
		}
		virtual void moveTo(axDelegateBase& de) override {
			if (auto sp = obj()) {
				de.bind(sp.ptr(), _func);
			}
		}
	};

	void _bind(ReturnType(*func)(ARGS...)) {
		unbindAll();
		AX_STATIC_ASSERT_MSG(sizeof(_functorLocalBuf) >= sizeof(StaticFunc), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) StaticFunc(func);
	}

	void _unsafeBind(axFunc<ReturnType (ARGS...)> func) {
		unbindAll();
		AX_STATIC_ASSERT_MSG(sizeof(_functorLocalBuf) >= sizeof(UnsafeLambdaFunc), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) UnsafeLambdaFunc(func);
	}

	template<class OBJ>
	void _bind(OBJ* obj, ReturnType(OBJ::*func)(ARGS...)) {
		unbindAll();
		AX_STATIC_ASSERT_MSG(sizeof(_functorLocalBuf) >= sizeof(LinkMemberFunc<OBJ>), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) LinkMemberFunc<OBJ>(obj, func);
	}

	template<class OBJ>
	void _bind(OBJ* obj, axFunc<ReturnType (ARGS...)> func) {
		unbindAll();
		AX_STATIC_ASSERT_MSG(sizeof(_functorLocalBuf) >= sizeof(LinkLambdaFunc<OBJ>), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) LinkLambdaFunc<OBJ>(obj, func);
	}

	template<class OBJ>
	void _weakBind(axSPtr<OBJ>& obj, ReturnType(OBJ::*func)(ARGS...)) {
		unbindAll();
		if (!obj) return;
		AX_STATIC_ASSERT_MSG(sizeof(_functorLocalBuf) >= sizeof(WeakMemberFunc<OBJ>), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) WeakMemberFunc<OBJ>(obj, func);
	}

	template<class OBJ>
	void _unsafeBind(OBJ* obj, ReturnType(OBJ::*func)(ARGS...)) {
		unbindAll();
		if (!obj) return;
		AX_STATIC_ASSERT_MSG(sizeof(_functorLocalBuf) >= sizeof(UnsafeMemberFunc<OBJ>), "_functorLocalBuf is too small");
		_functor = new(_functorLocalBuf) UnsafeMemberFunc<OBJ>(obj, func);
	}

	void _unbindAll() {
		if (_functor) {
			ax_call_destructor(_functor);
			_functor = nullptr;
		}
	}

	bool _hasBoundTo(ReturnType(*func)(ARGS...)) {
		if (!_functor) return false;
		switch (_functor->_type) {
			case Type::StaticFunc:			{ auto* f = static_cast<StaticFunc*>(_functor); return f->_func == func; } break;
		}
		return false;
	}

	template<class OBJ>
	bool _hasBoundTo(OBJ* obj, ReturnType(OBJ::*func)(ARGS...)) {
		if (!_functor) return false;
		switch(_functor->_type) {
			case Type::UnsafeMemberFunc:	{ auto* f = static_cast<UnsafeMemberFunc<OBJ>*>(_functor); return (f->_obj  == obj && f->_func == func); }break;
			case Type::LinkMemberFunc:		{ auto* f = static_cast<LinkMemberFunc<  OBJ>*>(_functor); return (f->_obj  == obj && f->_func == func); }break;
			case Type::WeakMemberFunc:		{ auto* f = static_cast<WeakMemberFunc<  OBJ>*>(_functor); auto sp = f->obj(); return (sp.ptr() == obj && f->_func == func); }break;
		}
		return false;
	}

	template<class OBJ>
	bool _hasBoundToObject(OBJ* obj) {
		if (!_functor) return false;
		return _functor->hasBoundToObject(obj);
	}

	static constexpr axInt kFunctorlocalBufSize = sizeof(void*) * 16;
	Functor*	_functor = nullptr;
	char		_functorLocalBuf[kFunctorlocalBufSize];
	axEnterOnce _enterOnce;
};

template<class FuncSig> using axDelegate			= axDelegateBase<true,  FuncSig>;
template<class FuncSig> using axReentrantDelegate	= axDelegateBase<false, FuncSig>;
