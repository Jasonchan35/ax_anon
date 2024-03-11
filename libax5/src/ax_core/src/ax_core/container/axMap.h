#pragma once

#include "axDList.h"
#include "axArray.h"
#include "axFixedArray.h"
#include "../pointer/axUPtr.h"

//--------------------
class axMapError : axError {};
class axMapError_Undefined		: axMapError{};
class axMapError_DuplicatedKey	: axMapError{};
class axMapError_NoSuchKey		: axMapError{};
class axMapError_AddingNullNode : axMapError{};


template<class NODE>
class axMapKeyAccessor {
public:
	using KEY = decltype(NODE::key);
	AX_INLINE const KEY& key(const NODE* node) { return node->key; }
};

class axMapNodeBase : public axNonCopyable {};

class axMapNodeEventBase {
public:
	static void onRemove(void* p) {}
};

template<class MAP_NODE>
class axMapNode_OrderedListEvent {
public:
	using T = typename MAP_NODE::MapNode_T;
	using Event = typename MAP_NODE::MapNode_Event;

	static void onAdd(MAP_NODE* node) {}

	static void onRemove(MAP_NODE* node) {
		AX_STATIC_ASSERT(axTypeTraits::isBaseOf<MAP_NODE, T>::value);
		auto* obj = static_cast<T*>(node);
		Event::onRemove(obj); 
	}
};

template<class KEY, class T, class EVENT = axMapNodeEventBase>
class axMapNode 
	: public axMapNodeBase
	, public axDListNode< axMapNode<KEY, T, EVENT>, axMapNode_OrderedListEvent< axMapNode<KEY, T, EVENT> > >
	, public axDListNode< axMapNode<KEY, T, EVENT> > 
{
	using This = axMapNode;
public:
	using MapNode_T = T;
	using MapNode_Event = EVENT;

	axMapNode() {
		AX_STATIC_ASSERT_MSG(axTypeTraits::isBaseOf<axMapNode, T>::value, "T must base on axMapNode");
	}
	using MapNode_OrderedNode = axDListNode<axMapNode, axMapNode_OrderedListEvent<axMapNode> >;
	using MapNode_HashNode    = axDListNode<axMapNode>;

	void removeFromMap() {
		MapNode_HashNode::removeFromList();
		MapNode_OrderedNode::removeFromList();
	}

	KEY		mapNode_key;
	axHash	mapNode_hash;

private:
	void removeFromList() = delete;
};

//! Intrusive MultiMap - Ordered Map, support multiple node with same key
template<class KEY, class T, axDeleteFunc<T> del, class MAP_NODE, class IN_KEY, bool ALLOW_MULTI>
class axMultiMap_Internal : public axNonCopyable {
	using This		= axMultiMap_Internal;
public:
	using Obj	= T;
	using Key	= KEY;
	using InKey	= IN_KEY;

	using MapNode_OrderedNode = typename MAP_NODE::MapNode_OrderedNode;
	using MapNode_HashNode    = typename MAP_NODE::MapNode_HashNode;

	using OrderedList	= axDList<MAP_NODE, ax_no_delete, MapNode_OrderedNode>;
	using HashList		= axDList<MAP_NODE, ax_no_delete, MapNode_HashNode>;
	using Table			= axArray<HashList, 0>;

	axMultiMap_Internal() {
		AX_STATIC_ASSERT_MSG(axTypeTraits::isBaseOf<axMapNodeBase, MAP_NODE>::value, "axMapNodeBase required");
		AX_STATIC_ASSERT_MSG(axTypeTraits::isBaseOf<MAP_NODE, T>::value, "T must base on axMapNode");
	}
	axMultiMap_Internal(This && r) 
		: _list(ax_move(r._list))
		, _table(ax_move(r._table))	
	{}

	~axMultiMap_Internal() { clear();	}

	void operator=(This && r)	{ _move(r); }
	void clear()				{ _clear(); }

	axInt	size	() const { return _list.size(); }

	T*		add				(const InKey& key, T* p)			{ return _add(key, p); }
	T*		add				(const InKey& key, axUPtr<T> && p)	{ _add(key, p.ptr()); return p.detach(); }
	T*		add				(const InKey& key)					{ return _add(key); }

	T*		addIfNotExists	(const InKey& key)			{ return _addIfNotExists(key); }
	T*		getOrAdd		(const InKey& key)			{ return _getOrAdd(key); }

