#pragma once

//----- float ----
template< class T > constexpr T		ax_nan_		()				{ return std::numeric_limits<T>::quiet_NaN(); }
template< class T > constexpr bool	ax_is_nan	( const T& v )	{ return std::isnan(v); }

template < class T > constexpr T	ax_inf_		()				{ return std::numeric_limits<T>::infinity(); }
template < class T > constexpr bool	ax_is_inf	( const T& v )	{ return std::numeric_limits<T>::has_infinity && v == ax_inf_<T>(); }

const float  ax_nan32 = ax_nan_<axFloat32>();
const double ax_nan64 = ax_nan_<axFloat64>();
const float  ax_nan   = ax_nan32;

const float  ax_inf32 = ax_inf_<axFloat32>();
const double ax_inf64 = ax_inf_<axFloat64>();
const float  ax_inf   = ax_inf32;

//-------------------------
template< class T > constexpr axInt	ax_sign			( const T& a )								{ return a == 0 ? 0 : (ax_less_than0(a) ? -1: 1); }
template< class T > constexpr T		ax_div			( const T& a, const T& b )					{ return ( b==0 ) ? 0 : a/b; }
template< class T > constexpr T		ax_abs			( const T& a )								{ return a>0 ? a : -a ; }

template< class T > constexpr T		ax_min			( const T& a, const T& b )					{ return (a<b)?a:b; }
template< class T > constexpr T		ax_max			( const T& a, const T& b )					{ return (a>b)?a:b; }

template< class T > constexpr T		ax_min			( const T& a, const T& b, const T& c )		{ return ax_min(ax_min(a,b),c); }
template< class T > constexpr T		ax_max			( const T& a, const T& b, const T& c )		{ return ax_max(ax_max(a,b),c); }

template< class T > constexpr T		ax_min			( const T& a, const T& b, const T& c, const T& d )	{ return ax_min(ax_min(a,b),ax_min(c,d)); }
template< class T > constexpr T		ax_max			( const T& a, const T& b, const T& c, const T& d )	{ return ax_max(ax_max(a,b),ax_max(c,d)); }

//! x clamped to the range [a,b]
template< class T > constexpr T		ax_clamp		( const T& x, const T& a, const T & b )		{ return ax_max(a, ax_min(b,x)); }
template< class T > constexpr T		ax_clamp01		( const T& x )								{ return ax_clamp(x, T(0), T(1)); }

// ------------  _itself ---------------
template< class T > AX_INLINE void		ax_div_it		( T& a, const T& b )						{ a = ax_div(a, b); }
template< class T > AX_INLINE void		ax_abs_it		( T& a )									{ a = ax_abs(a); }

template< class T > AX_INLINE void		ax_min_it		( T& a, const T& b )						{ a = ax_min(a,b); }
template< class T > AX_INLINE void		ax_max_it		( T& a, const T& b )						{ a = ax_max(a,b); }

template< class T > AX_INLINE void		ax_min_it		( T& a, const T& b, const T& c )			{ a = ax_min(a,b,c); }
template< class T > AX_INLINE void		ax_max_it		( T& a, const T& b, const T& c )			{ a = ax_max(a,b,c); }

template< class T > AX_INLINE void		ax_min_it		( T& a, const T& b, const T& c, const T&d )	{ a = ax_min(a,b,c,d); }
template< class T > AX_INLINE void		ax_max_it		( T& a, const T& b, const T& c, const T&d )	{ a = ax_max(a,b,c,d); }

template< class T > AX_INLINE void		ax_clamp_it		( T& x, const T& a, const T & b )			{ x = ax_clamp(x,a,b); }
template< class T > AX_INLINE void		ax_clamp01_it	( T& x )									{ x = ax_clamp01(x); }

//-------------------------
AX_INLINE float		ax_pow(float  base, float  exp)	{ return std::pow(base, exp); }
AX_INLINE double	ax_pow(double base, double exp)	{ return std::pow(base, exp); }

template< class T > constexpr bool	ax_is_pow2		( const T& v )	{ return v != 0 && (v & (v - 1)) == 0; }

