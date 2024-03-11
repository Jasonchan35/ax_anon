#pragma once

class axUISelectionListNodeBase {};

template<class T>
class axUISelectionListNode : public axDListNode<T> {
	using SelectionNode = axDListNode<T>;
public:
	~axUISelectionListNode() { this->removeFromList(); }
	bool isSelected() const { return this->listNode_list != nullptr; }
};

template<class T, class LIST_NODE = axUISelectionListNode<T> >
class axUISelectionList {
	using This = axUISelectionList;
	using List = axDList<T, ax_no_delete, LIST_NODE>;
public:
	using Iterator		= typename List::Iterator;
	using Enumerator	= typename List::Enumerator;

	bool set	(T* item)				{ return set(axMutSpan_make(&item, 1)); }
	bool set	(axMutSpan<T*> items)	{ return _set(items); }

	bool add	(T* item)				{ return add(axMutSpan_make(&item, 1)); }
	bool add	(axMutSpan<T*> items)	{ return _add(items); }

	bool remove	(T* item)				{ return remove(axMutSpan_make(&item, 1)); }
	bool remove	(axMutSpan<T*> items)	{ return _remove(items); }

	bool clear	()						{ return _clear(); }

	bool operator==(axSpan<T*>  items) const	{ return _equals(items); }

	Iterator   begin()		{ return _list.begin(); }
	Iterator   end	()		{ return _list.end(); }

	Enumerator each	()		{ return _list.each(); }

	axInt		size() const { return _list.size(); }

	explicit operator bool() const { return size(); }

private:
	bool _equals(axSpan<T*> items) const {
		if (items.size() != _list.size())
			return false;

		auto* s = items.data();
		for (auto& e : _list) {
			if (*s != &e) return false;
			s++;
		}
		return true;
	}

	bool _set	(axMutSpan<T*> items) {
		if (operator==(items))
			return false; // no change
		clear();
		add(items);
		return true;
	}

	bool _add(axMutSpan<T*> items) {
		bool changed = false;
		for (auto& e : items) {
			if (_list.contains(e)) continue;
			changed = true;
			_list.append(e);
		}
		return changed;
	}

	bool _remove(axMutSpan<T*> items) {
		bool changed = false;
		for (auto& e : items) {
			if (!_list.contains(e)) continue;
			changed = true;
			_list.remove(e);
		}
		return changed;
	}

	bool _clear() {
		if (!_list.size()) return false;
		_list.removeAll();
		return true;
	}

	axDList<T, ax_no_delete, LIST_NODE> _list;
};