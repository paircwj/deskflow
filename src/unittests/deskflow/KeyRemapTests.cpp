/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#include "KeyRemapTests.h"

#include "deskflow/KeyRemap.h"

using namespace deskflow;

void KeyRemapTests::addMapping_unknownKey_returnsFalse()
{
  KeyRemap keyRemap;

  QVERIFY(keyRemap.addMapping("Control_L", "Super_L"));
  QVERIFY(!keyRemap.addMapping("Control_L", "NotAKey"));
  QVERIFY(!keyRemap.addMapping("NotAKey", "Super_L"));
}

void KeyRemapTests::addMapping_identity_removesMapping()
{
  KeyRemap keyRemap;
  keyRemap.addMapping(kKeyControl_L, kKeySuper_L);

  QVERIFY(!keyRemap.empty());

  keyRemap.addMapping(kKeyControl_L, kKeyControl_L);

  QVERIFY(keyRemap.empty());
  QCOMPARE(keyRemap.remapKey(kKeyControl_L), kKeyControl_L);
}

void KeyRemapTests::addMapping_modifierNames_mapsBothSides()
{
  KeyRemap keyRemap;
  QVERIFY(keyRemap.addMapping("ctrl", "super"));

  QCOMPARE(keyRemap.remapKey(kKeyControl_L), kKeySuper_L);
  QCOMPARE(keyRemap.remapKey(kKeyControl_R), kKeySuper_R);
  QCOMPARE(keyRemap.remapMask(KeyModifierControl), KeyModifierSuper);
}

void KeyRemapTests::remapKey_unmappedKey_returnsKey()
{
  KeyRemap keyRemap;
  keyRemap.addMapping(kKeyControl_L, kKeySuper_L);

  QCOMPARE(keyRemap.remapKey(kKeyCapsLock), kKeyCapsLock);
}

void KeyRemapTests::remapKey_mappedKey_returnsMappedKey()
{
  KeyRemap keyRemap;
  keyRemap.addMapping(kKeyCapsLock, kKeyEscape);

  QCOMPARE(keyRemap.remapKey(kKeyCapsLock), kKeyEscape);
}

void KeyRemapTests::remapMask_noMappings_keepsMask()
{
  KeyRemap keyRemap;

  QCOMPARE(keyRemap.remapMask(KeyModifierControl | KeyModifierShift), KeyModifierControl | KeyModifierShift);
}

void KeyRemapTests::remapMask_modifierToModifier_mapsBit()
{
  KeyRemap keyRemap;
  keyRemap.addMapping(kKeyControl_L, kKeySuper_L);

  QCOMPARE(keyRemap.remapMask(KeyModifierControl), KeyModifierSuper);
  QCOMPARE(keyRemap.remapMask(KeyModifierControl | KeyModifierShift), KeyModifierSuper | KeyModifierShift);
  QCOMPARE(keyRemap.remapMask(0), static_cast<KeyModifierMask>(0));
}

void KeyRemapTests::remapMask_modifierToKey_clearsBit()
{
  KeyRemap keyRemap;
  keyRemap.addMapping(kKeyControl_L, kKeyEscape);

  QCOMPARE(keyRemap.remapMask(KeyModifierControl), static_cast<KeyModifierMask>(0));
}

void KeyRemapTests::remapMask_swappedModifiers_swapsBits()
{
  KeyRemap keyRemap;
  keyRemap.addMapping(kKeyControl_L, kKeySuper_L);
  keyRemap.addMapping(kKeySuper_L, kKeyControl_L);

  QCOMPARE(keyRemap.remapMask(KeyModifierControl), KeyModifierSuper);
  QCOMPARE(keyRemap.remapMask(KeyModifierSuper), KeyModifierControl);
  QCOMPARE(keyRemap.remapMask(KeyModifierControl | KeyModifierSuper), KeyModifierControl | KeyModifierSuper);
}

void KeyRemapTests::remapMask_rightHandOnly_mapsBit()
{
  KeyRemap keyRemap;
  keyRemap.addMapping(kKeyControl_R, kKeySuper_R);

  QCOMPARE(keyRemap.remapMask(KeyModifierControl), KeyModifierSuper);
}

void KeyRemapTests::remapMask_lockBits_areNotChanged()
{
  KeyRemap keyRemap;
  keyRemap.addMapping(kKeyCapsLock, kKeyEscape);

  QCOMPARE(keyRemap.remapMask(KeyModifierCapsLock | KeyModifierNumLock), KeyModifierCapsLock | KeyModifierNumLock);
}

void KeyRemapTests::parseKey_keyName_returnsKey()
{
  KeyID key = kKeyNone;

  QVERIFY(KeyRemap::parseKey("Control_L", key));
  QCOMPARE(key, kKeyControl_L);

  QVERIFY(KeyRemap::parseKey("Escape", key));
  QCOMPARE(key, kKeyEscape);
}

void KeyRemapTests::parseKey_modifierName_returnsLeftKey()
{
  KeyID key = kKeyNone;

  QVERIFY(KeyRemap::parseKey("ctrl", key));
  QCOMPARE(key, kKeyControl_L);

  QVERIFY(KeyRemap::parseKey("Super", key));
  QCOMPARE(key, kKeySuper_L);
}

void KeyRemapTests::parseKey_unknownName_returnsFalse()
{
  KeyID key = kKeyNone;

  QVERIFY(!KeyRemap::parseKey("NotAKey", key));
  QCOMPARE(key, kKeyNone);
}

QTEST_MAIN(KeyRemapTests)