AX_INLINE axInt8	ax_next_pow2	( axInt8  v )	{ if (v <= 0) return 0; v--; v|=v>>1; v|=v>>2; v|=v>>4;                              v++; return v; }
AX_INLINE axInt16	ax_next_pow2	( axInt16 v )	{ if (v <= 0) return 0; v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8;                     v++; return v; }
AX_INLINE axInt32	ax_next_pow2	( axInt32 v )	{ if (v <= 0) return 0; v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16;           v++; return v; }
AX_INLINE axInt64	ax_next_pow2	( axInt64 v )	{ if (v <= 0) return 0; v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16; v|=v>>32; v++; return v; }

AX_INLINE axUInt8	ax_next_pow2	( axUInt8  v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4;                              v++; return v; }
AX_INLINE axUInt16	ax_next_pow2	( axUInt16 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8;                     v++; return v; }
AX_INLINE axUInt32	ax_next_pow2	( axUInt32 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16;           v++; return v; }
AX_INLINE axUInt64	ax_next_pow2	( axUInt64 v )	{ v--; v|=v>>1; v|=v>>2; v|=v>>4; v|=v>>8; v|=v>>16; v|=v>>32; v++; return v; }

template< class T > AX_INLINE
T ax_next_pow2_half(T v) {
	auto o = ax_next_pow2(v);
	auto h = o - (o >> 2);
	return h >= v ? h : o;
}


//Splits a floating-point value into fractional and integer parts
AX_INLINE float		ax_modf	( float  v, float  *i ) { return std::modf( v, i ); }
AX_INLINE double	ax_modf	( double v, double *i ) { return std::modf( v, i ); }

AX_INLINE float		ax_frac	( float  v )	{ float  intPart; return ax_modf(v, &intPart); }
AX_INLINE double	ax_frac	( double v )	{ double intPart; return ax_modf(v, &intPart); }

AX_INLINE float		ax_ceil	( float  a )	{ return std::ceil(a); }
AX_INLINE double	ax_ceil	( double a )	{ return std::ceil(a); }

AX_INLINE float		ax_floor( float  a )	{ return std::floor(a); }
AX_INLINE double	ax_floor( double a )	{ return std::floor(a); }

//get remainder
AX_INLINE float		ax_fmod	( float  a, float  b ) { return std::fmod(a,b); }
AX_INLINE double	ax_fmod	( double a, double b ) { return std::fmod(a,b); }

#if AX_COMPILER_VC | AX_OS_CYGWIN
	AX_INLINE float  ax_trunc	( float  n )	{ float  i; return std::modf( n, &i ); }
	AX_INLINE double ax_trunc	( double n )	{ double i; return std::modf( n, &i ); }

	AX_INLINE float  ax_round	( float  a )	{ return a > 0 ? ax_floor(a+0.5f) : ax_ceil(a-0.5f); }
	AX_INLINE double ax_round	( double a )	{ return a > 0 ? ax_floor(a+0.5 ) : ax_ceil(a-0.5 ); }
#else
	AX_INLINE float  ax_trunc	( float  n )	{ return std::trunc(n); }
	AX_INLINE double ax_trunc	( double n )	{ return std::trunc(n); }

	AX_INLINE float  ax_round	( float  a )	{ return std::round(a); }
	AX_INLINE double ax_round	( double a )	{ return std::round(a); }
#endif

AX_INLINE axInt	ax_trunc_to_int	( float  a )	{ return static_cast<axInt>( ax_trunc(a) ); }
AX_INLINE axInt	ax_trunc_to_int	( double a )	{ return static_cast<axInt>( ax_trunc(a) ); }
AX_INLINE axInt	ax_round_to_int	( float  a )	{ return static_cast<axInt>( ax_round(a) ); }
AX_INLINE axInt	ax_round_to_int	( double a )	{ return static_cast<axInt>( ax_round(a) ); }
AX_INLINE axInt	ax_ceil_to_int	( float  a )	{ return static_cast<axInt>( ax_ceil (a) ); }
AX_INLINE axInt	ax_ceil_to_int	( double a )	{ return static_cast<axInt>( ax_ceil (a) ); }
AX_INLINE axInt	ax_floor_to_int	( float  a )	{ return static_cast<axInt>( ax_floor(a) ); }
AX_INLINE axInt	ax_floor_to_int	( double a )	{ return static_cast<axInt>( ax_floor(a) ); }

