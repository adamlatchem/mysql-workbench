/* 
 * Copyright (c) 2007, 2014, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#pragma once

#pragma warning(disable: 4793) // 'vararg' causes native code generation
#pragma warning(disable: 4996) // 'std::_Uninitialized_copy0': Function call with parameters that may be unsafe
                               // A warning caused by the usage of boost in this context.

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <WinCrypt.h>
#include <ShellAPI.h>
#include <commctrl.h>
#include <vcclr.h>

#define STRICT_TYPED_ITEMIDS // Better type safety for IDLists
#include <shlobj.h>

#define _USE_MATH_DEFINES
#include <math.h>
#define INFINITY FLT_MAX

#include "gl/gl.h"

#include <sstream>
#include <string>
#include <assert.h>
#include <hash_map>

#include <cairo/cairo.h>
#include <cairo/cairo-Win32.h>

#include <boost/function.hpp>
#include <boost/signals2.hpp>

// In order to use native types from other assemblies we have to make them public in *this*
// assembly (they are made private by default). However in order to avoid linker error LNK2022
// this must happen in a very specific order. To ease this we do it in this header.
#include "mforms/mforms.h"

#pragma make_public(mforms::Object)
#pragma make_public(mforms::ContextMenu)
#pragma make_public(mforms::DockingPointDelegate)
#pragma make_public(mforms::AppView)
#pragma make_public(mforms::View)
