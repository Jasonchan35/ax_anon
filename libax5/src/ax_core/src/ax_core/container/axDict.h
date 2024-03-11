#pragma once
#include "axMap.h"
#include "../pointer/axUPtr.h"

template<class KEY, class T, class IN_KEY = typename axTypeTraits::spanType<KEY>::Type>
class axDict : public axNonCopyable {
public:
	class Pair;
	using MapNode = axMapNode<KEY, Pair>;

	class Pair : public MapNode {
	public:
		const KEY&	key	() const	{ return MapNode::mapNode_key; }
				T&	value()			{ return _value; }
		const	T&	value() const	{ return _value; }

	friend class axDict;
	protected:
		template< class... Args >
		Pair(Args&&... args) : _value(ax_forward(args)...) {}
		T	_value;
	};

	using Key = KEY;
	using InKey = IN_KEY;
	using Map = axMap<Key, Pair, ax_delete, MapNode, InKey>;
	using OrderedList = typename Map::OrderedList;

	template<class A>
	using PairIterator = typename Map::template Iterator<A>;

	template< class... Args >
	T&		add		(const InKey& key, Args&&... args)	{ return _add(key, ax_forward(args)...)->_value; }
	T&		getOrAdd(const InKey& key, bool* newAdded = nullptr);

			T*	find		(const InKey& key)				{ return _find(key); }
	const	T*	find		(const InKey& key) const		{ return ax_const_cast(this)->_find(key); }

		axInt	size		() const						{ return _map.size(); }
		void	clear		()								{ _map.clear(); }

		bool	remove		(const InKey& key)				{ return _remove(key); }

	void			setAllocator(axAllocator* a)			{ _allocator = a; }
	axAllocator*	allocator() const						{ return AX_IF_GET(_allocator, ax_default_dict_allocator()); }

	void			setDictAllocator(axAllocator* a)		{ _map.setAllocator(a); }
	axAllocator*	dictAllocator() const					{ return _map.allocator(); }

	PairIterator<Pair>	begin() { return _map.begin(); }
	PairIterator<Pair>	end()	{ return _map.end(); }

	PairIterator<const Pair>	begin() const { return _map.begin(); }
	PairIterator<const Pair>	end() const	  { return _map.end(); }

	template<class A>
	class ValueIterator {
	public:
		ValueIterator(PairIterator<A> p) : _p(p) {}

		T*		operator->()		{ return _p->value(); }
		T&		operator*()			{ return _p->value(); }
		void	operator++()		{ ++_p; }

		bool operator!=(const ValueIterator& r) { return _p != r._p; }
	private:
		PairIterator<A> _p;
	};

	template<class A>
	class ValueEnumerator {
	public:
		ValueEnumerator(PairIterator<A> begin, PairIterator<A> end) : _begin(begin), _end(end) {}

		ValueIterator<A> begin() { return _begin; }
		ValueIterator<A> end()   { return _end;   }

	private:
		PairIterator<A> _begin = nullptr;
		PairIterator<A> _end = nullptr;
	};

	ValueEnumerator<Pair>	    values()		{ return ValueEnumerator<Pair>(begin(), end()); }
	ValueEnumerator<const Pair>	values() const	{ return ValueEnumerator<const Pair>(begin(), end()); }


private:
	template< class... Args > inline
	Pair* _add(const InKey & key, Args&&... args) {
		auto p = axUPtr_new(Pair( ax_forward(args)... ));
		_map.add(key, p.ptr());
		return p.detach();
	}

	T* _find(const InKey & key) {
		auto* p = _map.find(key);
		return p ? &p->_value : nullptr;
	}

	bool _remove(const InKey& key)	{
		if (auto* p = _map.find(key)) {
			_map.removeNode(p);
			ax_delete(p);
			return true;
		}
		return false;
	}

protected:
	Map	_map;
	axAllocator*	_allocator = nullptr;
};

template<class KEY, class T, class IN_KEY /*= typename axMap_DefaultInKey<KEY>::Type*/> inline
T& axDict<KEY, T, IN_KEY>::getOrAdd(const InKey& key, bool* newAdded /*= nullptr*/) {
	auto* o = find(key);
	if (o) {
		if (newAdded) *newAdded = false;
		return *o;
	}
	o = &add(key, T());
	if (newAdded) *newAdded = true;
	return *o;
}

//--------------