template<class T>
constexpr axInt  ax_roundup_div(T x, T d) {
	AX_STATIC_ASSERT(axTypeTraits::isIntegral<T>::value);
	return (x > 0 ? x + (ax_abs(d) - 1) : x - (ax_abs(d) - 1)) / d;
}

template<class T> AX_INLINE void ax_round_it	( T & v ) { v = ax_round(v); }
template<class T> AX_INLINE void ax_floor_it	( T & v ) { v = ax_floor(v); }
template<class T> AX_INLINE void ax_ceil_it 	( T & v ) { v = ax_ceil (v); }

template<class T> T ax_epsilon();
template<>	constexpr axInt			ax_epsilon<axInt     >() { return 0; }
template<>	constexpr axFloat32		ax_epsilon<axFloat32 >() { return FLT_EPSILON; }
template<>	constexpr axFloat64		ax_epsilon<axFloat64 >() { return DBL_EPSILON; }
template<>	constexpr axFloat128	ax_epsilon<axFloat128>() { return LDBL_EPSILON; }

template<class T> AX_INLINE bool	ax_math_equals (const T& a, const T& b, const T& ep=ax_epsilon<T>()) { return ( ax_abs(a-b) <= ep) ; }
template<class T> AX_INLINE bool	ax_math_equals0(const T& a,             const T& ep=ax_epsilon<T>()) { return ( ax_abs( a ) <= ep) ; }

//------------------------------------
template<class T> AX_INLINE T ax_square(const T& v) { return v * v; }

AX_INLINE float		ax_sqrt(float  n) { return std::sqrt(n); }
AX_INLINE double	ax_sqrt(double n) { return std::sqrt(n); }
AX_INLINE axInt		ax_sqrt(axInt  n) { return static_cast<axInt>(std::sqrt(static_cast<double>(n))); }

AX_INLINE float		ax_cbrt(float  n) { return std::cbrt(n); }
AX_INLINE double	ax_cbrt(double n) { return std::cbrt(n); }
AX_INLINE axInt		ax_cbrt(axInt  n) { return static_cast<axInt>(std::cbrt(static_cast<double>(n))); }

//------- reciprocal square root ---------------
AX_INLINE float ax_rsqrt_fast(float n) {
#if AX_CPU_FEATURE_SSE2
	return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(n)));
#else // https://en.wikipedia.org/wiki/Fast_inverse_square_root
	uint32_t i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = n * 0.5f;
	y = n;
	i = *reinterpret_cast<uint32_t *>(&y);
	i = 0x5F375A86UL - (i >> 1);
	y = *reinterpret_cast<float *>(&i);
	y = y * (threehalfs - (x2 * y * y));
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
#endif
}

inline double ax_rsqrt_fast(double n) {
	uint64_t i;
	double x2, y;
	const float threehalfs = 1.5;

	x2 = n * 0.5f;
	y = n;
	i = *reinterpret_cast<uint64_t *>(&y);
	i = 0x5FE6EB50C7B537A9ULL - (i >> 1);
	y = *reinterpret_cast<double *>(&i);
	y = y * (threehalfs - (x2 * y * y));
	//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
	return y;
}

inline axInt ax_rsqrt_fast(axInt n) {
	double d = static_cast<double>(n);
	return ax_round_to_int(ax_rsqrt_fast(d));
}

AX_INLINE float ax_rsqrt(float v) {
#if 0 //AX_CPU_FEATURE_SSE2
	float  xrsqrt_est = ax_rsqrt_fast(v);
	return xrsqrt_est * (1.5f - v * 0.5f * xrsqrt_est * xrsqrt_est); // NR iteration
#else
	return 1.0f / ax_sqrt(v); 
#endif
}

