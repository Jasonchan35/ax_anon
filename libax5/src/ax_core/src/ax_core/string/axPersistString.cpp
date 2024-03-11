#include "axPersistString.h"
#include "../other/axGlobalSingleton.h"

template<class T>
class axPersistStringT<T>::Manager : public axNonCopyable {
	using This = Manager;
public:
	using Key			= axStringT<T>;
	using StrView		= axStrViewT<T>;
	using StrLiteral	= axStrLiteralT<T>;
	using PersistString	= axPersistStringT<T>;

	Manager() {
		ax_fallback_allocator();
	}

	class Node : public axMapNode<Key, Node> {
		using MapNode = axMapNode<Key, Node>;
	public:
		const Key&	key () const { return MapNode::mapNode_key; }
		StrView	strViewHolder;
	};

	class MTData {
	public:
		axMap<Key, Node>	map;
	};
	axSpinLockProtected<MTData> _mtData;

	static This* s_instance() {
		static axGlobalSingleton<This> s;
		return s.ptr();
	}

	PersistString kEmptyString;

	PersistString lookup(StrView s) {
		if (!s) 
			return PersistString::s_empty();

		auto mt = _mtData.scopedLock();
		auto* p = mt->map.find(s);
		if (!p) {
			p = mt->map.add(s, ax_new Node);
			p->strViewHolder = p->key();
		}

		AX_STATIC_ASSERT(sizeof(StrLiteral) >= sizeof(StrView));
		auto ret = PersistString(static_cast<const StrLiteral*>(&p->strViewHolder));
		return ret;
	}
};

template<class T>
const axStrLiteralT<T>*	axPersistStringT<T>::s_empty() {
	static axStrLiteralT<T> s;
	return &s;
}

template<class T>
axPersistStringT<T> axPersistStringT<T>::s_make(StrView s) {
	return Manager::s_instance()->lookup(s);
}

//-----------
#define	AX_MACRO_OP(T)	\
	/*---- The explicit instantiation ---*/ \
	template class axPersistStringT<T>; \
//--------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef	AX_MACRO_OP
