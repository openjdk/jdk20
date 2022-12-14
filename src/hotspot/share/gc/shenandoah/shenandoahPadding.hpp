/*
 * Copyright (c) 2020, Red Hat, Inc. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_GC_SHENANDOAH_SHENANDOAHPADDING_HPP
#define SHARE_GC_SHENANDOAH_SHENANDOAHPADDING_HPP

#include "memory/padded.hpp"

// 64 bytes is enough to cover all existing architectures. If we have some
// other platforms, we would need to provide the architecture-specific
// versions here. Shared code provides DEFAULT_CACHE_LINE_SIZE, which is
// inconveniently large by default.

#define SHENANDOAH_CACHE_LINE_SIZE 64

#define shenandoah_padding(id) \
  DEFINE_PAD_MINUS_SIZE(id, SHENANDOAH_CACHE_LINE_SIZE, 0)

#define shenandoah_padding_minus_size(id, size) \
  DEFINE_PAD_MINUS_SIZE(id, SHENANDOAH_CACHE_LINE_SIZE, size)

#endif // SHARE_GC_SHENANDOAH_SHENANDOAHPADDING_HPP