AX_INLINE double ax_rsqrt(double v) { return 1.0 / ax_sqrt(v); }
AX_INLINE axInt  ax_rsqrt(axInt  v) { return ax_round_to_int(ax_rsqrt(static_cast<double>(v))); }

//-------------- Lerp ----------------
//! linear interpolation out = a+w*(b-a)

template<class T> T AX_INLINE constexpr ax_int_lerp( const T & a, const T & b, float  w ) { return static_cast<T>( ax_round( a+w*( b - a) ) ); }
template<class T> T AX_INLINE constexpr ax_int_lerp( const T & a, const T & b, double w ) { return static_cast<T>( ax_round( a+w*( b - a) ) ); }

#define AX_MACRO_OP(T) \
	AX_INLINE constexpr T ax_lerp( const T & a, const T & b, float  w ) { return ax_int_lerp(a,b,w); } \
	AX_INLINE constexpr T ax_lerp( const T & a, const T & b, double w ) { return ax_int_lerp(a,b,w); } \
//--------
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
#undef AX_MACRO_OP

AX_INLINE constexpr float	ax_lerp( float    a, float    b, float  w ) { return (1-w)*a + w*b; }
AX_INLINE constexpr double	ax_lerp( double   a, double   b, double w ) { return (1-w)*a + w*b; }

AX_INLINE constexpr float	ax_step( float 	a, float  x ) 	{ return x>=a ? 1.0f : 0.0f; }
AX_INLINE constexpr double	ax_step( double a, double x ) 	{ return x>=a ? 1.0  : 0.0;  }

AX_INLINE constexpr float	ax_ease_in ( float  a ) { return a*a; }
AX_INLINE constexpr double	ax_ease_in ( double a ) { return a*a; }
AX_INLINE constexpr float	ax_ease_out( float  a ) { a=1-a; return 1-a*a; }
AX_INLINE constexpr double	ax_ease_out( double a ) { a=1-a; return 1-a*a; }

AX_INLINE constexpr float	ax_smooth_step( float  w ) { return (-2.0f*w*w*w + 3.0f*w*w); }
AX_INLINE constexpr double	ax_smooth_step( double w ) { return (-2.0 *w*w*w + 3.0 *w*w); }

template<class T> AX_INLINE constexpr T ax_smooth_step( const T &a, const T &b, float  w ) { return ax_lerp(a,b,ax_smooth_step(w) ); }
template<class T> AX_INLINE constexpr T ax_smooth_step( const T &a, const T &b, double w ) { return ax_lerp(a,b,ax_smooth_step(w) ); }

//---------- trigonometric ----------------

constexpr float  ax_pi_f = 3.14159265358979323846f;
constexpr double ax_pi   = 3.14159265358979323846;

template< class T> constexpr T	ax_pi_();
template<> constexpr float		ax_pi_<float>	()		{ return ax_pi_f; }
template<> constexpr double		ax_pi_<double>	()		{ return ax_pi;  }

template<class T> constexpr T	ax_two_pi		()		{ return ax_pi_<T>() * 2; }
template<class T> constexpr T	ax_half_pi	()		{ return ax_pi_<T>() * 0.5; }

template<class T> AX_INLINE T	ax_radians	(T deg) { return deg * (ax_pi_<T>() / T(180)); }
template<class T> AX_INLINE T	ax_degrees	(T rad) { return rad * (T(180) / ax_pi_<T>()); }

AX_INLINE float		ax_sin	( float  rad ) { return std::sin(rad); }
AX_INLINE double	ax_sin	( double rad ) { return std::sin(rad); }

AX_INLINE float		ax_cos	( float  rad ) { return std::cos(rad); }
AX_INLINE double	ax_cos	( double rad ) { return std::cos(rad); }

#if AX_OS_MACOSX
	AX_INLINE void ax_sincos( float  rad, float  & out_sin, float  & out_cos ) { ::__sincosf(rad, &out_sin, &out_cos); }
	AX_INLINE void ax_sincos( double rad, double & out_sin, double & out_cos ) { ::__sincos (rad, &out_sin, &out_cos); }
