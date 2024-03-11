#pragma once

#include <ax_core/pointer/axUPtr.h>

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

class axSListNodeBase : public axNonCopyable {};

class axSListBase : public axNonCopyable {};

class axSListEventBase {
public:
	static void onAdd(void*) {}
	static void onRemove(void*) {}
};

//! Intrusive Single Linked List Node
template<class T, class EVENT = axSListEventBase>
class axSListNode : public axSListNodeBase {
public:
	using ListNode_Event = EVENT;

	~axSListNode() {
		AX_ASSERT_MSG(!listNode_next, "still in list when destroy");
	}
	T* listNode_next = nullptr;
};

template<class List>
class axSList_Iterator {
	using This = axSList_Iterator;
public:
	using T = typename axTypeTraits::selectConst<typename List::Obj, List>::Type;

	axSList_Iterator( T* p = nullptr ) : _p(p) {}
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
class axSList_Enumerator {
	using This = axSList_Enumerator;
public:
	using Iterator = axSList_Iterator<List>;

	axSList_Enumerator(List* list) : _list(list) {}

	Iterator	begin	()			{ return Iterator( _list->head() ); }
	Iterator	end		()			{ return Iterator( nullptr ); }
	axInt		size	()			{ return _list->size(); }
private:
	List*	_list;
};

//! Intrusive Single Linked List
template<class T, axDeleteFunc<T> del = ax_delete<T>, typename LIST_NODE = axSListNode<T> >
class axSList : public axNonCopyable {
public:
	using Obj = T;
	using List = axSList;
	using Event = typename LIST_NODE::ListNode_Event;

	template<class L> using IteratorT	= axSList_Iterator<L>;
	template<class L> using EnumeratorT	= axSList_Enumerator<L>;

	using Iterator   = IteratorT<List>;
	using Enumerator = EnumeratorT<List>;

	using ConstIterator   = IteratorT<const List>;
	using ConstEnumerator = EnumeratorT<const List>;

	static		 LIST_NODE* _getNode(      T* obj) { return static_cast<      LIST_NODE*>(obj); }
	static const LIST_NODE* _getNode(const T* obj) { return static_cast<const LIST_NODE*>(obj); }

	static       T* _getObj(      LIST_NODE* node) { return static_cast<      T*>(node); }
	static const T* _getObj(const LIST_NODE* node) { return static_cast<const T*>(node); }

	axSList() {
		using namespace axTypeTraits;
		AX_STATIC_ASSERT_MSG(!isPolymorphic<LIST_NODE>::value, "ListNode should not contain any virtual function");
		AX_STATIC_ASSERT_MSG(isBaseOf<axSListNodeBase, LIST_NODE>::value, "axSListNodeBase required");
		AX_STATIC_ASSERT_MSG(isBaseOf<LIST_NODE, T>::value, "T must base on ListNode");
	}

	axSList(axSList && r) noexcept			{ _move(r); }
	void operator=(axSList && r) noexcept	{ _move(r); }

	~axSList() { clear(); }

	axInt	size		() const	{ return _size; }

	IteratorT<      List> begin	()			{ return _head; }
	IteratorT<const List> begin	() const	{ return _head; }

	IteratorT<      List> end	()			{ return nullptr; }
	IteratorT<const List> end	() const	{ return nullptr; }

		EnumeratorT<      List>	each	() 			{ return EnumeratorT<      List>( this ); }
		EnumeratorT<const List>	each	() const	{ return EnumeratorT<const List>( ax_const_cast(this) ); }

	T* insert			(T* p)				{ return _insert(p); }
	T* insertBeforeSlow	(T* p, T* before)	{ return _insertBefore(p, before); }
	T* insertAfter		(T* p, T* after )	{ return _insertAfter (p, after ); }

	T* append			(T* p)				{ return _append(p); }

	T* insert		(axUPtr<T> && p)			{ insert(p.ptr()); return p.detach(); }
	T* append		(axUPtr<T> && p)			{ append(p.ptr()); return p.detach(); }

	AX_INLINE void appendRange(axSList && r) { _appendRange(ax_move(r)); }

	T* remove(T* p) { return _remove(p); }
	void	removeAll	()			{ while(auto p = popHeadPtr()) {} }

		  T*	head	()			{ return _head; }
	const T*	head	() const	{ return _head; }

		  T*	tail	()			{ return _tail; }
	const T*	tail	() const	{ return _tail; }

	T* popHeadPtr		()	{ return _head ? _remove(_head) : nullptr; }
	T* popTailPtrSlow	()	{ return _tail ? _remove(_tail) : nullptr; }

	axUPtr<T> popHead		()	{ return axUPtr_make(popHeadPtr()); }
	axUPtr<T> popTailSlow	()	{ return axUPtr_make(popTailPtrSlow()); }

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

		if (_head) {
			_head = p;
		}else{
			_head = p;
			_tail = p;
		}

		_size++;
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
		if (!before) return append(p);
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

		_size++;
		Event::onAdd(p);
		return p;
	}

	T* _insertAfter(T* p, T* after) {
		if (!after) return insert(p);
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* afterEntry = _getNode(after);
		auto* node = _getNode(p);

		node->listNode_next = afterEntry->listNode_next;

		if (!afterEntry->listNode_next) {
			_tail = p;
		}

		afterEntry->listNode_next = p;
		_size++;

		Event::onAdd(p);
		return p;
	}

	T* _append(T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* node = _getNode(p);
		node->listNode_next = nullptr;

		if (_tail) {
			_getNode(_tail)->listNode_next = p;
		}else{
			_head = p;
		}
		_tail = p;

		_size++;

		Event::onAdd(p);
		return p;
	}

	void _appendRange(axSList && r) {
		if (!r._size) return;
		if (!_size) { operator=(ax_move(r)); return; }

		auto* tailNode = _getNode(_tail);
		tailNode->listNode_next = r._head;

		_tail = r._tail;
		_size += r._size;

		r._head = nullptr;
		r._tail = nullptr;
	}

	T* _remove(T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "remove null node");
			return nullptr;
		}

		auto* node = _getNode(p);
		Event::onRemove(p);

		auto* prevEntry = _findPrev(p);

		if (!node->listNode_next) {
			_tail = _getObj(prevEntry);
		}

		if (prevEntry) {
			prevEntry->listNode_next = _getNode(p)->listNode_next;
		}else{
			_head = node->listNode_next;
		}

		node->listNode_next = nullptr;
		AX_ASSERT_MSG(_size != 0, "remove when size == 0");
		_size--;
		return p;
	}

	void _clear() {
		while(auto* p = popHeadPtr()) {
			_doDelete(p);
		}
	}

	void _move(List& r) {
		clear();
		_head = r._head;
		_tail = r._tail;
		_size = r._size;
		r._head = nullptr;
		r._tail = nullptr;
		r._size = 0;
	}

	void _doDelete(T* p) {
		del(p);
	}

	T*		_head = nullptr;
	T*		_tail = nullptr;
	axInt	_size = 0;
};
