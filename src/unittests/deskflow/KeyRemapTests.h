/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */
#include "base/Log.h"

#include <QTest>

namespace deskflow {
class KeyRemapTests : public QObject
{
  Q_OBJECT
private Q_SLOTS:
  void addMapping_unknownKey_returnsFalse();
  void addMapping_identity_removesMapping();
  void addMapping_modifierNames_mapsBothSides();
  void remapKey_unmappedKey_returnsKey();
  void remapKey_mappedKey_returnsMappedKey();
  void remapMask_noMappings_keepsMask();
  void remapMask_modifierToModifier_mapsBit();
  void remapMask_modifierToKey_clearsBit();
  void remapMask_swappedModifiers_swapsBits();
  void remapMask_rightHandOnly_mapsBit();
  void remapMask_lockBits_areNotChanged();
  void parseKey_keyName_returnsKey();
  void parseKey_modifierName_returnsLeftKey();
  void parseKey_unknownName_returnsFalse();

private:
  Log m_log;
};
} // namespace deskflow
