/*
 * Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.
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

#include "precompiled.hpp"
#include "opto/loopnode.hpp"
#include "opto/opaquenode.hpp"
#include "opto/phaseX.hpp"

//=============================================================================
// Do not allow value-numbering
uint Opaque1Node::hash() const { return NO_HASH; }
bool Opaque1Node::cmp( const Node &n ) const {
  return (&n == this);          // Always fail except on self
}

//------------------------------Identity---------------------------------------
// Do NOT remove the opaque node until no more loop opts can happen.
Node* Opaque1Node::Identity(PhaseGVN* phase) {
  if (phase->C->post_loop_opts_phase()) {
    return in(1);
  } else {
    phase->C->record_for_post_loop_opts_igvn(this);
  }
  return this;
}

// Is there to hold off with optimizing post loop based on type of limit
// as long as main loop is around and can still change the limit due to
// unrolling. Pattern we are looking for:
//
//               +--------+
//               |        |
//      strip_mined_head  |
//       |                |
//      main_head (main)  |
//       |                |
//      ...               |
//       |                |
//      strip_mined_end   |
//       |           |    |
//       |           +----+     opaq (this)
//       |                       |
//      main_exit               zer_cmp
//       |                       |
//   main_zer_region            zer_bol
//    |                          |
//    +-----------+  +-----------+
//                |  |
//               zer_if (zero trip guard post loop)
//
// In some cases, the main-loop is not strip-mined:
//
//               +--------+
//               |        |
//      main_head (main)  |
//       |                |
//      ...               |
//       |                |
//      main_end          |
//       |    |           |
//       |    +-----------+     opaq (this)
//       |                       |
//      main_exit               zer_cmp
//       |                       |
//   main_zer_region            zer_bol
//    |                          |
//    +-----------+  +-----------+
//                |  |
//               zer_if (zero trip guard post loop)
//
Node* OpaqueZeroTripGuardPostLoopNode::Identity(PhaseGVN* phase) {
  Node* result = OpaqueZeroTripGuardNode::Identity(phase);
  if (result != this) {
    return result;
  }
  // If must still be there, after all we are guarding it
  Node* zer_cmp = unique_out();
  assert(zer_cmp->Opcode() == Op_CmpI, "Opaque guards CmpI");
  Node* zer_bol = zer_cmp->unique_out();
  assert(zer_bol->is_Bool(), "Opaque guards Bool");
  Node* zer_if = zer_bol->unique_out();
  assert(zer_if->is_If(), "Opaque guards If");
  // From here, no assumption is given, the graph may be degenerate
  Node* main_zer_region = zer_if->in(0);
  if (main_zer_region == nullptr ||
      !main_zer_region->is_Region() ||
      main_zer_region->req() != 3) {
    return in(1); // degenerate at region, remove Opaque
  }
  Node* main_exit = main_zer_region->in(2);
  if (main_exit == nullptr ||
      !main_exit->is_IfProj()) {
    return in(1); // degenerate at main exit, remove Opaque
  }
  Node* outer_end = main_exit->in(0);
  if (outer_end == nullptr) {
    return in(1); // degenerate at outer end, remove Opaque
  }
  // find main_head, with or without strip-mining
  Node* main_head = nullptr;
  if (outer_end->is_OuterStripMinedLoopEnd()) {
    OuterStripMinedLoopNode* strip_mined_head = outer_end->as_OuterStripMinedLoopEnd()->loopnode();
    if (strip_mined_head == nullptr) {
      return in(1); // degenerate at outer loopnode, remove Opaque
    }
    main_head = strip_mined_head->unique_ctrl_out_or_null();
  } else if (outer_end->is_BaseCountedLoopEnd()) {
    main_head = outer_end->as_BaseCountedLoopEnd()->loopnode();
  } else {
    return in(1); // degenerate at outer end, remove Opaque
  }
  if (main_head == nullptr ||
      !main_head->is_CountedLoop() ||
      !main_head->as_CountedLoop()->is_main_loop()) {
    return in(1); // degenerate at main loop, remove Opaque
  }
  return this; // main loop still here, keep this Opaque
}

// Do NOT remove the opaque node until no more loop opts can happen.
Node* Opaque3Node::Identity(PhaseGVN* phase) {
  if (phase->C->post_loop_opts_phase()) {
    return in(1);
  } else {
    phase->C->record_for_post_loop_opts_igvn(this);
  }
  return this;
}

// Do not allow value-numbering
uint Opaque3Node::hash() const { return NO_HASH; }
bool Opaque3Node::cmp(const Node &n) const {
  return (&n == this);          // Always fail except on self
}

const Type* Opaque4Node::Value(PhaseGVN* phase) const {
  return phase->type(in(1));
}

//=============================================================================

uint ProfileBooleanNode::hash() const { return NO_HASH; }
bool ProfileBooleanNode::cmp( const Node &n ) const {
  return (&n == this);
}

Node *ProfileBooleanNode::Ideal(PhaseGVN *phase, bool can_reshape) {
  if (can_reshape && _delay_removal) {
    _delay_removal = false;
    return this;
  } else {
    return NULL;
  }
}

Node* ProfileBooleanNode::Identity(PhaseGVN* phase) {
  if (_delay_removal) {
    return this;
  } else {
    assert(_consumed, "profile should be consumed before elimination");
    return in(1);
  }
}
