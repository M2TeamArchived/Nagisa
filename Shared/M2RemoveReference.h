/******************************************************************************
Project: M2-Team Common Library
Description: Definition for the M2RemoveReference struct template
File Name: M2RemoveReference.h
License: The MIT License
******************************************************************************/

#pragma once

#ifndef _M2_REMOVE_REFERENCE_
#define _M2_REMOVE_REFERENCE_

// If the type T is a reference type, provides the member typedef type which is
// the type referred to by T. Otherwise type is T.
template<class T> struct M2RemoveReference { typedef T Type; };
template<class T> struct M2RemoveReference<T&> { typedef T Type; };
template<class T> struct M2RemoveReference<T&&> { typedef T Type; };
template<class T> struct M2RemoveReference<T^> { typedef T Type; };

#endif // _M2_REMOVE_REFERENCE_