	template<class... Args>
	AX_INLINE	T*	emplaceAdd	(const InKey& key, Args&&... args )	{ 
		auto u = axUPtr_new(T(ax_forward(args)...));
		add(key, u.ptr());
		return u.detach();
	}

	T*		popHead			()							{ return _popHead(); }
	T*		remove			(const InKey& key)			{ return _remove(key); }
	void	removeNode		(T* p)						{ _removeNode(p); }

	void	resizeTable(axInt n)						{ _resizeTable(n); }

	class FindIterator {
	public:
		FindIterator( T* p=nullptr ) : _p(p) {}
		operator T*			()  { return  _p; }
		T&		operator*	()	{ return *_p; }
		T*		operator->	()	{ return  _p; }
		bool	operator==	( const FindIterator & rhs )	{ return _p == rhs._p; }
		bool	operator!=	( const FindIterator & rhs )	{ return _p != rhs._p; }

		void	operator++	()	{
			//find next node have the same key
			if(!_p) return;
			T* t = _p;
			for (;;) {
				t = _getEntry(t)->hashListEntry.next;
				// no more
				if (!t) { _p = nullptr; break; }
				// found
				if (_p->isMapKeyEquals(t)) { _p = t; break; }
			}
		}

		T*		ptr() { return _p; }
	private:
		T*	_p;		
	};

	class FindEnumator {
	public:
		FindEnumator() = default;
		FindEnumator(const InKey& key, FindIterator begin) : _begin(begin), _key(&key) {}

		FindIterator	begin	() { return _begin;  }
		FindIterator	end		() { return nullptr; }
	private:
		FindIterator _begin;
		const InKey* _key {nullptr};
	};

	FindEnumator findAll(const InKey& key, axHash hash) {
		auto& hashList = _getHashListForAdd(hash);
		return FindEnumator(key, _findInHashList(hashList, hash, key));
	}

	FindEnumator findAll(const InKey& key) {
		auto hash = axHash_get(key);
		return findAll(key, hash);
	}

	//return the first element
	T* find(const InKey& key) {
		return findAll(key).begin().ptr();
	}

	//return the first element
	T* find(const InKey& key, axHash hash) {
		return findAll(key, hash).begin().ptr();
	}

	template<class A>
	class Iterator {
	public:
		Iterator(A* p) : _p(p) {}
		A& operator*()  { return *_p; }
		A* operator->() { return _p; }

		void operator++() { _p = _getObj(_getOrderedNode(_p)->listNode_next); }
		bool operator!=(const Iterator& r) { return _p != r._p; }

	private:
		A*	_p;
	};

	template<class A>
	class Enumerator {
	public:
		using List = typename axTypeTraits::selectType<const OrderedList, OrderedList, axTypeTraits::isConst<A>::value>::Type;

		Enumerator(List* list) : _list(list) {}

		Iterator<A>	begin	()			{ return Iterator<A>( _list->head() ); }
		Iterator<A>	end		()			{ return Iterator<A>( nullptr ); }
		axInt		size	()			{ return _list->size(); }
	private:
		List* _list;
	};

	Enumerator<      T>	each	() 			{ return &_list; }
	Enumerator<const T>	each	() const	{ return &_list; }

    Iterator<      T>	begin	()			{ return _getObj(_list.head()); }
    Iterator<const T>	begin	() const	{ return _getObj(_list.head()); }
    Iterator<      T>	end		()			{ return nullptr; }
    Iterator<const T>	end		() const	{ return nullptr; }

				  T*	head	()			{ return _getObj(_list.head()); }
			const T*	head	() const	{ return _getObj(_list.head()); }

				  T*	tail	()			{ return _getObj(_list.tail()); }
			const T*	tail	() const	{ return _getObj(_list.tail()); }

	void			setAllocator(axAllocator* a)	{ _table.setAllocator(a); }
	axAllocator*	allocator() const				{ return _table.allocator(); }

	static       T*			_getObj			(      MAP_NODE* node)	{ return static_cast<      T*>(node); }
	static const T*			_getObj			(const MAP_NODE* node) 	{ return static_cast<const T*>(node); }

	static       MAP_NODE*	_getMapNode		(      T* obj)			{ return static_cast<      MAP_NODE*>(obj); }
	static const MAP_NODE*	_getMapNode		(const T* obj) 			{ return static_cast<const MAP_NODE*>(obj); }

