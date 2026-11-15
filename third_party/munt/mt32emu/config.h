/* Copyright (C) 2003, 2004, 2005, 2006, 2008, 2009 Dean Beeler, Jerome Fisher
 * Copyright (C) 2011-2022 Dean Beeler, Jerome Fisher, Sergey V. Mikayev
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* config.h.in as CMake configures it for a static library with both the C
 * and C++ interfaces and no version tagging (see README.wc1). */

#ifndef MT32EMU_CONFIG_H
#define MT32EMU_CONFIG_H

#define MT32EMU_VERSION      "2.8.6"
#define MT32EMU_VERSION_MAJOR 2
#define MT32EMU_VERSION_MINOR 8
#define MT32EMU_VERSION_PATCH 6

#define MT32EMU_EXPORTS_TYPE 3

#undef MT32EMU_SHARED

#define MT32EMU_WITH_VERSION_TAGGING 0

#if MT32EMU_WITH_VERSION_TAGGING
#  ifndef MT32EMU_RUNTIME_VERSION_CHECK
#    define MT32EMU_RUNTIME_VERSION_CHECK 0
#  endif
#else
#  undef MT32EMU_RUNTIME_VERSION_CHECK
#endif

#endif /* #ifndef MT32EMU_CONFIG_H */
