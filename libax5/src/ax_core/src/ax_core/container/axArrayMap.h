#pragma once

#include "axPair.h"

template< class KEY, class T, axInt LOCAL_BUF_SIZE = 0>
class axArrayMap : public axNonCopyable {
public:
	using Key   = KEY;
	using Value = T;

	struct Pair {
		Key		key;
		Value	value;

		void onFormat(axFormat& f) const {
			f.format("  key=({?}), value=({?})\n", key, value);
		}
	};

	template<class IN_KEY>
	T& add(const IN_KEY& key) & {
		auto& p = pairs.emplaceBack();
		p.key = key;
		return p.value;
	}

	template<class IN_KEY>
	T* find(const IN_KEY& key) & {
		for (auto& p : pairs) {
			if (p.key == key) return &p.value;
		}
		return nullptr;
	}

	template<class IN_KEY>
	T& getOrAdd(const IN_KEY& key) & {
		if (auto* p = find(key)) {
			return p->value;
		}
		return add(key);
	}

	T* tryAt(axInt i) & { auto* o = pairs.tryAt(i); return o ? &o->value : nullptr; }

	void reserve(axInt i) { pairs.reserve(i); }

	AX_INLINE       T& operator[](axInt i)       { return pairs[i].value; }
	AX_INLINE const T& operator[](axInt i) const { return pairs[i].value; }

	axInt size() const { return pairs.size(); }

	template<class R> using Enumerator = axArrayInterleaveEnumerator<R>;
	template<class R> using Iterator = typename Enumerator<R>::Iterator;

	Iterator<T> begin()	& { 
		if (!pairs.size()) return Iterator<T>(nullptr, sizeof(Pair));
		auto* p = pairs.data();
		return Iterator<T>(&p->value, sizeof(Pair));
	}

	Iterator<T> end() & { 
		if (!pairs.size()) return Iterator<T>(nullptr, sizeof(Pair));
		auto* p = pairs.data() + pairs.size();
		return Iterator<T>(&p->value, sizeof(Pair));
	}

	Iterator<const T> begin() const & { return ax_const_cast(this)->begin(); }
	Iterator<const T> end  () const & { return ax_const_cast(this)->end();   }

	void onFormat(axFormat& f) const {
		f << "axArrayMap\n";
		f << pairs;
	}

	axArray<Pair, LOCAL_BUF_SIZE>	pairs;
};