#elif AX_OS_LINUX
	AX_INLINE void ax_sincos( float  rad, float  & out_sin, float  & out_cos ) {   ::sincosf(rad, &out_sin, &out_cos); }
	AX_INLINE void ax_sincos( double rad, double & out_sin, double & out_cos ) {   ::sincos (rad, &out_sin, &out_cos); }
#else
	AX_INLINE void ax_sincos( float  rad, float  & out_sin, float  & out_cos ) { out_sin = ax_sin(rad); out_cos = ax_cos(rad); }
	AX_INLINE void ax_sincos( double rad, double & out_sin, double & out_cos ) { out_sin = ax_sin(rad); out_cos = ax_cos(rad); }
#endif

AX_INLINE float		ax_tan	( float  rad ) { return std::tan(rad); }
AX_INLINE double	ax_tan	( double rad ) { return std::tan(rad); }

AX_INLINE float		ax_asin	( float  rad ) { return std::asin(rad); }
AX_INLINE double	ax_asin	( double rad ) { return std::asin(rad); }

AX_INLINE float		ax_acos	( float  rad ) { return std::acos(rad); }
AX_INLINE double	ax_acos	( double rad ) { return std::acos(rad); }

AX_INLINE float		ax_atan2( float  a, float  b ) { return std::atan2(a,b); }
AX_INLINE double	ax_atan2( double a, double b ) { return std::atan2(a,b); }

//---------------------------------------
template< class T> AX_INLINE
T _ax_multiple_of_uint( T n, T a ) {
	T r = n % a;
	if( r == 0 ) return n;
	return n + (a-r);
}

template< class T> AX_INLINE
T _ax_multiple_of_int( T n, T a ) {
	a = ax_abs(a);

	if( n >= 0 ) {
		T r = n % a;
		if( r == 0 ) return n;
		return n + (a-r);

	}else{
		T r = (-n) % a;
		if( r == 0 ) return n;
		return n - (a-r);
	}
}

AX_INLINE int8_t	ax_multiple_of( int8_t  n, int8_t  a ) { return _ax_multiple_of_int(n,a); }
AX_INLINE int16_t	ax_multiple_of( int16_t n, int16_t a ) { return _ax_multiple_of_int(n,a); }
AX_INLINE int32_t	ax_multiple_of( int32_t n, int32_t a ) { return _ax_multiple_of_int(n,a); }
AX_INLINE int64_t	ax_multiple_of( int64_t n, int64_t a ) { return _ax_multiple_of_int(n,a); }

AX_INLINE uint8_t	ax_multiple_of( uint8_t  n, uint8_t  a ) { return _ax_multiple_of_uint(n,a); }
AX_INLINE uint16_t	ax_multiple_of( uint16_t n, uint16_t a ) { return _ax_multiple_of_uint(n,a); }
AX_INLINE uint32_t	ax_multiple_of( uint32_t n, uint32_t a ) { return _ax_multiple_of_uint(n,a); }
AX_INLINE uint64_t	ax_multiple_of( uint64_t n, uint64_t a ) { return _ax_multiple_of_uint(n,a); }

//! floating-point align
template< class T> AX_INLINE
T  _ax_multiple_of_float( T   n, T   a ) {
	T i = ax_floor( n / a ) * a;
	if( i == n ) return i;
	return (n > 0) ? i+a : i-a;
}

AX_INLINE float  ax_multiple_of( float  n, float  a ) { return _ax_multiple_of_float(n,a); }
AX_INLINE double ax_multiple_of( double n, double a ) { return _ax_multiple_of_float(n,a); }

template<class T>
AX_INLINE T ax_is_multiple_of(T n, T a) { return ax_multiple_of(n,a) == n; }

template < class T, class W > inline
T ax_cubic_bezier( T p0, T p1, T p2, T p3, W w) {
	W iw  = 1-w;
	W iw2 = iw*iw;
	W w2  = w*w;

	return	p0 * iw * iw2
		  + p1 * 3  * iw2 * w
		  + p2 * 3  * iw  * w2
		  + p3 * w2 * w;
}
