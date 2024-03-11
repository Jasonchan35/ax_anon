 #pragma once

#include <ax_core/pointer/axUPtr.h>
#include <ax_core/base/axTypeTraits.h>

// support multiple list per node
// +------- Node --------+
// |                     |
// +--- LIST_NODE (0) ---+
// |     Node* next      |
// |     Node* prev      |
// |     List* list      |
// +---------------------+
// |                     |
// |                     |
// +--- LIST_NODE (1) ---+
// |     Node* next      |
// |     Node* prev      |
// |     List* list      |
// +---------------------+
// |                     |
// |       ....          |

class axDListNodeBase : public axNonCopyable {};

class axDListEventBase {
public:
	static void onAdd(void*) {}
	static void onRemove(void*) {}
};

class axDListBase : public axNonCopyable {
public:
	virtual void onNodeRequestRemove(axDListNodeBase* entry) = 0;
};

template<class T, class EVENT = axDListEventBase>
class axDListNode : public axDListNodeBase {
public:
	using ListNode_T = T;
	using ListNode_Event = EVENT;

	void removeFromList() {
		if (listNode_list) 
			listNode_list->onNodeRequestRemove(this);
	}

	~axDListNode() {
		AX_ASSERT_MSG(!listNode_next && !listNode_prev && !listNode_list, "still in list when destroy");
	}

	T* listNode_next = nullptr;
	T* listNode_prev = nullptr;
	axDListBase* listNode_list = nullptr;
};

template<class List>
class axDList_Iterator {
	using This = axDList_Iterator;
public:
	using T = typename axTypeTraits::selectConst<typename List::Obj, List>::Type;

	axDList_Iterator( T* p = nullptr ) : _p(p) {}
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
class axDList_RevIterator {
	using This = axDList_RevIterator;
public:
	using T = typename axTypeTraits::selectConst<typename List::Obj, List>::Type;

	axDList_RevIterator( T* p = nullptr ) : _p(p) {}
	operator T*			()  { return  _p; }
	T&		operator*	()	{ return *_p; }
	T*		operator->	()	{ return  _p; }
	bool	operator==	( const This & r ) const { return _p == r._p; }
	bool	operator!=	( const This & r ) const { return _p != r._p; }
	void	operator++	()	{ if(_p) { _p = List::_getNode(_p)->listNode_prev; } }

	T*		ptr() { return _p; }
private:
	T*	_p;
};

template<class List>
class axDList_Enumerator {
	using This = axDList_Enumerator;
public:
	using Iterator = axDList_Iterator<List>;

	axDList_Enumerator(List* list) : _list(list) {}

	Iterator	begin	()			{ return Iterator( _list->head() ); }
	Iterator	end		()			{ return Iterator( nullptr ); }
	axInt		size	()			{ return _list->size(); }
private:
	List*	_list;
};

template<class List>
class axDList_RevEnumerator {
public:
	using Iterator = axDList_RevIterator<List>;

	axDList_RevEnumerator(List* list) : _list(list) {}

	Iterator	begin	()			{ return Iterator( _list->tail() ); }
	Iterator	end		()			{ return Iterator( nullptr ); }
	axInt		size	()			{ return _list->size(); }
private:
	List*	_list;
};

//! Intrusive Doubly linked list
template<class T, axDeleteFunc<T> del = ax_delete<T>, class LIST_NODE = axDListNode<T> >
class axDList : public axDListBase {
public:
	using Obj = T;
	using List = axDList;
	using Event = typename LIST_NODE::ListNode_Event;

	template<class L> using IteratorT	= axDList_Iterator<L>;
	template<class L> using EnumeratorT	= axDList_Enumerator<L>;

	template<class L> using RevIteratorT	= axDList_RevIterator<L>;
	template<class L> using RevEnumeratorT	= axDList_RevEnumerator<L>;

	using Iterator   = IteratorT<List>;
	using Enumerator = EnumeratorT<List>;

	using RevIterator   = RevIteratorT<List>;
	using RevEnumerator = RevEnumeratorT<List>;

	using ConstIterator   = IteratorT<const List>;
	using ConstEnumerator = EnumeratorT<const List>;

	using ConstRevIterator   = RevIteratorT<const List>;
	using ConstRevEnumerator = RevEnumeratorT<const List>;

	static       LIST_NODE*	_getNode(      T* obj)			{ return static_cast<      LIST_NODE*>(obj); }
	static const LIST_NODE*	_getNode(const T* obj)			{ return static_cast<const LIST_NODE*>(obj); }

	static       T*			_getObj	(      LIST_NODE* node)	{ return static_cast<      T*>(node); }
	static const T*			_getObj	(const LIST_NODE* node)	{ return static_cast<const T*>(node); }

	axDList() {
		using namespace axTypeTraits;
		AX_STATIC_ASSERT_MSG(!isPolymorphic<LIST_NODE>::value, "ListNode should not contain any virtual function");
		AX_STATIC_ASSERT_MSG(isBaseOf<axDListNodeBase, LIST_NODE>::value, "axDListNodeBase required");
		AX_STATIC_ASSERT_MSG(isBaseOf<LIST_NODE, T>::value, "T must base on ListNode");
	}

	explicit axDList(List && r) noexcept	{ _move(r); }
	void operator=(List && r)	{ _move(r); }

	virtual ~axDList() { clear(); }

	axInt	size() const	{ return _size; }

