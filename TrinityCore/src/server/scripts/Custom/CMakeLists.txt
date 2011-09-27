# Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  Custom/dynamic_teleporter.cpp
  Custom/custom_npc_doctor.cpp
  Custom/npc_petmaster.cpp
  Custom/npc_quelthalas.cpp
  Custom/OutdoorPvPAzshara.h
  Custom/OutdoorPvPAzshara.cpp
  Custom/AzsharaPvP.cpp
  Custom/AzsharaPvP.h
  Custom/AzsharaPvP_Caster.cpp
  Custom/AzsharaPvP_Ranged.cpp
  Custom/AzsharaPvP_Melee.cpp
)

message("  -> Prepared: Custom")
