// Additional version of 'struct hash' for unsigned long long
// Based on /usr/include/c++/3.2.3/ext/stl_hash_fun.h from gcc3.2.3,
// original comments retained below. This is needed only for compiling
// Docsim libraries using GCC prior to 4.3 which does not support
// unsigned long long in hash types.
//
// Simeon Warner - 2007...2011
//------------------------------------------------------------------

// 'struct hash' from SGI -*- C++ -*-

// Copyright (C) 2001, 2002 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

/*
 * Copyright (c) 1996-1998
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/** @file ext/stl_hash_fun.h
 *  This file is a GNU extension to the Standard C++ Library (possibly
 *  containing extensions from the HP/SGI STL subset).  You should only
 *  include this header if you are using GCC 3 or later.
 */

#ifndef _HASH_ULL
#define _HASH_ULL 1

#include <cstddef>

namespace __gnu_cxx
{
using std::size_t;

//template <class _Key> struct hash { };

template<> struct hash<unsigned long long> {
  size_t operator()(unsigned long long __x) const { return __x; }
};

} // namespace __gnu_cxx

#endif /* _HASH_ULL */

// Local Variables:
// mode:C++
// End:
