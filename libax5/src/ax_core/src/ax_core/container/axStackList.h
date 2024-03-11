#pragma once

// support multiple list per node
// +------- Node --------+
// |                     |
// +--- LIST_NODE (0) ---+
// |     Node* next      |
// +---------------------+
// |                     |
// |                     |
// +--- LIST_NODE (1) ---+
// |     Node* next      |
// +---------------------+
// |                     |
// |       ....          |

class axStackListNodeBase : public axNonCopyable {};

class axStackListBase : public axNonCopyable {};

class axStackListEventBase {
public:
	static void onAdd(void* p) {}
	static void onRemove(void* p) {}
};

//! Intrusive FIFO Single Linked List Node
template<class T, typename EVENT = axStackListEventBase>
class axStackListNode : public axStackListNodeBase {
public:
	using ListNode_Event = EVENT;

	~axStackListNode() {
		AX_STATIC_ASSERT(axTypeTraits::isBaseOf<axStackListEventBase, EVENT>::value);
		AX_ASSERT_MSG(!listNode_next, "still in list when destroy");
	}
	T* listNode_next = nullptr;
};

template<class List>
class axStackList_Iterator {
	using This = axStackList_Iterator;
public:
	using T = typename axTypeTraits::selectConst<typename List::Node, List>::Type;

	axStackList_Iterator( T* p = nullptr ) : _p(p) {}
	operator T*			()  { return  _p; }
	T&		operator*	()	{ return *_p; }
	T*		operator->	()	{ return  _p; }
	bool	operator==	( const This & r )	{ return _p == r._p; }
	bool	operator!=	( const This & r )	{ return _p != r._p; }
	void	operator++	()	{ if(_p) { _p = List::_getNode(_p)->listNode_next; } }

	T*		ptr() { return _p; }
private:
	T*	_p;
};

template<class List>
class axStackList_Enumerator {
	using This = axStackList_Enumerator;
public:
	using Iterator = axStackList_Iterator<List>;

	axStackList_Enumerator(List* list) : _list(list) {}

	Iterator	begin	()			{ return Iterator( _list->head() ); }
	Iterator	end		()			{ return Iterator( nullptr ); }
	axInt		size	()			{ return _list->size(); }
private:
	List*	_list;
};

//! Intrusive Single Linked List
template<class T, axDeleteFunc<T> del = ax_delete<T>, class LIST_NODE = axStackListNode<T> >
class axStackList : public axNonCopyable {
public:
	using Node = T;
	using List = axStackList;
	using Event = typename LIST_NODE::ListNode_Event;

	template<class L> using IteratorT	= axStackList_Iterator<L>;
	template<class L> using EnumeratorT	= axStackList_Enumerator<L>;

	using Iterator   = IteratorT<List>;
	using Enumerator = EnumeratorT<List>;

	using ConstIterator   = IteratorT<const List>;
	using ConstEnumerator = EnumeratorT<const List>;

	static		 LIST_NODE* _getNode(      T* obj) { return static_cast<      LIST_NODE*>(obj); }
	static const LIST_NODE* _getNode(const T* obj) { return static_cast<const LIST_NODE*>(obj); }

	static       T* _getObj(      LIST_NODE* node) { return static_cast<      T*>(node); }
	static const T* _getObj(const LIST_NODE* node) { return static_cast<const T*>(node); }

	axStackList() {
		using namespace axTypeTraits;
		AX_STATIC_ASSERT_MSG(!isPolymorphic<LIST_NODE>::value, "ListNode should not contain any virtual function");
		AX_STATIC_ASSERT_MSG(isBaseOf<axStackListNodeBase, LIST_NODE>::value, "axStackListNodeBase required");
		AX_STATIC_ASSERT_MSG(isBaseOf<LIST_NODE, T>::value, "T must base on ListNode");
	}

	axStackList(axStackList && r)			{ _move(r); }
	void operator=(axStackList && r)	{ _move(r); }

	~axStackList() { clear(); }

	IteratorT<      List> begin	()			{ return _head; }
	IteratorT<const List> begin	() const	{ return _head; }

	IteratorT<      List> end	()			{ return nullptr; }
	IteratorT<const List> end	() const	{ return nullptr; }

		EnumeratorT<      List>	each	() 			{ return EnumeratorT<      List>( this ); }
		EnumeratorT<const List>	each	() const	{ return EnumeratorT<const List>( ax_const_cast(this) ); }

	T* insert(T* p)							{ return _insert(p); }
	T* slowInsertBefore	(T* p, T* before)	{ return _insertBefore(p, before); }
	T* insertAfter	(T* p, T* after )		{ return _insertAfter (p, after ); }

	T* slowAppend(T* p)	{ return _append(p); }
	T* remove(T* p) { return _remove(p); }
	void	removeAll	()			{ while(auto p = popHead()) {} }

		  T*	head	()			{ return _head; }
	const T*	head	() const	{ return _head; }

	T* popHead()		{ return _head ? _remove(_head) : nullptr; }
	T* slowPopTail()	{ return _popTail(); }

	void clear()	{ _clear(); }

	explicit operator bool() const { return _head != nullptr; }

private:
	T* _insert(T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* node = _getNode(p);
		node->listNode_next = _head;
		_head = p;

		Event::onAdd(p);
		return p;
	}

	LIST_NODE* _findPrev(T* p) {
		auto* t = _head;
		while (t) {
			auto* node = _getNode(t);
			if (node->listNode_next == p)
				return node;
			t = node->listNode_next;
		}
		return nullptr;
	}

	T* _insertBefore(T* p, T* before) {
		if (!before) return _append(p);
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* node = _getNode(p);
		node->listNode_next = before;

		auto* prev = _findPrev(before);
		if (prev) {
			prev->listNode_next = p;
		}else{
			_head = p;
		}
		Event::onAdd(p);
		return p;
	}

	T* _insertAfter(T* p, T* after) {
		if (!after) return insert(p);
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* afterNode = _getNode(after);
		auto* node = _getNode(p);

		node->listNode_next = afterNode->listNode_next;
		afterNode->listNode_next = p;
		Event::onAdd(p);
		return p;
	}

	LIST_NODE* _getTail() {
		auto* t = _head;
		while (t) {
			auto* node = _getNode(t);
			if (node->listNode_next == nullptr) {
				return node;
			}
			t = node->listNode_next;
		}
		return nullptr;
	}

	T* _append(T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* node = _getNode(p);

		node->listNode_next = nullptr;

		if (_head) {
			_getTail()->listNode_next = p;
		} else {
			_head = p;
		}
		Event::onAdd(p);
		return p;
	}

	T* _remove(T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "remove null node");
			return nullptr;
		}

		auto* node = _getNode(p);
		Event::onRemove(p);

		auto* prevEntry = _findPrev(p);

		if (prevEntry) {
			prevEntry->listNode_next = _getNode(p)->listNode_next;
		}else{
			_head = node->listNode_next;
		}

		node->listNode_next = nullptr;
		return p;
	}

	T* _popTail()	{ 
		if (auto* tailNode = _getTail()) {
			auto* tail = _getObj(tailNode);
			return _remove(tail);
		}
		return nullptr;
	}

	void _clear() {
		while(auto p = popHead()) {
			_doDelete(p);
		}
	}

	void _move(List& r) {
		clear();
		_head = r._head;
		r._head = nullptr;
	}

	void _doDelete(T* p) {
		del(p);
	}

	T*		_head = nullptr;
};