	static       MapNode_OrderedNode*	_getOrderedNode	(      T* obj)			{ return static_cast<      MapNode_OrderedNode*>(obj); }
	static const MapNode_OrderedNode*	_getOrderedNode	(const T* obj) 			{ return static_cast<const MapNode_OrderedNode*>(obj); }

	static       MapNode_HashNode*		_getHashNode	(      T* obj)			{ return static_cast<      MapNode_HashNode*>(obj); }
	static const MapNode_HashNode*		_getHashNode	(const T* obj) 			{ return static_cast<const MapNode_HashNode*>(obj); }

private:
	void _doDelete(T* p) {
		del(p);
	}

	T*	_addIfNotExists(const InKey& key) {
		auto* p = find(key);
		if (p) return nullptr; // do not create if exists;
		return _add(key);
	}

	T*	_getOrAdd(const InKey& key) {
		auto* p = find(key);
		if (p) return p;
		return _add(key);
	}

	T* _popHead() {
		auto* entry = _list.head();
		if (!entry) return nullptr;
		auto* obj = _getObj(entry);
		removeNode(obj);
		return obj;
	}

	void _clear() {
		while(auto p = popHead()) {
			_doDelete(p);
		}
	}

	void _move(This & r) {
		clear();
		_list = ax_move(r._list);
		_table = ax_move(r._table);
	}

	T* _remove(const InKey& key) {
		auto* p = find(key);
		if (p) removeNode(p);
		return p;
	}

	void _removeNode(T* p) {
		auto* node = _getMapNode(p);
		_list.remove(node);

		auto* hashList = static_cast<HashList*>(_getHashNode(p)->listNode_list);
		hashList->remove(node);
	}

	T* _add(const InKey& key) {
		auto u = axUPtr_new(T());
		add(key, u.ptr());
		auto* p = u.detach();
		return p;
	}

	T* _add(const InKey& key, T* p) {
		if (!p) {
			AX_ASSERT_MSG(false, "insert null node");
			return nullptr;
		}

		auto hv = axHash_get(key);
		auto& hashList = _getHashListForAdd(hv);

		_findInHashList(hashList, hv, key);

		auto* node = _getMapNode(p);
		node->mapNode_key = key;
		node->mapNode_hash = hv;

		_list.append(node);
		hashList.append(node);
		return p;
	}

	void _resizeTable( axInt newSize ) {
		if( newSize == _table.size() ) return;

		if( newSize <= 0 && _list.size() > 0 ) {
			throw axError_Undefined(AX_LOC);
		}

		for(auto& t : _table) {
			t.removeAll();
		}

		_table.resize(newSize);

		//re-insert to table
		for(auto& node : _list) {
			auto& hashList = _getHashList(node.mapNode_hash);
			hashList.insert(&node);
		}
	}

	HashList& _getHashListForAdd(const axHash& hv) {
		auto n  = _list.size();
		auto ts = _table.size();

		if( ts == 0 ) {
			ts = 4; // first resize table
		}else if( ts < n / 8 + 1 ) {
			ts = n / 4;
		}
		resizeTable(ts);
		return _getHashList(hv);
	}

	HashList&	_getHashList(const axHash& hv) { 
		return _table[(axInt)(hv.value % (axUInt)_table.size())]; 
	}

	T*	_findInHashList(HashList& hashList, axHash hv, const InKey& key) {
		for(auto& node : hashList) {
			if (node.mapNode_hash == hv && node.mapNode_key == key) {
				return _getObj(&node);
			}
		}
		return nullptr;
	}

	OrderedList	_list;
	Table _table;
};

template<	class KEY,
			class T,
			axDeleteFunc<T> del = ax_delete<T>,
			class MAP_NODE = axMapNode<KEY, T>,
			class IN_KEY = typename axTypeTraits::spanType<KEY>::Type 
>
class axMap : public axMultiMap_Internal<KEY, T, del, MAP_NODE, IN_KEY, false> {
};

template<	class KEY,
			class T,
			axDeleteFunc<T> del = ax_delete<T>,
			class MAP_NODE = axMapNode<KEY, T>,
			class IN_KEY = typename axTypeTraits::spanType<KEY>::Type
>
class axMultiMap : public axMultiMap_Internal<KEY, T, del, MAP_NODE, IN_KEY, true> {
};
