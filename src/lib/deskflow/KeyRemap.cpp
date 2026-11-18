/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "deskflow/KeyRemap.h"

#include "base/String.h"
#include "common/KeyboardModifier.h"
#include "deskflow/KeyMap.h"

using namespace deskflow::string;

namespace deskflow {

namespace {

//! Names accepted for a modifier
/*!
The names used by the modifier map options and the shorter name used by the
GUI for control.  A name here refers to both sides of the modifier, a mapping
of a single side has to use the key name, e.g. "Control_R".
*/
const KeyModifierNameMapEntry kModifierNames[] = {
    {"Alt", KeyModifierAlt},
    {"AltGr", KeyModifierAltGr},
    {"Control", KeyModifierControl},
    {"Meta", KeyModifierMeta},
    {"Shift", KeyModifierShift},
    {"Super", KeyModifierSuper},
    {kModifierNameCtrl, KeyModifierControl},
    {nullptr, 0},
};

//! Get the modifier a name refers to
/*!
Returns the modifier mask for a modifier name and 0 if \c name is not a
modifier name.
*/
KeyModifierMask modifierForName(const std::string &name)
{
  for (const KeyModifierNameMapEntry *i = kModifierNames; i->m_name != nullptr; ++i) {
    if (CaselessCmp::equal(name, i->m_name)) {
      return i->m_mask;
    }
  }

  return 0;
}

//! Modifier key mask for a key
/*!
Returns the mask of the modifier the key generates or 0 if the key is not a
modifier.
*/
KeyModifierMask modifierMaskForKey(KeyID id)
{
  switch (id) {
  case kKeyShift_L:
  case kKeyShift_R:
    return KeyModifierShift;

  case kKeyControl_L:
  case kKeyControl_R:
    return KeyModifierControl;

  case kKeyAlt_L:
  case kKeyAlt_R:
    return KeyModifierAlt;

  case kKeyMeta_L:
  case kKeyMeta_R:
    return KeyModifierMeta;

  case kKeySuper_L:
  case kKeySuper_R:
    return KeyModifierSuper;

  case kKeyAltGr:
    return KeyModifierAltGr;

  default:
    return 0;
  }
}

//! Left hand modifier key for a modifier mask
/*!
Returns the key id of the left hand key that generates \c mask or kKeyNone if
\c mask is not a single modifier.
*/
KeyID leftModifierKeyForMask(KeyModifierMask mask)
{
  switch (mask) {
  case KeyModifierShift:
    return kKeyShift_L;

  case KeyModifierControl:
    return kKeyControl_L;

  case KeyModifierAlt:
    return kKeyAlt_L;

  case KeyModifierMeta:
    return kKeyMeta_L;

  case KeyModifierSuper:
    return kKeySuper_L;

  case KeyModifierAltGr:
    return kKeyAltGr;

  default:
    return kKeyNone;
  }
}

//! Right hand modifier key for a modifier mask
/*!
Returns the key id of the right hand key that generates \c mask.  Returns the
same key as leftModifierKeyForMask() for a modifier that only has one key.
*/
KeyID rightModifierKeyForMask(KeyModifierMask mask)
{
  switch (mask) {
  case KeyModifierShift:
    return kKeyShift_R;

  case KeyModifierControl:
    return kKeyControl_R;

  case KeyModifierAlt:
    return kKeyAlt_R;

  case KeyModifierMeta:
    return kKeyMeta_R;

  case KeyModifierSuper:
    return kKeySuper_R;

  case KeyModifierAltGr:
    return kKeyAltGr;

  default:
    return kKeyNone;
  }
}

} // namespace

bool KeyRemap::addMapping(const std::string &from, const std::string &to)
{
  KeyID toId = kKeyNone;
  if (!parseKey(to, toId)) {
    return false;
  }

  // a modifier name refers to both sides of the modifier so that the key and
  // the modifier mask can not disagree about which side was pressed
  if (const auto fromModifier = modifierForName(from); fromModifier != 0) {
    const auto toModifier = modifierForName(to);
    const auto left = leftModifierKeyForMask(fromModifier);

    // a modifier that is mapped to itself is not a mapping
    if (toModifier == fromModifier) {
      addMapping(left, left);
      addMapping(rightModifierKeyForMask(fromModifier), rightModifierKeyForMask(fromModifier));
      return true;
    }

    addMapping(left, toId);

    // alt gr is a single key so there is no right hand side to map
    if (const auto right = rightModifierKeyForMask(fromModifier); right != left) {
      // keep both sides on the same side of a modifier target
      addMapping(right, toModifier != 0 ? rightModifierKeyForMask(toModifier) : toId);
    }

    return true;
  }

  KeyID fromId = kKeyNone;
  if (!parseKey(from, fromId)) {
    return false;
  }

  addMapping(fromId, toId);
  return true;
}

void KeyRemap::addMapping(KeyID from, KeyID to)
{
  if (from == kKeyNone || to == kKeyNone) {
    return;
  }

  if (from == to) {
    m_keys.erase(from);
    return;
  }

  m_keys[from] = to;
}

KeyID KeyRemap::remapKey(KeyID id) const
{
  const auto i = m_keys.find(id);
  return i == m_keys.end() ? id : i->second;
}

KeyModifierMask KeyRemap::remapMask(KeyModifierMask mask) const
{
  if (m_keys.empty()) {
    return mask;
  }

  static const KeyModifierMask s_modifierMasks =
      KeyModifierShift | KeyModifierControl | KeyModifierAlt | KeyModifierMeta | KeyModifierSuper | KeyModifierAltGr;

  // the lock and toggle bits are the same on the computer and are kept as is
  KeyModifierMask newMask = mask & ~s_modifierMasks;

  static const struct
  {
    KeyModifierMask m_mask;
    KeyID m_left;
    KeyID m_right;
  } s_modifiers[] = {
      {KeyModifierShift, kKeyShift_L, kKeyShift_R},
      {KeyModifierControl, kKeyControl_L, kKeyControl_R},
      {KeyModifierAlt, kKeyAlt_L, kKeyAlt_R},
      {KeyModifierMeta, kKeyMeta_L, kKeyMeta_R},
      {KeyModifierSuper, kKeySuper_L, kKeySuper_R},
      {KeyModifierAltGr, kKeyAltGr, kKeyAltGr},
  };

  for (const auto &modifier : s_modifiers) {
    if ((mask & modifier.m_mask) == 0) {
      continue;
    }

    // the mask has no side so the left hand mapping wins, otherwise use the
    // right hand mapping if that is the only one that was mapped
    auto i = m_keys.find(modifier.m_left);
    if (i == m_keys.end()) {
      i = m_keys.find(modifier.m_right);
    }

    // a modifier that is mapped to a key that is not a modifier no longer sets
    // a modifier bit
    newMask |= i == m_keys.end() ? modifier.m_mask : modifierMaskForKey(i->second);
  }

  return newMask;
}

bool KeyRemap::parseKey(const std::string &name, KeyID &id)
{
  // the key names used by the protocol, e.g. "Control_L", "CapsLock" or "Escape"
  if (KeyMap::parseKey(name, id) && id != kKeyNone) {
    return true;
  }

  // a modifier name refers to the left hand key
  const auto modifier = modifierForName(name);
  if (modifier != 0) {
    id = leftModifierKeyForMask(modifier);
    return id != kKeyNone;
  }

  id = kKeyNone;
  return false;
}

} // namespace deskflow
