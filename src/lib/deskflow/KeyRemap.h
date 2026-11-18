/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include "deskflow/KeyTypes.h"

#include <map>
#include <string>

namespace deskflow {

//! Per computer key remapping
/*!
Remaps the keys the server sends so that a computer can receive a different key
than the one pressed on the primary computer.  The per computer modifier map can
only exchange the modifier roles; this class also covers keys that are not
modifiers, e.g. CapsLock to Escape, and modifiers to keys that are not modifiers.

A remapped modifier key also changes the modifier mask the computer receives so
that key combinations keep working, e.g. remapping Control to Super makes
control+c arrive as super+c.
*/
class KeyRemap
{
public:
  //! Add a mapping
  /*!
  Adds a mapping from the key named \c from to the key named \c to.  Returns
  \c false if either name is unknown.  A later mapping for the same key
  replaces the earlier one.  Two modifier names, e.g. "ctrl" and "super", map
  both sides of the modifiers; a key name maps a single key.
  */
  bool addMapping(const std::string &from, const std::string &to);

  //! Add a mapping
  /*!
  Adds a mapping using key ids.  A mapping of a key to itself is removed.
  */
  void addMapping(KeyID from, KeyID to);

  //! Test if there are no mappings
  bool empty() const
  {
    return m_keys.empty();
  }

  //! Remap a key
  /*!
  Returns the mapped key or \c id if the key is not mapped.
  */
  KeyID remapKey(KeyID id) const;

  //! Remap a modifier mask
  /*!
  Maps the modifier bits of \c mask using the modifier mappings.  The lock and
  toggle bits are not changed.
  */
  KeyModifierMask remapMask(KeyModifierMask mask) const;

  //! Parse a key name
  /*!
  Accepts the key names from kKeyNameMap (e.g. "Control_L" or "Escape") and the
  modifier names used by the modifier options (e.g. "ctrl").  A modifier name
  always refers to the left hand modifier key.
  */
  static bool parseKey(const std::string &name, KeyID &id);

  bool operator==(const KeyRemap &x) const
  {
    return m_keys == x.m_keys;
  }

private:
  std::map<KeyID, KeyID> m_keys;
};

} // namespace deskflow
