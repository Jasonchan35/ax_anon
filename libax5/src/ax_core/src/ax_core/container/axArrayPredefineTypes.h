#pragma once

#include "axIndirectArray.h"
#include "../pointer/axUPtr.h"
#include "../pointer/axWeak.h"
#include "../pointer/axLink.h"

template< class T > class axIArray;
template< class T, axInt N > class axArray;

using axIByteArray = axIArray< axByte >;
using axByteArray = axArray< axByte, axArray_kDefaultSize(axByte) >;
template<axInt N> using axByteArray_ = axArray<axByte, N>;

using axIIntArray = axIArray< axInt >;
using axIntArray = axArray< axInt, axArray_kDefaultSize(axInt) >;
template<axInt N> using axIntArray_ = axArray<axInt, N>;

template<class T, axInt N = 0> using axUPtrArray = axArray<axUPtr<T>, N>;
template<class T, axInt N = 0> using axSPtrArray = axArray<axSPtr<T>, N>;
template<class T, axInt N = 0> using axWeakArray = axArray<axWeak<T>, N>;

template<class T> using axUPtrSpan = axSpan<axUPtr<T>>;
template<class T> using axSPtrSpan = axSpan<axSPtr<T>>;
template<class T> using axWeakSpan = axSpan<axWeak<T>>;
template<class T> using axLinkSpan = axSpan<axLink<T>>;

template<class T> using axUPtrMutSpan = axMutSpan<axUPtr<T>>;
template<class T> using axSPtrMutSpan = axMutSpan<axSPtr<T>>;
template<class T> using axWeakMutSpan = axMutSpan<axWeak<T>>;
template<class T> using axLinkMutSpan = axMutSpan<axLink<T>>;