	IteratorT<      List> begin	()			{ return _head; }
	IteratorT<const List> begin	() const	{ return _head; }

	IteratorT<      List> end	()			{ return nullptr; }
	IteratorT<const List> end	() const	{ return nullptr; }

		EnumeratorT<      List>	each	() 			{ return EnumeratorT<      List>( this ); }
		EnumeratorT<const List>	each	() const	{ return EnumeratorT<const List>( ax_const_cast(this) ); }

	RevEnumeratorT<      List>	revEach	() 			{ return RevEnumeratorT<      List>( this ); }
	RevEnumeratorT<const List>	revEach	() const	{ return RevEnumeratorT<const List>( ax_const_cast(this) ); }

	T* insert		(T* p)				{ return _insert(p); }
	T* insertBefore	(T* p, T* before)	{ return _insertBefore(p, before); }
	T* insertAfter	(T* p, T* after )	{ return _insertAfter (p, after ); }

	bool contains		(T* p) { return p->LIST_NODE::listNode_list == this; }

	T* forceAppend		(T* p)	{ p->LIST_NODE::removeFromList(); return _append(p); }

	AX_INLINE T* append (T* p)	{ return _append(p); }

	T* insert			(axUPtr<T> && u)	{ insert(u.ptr()); return u.detach(); }
	T* append			(axUPtr<T> && u)	{ append(u.ptr()); return u.detach(); }

	T* remove(T* p) { return _remove(p); }

	void	removeAll	()			{ while(auto p = popHeadPtr()) {} }

		  T*	head	()			{ return _head; }
	const T*	head	() const	{ return _head; }

		  T*	tail	()			{ return _tail; }
	const T*	tail	() const	{ return _tail; }

	T* popHeadPtr()	{ return _head ? _remove(_head) : nullptr; }
	T* popTailPtr()	{ return _tail ? _remove(_tail) : nullptr; }

	axUPtr<T> popHead()	{ return axUPtr<T>::s_make(popHeadPtr()); }
	axUPtr<T> popTail()	{ return axUPtr<T>::s_make(popTailPtr()); }

	void clear()	{ _clear(); }

	virtual void onNodeRequestRemove(axDListNodeBase* node) override {
		remove(_getObj(static_cast<LIST_NODE*>(node))); 
	}

	explicit operator bool() const { return _head != nullptr; }

	void onFormat(axFormat& f) const; // implement in axFormat.h

private:

	T* _insert(T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* node = _getNode(p);
		if (node->listNode_list) {
			AX_ASSERT_MSG(false, "insert node belongs to another list");
		}

		node->listNode_prev = nullptr;
		node->listNode_next = _head;
		node->listNode_list = this;

		if (_head) {
			_getNode(_head)->listNode_prev = p;
		}else{
			_tail = p;
		}
		_head = p;

		_size++;
		Event::onAdd(p);
		return p;
	}

	T* _insertBefore(T* p, T* before) {
		if (!before) return append(p);
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* beforeNode = _getNode(before);
		auto* node = _getNode(p);
		if (node->listNode_list) {
			AX_ASSERT_MSG(false, "insert node belongs to another list");
		}

		node->listNode_next = before;
		node->listNode_prev = beforeNode->listNode_prev;
		node->listNode_list = this;

		if (beforeNode->listNode_prev) {
			_getNode(beforeNode->listNode_prev)->listNode_next = p;
		}else{
			_head = p;
		}

		beforeNode->listNode_prev = p;
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

		auto* afterNode = _getNode(after);
		auto* node = _getNode(p);

		if (node->listNode_list) {
			AX_ASSERT_MSG(false, "insert node belongs to another list");
		}

		node->listNode_prev = after;
		node->listNode_next = afterNode->listNode_next;
		node->listNode_list = this;

		if (afterNode->listNode_next) {
			_getNode(afterNode->listNode_next)->listNode_prev = p;
		}else{
			_tail = p;
		}

		afterNode->listNode_next = p;
		_size++;
		Event::onAdd(p);
		return p;
	}

	AX_INLINE T* _append(T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto* node = _getNode(p);
		if (node->listNode_list != nullptr) {
			AX_ASSERT_MSG(false, "insert node belongs to another list");
		}

		node->listNode_next = nullptr;
		node->listNode_prev = _tail;
		node->listNode_list = this;

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

	T* _remove(T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "remove null node");
			return nullptr;
		}

		AX_ASSERT_MSG(_size != 0, "remove when size == 0");

		auto* node = _getNode(p);
		Event::onRemove(p);

		if (node->listNode_list != this) {
			AX_ASSERT_MSG(false, "remove node doesn't belong to this list");
			return p;
		}

		if (node->listNode_next) {
			_getNode(node->listNode_next)->listNode_prev = _getNode(p)->listNode_prev;
		}else{
			_tail = node->listNode_prev;
		}

		if (node->listNode_prev) {
			_getNode(node->listNode_prev)->listNode_next = _getNode(p)->listNode_next;
		}else{
			_head = node->listNode_next;
		}

		node->listNode_next = nullptr;
		node->listNode_prev = nullptr;
		node->listNode_list = nullptr;

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

		for(auto& p : *this) {
			_getNode(&p)->listNode_list = this;
		}
	}

	void _doDelete(T* p) {
		del(p);
	}

	T*		_head = nullptr;
	T*		_tail = nullptr;
	axInt	_size = 0;
};
