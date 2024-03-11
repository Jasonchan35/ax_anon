#pragma once

template<class... ARGS>
class axTuple : public std::tuple<ARGS...> {
	using This = axTuple;
	using Base = std::tuple<ARGS...>;
public:
	        static constexpr axInt kSize = std::tuple_size<Base>::value;
	AX_INLINE constexpr axInt size() const { return kSize; }

	AX_INLINE constexpr axTuple(ARGS&&... args) : Base(ax_forward(args)...) {};

	template<axInt INDEX> using Element = typename std::tuple_element<INDEX, Base>::type;
	template<axInt INDEX, class T = Element<INDEX>> AX_INLINE constexpr       T& get()				{ return std::get<INDEX>(*this); }
	template<axInt INDEX, class T = Element<INDEX>> AX_INLINE constexpr const T& get() const		{ return std::get<INDEX>(*this); }
	template<axInt INDEX, class T = Element<INDEX>> AX_INLINE constexpr       T  getValue() const	{ return std::get<INDEX>(*this); }

	void onFormat(axFormat& f) const; // implement in axFormat.h
	template<class TUPLE2> auto join(const TUPLE2& tuple2) const { return axTuple_join(*this, tuple2); }

	template<class FUNC> constexpr void forEach(FUNC& h)			{ UnrollHelper<      This, FUNC, kSize>::call(this, h); }
	template<class FUNC> constexpr void forEach(FUNC&& h) const		{ UnrollHelper<const This, FUNC, kSize>::call(this, h); }
	template<class HANDLER> constexpr static void s_forEachType(HANDLER& h) { UnrollTypeHelper<HANDLER, kSize>::call(h); }

private:
	template<class TUPLE, class FUNC, axInt N> struct UnrollHelper;
	template<class TUPLE, class FUNC> struct UnrollHelper<TUPLE, FUNC, 0> { static void call(TUPLE* tuple, FUNC& func) {} };

	template<class TUPLE, class FUNC, axInt N>
	struct UnrollHelper {
		static void call(TUPLE* tuple, FUNC& func) {
			static constexpr axInt INDEX = N-1;
			UnrollHelper<TUPLE, FUNC, INDEX>::call(tuple, func);
			func(INDEX, tuple->template get<INDEX>());
		}
	};

	template<class HANDLER, axInt N> struct UnrollTypeHelper;
	template<class HANDLER> struct UnrollTypeHelper<HANDLER, 0> { static void call(HANDLER& h) {} };

	template<class HANDLER, axInt N>
	struct UnrollTypeHelper {
		static void call(HANDLER& h) {
			static constexpr axInt INDEX = N-1;
			using Field = typename axTuple::template Element<INDEX>;
			UnrollTypeHelper<HANDLER, INDEX>::call(h);
			h.template handle< INDEX, Field >();
		}
	};

};

template<class... ARGS> AX_INLINE constexpr
auto axTuple_make(ARGS&&... args) {
	return axTuple<ARGS...>(ax_forward(args)...);
}

template<
	class TUPLE0, axInt... INDEX0,
	class TUPLE1, axInt... INDEX1
> AX_INLINE constexpr
auto axTuple_join_helper(	const TUPLE0& t0, const axIntSequence<INDEX0...>&, 
							const TUPLE1& t1, const axIntSequence<INDEX1...>&) {
	return axTuple_make(
		t0.template getValue<INDEX0>() ...,
		t1.template getValue<INDEX1>() ...
	);
}

template<class TUPLE0, class TUPLE1>
AX_INLINE constexpr
auto axTuple_join(const TUPLE0& t0, const TUPLE1& t1) {
	return axTuple_join_helper(	t0, axIntSequence_make<TUPLE0::kSize>(),
								t1, axIntSequence_make<TUPLE1::kSize>());
}


template<class...T> struct axTuple_JoinType_Helper;

template<
	class TUPLE0, axInt... INDEX0,
	class TUPLE1, axInt... INDEX1
>
struct axTuple_JoinType_Helper<	TUPLE0, axIntSequence<INDEX0...>, 
								TUPLE1, axIntSequence<INDEX1...> >
{
	using Type = axTuple<	typename TUPLE0::template Element<INDEX0> ...,
							typename TUPLE1::template Element<INDEX1> ...
							>;
};

template<class TUPLE0, class TUPLE1>
struct axTuple_JoinType {
	using Type = typename axTuple_JoinType_Helper<	TUPLE0, axIntSequence_make<TUPLE0::kSize>,
													TUPLE1, axIntSequence_make<TUPLE1::kSize>
													>::Type;
};
