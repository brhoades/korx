/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2006 Tim Angus

This file is part of Tremfusion.

Tremfusion is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremfusion is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremfusion; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "g_local.h"

level_locals_t  level;

typedef struct
{
  vmCvar_t  *vmCvar;
  char    *cvarName;
  char    *defaultString;
  int     cvarFlags;
  int     modificationCount;  // for tracking changes
  qboolean  trackChange;  // track this variable, and announce if changed
} cvarTable_t;

gentity_t   g_entities[ MAX_GENTITIES ];
gclient_t   g_clients[ MAX_CLIENTS ];

vmCvar_t  g_timelimit;
vmCvar_t  g_suddenDeathTime;
vmCvar_t  g_suddenDeath;
vmCvar_t  g_suddenDeathMode;
vmCvar_t  g_suddenDeathVotePercent;
vmCvar_t  g_suddenDeathVoteDelay;
vmCvar_t  g_extremeSuddenDeathTime;
vmCvar_t  g_extremeSuddenDeath;
vmCvar_t  g_extremeSuddenDeathVotePercent;
vmCvar_t  g_extremeSuddenDeathVoteMinTime;
vmCvar_t  g_extremeSuddenDeathVoteDelay;
vmCvar_t  g_extremeSuddenDeathVote;
vmCvar_t  g_smartesd;

vmCvar_t  g_friendlyFire;
vmCvar_t  g_friendlyFireAliens;
vmCvar_t  g_friendlyFireHumans;
vmCvar_t  g_friendlyBuildableFire;
vmCvar_t  g_friendlyFireMovementAttacks;
vmCvar_t  g_retribution;
vmCvar_t  g_password;
vmCvar_t  g_needpass;
vmCvar_t  g_maxclients;
vmCvar_t  g_maxGameClients;
vmCvar_t  g_dedicated;
vmCvar_t  g_speed;
vmCvar_t  g_gravity;
vmCvar_t  g_cheats;
vmCvar_t  g_demoState;
vmCvar_t  g_knockback;
vmCvar_t  g_inactivity;
vmCvar_t  g_debugMove;
vmCvar_t  g_debugDamage;
vmCvar_t  g_weaponRespawn;
vmCvar_t  g_weaponTeamRespawn;
vmCvar_t  g_motd;
vmCvar_t  g_synchronousClients;
vmCvar_t  g_warmup;
vmCvar_t  g_doWarmup;
vmCvar_t  g_warmupMode;
vmCvar_t  g_restarted;
vmCvar_t  g_lockTeamsAtStart;
vmCvar_t  g_logFile;
vmCvar_t  g_logFileSync;
vmCvar_t  g_allowVote;
vmCvar_t  g_requireVoteReasons;
vmCvar_t  g_voteLimit;
vmCvar_t  g_mapVotesPercent;
vmCvar_t  g_designateVotes;
vmCvar_t  g_teamAutoJoin;
vmCvar_t  g_teamForceBalance;
vmCvar_t  g_smoothClients;
vmCvar_t  pmove_fixed;
vmCvar_t  pmove_msec;
vmCvar_t  g_listEntity;
vmCvar_t  g_minCommandPeriod;
vmCvar_t  g_minNameChangePeriod;
vmCvar_t  g_maxNameChanges;
vmCvar_t  g_newbieNumbering;
vmCvar_t  g_newbieNamePrefix;

vmCvar_t  g_humanBuildPoints;
vmCvar_t  g_alienBuildPoints;
vmCvar_t  g_humanBuildQueueTime;
vmCvar_t  g_alienBuildQueueTime;
vmCvar_t  g_humanStage;
vmCvar_t  g_humanCredits;
vmCvar_t  g_humanMaxStage;
vmCvar_t  g_humanStage2Threshold;
vmCvar_t  g_humanStage3Threshold;
vmCvar_t  g_alienStage;
vmCvar_t  g_alienCredits;
vmCvar_t  g_alienMaxStage;
vmCvar_t  g_alienStage2Threshold;
vmCvar_t  g_alienStage3Threshold;

vmCvar_t  g_teamImbalanceWarnings;

vmCvar_t  g_unlagged;

vmCvar_t  g_disabledEquipment;
vmCvar_t  g_disabledClasses;
vmCvar_t  g_disabledBuildables;

vmCvar_t  g_markDeconstruct;
vmCvar_t  g_deconDead;

vmCvar_t  g_debugMapRotation;
vmCvar_t  g_currentMapRotation;
vmCvar_t  g_currentMap;
vmCvar_t  g_nextMap;
vmCvar_t  g_initialMapRotation;

vmCvar_t  g_debugVoices;
vmCvar_t  g_voiceChats;

vmCvar_t  g_shove;

vmCvar_t  g_mapConfigs;
vmCvar_t  g_sayAreaRange;

vmCvar_t  g_floodMaxDemerits;
vmCvar_t  g_floodMinTime;

vmCvar_t  g_layouts;
vmCvar_t  g_layoutAuto;

vmCvar_t  g_emoticonsAllowedInNames;
vmCvar_t  g_allowBlackInNames;

vmCvar_t  g_admin;
vmCvar_t  g_adminLog;
vmCvar_t  g_adminParseSay;
vmCvar_t  g_adminSayFilter;
vmCvar_t  g_adminNameProtect;
vmCvar_t  g_adminTempBan;
vmCvar_t  g_adminMaxBan;
vmCvar_t  g_adminMapLog;
vmCvar_t  g_minLevelToJoinTeam;
vmCvar_t  g_forceAutoSelect;

vmCvar_t  g_dretchPunt;

vmCvar_t  g_killDamage;
vmCvar_t  g_slapKnockback;
vmCvar_t  g_slapDamage;

vmCvar_t  g_privateMessages;
vmCvar_t  g_specChat;
vmCvar_t  g_publicAdminMessages;
vmCvar_t  g_publicClanMessages;

vmCvar_t  g_minLevelToSpecMM1;
vmCvar_t  g_actionPrefix;
vmCvar_t  g_myStats;

vmCvar_t  g_buildLogMaxLength;

vmCvar_t  g_tag;

vmCvar_t  g_reportFile;
vmCvar_t  g_reportLimit;

vmCvar_t  g_allowShare;

vmCvar_t  g_devmapKillerHP;
vmCvar_t  g_KillerHP;
vmCvar_t  g_antiSpawnBlock;

vmCvar_t  g_clientUpgradeNotice;

vmCvar_t  g_devmapNoGod;
vmCvar_t  g_devmapNoStructDmg;

vmCvar_t  g_voteMinTime;
vmCvar_t  g_mapvoteMaxTime;

vmCvar_t  g_extendvote;
vmCvar_t  g_extendvotetime;
vmCvar_t  g_extendvotepercent;

vmCvar_t  g_specmetimeout;

vmCvar_t  g_decolourLogfiles;

vmCvar_t  g_banNotice;

vmCvar_t  g_msg;
vmCvar_t  g_msgTime;

vmCvar_t  g_tkmap;
vmCvar_t  g_nodretchtogranger;

static cvarTable_t   gameCvarTable[ ] =
{
  // don't override the cheat state set by the system
  { &g_cheats, "sv_cheats", "", 0, 0, qfalse },

  // demo state
  { &g_demoState, "sv_demoState", "", 0, 0, qfalse },

  // noset vars
  { NULL, "gamename", GAME_VERSION , CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
  { NULL, "gamedate", __DATE__ , CVAR_ROM, 0, qfalse  },
  { &g_restarted, "g_restarted", "0", CVAR_ROM, 0, qfalse  },
  { &g_lockTeamsAtStart, "g_lockTeamsAtStart", "0", CVAR_ROM, 0, qfalse  },
  { NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
  { NULL, "P", "", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },
  { NULL, "ff", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse  },

  // latched vars

  { &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0, qfalse  },

  // change anytime vars
  { &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse  },

  { &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
  { &g_suddenDeath, "g_suddenDeath", "0", CVAR_SERVERINFO | CVAR_NORESTART, 0, qtrue },
  { &g_suddenDeathTime, "g_suddenDeathTime", "30", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
  { &g_suddenDeathMode, "g_suddenDeathMode", "2", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
  { &g_suddenDeathVotePercent, "g_suddenDeathVotePercent", "75", CVAR_ARCHIVE, 0, qfalse },
  { &g_suddenDeathVoteDelay, "g_suddenDeathVoteDelay", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_extremeSuddenDeath, "g_extremeSuddenDeath", "0", CVAR_SERVERINFO, 0, qfalse },
  { &g_extremeSuddenDeathTime, "g_extremeSuddenDeathTime", "0", CVAR_ARCHIVE | CVAR_NORESTART, 0, qtrue },
  { &g_extremeSuddenDeathVotePercent, "g_extremeSuddenDeathVotePercent", "75", CVAR_ARCHIVE, 0, qfalse },
  { &g_extremeSuddenDeathVoteMinTime, "g_extremeSuddenDeathVoteMinTime", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_extremeSuddenDeathVoteDelay, "g_extremeSuddenDeathVoteDelay", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_extremeSuddenDeathVote, "g_extremeSuddenDeathVote", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_smartesd, "g_smartesd", "1", CVAR_ARCHIVE, 0, qfalse },
  { &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0, qfalse  },

  { &g_friendlyFire, "g_friendlyFire", "1", CVAR_ARCHIVE, 0, qtrue  },
  { &g_friendlyFireAliens, "g_friendlyFireAliens", "0", CVAR_ARCHIVE, 0, qtrue  },
  { &g_friendlyFireHumans, "g_friendlyFireHumans", "0", CVAR_ARCHIVE, 0, qtrue  },
  { &g_friendlyBuildableFire, "g_friendlyBuildableFire", "0", CVAR_ARCHIVE, 0, qtrue  },
  { &g_friendlyFireMovementAttacks, "g_friendlyFireMovementAttacks", "0", CVAR_ARCHIVE, 0, qtrue  },
  { &g_retribution, "g_retribution", "1", CVAR_ARCHIVE, 0, qtrue  },

  { &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE  },
  { &g_teamForceBalance, "g_teamForceBalance", "1", CVAR_ARCHIVE  },

  { &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0, qtrue  },
  { &g_warmupMode, "g_warmupMode", "1", CVAR_ARCHIVE, 0, qtrue  },
  { &g_doWarmup, "g_doWarmup", "0", 0, 0, qtrue  },
  { &g_logFile, "g_logFile", "games.log", CVAR_ARCHIVE, 0, qfalse  },
  { &g_logFileSync, "g_logFileSync", "0", CVAR_ARCHIVE, 0, qfalse  },

  { &g_password, "g_password", "", CVAR_USERINFO, 0, qfalse  },

  { &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0, qfalse },

  { &g_dedicated, "dedicated", "0", 0, 0, qfalse  },

  { &g_speed, "g_speed", "320", 0, 0, qtrue  },
  { &g_gravity, "g_gravity", "667", 0, 0, qtrue  },
  { &g_knockback, "g_knockback", "1000", 0, 0, qtrue  },
  { &g_weaponRespawn, "g_weaponrespawn", "5", 0, 0, qtrue  },
  { &g_weaponTeamRespawn, "g_weaponTeamRespawn", "30", 0, 0, qtrue },
  { &g_inactivity, "g_inactivity", "0", 0, 0, qtrue },
  { &g_debugMove, "g_debugMove", "0", 0, 0, qfalse },
  { &g_debugDamage, "g_debugDamage", "0", 0, 0, qfalse },
  { &g_motd, "g_motd", "", 0, 0, qfalse },

  { &g_allowVote, "g_allowVote", "1", CVAR_ARCHIVE, 0, qfalse },
  { &g_requireVoteReasons, "g_requireVoteReasons", "1", CVAR_ARCHIVE, 0, qfalse },
  { &g_voteLimit, "g_voteLimit", "5", CVAR_ARCHIVE, 0, qfalse },
  { &g_mapVotesPercent, "g_mapVotesPercent", "50", CVAR_ARCHIVE, 0, qfalse },
  { &g_voteMinTime, "g_voteMinTime", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_mapvoteMaxTime, "g_mapvoteMaxTime", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_extendvote, "g_extendvote", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_extendvotetime, "g_extendvotetime", "5", CVAR_ARCHIVE, 0, qfalse },
  { &g_extendvotepercent, "g_extendvotepercent", "65", CVAR_ARCHIVE, 0, qfalse },
  { &g_specmetimeout, "g_specmetimeout", "1", 0, 0, qfalse },
  { &g_designateVotes, "g_designateVotes", "1", CVAR_ARCHIVE, 0, qfalse },
  { &g_listEntity, "g_listEntity", "0", 0, 0, qfalse },
  { &g_minCommandPeriod, "g_minCommandPeriod", "500", 0, 0, qfalse},
  { &g_minNameChangePeriod, "g_minNameChangePeriod", "5", 0, 0, qfalse},
  { &g_maxNameChanges, "g_maxNameChanges", "5", 0, 0, qfalse},
  { &g_newbieNumbering, "g_newbieNumbering", "1", CVAR_ARCHIVE, 0, qfalse},
  { &g_newbieNamePrefix, "g_newbieNamePrefix", "^5N^3e^6w^4b^1i^5e^2#^6", CVAR_ARCHIVE, 0, qfalse},

  { &g_smoothClients, "g_smoothClients", "1", 0, 0, qfalse},
  { &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0, qfalse},
  { &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0, qfalse},

  { &g_humanBuildPoints, "g_humanBuildPoints", DEFAULT_HUMAN_BUILDPOINTS, 0, 0, qfalse  },
  { &g_alienBuildPoints, "g_alienBuildPoints", DEFAULT_ALIEN_BUILDPOINTS, 0, 0, qfalse  },
  { &g_humanBuildQueueTime, "g_humanBuildQueueTime", DEFAULT_HUMAN_QUEUE_TIME, 0, 0, qfalse  },
  { &g_alienBuildQueueTime, "g_alienBuildQueueTime", DEFAULT_ALIEN_QUEUE_TIME, 0, 0, qfalse  },
  { &g_humanStage, "g_humanStage", "0", 0, 0, qfalse  },
  { &g_humanCredits, "g_humanCredits", "0", 0, 0, qfalse  },
  { &g_humanMaxStage, "g_humanMaxStage", DEFAULT_HUMAN_MAX_STAGE, 0, 0, qfalse  },
  { &g_humanStage2Threshold, "g_humanStage2Threshold", DEFAULT_HUMAN_STAGE2_THRESH, 0, 0, qfalse  },
  { &g_humanStage3Threshold, "g_humanStage3Threshold", DEFAULT_HUMAN_STAGE3_THRESH, 0, 0, qfalse  },
  { &g_alienStage, "g_alienStage", "0", 0, 0, qfalse  },
  { &g_alienCredits, "g_alienCredits", "0", 0, 0, qfalse  },
  { &g_alienMaxStage, "g_alienMaxStage", DEFAULT_ALIEN_MAX_STAGE, 0, 0, qfalse  },
  { &g_alienStage2Threshold, "g_alienStage2Threshold", DEFAULT_ALIEN_STAGE2_THRESH, 0, 0, qfalse  },
  { &g_alienStage3Threshold, "g_alienStage3Threshold", DEFAULT_ALIEN_STAGE3_THRESH, 0, 0, qfalse  },
  { &g_teamImbalanceWarnings, "g_teamImbalanceWarnings", "30", CVAR_ARCHIVE, 0, qfalse  },

  { &g_unlagged, "g_unlagged", "1", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse  },

  { &g_disabledEquipment, "g_disabledEquipment", "", CVAR_ROM, 0, qfalse  },
  { &g_disabledClasses, "g_disabledClasses", "", CVAR_ROM, 0, qfalse  },
  { &g_disabledBuildables, "g_disabledBuildables", "", CVAR_ROM, 0, qfalse  },

  { &g_sayAreaRange, "g_sayAreaRange", "1000.0", CVAR_ARCHIVE, 0, qtrue  },

  { &g_floodMaxDemerits, "g_floodMaxDemerits", "5000", CVAR_ARCHIVE, 0, qfalse  },
  { &g_floodMinTime, "g_floodMinTime", "3000", CVAR_ARCHIVE, 0, qfalse  },

  { &g_markDeconstruct, "g_markDeconstruct", "1", CVAR_SERVERINFO | CVAR_ARCHIVE, 0, qfalse  },
  { &g_deconDead, "g_deconDead", "0", CVAR_ARCHIVE, 0, qtrue  },

  { &g_debugMapRotation, "g_debugMapRotation", "0", 0, 0, qfalse  },
  { &g_currentMapRotation, "g_currentMapRotation", "-1", 0, 0, qfalse  }, // -1 = NOT_ROTATING
  { &g_currentMap, "g_currentMap", "0", 0, 0, qfalse  },
  { &g_nextMap, "g_nextMap", "", 0 , 0, qtrue  },
  { &g_initialMapRotation, "g_initialMapRotation", "", CVAR_ARCHIVE, 0, qfalse  },
  { &g_debugVoices, "g_debugVoices", "0", 0, 0, qfalse  },
  { &g_voiceChats, "g_voiceChats", "1", CVAR_ARCHIVE, 0, qfalse },
  { &g_shove, "g_shove", "0.0", CVAR_ARCHIVE, 0, qfalse  },
  { &g_mapConfigs, "g_mapConfigs", "", CVAR_ARCHIVE, 0, qfalse  },
  { NULL, "g_mapConfigsLoaded", "0", CVAR_ROM, 0, qfalse  },

  { &g_layouts, "g_layouts", "", CVAR_LATCH, 0, qfalse  },
  { &g_layoutAuto, "g_layoutAuto", "1", CVAR_ARCHIVE, 0, qfalse  },
  
  { &g_emoticonsAllowedInNames, "g_emoticonsAllowedInNames", "1", CVAR_LATCH|CVAR_ARCHIVE, 0, qfalse  },
  { &g_allowBlackInNames, "g_allowBlackInNames", "1", CVAR_LATCH|CVAR_ARCHIVE, 0, qfalse  },
  
  { &g_admin, "g_admin", "admin.dat", CVAR_ARCHIVE, 0, qfalse  },
  { &g_adminLog, "g_adminLog", "admin.log", CVAR_ARCHIVE, 0, qfalse  },
  { &g_adminParseSay, "g_adminParseSay", "1", CVAR_ARCHIVE, 0, qfalse  },
  { &g_adminSayFilter, "g_adminSayFilter", "0", CVAR_ARCHIVE, 0, qfalse  },
  { &g_adminNameProtect, "g_adminNameProtect", "1", CVAR_ARCHIVE, 0, qfalse  },
  { &g_adminTempBan, "g_adminTempBan", "2m", CVAR_ARCHIVE, 0, qfalse  },
  { &g_adminMaxBan, "g_adminMaxBan", "2w", CVAR_ARCHIVE, 0, qfalse  },
  { &g_adminMapLog, "g_adminMapLog", "", CVAR_ROM, 0, qfalse  },
  { &g_minLevelToJoinTeam, "g_minLevelToJoinTeam", "-10", CVAR_ARCHIVE, 0, qfalse  },
  { &g_forceAutoSelect, "g_forceAutoSelect", "0", CVAR_ARCHIVE, 0, qtrue },

  { &g_dretchPunt, "g_dretchPunt", "0", CVAR_ARCHIVE, 0, qfalse  },

  { &g_killDamage, "g_killDamage", "600", CVAR_ARCHIVE, 0, qfalse},
  { &g_slapKnockback, "g_slapKnockback", "200", CVAR_ARCHIVE, 0, qfalse},
  { &g_slapDamage, "g_slapDamage", "5", CVAR_ARCHIVE, 0, qfalse},

  { &g_privateMessages, "g_privateMessages", "1", CVAR_ARCHIVE, 0, qfalse  },
  { &g_specChat, "g_specChat", "1", CVAR_ARCHIVE, 0, qfalse  },
  { &g_publicAdminMessages, "g_publicAdminMessages", "1", CVAR_ARCHIVE, 0, qfalse  },
  { &g_publicClanMessages, "g_publicClanMessages", "1", CVAR_ARCHIVE, 0, qfalse  },
  
  { &g_minLevelToSpecMM1, "g_minLevelToSpecMM1", "-10", CVAR_ARCHIVE, 0, qfalse  },
  { &g_actionPrefix, "g_actionPrefix", "* ", CVAR_ARCHIVE, 0, qfalse },
  { &g_myStats, "g_myStats", "1", CVAR_ARCHIVE, 0, qfalse  },
  { &g_buildLogMaxLength, "g_buildLogMaxLength", "25", CVAR_ARCHIVE, 0, qfalse  },

  { &g_reportFile, "g_reportFile", "reports.txt", CVAR_ARCHIVE, 0, qfalse  },
  { &g_reportLimit, "g_reportLimit", "1", CVAR_ARCHIVE, 0, qfalse  },

  { &g_allowShare, "g_allowShare", "0", CVAR_ARCHIVE | CVAR_SERVERINFO, 0, qfalse},

  { &g_devmapKillerHP, "g_devmapKillerHP", "0", CVAR_ARCHIVE, 0, qtrue  },
  { &g_KillerHP, "g_KillerHP", "0", CVAR_ARCHIVE, 0, qtrue  },
  { &g_antiSpawnBlock, "g_antiSpawnBlock", "150", CVAR_ARCHIVE, 0, qfalse  },

  { &g_clientUpgradeNotice, "g_clientUpgradeNotice", "1", 0, 0, qfalse},

  { &g_devmapNoGod, "g_devmapNoGod", "0", CVAR_ARCHIVE, 0, qtrue  },
  { &g_devmapNoStructDmg, "g_devmapNoStructDmg", "0", CVAR_ARCHIVE, 0, qtrue  },

  { &g_msg, "g_msg", "", CVAR_ARCHIVE, 0, qfalse  },
  { &g_msgTime, "g_msgTime", "0", CVAR_ARCHIVE, 0, qfalse  },

  { &g_decolourLogfiles, "g_decolourLogfiles", "0", CVAR_ARCHIVE, 0, qfalse  },

  { &g_banNotice, "g_banNotice", "", CVAR_ARCHIVE, 0, qfalse  },

  { &g_tag, "g_tag", "main", CVAR_INIT, 0, qfalse },
  { &g_tkmap, "g_tkmap", "0", CVAR_ARCHIVE, 0, qfalse },
  { &g_nodretchtogranger, "g_nodretchtogranger", "0", CVAR_ARCHIVE, 0, qfalse }
  
};

static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[ 0 ] );


void G_InitGame( int levelTime, int randomSeed, int restart );
void G_RunFrame( int levelTime );
void G_ShutdownGame( int restart );
void CheckExitRules( void );
void G_DemoSetClient( void );
void G_DemoRemoveClient( void );
void G_DemoSetStage( void );

void G_CountSpawns( void );
void G_CalculateBuildPoints( void );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4,
                              int arg5, int arg6, int arg7, int arg8, int arg9,
                              int arg10, int arg11 )
{
  switch( command )
  {
    case GAME_INIT:
      G_InitGame( arg0, arg1, arg2 );
      return 0;

    case GAME_SHUTDOWN:
      G_ShutdownGame( arg0 );
      return 0;

    case GAME_CLIENT_CONNECT:
      return (intptr_t)ClientConnect( arg0, arg1 );

    case GAME_CLIENT_THINK:
      ClientThink( arg0 );
      return 0;

    case GAME_CLIENT_USERINFO_CHANGED:
      ClientUserinfoChanged( arg0 );
      return 0;

    case GAME_CLIENT_DISCONNECT:
      ClientDisconnect( arg0 );
      return 0;

    case GAME_CLIENT_BEGIN:
      ClientBegin( arg0 );
      return 0;

    case GAME_CLIENT_COMMAND:
      ClientCommand( arg0 );
      return 0;

    case GAME_RUN_FRAME:
      G_RunFrame( arg0 );
      return 0;

    case GAME_CONSOLE_COMMAND:
      return ConsoleCommand( );

    case GAME_DEMO_COMMAND:
      switch ( arg0 )
      {
      case DC_CLIENT_SET:
        G_DemoSetClient( );
        break;
      case DC_CLIENT_REMOVE:
        G_DemoRemoveClient( );
        break;
      case DC_SET_STAGE:
        G_DemoSetStage( );
        break;
      }

    case GAME_PINGPRIV_OVERRIDE:
			return ClientPingPrivOverride( );
  }

  return -1;
}


void QDECL G_Printf( const char *fmt, ... )
{
  va_list argptr;
  char    text[ 1024 ];

  va_start( argptr, fmt );
  Q_vsnprintf( text, sizeof( text ), fmt, argptr );
  va_end( argptr );

  trap_Print( text );
}

void QDECL G_Error( const char *fmt, ... )
{
  va_list argptr;
  char    text[ 1024 ];

  va_start( argptr, fmt );
  Q_vsnprintf( text, sizeof( text ), fmt, argptr );
  va_end( argptr );

  trap_Error( text );
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void )
{
  gentity_t *e, *e2;
  int       i, j;
  int       c, c2;

  c = 0;
  c2 = 0;

  for( i = 1, e = g_entities+i; i < level.num_entities; i++, e++ )
  {
    if( !e->inuse )
      continue;

    if( !e->team )
      continue;

    if( e->flags & FL_TEAMSLAVE )
      continue;

    e->teammaster = e;
    c++;
    c2++;

    for( j = i + 1, e2 = e + 1; j < level.num_entities; j++, e2++ )
    {
      if( !e2->inuse )
        continue;

      if( !e2->team )
        continue;

      if( e2->flags & FL_TEAMSLAVE )
        continue;

      if( !strcmp( e->team, e2->team ) )
      {
        c2++;
        e2->teamchain = e->teamchain;
        e->teamchain = e2;
        e2->teammaster = e;
        e2->flags |= FL_TEAMSLAVE;

        // make sure that targets only point at the master
        if( e2->targetname )
        {
          e->targetname = e2->targetname;
          e2->targetname = NULL;
        }
      }
    }
  }

  G_Printf( "%i teams with %i entities\n", c, c2 );
}


/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void )
{
  int         i;
  cvarTable_t *cv;

  for( i = 0, cv = gameCvarTable; i < gameCvarTableSize; i++, cv++ )
  {
    trap_Cvar_Register( cv->vmCvar, cv->cvarName,
      cv->defaultString, cv->cvarFlags );

    if( cv->vmCvar )
      cv->modificationCount = cv->vmCvar->modificationCount;
  }

  // check some things
  level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
void G_UpdateCvars( void )
{
  int         i;
  cvarTable_t *cv;

  for( i = 0, cv = gameCvarTable; i < gameCvarTableSize; i++, cv++ )
  {
    if( cv->vmCvar )
    {
      trap_Cvar_Update( cv->vmCvar );

      if( cv->modificationCount != cv->vmCvar->modificationCount )
      {
        cv->modificationCount = cv->vmCvar->modificationCount;

        if( cv->trackChange )
        {
          trap_SendServerCommand( -1, va( "print \"Server: %s changed to %s\n\"",
            cv->cvarName, cv->vmCvar->string ) );
          // update serverinfo in case this cvar is passed to clients indirectly
          CalculateRanks( );
        }
      }
    }
  }
}

/*
=================
G_MapConfigs
=================
*/
void G_MapConfigs( const char *mapname )
{

  if( !g_mapConfigs.string[0] )
    return;

  if( trap_Cvar_VariableIntegerValue( "g_mapConfigsLoaded" ) )
    return;

  trap_SendConsoleCommand( EXEC_APPEND,
    va( "exec \"%s/default.cfg\"\n", g_mapConfigs.string ) );

  trap_SendConsoleCommand( EXEC_APPEND,
    va( "exec \"%s/%s.cfg\"\n", g_mapConfigs.string, mapname ) );

  trap_Cvar_Set( "g_mapConfigsLoaded", "1" );
}

/*
============
G_InitGame

============
*/
void G_InitGame( int levelTime, int randomSeed, int restart )
{
  int i;
  char buffer[ MAX_CVAR_VALUE_STRING ];
  int a, b;

  srand( randomSeed );

  G_RegisterCvars( );

  G_Printf( "------- Game Initialization -------\n" );
  G_Printf( "gamename: %s\n", GAME_VERSION );
  G_Printf( "gamedate: %s\n", __DATE__ );

  BG_InitMemory( );

  // set some level globals
  memset( &level, 0, sizeof( level ) );
  level.time = levelTime;
  level.startTime = levelTime;
  level.alienStage2Time = level.alienStage3Time =
    level.humanStage2Time = level.humanStage3Time = level.startTime;

  trap_Cvar_VariableStringBuffer( "session", buffer, sizeof( buffer ) );
  sscanf( buffer, "%i %i", &a, &b );
  if ( a != trap_Cvar_VariableIntegerValue( "sv_maxclients" ) ||
       b != trap_Cvar_VariableIntegerValue( "sv_democlients" ) )
    level.newSession = qtrue;

  level.snd_fry = G_SoundIndex( "sound/misc/fry.wav" ); // FIXME standing in lava / slime

  if( g_logFile.string[ 0 ] )
  {
    if( g_logFileSync.integer )
      trap_FS_FOpenFile( g_logFile.string, &level.logFile, FS_APPEND_SYNC );
    else
      trap_FS_FOpenFile( g_logFile.string, &level.logFile, FS_APPEND );

    if( !level.logFile )
      G_Printf( "WARNING: Couldn't open logfile: %s\n", g_logFile.string );
    else
    {
      char serverinfo[ MAX_INFO_STRING ];
      qtime_t qt;
      int t;

      trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

      G_LogPrintf( "------------------------------------------------------------\n" );
      G_LogPrintf( "InitGame: %s\n", serverinfo );

      t = trap_RealTime( &qt );
      G_LogPrintf("RealTime: %04i/%02i/%02i %02i:%02i:%02i\n",
            qt.tm_year+1900, qt.tm_mon+1, qt.tm_mday, 
            qt.tm_hour, qt.tm_min, qt.tm_sec );

    }
  }
  else
    G_Printf( "Not logging to disk\n" );

  {
    char map[ MAX_CVAR_VALUE_STRING ] = {""};

    trap_Cvar_VariableStringBuffer( "mapname", map, sizeof( map ) );
    G_MapConfigs( map );
  }

  // we're done with g_mapConfigs, so reset this for the next map
  trap_Cvar_Set( "g_mapConfigsLoaded", "0" );

  G_admin_readconfig( NULL, 0 );

  // initialize all entities for this game
  memset( g_entities, 0, MAX_GENTITIES * sizeof( g_entities[ 0 ] ) );
  level.gentities = g_entities;

  // initialize all clients for this game
  level.maxclients = g_maxclients.integer;
  memset( g_clients, 0, MAX_CLIENTS * sizeof( g_clients[ 0 ] ) );
  level.clients = g_clients;

  // set client fields on player ents
  for( i = 0; i < level.maxclients; i++ )
    g_entities[ i ].client = level.clients + i;

  // always leave room for the max number of clients,
  // even if they aren't all used, so numbers inside that
  // range are NEVER anything but clients
  level.num_entities = MAX_CLIENTS;

  // let the server system know where the entites are
  trap_LocateGameData( level.gentities, level.num_entities, sizeof( gentity_t ),
    &level.clients[ 0 ].ps, sizeof( level.clients[ 0 ] ) );

  level.emoticonCount = BG_LoadEmoticons( level.emoticons, NULL );

  trap_SetConfigstring( CS_INTERMISSION, "0" );

  // update maplog
  G_admin_maplog_update( );

  // test to see if a custom buildable layout will be loaded
  G_LayoutSelect( );

  // parse the key/value pairs and spawn gentities
  G_SpawnEntitiesFromString( );

  // load up a custom building layout if there is one
  G_LayoutLoad( );

  // the map might disable some things
  BG_InitAllowedGameElements( );

  // general initialization
  G_FindTeams( );

  BG_InitClassConfigs( );
  BG_InitBuildableConfigs( );
  G_InitDamageLocations( );
  G_InitMapRotations( );
  G_InitSpawnQueue( &level.alienSpawnQueue );
  G_InitSpawnQueue( &level.humanSpawnQueue );

  if( g_debugMapRotation.integer )
    G_PrintRotations( );

  level.voices = BG_VoiceInit( );
  BG_PrintVoices( level.voices, g_debugVoices.integer );

  //reset stages
  trap_Cvar_Set( "g_alienStage", va( "%d", S1 ) );
  trap_Cvar_Set( "g_humanStage", va( "%d", S1 ) );
  trap_Cvar_Set( "g_alienCredits", 0 );
  trap_Cvar_Set( "g_humanCredits", 0 );
  trap_Cvar_Set( "g_suddenDeath", 0 );

  //reset sd/esd settings
  trap_Cvar_Set( "g_suddenDeath", "0" );
  trap_Cvar_Set( "g_extremeSuddenDeath", "0" );
  trap_Cvar_Set( "g_extremeSuddenDeathVote", "0" );
  
  //reset nextmap
  trap_Cvar_Set( "g_nextMap", "" );
  
  //reset extendvote, just in case
  trap_Cvar_Set( "g_extendvote", "0" );
  trap_Cvar_Set( "g_tkmap", "0" );
  level.suddenDeath = qfalse;
  level.extremeSuddenDeath = qfalse;
  level.suddenDeathVote = qfalse;
  level.extremeSuddenDeathVote = qfalse;

  //this allows for the vote delay to be precise
  level.suddenDeathTime = g_suddenDeathTime.integer*60000;
  level.extremeSuddenDeathTime = g_extremeSuddenDeathTime.integer*60000;

  G_Printf( "-----------------------------------\n" );

  // so the server counts the spawns without a client attached
  G_CountSpawns( );

  G_ResetPTRConnections( );

  if(g_lockTeamsAtStart.integer)
  {
    level.alienTeamLocked=qtrue;
    level.humanTeamLocked=qtrue;
    trap_Cvar_Set( "g_lockTeamsAtStart", "0" );
  }
}

/*
==================
G_ClearVotes

remove all currently active votes
==================
*/
static void G_ClearVotes( void )
{
  level.voteTime = 0;
  trap_SetConfigstring( CS_VOTE_TIME, "" );
  trap_SetConfigstring( CS_VOTE_STRING, "" );
  level.teamVoteTime[ 0 ] = 0;
  trap_SetConfigstring( CS_TEAMVOTE_TIME, "" );
  trap_SetConfigstring( CS_TEAMVOTE_STRING, "" );
  level.teamVoteTime[ 1 ] = 0;
  trap_SetConfigstring( CS_TEAMVOTE_TIME + 1, "" );
  trap_SetConfigstring( CS_TEAMVOTE_STRING + 1, "" );
}

/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart )
{
  int i, clients;

  // in case of a map_restart
  G_ClearVotes( );

  G_Printf( "==== ShutdownGame ====\n" );

  if( level.logFile )
  {
    G_LogPrintf( "ShutdownGame:\n" );
    G_LogPrintf( "------------------------------------------------------------\n" );
    trap_FS_FCloseFile( level.logFile );
  }

  // write all the client session data so we can get it back
  G_WriteSessionData( );

  G_admin_cleanup( );
  G_admin_namelog_cleanup( );

  level.restarted = qfalse;
  level.surrenderTeam = TEAM_NONE;
  trap_SetConfigstring( CS_WINNER, "" );

  // clear all demo clients
  clients = trap_Cvar_VariableIntegerValue( "sv_democlients" );
  for( i = 0; i < clients; i++ )
    trap_SetConfigstring( CS_PLAYERS + i, NULL );
}



//===================================================================

void QDECL Com_Error( int level, const char *error, ... )
{
  va_list argptr;
  char    text[ 1024 ];

  va_start( argptr, error );
  Q_vsnprintf( text, sizeof( text ), error, argptr );
  va_end( argptr );

  G_Error( "%s", text );
}

void QDECL Com_Printf( const char *msg, ... )
{
  va_list argptr;
  char    text[ 1024 ];

  va_start( argptr, msg );
  Q_vsnprintf( text, sizeof( text ), msg, argptr );
  va_end( argptr );

  G_Printf( "%s", text );
}

/*
========================================================================

PLAYER COUNTING / SCORE SORTING

========================================================================
*/


/*
=============
SortRanks

=============
*/
int QDECL SortRanks( const void *a, const void *b )
{
  gclient_t *ca, *cb;

  ca = &level.clients[ *(int *)a ];
  cb = &level.clients[ *(int *)b ];

  // then sort by score
  if( ca->ps.persistant[ PERS_KILLS ] > cb->ps.persistant[ PERS_KILLS ] )
    return -1;
  if( ca->ps.persistant[ PERS_KILLS ] < cb->ps.persistant[ PERS_KILLS ] )
    return 1;
  return 0;
}

/*
============
G_InitSpawnQueue

Initialise a spawn queue
============
*/
void G_InitSpawnQueue( spawnQueue_t *sq )
{
  int i;

  sq->back = sq->front = 0;
  sq->back = QUEUE_MINUS1( sq->back );

  //0 is a valid clientNum, so use something else
  for( i = 0; i < MAX_CLIENTS; i++ )
    sq->clients[ i ] = -1;
}

/*
============
G_GetSpawnQueueLength

Return the length of a spawn queue
============
*/
int G_GetSpawnQueueLength( spawnQueue_t *sq )
{
  int length = sq->back - sq->front + 1;

  while( length < 0 )
    length += MAX_CLIENTS;

  while( length >= MAX_CLIENTS )
    length -= MAX_CLIENTS;

  return length;
}

/*
============
G_PopSpawnQueue

Remove from front element from a spawn queue
============
*/
int G_PopSpawnQueue( spawnQueue_t *sq )
{
  int clientNum = sq->clients[ sq->front ];

  if( G_GetSpawnQueueLength( sq ) > 0 )
  {
    sq->clients[ sq->front ] = -1;
    sq->front = QUEUE_PLUS1( sq->front );
    G_StopFollowing( g_entities + clientNum );
    g_entities[ clientNum ].client->ps.pm_flags &= ~PMF_QUEUED;

    return clientNum;
  }
  return -1;
}

/*
============
G_PeekSpawnQueue

Look at front element from a spawn queue
============
*/
int G_PeekSpawnQueue( spawnQueue_t *sq )
{
  return sq->clients[ sq->front ];
}

/*
============
G_SearchSpawnQueue

Look to see if clientNum is already in the spawnQueue
============
*/
qboolean G_SearchSpawnQueue( spawnQueue_t *sq, int clientNum )
{
  int i;

  for( i = 0; i < MAX_CLIENTS; i++ )
    if( sq->clients[ i ] == clientNum )
      return qtrue;
  return qfalse;
}

/*
============
G_PushSpawnQueue

Add an element to the back of the spawn queue
============
*/
qboolean G_PushSpawnQueue( spawnQueue_t *sq, int clientNum )
{
  // don't add the same client more than once
  if( G_SearchSpawnQueue( sq, clientNum ) )
    return qfalse;

  sq->back = QUEUE_PLUS1( sq->back );
  sq->clients[ sq->back ] = clientNum;

  g_entities[ clientNum ].client->ps.pm_flags |= PMF_QUEUED;
  return qtrue;
}

/*
============
G_RemoveFromSpawnQueue

remove a specific client from a spawn queue
============
*/
qboolean G_RemoveFromSpawnQueue( spawnQueue_t *sq, int clientNum )
{
  int i = sq->front;

  if( G_GetSpawnQueueLength( sq ) )
  {
    do
    {
      if( sq->clients[ i ] == clientNum )
      {
        //and this kids is why it would have
        //been better to use an LL for internal
        //representation
        do
        {
          sq->clients[ i ] = sq->clients[ QUEUE_PLUS1( i ) ];

          i = QUEUE_PLUS1( i );
        } while( i != QUEUE_PLUS1( sq->back ) );

        sq->back = QUEUE_MINUS1( sq->back );
        g_entities[ clientNum ].client->ps.pm_flags &= ~PMF_QUEUED;

        return qtrue;
      }

      i = QUEUE_PLUS1( i );
    } while( i != QUEUE_PLUS1( sq->back ) );
  }

  return qfalse;
}

/*
============
G_GetPosInSpawnQueue

Get the position of a client in a spawn queue
============
*/
int G_GetPosInSpawnQueue( spawnQueue_t *sq, int clientNum )
{
  int i = sq->front;

  if( G_GetSpawnQueueLength( sq ) )
  {
    do
    {
      if( sq->clients[ i ] == clientNum )
      {
        if( i < sq->front )
          return i + MAX_CLIENTS - sq->front;
        else
          return i - sq->front;
      }

      i = QUEUE_PLUS1( i );
    } while( i != QUEUE_PLUS1( sq->back ) );
  }

  return -1;
}

/*
============
G_PrintSpawnQueue

Print the contents of a spawn queue
============
*/
void G_PrintSpawnQueue( spawnQueue_t *sq )
{
  int i = sq->front;
  int length = G_GetSpawnQueueLength( sq );

  G_Printf( "l:%d f:%d b:%d    :", length, sq->front, sq->back );

  if( length > 0 )
  {
    do
    {
      if( sq->clients[ i ] == -1 )
        G_Printf( "*:" );
      else
        G_Printf( "%d:", sq->clients[ i ] );

      i = QUEUE_PLUS1( i );
    } while( i != QUEUE_PLUS1( sq->back ) );
  }

  G_Printf( "\n" );
}

/*
============
G_SpawnClients

Spawn queued clients
============
*/
void G_SpawnClients( team_t team )
{
  int           clientNum;
  gentity_t     *ent, *spawn;
  vec3_t        spawn_origin, spawn_angles;
  spawnQueue_t  *sq = NULL;
  int           numSpawns = 0;
  
  if( g_doWarmup.integer && ( g_warmupMode.integer==1 || g_warmupMode.integer == 2 ) &&
      level.time - level.startTime < g_warmup.integer * 1000 )
    return;

  if( team == TEAM_ALIENS )
  {
    sq = &level.alienSpawnQueue;
    numSpawns = level.numAlienSpawns;
  }
  else if( team == TEAM_HUMANS )
  {
    sq = &level.humanSpawnQueue;
    numSpawns = level.numHumanSpawns;
  }

  if( G_GetSpawnQueueLength( sq ) > 0 && numSpawns > 0 )
  {
    clientNum = G_PeekSpawnQueue( sq );
    ent = &g_entities[ clientNum ];

    if( ( spawn = G_SelectTremulousSpawnPoint( team,
            ent->client->pers.lastDeathLocation,
            spawn_origin, spawn_angles ) ) )
    {
      clientNum = G_PopSpawnQueue( sq );

      if( clientNum < 0 )
        return;

      ent = &g_entities[ clientNum ];

      ent->client->sess.spectatorState = SPECTATOR_NOT;
      ClientUserinfoChanged( clientNum );
      ClientSpawn( ent, spawn, spawn_origin, spawn_angles );
    }
  }
}

/*
============
G_CountSpawns

Counts the number of spawns for each team
============
*/
void G_CountSpawns( void )
{
  int i;
  gentity_t *ent;

  level.numAlienSpawns = 0;
  level.numHumanSpawns = 0;

  if( g_extremeSuddenDeath.integer )
    return;

  for( i = 1, ent = g_entities + i ; i < level.num_entities ; i++, ent++ )
  {
    if( !ent->inuse || ent->s.eType != ET_BUILDABLE || ent->health <= 0 )
      continue;

    if( ent->s.modelindex == BA_A_SPAWN )
        level.numAlienSpawns++;

    if( ent->s.modelindex == BA_H_SPAWN )
      level.numHumanSpawns++;
  }
}

/*
============
G_TimeTilSuddenDeath
============
*/
int G_TimeTilSuddenDeath( void )
{
  if( !level.suddenDeathTime )
    return 999999999; // Always some time away

  return ( level.suddenDeathTime - ( level.time - level.startTime ) );
}


/*
============
G_TimeTilExtremeSuddenDeath
============
*/
int G_TimeTilExtremeSuddenDeath( void )
{
  if( !level.extremeSuddenDeathTime )
    return 999999999; // Always some time away

  return ( level.extremeSuddenDeathTime - ( level.time - level.startTime ) );
}

#define PLAYER_COUNT_MOD 6.0f

/*
============
G_CalculateBuildPoints

Recalculate the quantity of building points available to the teams
============
*/
void G_CalculateBuildPoints( void )
{
  int         i;
  buildable_t buildable;
  gentity_t   *ent;
  int         localHTP = level.humanBuildPoints = g_humanBuildPoints.integer,
              localATP = level.alienBuildPoints = g_alienBuildPoints.integer;

  // BP queue updates
  while( level.alienBuildPointQueue > 0 &&
         level.alienNextQueueTime < level.time )
  {
    level.alienBuildPointQueue--;
    level.alienNextQueueTime += g_alienBuildQueueTime.integer;
  }

  while( level.humanBuildPointQueue > 0 &&
         level.humanNextQueueTime < level.time )
  {
    level.humanBuildPointQueue--;
    level.humanNextQueueTime += g_alienBuildQueueTime.integer;
  }
  
  if( level.extremeSuddenDeathTime > 0 )
  {
    if( level.time >= level.extremeSuddenDeathTime + VAMPIRIC_ESD_DELAY-2500
        && level.time <= level.extremeSuddenDeathTime + VAMPIRIC_ESD_DELAY-2000
        && !level.vesd )
      AP( "cp \"^1Your base will explode shortly\n^1You will also start taking constant damage\"" );
    else if( level.time >= level.extremeSuddenDeathTime + VAMPIRIC_ESD_DELAY
              && !level.vesd )
    {
      for( i = 1, ent = g_entities + i; i < level.num_entities; i++, ent++ )
      {        
        if( !ent || ent->health <= 0 )
          continue;
        else if( ent->s.eType != ET_BUILDABLE && ent->client 
                 && ent->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS )
        {
          if( BG_InventoryContainsUpgrade( UP_MEDKIT, ent->client->ps.stats ) )
            BG_RemoveUpgradeFromInventory( UP_MEDKIT, ent->client->ps.stats );
          continue;
        }
        
        G_Damage( ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
      }
      level.vesd = qtrue;
    }
  }
  
  //Extend votes
 if( g_extendvote.integer )
 {
    if( !g_suddenDeath.integer && g_suddenDeathTime.integer )
    {
      g_suddenDeathTime.integer += g_extendvotetime.integer;				
      AP( va( "print \"^7Sudden Death is now at %d\n\"", g_suddenDeathTime.integer ) );
    }
    
    if( !g_extremeSuddenDeath.integer && g_extremeSuddenDeathTime.integer )
    {
      g_extremeSuddenDeathTime.integer += g_extendvotetime.integer;
      AP( va( "print \"^7Extreme Sudden Death is now at %d\n\"", g_extremeSuddenDeathTime.integer ) );
    }
    
    g_timelimit.integer += g_extendvotetime.integer;
    AP( va( "print \"^7The Time Limit is now at %d\n\"", g_timelimit.integer ) );

    trap_Cvar_Set( "g_extendvote", "0" );
 }

  //check to see if suddendeath has been turned off
  if( level.suddenDeath && ! g_suddenDeath.integer )
  {
    level.suddenDeath = qfalse;
    trap_SendServerCommand( -1, "print \"Sudden Death Has Been Turned Off\n\"" );
  }

  //check to see if extremesuddendeath has been turned off
  if( level.extremeSuddenDeath && ! g_extremeSuddenDeath.integer )
  {
    level.extremeSuddenDeath = qfalse;
    trap_SendServerCommand( -1, "print \"Extreme Sudden Death Has Been Turned Off\n\"" );
  }

  //check if suddendeathtime has been changed by hand
  if( !level.suddenDeathVote &&
    ( g_suddenDeathTime.integer * 60000 ) != level.suddenDeathTime )
    level.suddenDeathTime = g_suddenDeathTime.integer * 60000;
  //check if extremesuddendeathtime has been changed by hand
  if( !level.extremeSuddenDeathVote && 
    ( g_extremeSuddenDeathTime.integer * 60000 ) != level.extremeSuddenDeathTime )
    level.extremeSuddenDeathTime = g_extremeSuddenDeathTime.integer * 60000;
  //it is time for suddendeath to start
  if( level.suddenDeathTime && !level.suddenDeath && G_TimeTilSuddenDeath( ) <= 0 )
    trap_Cvar_Set( "g_suddenDeath", "1" );
  //it is time for extremesuddendeath to start
  if( level.extremeSuddenDeathTime  && !level.extremeSuddenDeath && G_TimeTilExtremeSuddenDeath( ) <= 0 )
    trap_Cvar_Set( "g_extremeSuddenDeath", "1" );

  //start suddendeath
  if( g_suddenDeath.integer && !level.suddenDeath )
  {
    level.suddenDeath = qtrue;
    //warn about sudden death
    if( level.suddenDeathWarning < TW_PASSED )
    {
      trap_SendServerCommand( -1, "cp \"Sudden Death!\"" );
      trap_SendServerCommand( -1, "print \"Sudden Death!\n\"" );
      level.suddenDeathWarning = TW_PASSED;
    }
    localHTP = 0;
    localATP = 0;
    if( g_suddenDeathMode.integer == SDMODE_SELECTIVE )
    {
      for( i = 1, ent = g_entities + i; i < level.num_entities; i++, ent++ )
      {
        if( ent->s.eType != ET_BUILDABLE )
          continue;

        if( BG_FindReplaceableTestForBuildable( ent->s.modelindex ) )
        {
          int t = BG_FindTeamForBuildable( ent->s.modelindex );

          if( t == BIT_HUMANS )
            localHTP += BG_FindBuildPointsForBuildable( ent->s.modelindex );
          else if( t == BIT_ALIENS )
            localATP += BG_FindBuildPointsForBuildable( ent->s.modelindex );
        }
      }
    }
    level.suddenDeathHBuildPoints = localHTP;
    level.suddenDeathABuildPoints = localATP;
  }
  else if ( level.suddenDeathTime )
  {
    //warn about sudden death when 5 minutes are left
    if( G_TimeTilSuddenDeath( ) <= 300000 && level.suddenDeathWarning < TW_CLOSE )
    {
      trap_SendServerCommand( -1, "cp \"Sudden Death in 5 minutes!\"" );
      trap_SendServerCommand( -1, "print \"Sudden Death in 5 minutes!\n\"" );
      level.suddenDeathWarning = TW_CLOSE;
    }
    //warn about sudden death
    if( G_TimeTilSuddenDeath( ) <= 60000 &&
      level.suddenDeathWarning < TW_IMMINENT )
    {
      trap_SendServerCommand( -1, "cp \"Sudden Death in 1 minute!\"" );
      trap_SendServerCommand( -1, "print \"Sudden Death in 1 minute!\n\"" );
      level.suddenDeathWarning = TW_IMMINENT;
    }
  }
  //start extremesuddendeath
  if( g_extremeSuddenDeath.integer && !level.extremeSuddenDeath )
  {
    level.extremeSuddenDeath = qtrue;
    level.suddenDeath = qtrue;
    level.extremeSuddenDeathTime = level.time;

    //destroy all spawns
    if( !g_tkmap.integer )
    {
      for( i = 1, ent = g_entities + i; i < level.num_entities; i++, ent++ )
      {        
        if( !ent || !ent->s.eType == ET_BUILDABLE || ent->health <= 0 )
          continue;
        
        if( ent->s.modelindex == BA_H_SPAWN || ent->s.modelindex == BA_A_SPAWN )
          G_Damage( ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
      }
    }

    if( g_alienStage.integer < 2 )
        trap_Cvar_Set( "g_alienStage", "2" ); // set aliens to stage 3
    if( g_humanStage.integer < 2 )
        trap_Cvar_Set( "g_humanStage", "2" ); // set humans to stage 3

    if( g_smartesd.integer && !g_tkmap.integer )
    {
      level.alienTeamLocked = qtrue;  // lock alien team
      level.humanTeamLocked = qtrue;  // lock human team
    }
    
    for( i = 0; i < MAX_CLIENTS; i++ )
    {
      if( level.clients[ i ].ps.stats[ STAT_TEAM ] == TEAM_HUMANS )
        level.clients[ i ].ps.persistant[ PERS_CREDIT ] = 2000;
      else if( level.clients[ i ].ps.stats[ STAT_TEAM ] == TEAM_ALIENS )
        level.clients[ i ].ps.persistant[ PERS_CREDIT ] = ALIEN_CREDITS_PER_FRAG*10; //Frags are 400 each, 10*400 = 4000, extra for adv. rant
    }
    
    if( level.extremeSuddenDeathWarning < TW_PASSED && !g_tkmap.integer )
    {
      trap_SendServerCommand( -1, "cp \"^1Extreme Sudden Death! NO SPAWNS\n ^1NO BUILDINGS and CONSTANT DAMAGE in 30 seconds\"" );
      trap_SendServerCommand( -1, "print \"^1Extreme Sudden Death! NO SPAWNS; NO BUILDINGS and CONSTANT DAMAGE starts in 30 seconds\n\"" );
      level.extremeSuddenDeathWarning = TW_PASSED;
    }
    else if( level.extremeSuddenDeathWarning < TW_PASSED && g_tkmap.integer )
    {
      trap_SendServerCommand( -1, "cp \"^1Extreme Sudden Death! BUILDING TEAMKILLING, NO BUILDING, LAST ONE ALIVE WINS\"" );
      trap_SendServerCommand( -1, "print \"^1Extreme Sudden Death! BUILDING TEAMKILLING, NO BUILDING, LAST ONE ALIVE WINS\n\"" );
      level.extremeSuddenDeathWarning = TW_PASSED;
    }
    
  }
  else if( level.extremeSuddenDeathTime ) // esd warnings if we are using it time based
  {
    //warn about extreme sudden death when 5 minutes are left
    if( G_TimeTilExtremeSuddenDeath( ) <= 300000 && level.extremeSuddenDeathWarning < TW_CLOSE )
    {
      trap_SendServerCommand( -1, "cp \"^1Extreme Sudden Death in 5 minutes!\"" );
      trap_SendServerCommand( -1, "print \"^1Extreme Sudden Death in 5 minutes!\n\"" );
      level.extremeSuddenDeathWarning = TW_CLOSE;
    }
    //warn about extreme sudden death when 1 minute is left
    if( G_TimeTilExtremeSuddenDeath( ) <= 60000 && level.extremeSuddenDeathWarning < TW_IMMINENT )
    {
      trap_SendServerCommand( -1, "cp \"^1Extreme Sudden Death in 1 minute!\"" );
      trap_SendServerCommand( -1, "print \"^1Extreme Sudden Death in 1 minute!\n\"" );
      level.extremeSuddenDeathWarning = TW_IMMINENT;
    }
    // how to handle a passed esd vote
		if( g_extremeSuddenDeathVote.integer && level.extremeSuddenDeathWarning < TW_CLOSE )
		{
			if( g_smartesd.integer )
			{
				level.extremeSuddenDeathWarning = TW_CLOSE;
				g_timelimit.integer = (int) ( ( g_timelimit.value - g_extremeSuddenDeathTime.value ) + ( ( ( level.time - level.startTime + 120000) /60000 ) ) );
				g_extremeSuddenDeathTime.integer = (int)( ( (level.time - level.startTime + 120000)/60000));
				AP( va( "print \"^7Extreme Sudden Death will now start at %d\n\"", g_extremeSuddenDeathTime.integer));
				AP( va( "print \"^7Time Limit is now at %d\n\"", g_timelimit.integer));
			}
			else
				g_extremeSuddenDeath.integer = 1;
		}
  }
  
  //set BP at each cycle
  if( level.extremeSuddenDeath )
  {
    localHTP = 0;
    localATP = 0;
  }
  else if( level.suddenDeath )
  {
    localHTP = level.suddenDeathHBuildPoints;
    localATP = level.suddenDeathABuildPoints;
  }
  else
  {
    localHTP = g_humanBuildPoints.integer;
    localATP = g_alienBuildPoints.integer;
  }


  level.humanBuildPoints = level.humanBuildPointsPowered = localHTP - level.humanBuildPointQueue;
  level.alienBuildPoints = localATP - level.alienBuildPointQueue;

  level.reactorPresent = qfalse;
  level.overmindPresent = qfalse;

  for( i = 1, ent = g_entities + i ; i < level.num_entities ; i++, ent++ )
  {
    if( !ent->inuse )
      continue;

    if( ent->s.eType != ET_BUILDABLE )
      continue;

    if( ent->s.eFlags & EF_DEAD )
      continue;

    buildable = ent->s.modelindex;

    if( buildable != BA_NONE )
    {
      if( buildable == BA_H_REACTOR && ent->spawned && ent->health > 0 )
        level.reactorPresent = qtrue;

      if( buildable == BA_A_OVERMIND && ent->spawned && ent->health > 0 )
        level.overmindPresent = qtrue;
      
      if( g_suddenDeath.integer == 1 
          && !BG_FindReplaceableTestForBuildable( buildable )
          && g_suddenDeathMode.integer == SDMODE_SELECTIVE )
        continue;

      if( BG_Buildable( buildable )->team == TEAM_HUMANS )
        level.humanBuildPoints -= BG_Buildable( buildable )->buildPoints;
      else if( BG_Buildable( buildable )->team == TEAM_ALIENS )
        level.alienBuildPoints -= BG_Buildable( buildable )->buildPoints;
    }
  }

  if( level.humanBuildPoints < 0 )
  {
    localHTP -= level.humanBuildPoints;
    level.humanBuildPoints = 0;
  }

  if( level.alienBuildPoints < 0 )
  {
    localATP -= level.alienBuildPoints;
    level.alienBuildPoints = 0;
  }

  trap_SetConfigstring( CS_BUILDPOINTS, va( "%d %d %d %d",
        level.alienBuildPoints, localATP,
        level.humanBuildPoints, localHTP ) );
    
  if( g_extremeSuddenDeath.integer && g_tkmap.integer
      && ( level.numLiveAlienClients == 1 || level.numLiveHumanClients == 1 ) )
      level.playerWin = qtrue;

  //may as well pump the stages here too
  {
    float alienPlayerCountMod = level.averageNumAlienClients / PLAYER_COUNT_MOD;
    float humanPlayerCountMod = level.averageNumHumanClients / PLAYER_COUNT_MOD;
    int   alienNextStageThreshold, humanNextStageThreshold;

    if( alienPlayerCountMod < 0.1f )
      alienPlayerCountMod = 0.1f;
    else if( alienPlayerCountMod > 2.0f )
      alienPlayerCountMod = 2.0f;

    if( humanPlayerCountMod < 0.1f )
      humanPlayerCountMod = 0.1f;
    else if( humanPlayerCountMod > 2.0f )
      humanPlayerCountMod = 2.0f;

    if( g_alienStage.integer == S1 && g_alienMaxStage.integer > S1 )
      alienNextStageThreshold = (int)( ceil( (float)g_alienStage2Threshold.integer * alienPlayerCountMod ) );
    else if( g_alienStage.integer == S2 && g_alienMaxStage.integer > S2 )
      alienNextStageThreshold = (int)( ceil( (float)g_alienStage3Threshold.integer * alienPlayerCountMod ) );
    else
      alienNextStageThreshold = -1;

    if( g_humanStage.integer == S1 && g_humanMaxStage.integer > S1 )
      humanNextStageThreshold = (int)( ceil( (float)g_humanStage2Threshold.integer * humanPlayerCountMod ) );
    else if( g_humanStage.integer == S2 && g_humanMaxStage.integer > S2 )
      humanNextStageThreshold = (int)( ceil( (float)g_humanStage3Threshold.integer * humanPlayerCountMod ) );
    else
      humanNextStageThreshold = -1;

    // save a lot of bandwidth by rounding thresholds up to the nearest 100
    if( alienNextStageThreshold > 0 )
      alienNextStageThreshold = ceil( (float)alienNextStageThreshold / 100 ) * 100;
    if( humanNextStageThreshold > 0 )
      humanNextStageThreshold = ceil( (float)humanNextStageThreshold / 100 ) * 100;

    trap_SetConfigstring( CS_STAGES, va( "%d %d %d %d %d %d",
          g_alienStage.integer, g_humanStage.integer,
          g_alienCredits.integer, g_humanCredits.integer,
          alienNextStageThreshold, humanNextStageThreshold ) );
  }
}

/*
============
G_CalculateStages
============
*/
void G_CalculateStages( void )
{
  float         alienPlayerCountMod     = level.averageNumAlienClients / PLAYER_COUNT_MOD;
  float         humanPlayerCountMod     = level.averageNumHumanClients / PLAYER_COUNT_MOD;
  static int    lastAlienStageModCount  = 1;
  static int    lastHumanStageModCount  = 1;
  static int    lastAlienStage  = -1;
  static int    lastHumanStage  = -1;

  if( alienPlayerCountMod < 0.1f )
    alienPlayerCountMod = 0.1f;
  else if( alienPlayerCountMod > 2.0f )
    alienPlayerCountMod = 2.0f;

  if( humanPlayerCountMod < 0.1f )
    humanPlayerCountMod = 0.1f;
  else if( humanPlayerCountMod > 2.0f )
    humanPlayerCountMod = 2.0f;

  if( g_alienCredits.integer >=
      (int)( ceil( (float)g_alienStage2Threshold.integer * alienPlayerCountMod ) ) &&
      g_alienStage.integer == S1 && g_alienMaxStage.integer > S1 )
  {
    trap_Cvar_Set( "g_alienStage", va( "%d", S2 ) );
    level.alienStage2Time = level.time;
    lastAlienStageModCount = g_alienStage.modificationCount;
  }

  if( g_alienCredits.integer >=
      (int)( ceil( (float)g_alienStage3Threshold.integer * alienPlayerCountMod ) ) &&
      g_alienStage.integer == S2 && g_alienMaxStage.integer > S2 )
  {
    trap_Cvar_Set( "g_alienStage", va( "%d", S3 ) );
    level.alienStage3Time = level.time;
    lastAlienStageModCount = g_alienStage.modificationCount;
  }

  if( g_humanCredits.integer >=
      (int)( ceil( (float)g_humanStage2Threshold.integer * humanPlayerCountMod ) ) &&
      g_humanStage.integer == S1 && g_humanMaxStage.integer > S1 )
  {
    trap_Cvar_Set( "g_humanStage", va( "%d", S2 ) );
    level.humanStage2Time = level.time;
    lastHumanStageModCount = g_humanStage.modificationCount;
  }

  if( g_humanCredits.integer >=
      (int)( ceil( (float)g_humanStage3Threshold.integer * humanPlayerCountMod ) ) &&
      g_humanStage.integer == S2 && g_humanMaxStage.integer > S2 )
  {
    trap_Cvar_Set( "g_humanStage", va( "%d", S3 ) );
    level.humanStage3Time = level.time;
    lastHumanStageModCount = g_humanStage.modificationCount;
  }

  if( g_alienStage.modificationCount > lastAlienStageModCount )
  {
    G_Checktrigger_stages( TEAM_ALIENS, g_alienStage.integer );

    if( g_alienStage.integer == S2 )
      level.alienStage2Time = level.time;
    else if( g_alienStage.integer == S3 )
      level.alienStage3Time = level.time;

    lastAlienStageModCount = g_alienStage.modificationCount;
  }

  if( g_humanStage.modificationCount > lastHumanStageModCount )
  {
    G_Checktrigger_stages( TEAM_HUMANS, g_humanStage.integer );

    if( g_humanStage.integer == S2 )
      level.humanStage2Time = level.time;
    else if( g_humanStage.integer == S3 )
      level.humanStage3Time = level.time;

    lastHumanStageModCount = g_humanStage.modificationCount;
  }

  if ( level.demoState == DS_RECORDING &&
       ( trap_Cvar_VariableIntegerValue( "g_alienStage" ) != lastAlienStage ||
         trap_Cvar_VariableIntegerValue( "g_humanStage" ) != lastHumanStage ) )
  {
    lastAlienStage = trap_Cvar_VariableIntegerValue( "g_alienStage" );
    lastHumanStage = trap_Cvar_VariableIntegerValue( "g_humanStage" );
    G_DemoCommand( DC_SET_STAGE, va( "%d %d", lastHumanStage, lastAlienStage ) );
  }
}

/*
============
CalculateAvgPlayers

Calculates the average number of players playing this game
============
*/
void G_CalculateAvgPlayers( void )
{
  //there are no clients or only spectators connected, so
  //reset the number of samples in order to avoid the situation
  //where the average tends to 0
  if( !level.numAlienClients )
  {
    level.numAlienSamples = 0;
    trap_Cvar_Set( "g_alienCredits", "0" );
  }

  if( !level.numHumanClients )
  {
    level.numHumanSamples = 0;
    trap_Cvar_Set( "g_humanCredits", "0" );
  }

  //calculate average number of clients for stats
  level.averageNumAlienClients =
    ( ( level.averageNumAlienClients * level.numAlienSamples )
      + level.numAlienClients ) /
    (float)( level.numAlienSamples + 1 );
  level.numAlienSamples++;

  level.averageNumHumanClients =
    ( ( level.averageNumHumanClients * level.numHumanSamples )
      + level.numHumanClients ) /
    (float)( level.numHumanSamples + 1 );
  level.numHumanSamples++;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void )
{
  int       i;
  char      P[ MAX_CLIENTS + 1 ] = {""};
  int       ff = 0;

  level.numConnectedClients = 0;
  level.numPlayingClients = 0;
  level.numVotingClients = 0;   // don't count bots
  level.numAlienClients = 0;
  level.numHumanClients = 0;
  level.numLiveAlienClients = 0;
  level.numLiveHumanClients = 0;

  for( i = 0; i < level.maxclients; i++ )
  {
    P[ i ] = '-';
    if ( level.clients[ i ].pers.connected != CON_DISCONNECTED ||
         level.clients[ i ].pers.demoClient )
    {
      level.sortedClients[ level.numConnectedClients ] = i;
      level.numConnectedClients++;
      P[ i ] = (char)'0' + level.clients[ i ].pers.teamSelection;

      if( level.clients[ i ].pers.connected != CON_CONNECTED &&
         !level.clients[ i ].pers.demoClient )
        continue;

      level.numVotingClients++;
      if( level.clients[ i ].pers.teamSelection != TEAM_NONE )
      {
        level.numPlayingClients++;

        if( level.clients[ i ].pers.teamSelection == TEAM_ALIENS )
        {
          level.numAlienClients++;
          if( level.clients[ i ].sess.spectatorState == SPECTATOR_NOT )
            level.numLiveAlienClients++;
        }
        else if( level.clients[ i ].pers.teamSelection == TEAM_HUMANS )
        {
          level.numHumanClients++;
          if( level.clients[ i ].sess.spectatorState == SPECTATOR_NOT )
            level.numLiveHumanClients++;
        }
      }
    }
  }
  level.numNonSpectatorClients = level.numLiveAlienClients +
    level.numLiveHumanClients;
  level.numteamVotingClients[ 0 ] = level.numHumanClients;
  level.numteamVotingClients[ 1 ] = level.numAlienClients;
  P[ i ] = '\0';
  trap_Cvar_Set( "P", P );

  if( g_friendlyFire.integer )
    ff |= ( FFF_HUMANS | FFF_ALIENS );
  if( g_friendlyFireHumans.integer )
    ff |=  FFF_HUMANS;
  if( g_friendlyFireAliens.integer )
    ff |=  FFF_ALIENS;
  if( g_friendlyBuildableFire.integer )
    ff |=  FFF_BUILDABLES;
  trap_Cvar_Set( "ff", va( "%i", ff ) );

  qsort( level.sortedClients, level.numConnectedClients,
    sizeof( level.sortedClients[ 0 ] ), SortRanks );

  // see if it is time to end the level
  CheckExitRules( );

  // if we are at the intermission, send the new info to everyone
  if( level.intermissiontime )
    SendScoreboardMessageToAllClients( );
}

/*
============
G_DemoCommand

Store a demo command to a demo if we are recording
============
*/
void G_DemoCommand( demoCommand_t cmd, const char *string )
{
  if( level.demoState == DS_RECORDING )
    trap_DemoCommand( cmd, string );
}

/*
============
G_DemoSetClient

Mark a client as a demo client and load info into it
============
*/
void G_DemoSetClient( void )
{
  char buffer[ MAX_INFO_STRING ];
  int clientNum;
  gclient_t *client;
  char *s;

  trap_Argv( 0, buffer, sizeof( buffer ) );
  clientNum = atoi( buffer );
  client = level.clients + clientNum;
  client->pers.demoClient = qtrue;

  trap_Argv( 1, buffer, sizeof( buffer ) );
  s = Info_ValueForKey( buffer, "n" );
  if( *s )
    Q_strncpyz( client->pers.netname, s, sizeof( client->pers.netname ) );
  s = Info_ValueForKey( buffer, "t" );
  if( *s )
    client->pers.teamSelection = atoi( s );
  client->sess.spectatorState = SPECTATOR_NOT;
  trap_SetConfigstring( CS_PLAYERS + clientNum, buffer );
}

/*
============
G_DemoRemoveClient

Unmark a client as a demo client
============
*/
void G_DemoRemoveClient( void )
{
  char buffer[ 3 ];
  int clientNum;

  trap_Argv( 0, buffer, sizeof( buffer ) );
  clientNum = atoi( buffer );
  level.clients[clientNum].pers.demoClient = qfalse;
  trap_SetConfigstring( CS_PLAYERS + clientNum, NULL );
}

/*
============
G_DemoSetStage

Set the stages in a demo
============
*/
void G_DemoSetStage( void )
{
  char buffer[ 2 ];

  trap_Argv( 0, buffer, sizeof( buffer ) );
  trap_Cvar_Set( "g_humanStage", buffer );
  trap_Argv( 1, buffer, sizeof( buffer ) );
  trap_Cvar_Set( "g_alienStage", buffer );
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void )
{
  int   i;

  for( i = 0; i < level.maxclients; i++ )
  {
    if( level.clients[ i ].pers.connected == CON_CONNECTED )
      ScoreboardMessage( g_entities + i );
  }
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent )
{
  // take out of follow mode if needed
  if( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
    G_StopFollowing( ent );

  // move to the spot
  VectorCopy( level.intermission_origin, ent->s.origin );
  VectorCopy( level.intermission_origin, ent->client->ps.origin );
  VectorCopy( level.intermission_angle, ent->client->ps.viewangles );
  ent->client->ps.pm_type = PM_INTERMISSION;

  // clean up powerup info
  memset( ent->client->ps.misc, 0, sizeof( ent->client->ps.misc ) );

  ent->client->ps.eFlags = 0;
  ent->s.eFlags = 0;
  ent->s.eType = ET_GENERAL;
  ent->s.modelindex = 0;
  ent->s.loopSound = 0;
  ent->s.event = 0;
  ent->r.contents = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void )
{
  gentity_t *ent, *target;
  vec3_t    dir;

  // find the intermission spot
  ent = G_Find( NULL, FOFS( classname ), "info_player_intermission" );

  if( !ent )
  { // the map creator forgot to put in an intermission point...
    G_SelectSpawnPoint( vec3_origin, level.intermission_origin, level.intermission_angle );
  }
  else
  {
    VectorCopy( ent->s.origin, level.intermission_origin );
    VectorCopy( ent->s.angles, level.intermission_angle );
    // if it has a target, look towards it
    if( ent->target )
    {
      target = G_PickTarget( ent->target );

      if( target )
      {
        VectorSubtract( target->s.origin, level.intermission_origin, dir );
        vectoangles( dir, level.intermission_angle );
      }
    }
  }

}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void )
{
  int     i;
  gentity_t *client;

  if( level.intermissiontime )
    return;   // already active

  level.intermissiontime = level.time;

  G_ClearVotes( );

  FindIntermissionPoint( );

  // move all clients to the intermission point
  for( i = 0; i < level.maxclients; i++ )
  {
    client = g_entities + i;

    if( !client->inuse )
      continue;

    // respawn if dead
    if( client->health <= 0 )
      respawn(client);

    MoveClientToIntermission( client );
  }

  // send the current scoring to all clients
  SendScoreboardMessageToAllClients( );
}


/*
=============
ExitLevel

When the intermission has been exited, the server is either moved
to a new map based on the map rotation or the current map restarted
=============
*/
void ExitLevel( void )
{
  int       i;
  gclient_t *cl;
  buildHistory_t *tmp, *mark;

  while( (tmp = level.buildHistory) )
  {
    level.buildHistory = level.buildHistory->next;
    while(( mark = tmp ))
    {
      tmp = tmp->marked;
      BG_Free( mark );
    }
  }
  
  if( G_MapExists( g_nextMap.string ) )
    trap_SendConsoleCommand( EXEC_APPEND, va( "map %s", g_nextMap.string ) );
  else if( G_MapRotationActive( ) )
    G_AdvanceMapRotation( );
  else
    trap_SendConsoleCommand( EXEC_APPEND, "map_restart\n" );

  level.restarted = qtrue;
  level.changemap = NULL;
  level.intermissiontime = 0;

  // reset all the scores so we don't enter the intermission again
  for( i = 0; i < g_maxclients.integer; i++ )
  {
    cl = level.clients + i;
    if( cl->pers.connected != CON_CONNECTED )
      continue;

    cl->ps.persistant[ PERS_KILLS ] = 0;
  }

  // we need to do this here before chaning to CON_CONNECTING
  G_WriteSessionData( );

  // change all client states to connecting, so the early players into the
  // next level will know the others aren't done reconnecting
  for( i = 0; i < g_maxclients.integer; i++ )
  {
    if( level.clients[ i ].pers.connected == CON_CONNECTED )
      level.clients[ i ].pers.connected = CON_CONNECTING;
  }
}

/*
=================
G_AdminMessage

Print to all active server admins, and to the logfile, and to the server console
Prepend *prefix, or '[SERVER]' if no *prefix is given
=================
*/
void QDECL G_AdminMessage( const char *prefix, const char *fmt, ... )
{
  va_list argptr;
  char    string[ 1024 ];
  char    outstring[ 1024 ];
  int     i;

  // Format the text
  va_start( argptr, fmt );
  Q_vsnprintf( string, sizeof( string ), fmt, argptr );
  va_end( argptr );

  // If there is no prefix, assume that this function was called directly
  // and we should add one
  if( !prefix || !prefix[ 0 ] )
  {
    prefix = "[SERVER]:";
  }

  // Create the final string
  Com_sprintf( outstring, sizeof( outstring ), "%s%s",
               prefix, string );
               
  // Send to all appropriate clients --- DO NOT REMOVE BRACKETS IN THESE CONDITIONALS
  if( !strcmp( prefix, "[ADMIN]" ) || !strcmp( prefix, "[SERVER]:" ) || !strcmp( prefix, "[A]" )  )
  {
    for( i = 0; i < level.maxclients; i++ )
    {
      if( G_admin_permission( &g_entities[ i ], ADMF_ADMINCHAT ) ) 
        trap_SendServerCommand( i, va( "chat \"%s\"", outstring ) );
    }
  }
  else if( !strcmp( prefix, "[CLAN]" )  || !strcmp( prefix, "[C]") ) 
  {
    for( i = 0; i < level.maxclients; i++ )
    {
      if( G_admin_permission( &g_entities[ i ], ADMF_CLANCHAT ) ) 
        trap_SendServerCommand( i, va( "chat \"%s\"", outstring ) );
    }
  }
        
  // Send to the logfile and server console
  G_LogPrintf("adminmsg: %s\n", outstring );
}

/*
=================
G_AdminsPrintf

Print to all active admins, and the logfile with a time stamp if it is open, and to the console
=================
*/
void QDECL G_AdminsPrintf( const char *fmt, ... )
{
  va_list argptr;
  char    string[ 1024 ];
  gentity_t   *tempent;
  int j;

  va_start( argptr, fmt );
  vsprintf( string, fmt,argptr );
  va_end( argptr );

  for( j = 0; j < level.maxclients; j++ )
  {
    tempent = &g_entities[ j ];
    if( G_admin_permission( tempent, ADMF_ADMINCHAT ) ) 
       trap_SendServerCommand( tempent-g_entities, va( "print \"^7[ADMIN]:^6 %s\"", string ) ); 
  }
  
  G_LogPrintf( "%s", string );

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open, and to the server console
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... )
{
  va_list argptr;
  char    string[ 1024 ], decoloured[ 1024 ];
  int     min, tens, sec;

  sec = level.time / 1000;

  min = sec / 60;
  sec -= min * 60;
  tens = sec / 10;
  sec -= tens * 10;

  Com_sprintf( string, sizeof( string ), "%3i:%i%i ", min, tens, sec );

  va_start( argptr, fmt );
  Q_vsnprintf( string + 7, sizeof( string ) - 7, fmt, argptr );
  va_end( argptr );

  if( g_dedicated.integer )
    G_Printf( "%s", string + 7 );

  if( !level.logFile )
    return;

  if( g_decolourLogfiles.integer )
  {
    G_DecolorString( string, decoloured, sizeof( decoloured ) );
    trap_FS_Write( decoloured, strlen( decoloured ), level.logFile );
  }
  else
  {
    trap_FS_Write( string, strlen( string ), level.logFile );
  }
}


/*
=================
G_LogPrintfColoured

Bypasses g_decolourLogfiles for events that need colors in the logs
=================
*/
void QDECL G_LogPrintfColoured( const char *fmt, ... )
{
  va_list argptr;
  char    string[ 1024 ];
  int     min, tens, sec;

  sec = (level.time - level.startTime) / 1000;

  min = sec / 60;
  sec -= min * 60;
  tens = sec / 10;
  sec -= tens * 10;

  Com_sprintf( string, sizeof( string ), "%3i:%i%i ", min, tens, sec );

  va_start( argptr, fmt );
  vsprintf( string +7 , fmt,argptr );
  va_end( argptr );

  if( g_dedicated.integer )
    G_Printf( "%s", string + 7 );

  if( !level.logFile )
    return;

  trap_FS_Write( string, strlen( string ), level.logFile );
}


/*
=================
G_SendGameStat
=================
*/
void G_SendGameStat( team_t team )
{
  char      map[ MAX_STRING_CHARS ];
  char      teamChar;
  char      data[ BIG_INFO_STRING ];
  char      entry[ MAX_STRING_CHARS ];
  int       i, dataLength, entryLength;
  gclient_t *cl;

  trap_Cvar_VariableStringBuffer( "mapname", map, sizeof( map ) );

  switch( team )
  {
    case TEAM_ALIENS: teamChar = 'A'; break;
    case TEAM_HUMANS: teamChar = 'H'; break;
    case TEAM_NONE:   teamChar = 'L'; break;
    default: return;
  }

  Com_sprintf( data, BIG_INFO_STRING,
      "%s %s T:%c A:%f H:%f M:%s D:%d SD:%d ESD:%d AS:%d AS2T:%d AS3T:%d HS:%d HS2T:%d HS3T:%d CL:%d",
      Q3_VERSION,
      g_tag.string,
      teamChar,
      level.averageNumAlienClients,
      level.averageNumHumanClients,
      map,
      level.time - level.startTime,
      G_TimeTilSuddenDeath( ),
      G_TimeTilExtremeSuddenDeath( ),
      g_alienStage.integer,
      level.alienStage2Time - level.startTime,
      level.alienStage3Time - level.startTime,
      g_humanStage.integer,
      level.humanStage2Time - level.startTime,
      level.humanStage3Time - level.startTime,
      level.numConnectedClients );

  dataLength = strlen( data );

  for( i = 0; i < level.numConnectedClients; i++ )
  {
    int ping;

    cl = &level.clients[ level.sortedClients[ i ] ];

    if( cl->pers.connected == CON_CONNECTING )
      ping = -1;
    else
      ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

    switch( cl->ps.stats[ STAT_TEAM ] )
    {
      case TEAM_ALIENS: teamChar = 'A'; break;
      case TEAM_HUMANS: teamChar = 'H'; break;
      case TEAM_NONE:   teamChar = 'S'; break;
      default: return;
    }

    Com_sprintf( entry, MAX_STRING_CHARS,
      " \"%s\" %c %d %d %d",
      cl->pers.netname,
      teamChar,
      cl->ps.persistant[ PERS_KILLS ],
      ping,
      ( level.time - cl->pers.enterTime ) / 60000 );

    entryLength = strlen( entry );

    if( dataLength + entryLength >= BIG_INFO_STRING )
      break;

    strcpy( data + dataLength, entry );
    dataLength += entryLength;
  }

  trap_SendGameStat( data );
}

/*
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string )
{
  int         i, numSorted;
  gclient_t   *cl;
  gentity_t   *ent;

  G_LogPrintf( "Exit: %s\n", string );

  level.intermissionQueued = level.time;

  // this will keep the clients from playing any voice sounds
  // that will get cut off when the queued intermission starts
  trap_SetConfigstring( CS_INTERMISSION, "1" );

  // don't send more than 32 scores (FIXME?)
  numSorted = level.numConnectedClients;
  if( numSorted > 32 )
    numSorted = 32;

  for( i = 0; i < numSorted; i++ )
  {
    int   ping;

    cl = &level.clients[ level.sortedClients[ i ] ];

    if( cl->ps.stats[ STAT_TEAM ] == TEAM_NONE )
      continue;

    if( cl->pers.connected == CON_CONNECTING )
      continue;

    ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

    G_LogPrintf( "score: %i  ping: %i  client: %i %s\n",
      cl->ps.persistant[ PERS_KILLS ], ping, level.sortedClients[ i ],
      cl->pers.netname );

  }

  for( i = 1, ent = g_entities + i ; i < level.num_entities ; i++, ent++ )
  {
    if( !ent->inuse )
      continue;

    if( !Q_stricmp( ent->classname, "trigger_win" ) )
    {
      if( level.lastWin == ent->stageTeam )
        ent->use( ent, ent, ent );
    }
  }

  G_SendGameStat( level.lastWin );
}


/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void )
{
  int       ready, notReady;
  int       i;
  gclient_t *cl;
  byte      readyMasks[ ( MAX_CLIENTS + 7 ) / 8 ];\
  // each byte in readyMasks will become two characters 00 - ff in the string
  char      readyString[ 2 * sizeof( readyMasks ) + 1 ];

  //if no clients are connected, just exit
  if( level.numConnectedClients == 0 )
  {
    ExitLevel( );
    return;
  }

  // see which players are ready
  ready = 0;
  notReady = 0;
  Com_Memset( readyMasks, 0, sizeof( readyMasks ) );
  for( i = 0; i < g_maxclients.integer; i++ )
  {
    cl = level.clients + i;
    if( cl->pers.connected != CON_CONNECTED )
      continue;

    if( cl->ps.stats[ STAT_TEAM ] == TEAM_NONE )
      continue;

    if( cl->readyToExit )
    {
      ready++;
      // the nth bit of readyMasks is for client (n - 1)
      readyMasks[ i / 8 ] |= 1 << ( 7 - ( i % 8 ) );
    }
    else
      notReady++;
  }

  // this is hex because we can convert bits to a hex string in pieces, 
  // whereas a decimal string would have to all be written at once 
  // (and we can't fit a number that large in an int)
  for( i = 0; i < ( g_maxclients.integer + 7 ) / 8; i++ )
    Com_sprintf( &readyString[ i * 2 ], sizeof( readyString ) - i * 2,
                 "%2.2x", readyMasks[ i ] );

  trap_SetConfigstring( CS_CLIENTS_READY, readyString );

  // never exit in less than five seconds
  if( level.time < level.intermissiontime + 5000 )
    return;

  // never let intermission go on for over 1 minute
  if( level.time > level.intermissiontime + 60000 )
  {
    ExitLevel( );
    return;
  }

  // if nobody wants to go, clear timer
  if( ready == 0 && notReady > 0 )
  {
    level.readyToExit = qfalse;
    return;
  }

  // if everyone wants to go, go now
  if( notReady == 0 )
  {
    ExitLevel( );
    return;
  }

  // the first person to ready starts the thirty second timeout
  if( !level.readyToExit )
  {
    level.readyToExit = qtrue;
    level.exitTime = level.time;
  }

  // if we have waited thirty seconds since at least one player
  // wanted to exit, go ahead
  if( level.time < level.exitTime + 30000 )
    return;

  ExitLevel( );
}

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void )
{
  int   a, b;

  if( level.numPlayingClients < 2 )
    return qfalse;

  a = level.clients[ level.sortedClients[ 0 ] ].ps.persistant[ PERS_KILLS ];
  b = level.clients[ level.sortedClients[ 1 ] ].ps.persistant[ PERS_KILLS ];

  return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
void CheckExitRules( void )
{
  // don't exit in demos
  if( level.demoState == DS_PLAYBACK )
    return;

  // if at the intermission, wait for all non-bots to
  // signal ready, then go to next level
  if( level.intermissiontime )
  {
    CheckIntermissionExit( );
    return;
  }

  if( level.intermissionQueued )
  {
    if( level.time - level.intermissionQueued >= INTERMISSION_DELAY_TIME )
    {
      level.intermissionQueued = 0;
      BeginIntermission( );
    }

    return;
  }

  if( g_timelimit.integer && !level.warmupTime )
  {
    if( level.time - level.startTime >= g_timelimit.integer * 60000 )
    {
      level.lastWin = TEAM_NONE;
      trap_SendServerCommand( -1, "print \"Timelimit hit\n\"" );
      trap_SetConfigstring( CS_WINNER, "Stalemate" );
      LogExit( "Timelimit hit." );
      G_admin_maplog_result( "t" );
      return;
    }
    else if( level.time - level.startTime >= ( g_timelimit.integer - 5 ) * 60000 &&
          level.timelimitWarning < TW_IMMINENT )
    {
      trap_SendServerCommand( -1, "cp \"5 minutes remaining!\"" );
      level.timelimitWarning = TW_IMMINENT;
    }
    else if( level.time - level.startTime >= ( g_timelimit.integer - 1 ) * 60000 &&
          level.timelimitWarning < TW_PASSED )
    {
      trap_SendServerCommand( -1, "cp \"1 minute remaining!\"" );
      level.timelimitWarning = TW_PASSED;
    }
  }

  if( level.uncondHumanWin ||
      ( ( level.time > level.startTime + 1000 ) &&
        ( level.numAlienSpawns == 0 ) &&
        ( level.numLiveAlienClients == 0 ) ) )
  {
    //humans win
    level.lastWin = TEAM_HUMANS;
    trap_SendServerCommand( -1, "print \"Humans win\n\"");
    trap_SetConfigstring( CS_WINNER, "Humans Win" );
    LogExit( "Humans win." );
    G_admin_maplog_result( "h" );
  }
  else if( level.uncondAlienWin ||
           ( ( level.time > level.startTime + 1000 ) &&
             ( level.numHumanSpawns == 0 ) &&
             ( level.numLiveHumanClients == 0 ) ) )
  {
    //aliens win
    level.lastWin = TEAM_ALIENS;
    trap_SendServerCommand( -1, "print \"Aliens win\n\"");
    trap_SetConfigstring( CS_WINNER, "Aliens Win" );
    LogExit( "Aliens win." );
    G_admin_maplog_result( "a" );
  }
  else if( level.playerWin )
  {
    int i;
    level.lastWin = TEAM_NONE;
    if( level.numLiveAlienClients == 1 && level.numLiveHumanClients != 1 )
    {
      for( i=0; i<level.maxclients; i++ )
      {
        if( level.clients[ i ].pers.connected != CON_DISCONNECTED ||
             level.clients[ i ].pers.demoClient )
          continue;

        if( level.clients[ i ].pers.connected != CON_CONNECTED &&
           !level.clients[ i ].pers.demoClient )
          continue;

        if( level.clients[ i ].pers.teamSelection == TEAM_NONE )
          continue;
        else if( level.clients[ i ].pers.teamSelection == TEAM_HUMANS )
          continue;
        else if( level.clients[ i ].pers.teamSelection == TEAM_ALIENS )
        {
          if( level.clients[ i ].sess.spectatorState == SPECTATOR_NOT )
            break;
        }
      }
      trap_SendServerCommand( -1, va( "print \"Alien Player %s Wins \n\"", level.clients[ i ].pers.netname ) );
      trap_SetConfigstring( CS_WINNER, va( "%s Wins", level.clients[ i ].pers.netname ) );
      LogExit( va( "%s Wins", level.clients[ i ].pers.netname )  );
      G_admin_maplog_result( "t" );   //FIXME: Should have something special
    }
    else if( level.numLiveHumanClients == 1 && level.numLiveAlienClients != 1 )
    {
      for( i=0; i<level.maxclients; i++ )
      {
        if( level.clients[ i ].pers.connected != CON_DISCONNECTED ||
             level.clients[ i ].pers.demoClient )
          continue;

        if( level.clients[ i ].pers.connected != CON_CONNECTED &&
           !level.clients[ i ].pers.demoClient )
          continue;

        if( level.clients[ i ].pers.teamSelection == TEAM_NONE )
          continue;
        else if( level.clients[ i ].pers.teamSelection == TEAM_ALIENS )
          continue;
        else if( level.clients[ i ].pers.teamSelection == TEAM_HUMANS )
        {
          if( level.clients[ i ].sess.spectatorState == SPECTATOR_NOT )
            break;
        }
      }
      trap_SendServerCommand( -1, va( "print \"Human Player %s Wins \n\"", level.clients[ i ].pers.netname ) );
      trap_SetConfigstring( CS_WINNER, va( "%s Wins", level.clients[ i ].pers.netname ) );
      LogExit( va( "%s Wins", level.clients[ i ].pers.netname )  );
      G_admin_maplog_result( "t" ); //FIXME: Should have something special
    }
    else if( level.numLiveHumanClients == 1 && level.numLiveAlienClients == 1 )
    {
      trap_SendServerCommand( -1, "print \"Draw \n\"" );
      trap_SetConfigstring( CS_WINNER, "Draw" );
      LogExit( "Draw" );
      G_admin_maplog_result( "t" ); //FIXME: Should have something special
    }
    return;
  }
}

/*
==================
G_Vote
==================
*/
void G_Vote( gentity_t *ent, qboolean voting )
{
  if( !level.voteTime )
    return;

  if( voting )
  {
    if( ent->client->ps.eFlags & EF_VOTED )
      return;
    ent->client->ps.eFlags |= EF_VOTED;
  }
  else
  {
    if( !( ent->client->ps.eFlags & EF_VOTED ) )
      return;
    ent->client->ps.eFlags &= ~EF_VOTED;
  }

  if( ent->client->pers.vote )
  {
    if( voting )
      level.voteYes++;
    else
      level.voteYes--;
    trap_SetConfigstring( CS_VOTE_YES, va( "%d", level.voteYes ) );
  }
  else
  {
    if( voting )
      level.voteNo++;
    else
      level.voteNo--;
    trap_SetConfigstring( CS_VOTE_NO, va( "%d", level.voteNo ) );
  }
}

/*
==================
G_TeamVote
==================
*/
void G_TeamVote( gentity_t *ent, qboolean voting )
{
  int cs_offset;

  if( ent->client->pers.teamSelection == TEAM_HUMANS )
    cs_offset = 0;
  else if( ent->client->pers.teamSelection == TEAM_ALIENS )
    cs_offset = 1;
  else
    return;

  if( !level.teamVoteTime[ cs_offset ] )
    return;

  if( voting )
  {
    if( ent->client->ps.eFlags & EF_TEAMVOTED )
      return;
    ent->client->ps.eFlags |= EF_TEAMVOTED;
  }
  else
  {
    if( !( ent->client->ps.eFlags & EF_TEAMVOTED ) )
      return;
    ent->client->ps.eFlags &= ~EF_TEAMVOTED;
  }

  if( ent->client->pers.teamVote )
  {
    if( voting )
      level.teamVoteYes[ cs_offset ]++;
    else
      level.teamVoteYes[ cs_offset ]--;
    trap_SetConfigstring( CS_TEAMVOTE_YES + cs_offset,
      va( "%d", level.teamVoteYes[ cs_offset ] ) );
  }
  else
  {
    if( voting )
      level.teamVoteNo[ cs_offset ]++;
    else
      level.teamVoteNo[ cs_offset ]--;
    trap_SetConfigstring( CS_TEAMVOTE_NO + cs_offset,
      va( "%d", level.teamVoteNo[ cs_offset ] ) );
  }
}


/*
========================================================================

FUNCTIONS CALLED EVERY FRAME

========================================================================
*/


/*
==================
CheckVote
==================
*/
void CheckVote( void )
{
  int votePassThreshold=level.votePassThreshold;
  int voteYesPercent;

  if( level.voteExecuteTime && level.voteExecuteTime < level.time )
  {
    level.voteExecuteTime = 0;

    if( !Q_stricmp( level.voteString, "map_restart" ) )
      G_admin_maplog_result( "r" );
    else if( !Q_stricmpn( level.voteString, "map", 3 ) )
      G_admin_maplog_result( "m" );

    if( !Q_stricmp( level.voteString, "suddendeath" ) )
    {
      level.suddenDeathTime = level.time + ( 1000 * g_suddenDeathVoteDelay.integer ) - level.startTime;
      level.suddenDeathVote = qtrue;
      level.voteString[0] = '\0';
      if( G_TimeTilSuddenDeath( ) < 300000 )
      {
        level.suddenDeathWarning = TW_CLOSE;
      }
      else if( G_TimeTilSuddenDeath( ) < 60000 )
      {
        level.suddenDeathWarning = TW_IMMINENT;
      }
      if( g_suddenDeathVoteDelay.integer )
        trap_SendServerCommand( -1, va("print \"Sudden Death will begin in %d seconds\n\"", g_suddenDeathVoteDelay.integer  ) );
    }
    
    if( !Q_stricmp( level.voteString, "extremesuddendeath" ) || !Q_stricmp( level.voteString, "esd" ) || !Q_stricmp( level.voteString, "extreme_sudden_death" ) )
    {
      level.extremeSuddenDeathTime = level.time + ( 1000 * g_extremeSuddenDeathVoteDelay.integer ) - level.startTime;
      level.extremeSuddenDeathVote = qtrue;
      level.voteString[0] = '\0';
      if( G_TimeTilExtremeSuddenDeath( ) < 300000 )
      {
        level.extremeSuddenDeathWarning = TW_CLOSE;
      }
      else if( G_TimeTilExtremeSuddenDeath( ) < 60000 )
      {
        level.extremeSuddenDeathWarning = TW_IMMINENT;
      }
      if( g_extremeSuddenDeathVoteDelay.integer )
        trap_SendServerCommand( -1, va("print \"Extreme Sudden Death will begin in %d seconds\n\"", g_extremeSuddenDeathVoteDelay.integer  ) );
    }
    if( level.voteString[0] )
      trap_SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.voteString ) );

    if( !Q_stricmp( level.voteString, "map_restart" ) ||
        !Q_stricmpn( level.voteString, "map", 3 ) )
      level.restarted = qtrue;
  }

  if( !level.voteTime )
    return;

  if( level.voteYes + level.voteNo > 0 )
   voteYesPercent = (int)( 100 * (level.voteYes)/(level.voteYes + level.voteNo ) );
  else
   voteYesPercent = 0; 
    
  
  if( level.time - level.voteTime >= VOTE_TIME || ( level.voteYes + level.voteNo >= level.numVotingClients ) )
  {
    if( voteYesPercent > votePassThreshold )
    {   
      // execute the command, then remove the vote
      trap_SendServerCommand( -1, va("print \"^2Vote Passed ^7(^2Y:^7%i ^1N:^7%i, %i percent)\n\"", level.voteYes, level.voteNo, voteYesPercent ));
      level.voteExecuteTime = level.time + 3000;
    }
    else if( voteYesPercent <= votePassThreshold  )
    {
      // same behavior as a timeout
      trap_SendServerCommand( -1, va("print \"^1Vote Failed ^7(^2Y:^7%i ^1N:^7%i, %i percent)\n\"", level.voteYes, level.voteNo, voteYesPercent ));
    }
    else
    {
      // No one voted or indecisive
      trap_SendServerCommand( -1, va("print \"^1Vote Failed ^7(^2Y:^7%i ^1N:^7%i, %i percent)\n\"", level.voteYes, level.voteNo, voteYesPercent ));
    }
  }
  else if( Q_stricmpn( level.voteDisplayString, "[Poll]", 6 ) )
  {
     if( level.voteYes > ( level.numVotingClients * votePassThreshold/100 ) && voteYesPercent > votePassThreshold )
    {   
      // execute the command, then remove the vote
      trap_SendServerCommand( -1, va("print \"^2Vote Passed ^7(^2Y:^7%i ^1N:^7%i, %i percent)\n\"", level.voteYes, level.voteNo, voteYesPercent ));
      level.voteExecuteTime = level.time + 3000;
    }
    else if( level.voteNo >= ceil( (float)level.numVotingClients * votePassThreshold/100 ) && voteYesPercent <= votePassThreshold )
    {
      // same behavior as a timeout
      trap_SendServerCommand( -1, va("print \"^1Vote Failed ^7(^2Y:^7%i ^1N:^7%i, %i percent)\n\"", level.voteYes, level.voteNo, voteYesPercent ));
    }
    else
    {
      //Waiting for a majority
      return;
    }
  }
  else
    return;
    //Still waiting for a majority

  level.voteTime = 0;
  trap_SetConfigstring( CS_VOTE_TIME, "" );
  trap_SetConfigstring( CS_VOTE_STRING, "" );
}


/*
==================
CheckTeamVote
==================
*/
void CheckTeamVote( team_t team )
{
  int cs_offset, voteYesPercent, nays, yays;
  int votePassThreshold=50; //for now

  if( team == TEAM_HUMANS )
    cs_offset = 0;
  else if( team == TEAM_ALIENS )
    cs_offset = 1;
  else
    return;

  if( !level.teamVoteTime[ cs_offset ] )
    return;
    
  yays = level.teamVoteYes[ cs_offset ];
  nays = level.teamVoteNo[ cs_offset ];

  if( yays + nays > 0 )
    voteYesPercent = (int)( 100 * ( yays )/( yays + nays ) );
  else
    voteYesPercent = 0; 

  if( level.time - level.teamVoteTime[ cs_offset ] >= VOTE_TIME )
  {
    if( voteYesPercent > votePassThreshold )
    {
      G_TeamCommand( team, va( "print \"^2Team Vote Passed^7 (^2Y^7:%i, ^1N^7:%i, %i percent)\n\"",
                      yays, nays, voteYesPercent ) );
      trap_SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.teamVoteString[ cs_offset ] ) );
    }
    else
    {
      G_TeamCommand( team, va( "print \"^1Team Vote Failed^7 (^2Y^7:%i, ^1N^7:%i, %i percent)\n\"",
                      yays, nays, voteYesPercent ) );
    }
  }
  else if( yays + nays > 0 )
  {
    if( yays > level.numteamVotingClients[ cs_offset ] / 2 )
    {
      // execute the command, then remove the vote
      G_TeamCommand( team, va( "print \"^2Team Vote Passed^7 (^2Y^7:%i, ^1N^7:%i, %i percent)\n\"",
                      yays, nays, voteYesPercent ) );
      trap_SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.teamVoteString[ cs_offset ] ) );
    }
    else if( nays >= level.numteamVotingClients[ cs_offset ] / 2 )
    {
      G_TeamCommand( team, va( "print \"^1Team Vote Failed^7 (^2Y^7:%i, ^1N^7:%i, %i percent)\n\"",
                      yays, nays, voteYesPercent ) );
    }
    else
    {
      //Still waiting for a majority
      return;
    }
  }
  else
    return; //Still waiting for a vote

  level.teamVoteTime[ cs_offset ] = 0;
  trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset, "" );
  trap_SetConfigstring( CS_TEAMVOTE_STRING + cs_offset, "" );
}

/*
==================
CheckMsgTimer
==================
*/
void CheckMsgTimer( void )
{
  if( !g_msgTime.integer )
    return;

  if( level.time - level.lastMsgTime < abs( g_msgTime.integer ) * 60000 )
    return;

  // negative settings only print once per map
  if( ( level.lastMsgTime ) && g_msgTime.integer < 0 )
    return;

  level.lastMsgTime = level.time;

  if( g_msg.string[0] )
  {
    char buffer[ MAX_STRING_CHARS ];

    Q_strncpyz( buffer, g_msg.string, sizeof( buffer ) );
    G_ParseEscapedString( buffer );
    trap_SendServerCommand( -1, va( "cp \"%s\"", buffer ) );
    trap_SendServerCommand( -1, va( "print \"%s\n\"", buffer ) );
  }
}


/*
==================
CheckCountdown
==================
*/
void CheckCountdown( void )
{
  static int lastmsg = 0;
  static qboolean altcolor = qfalse;
  int timeleft = g_warmup.integer - ( level.time - level.startTime ) / 1000;
  char *leftarrows, *rightarrows;
  if( !g_doWarmup.integer || timeleft < 0 )
    return;

  if( level.time - lastmsg < 500 )
    return;
  lastmsg = level.time;
  if( timeleft > 0 && timeleft < 6)
  {
    switch( timeleft )
    {
      case 5:
        leftarrows=">>>>";
        rightarrows="<<<<";
        break;
      case 4:
        leftarrows=">>>";
        rightarrows="<<<";
        break;
      case 3:
        leftarrows=">>";
        rightarrows="<<";
        break;
      case 2:
        leftarrows=">";
        rightarrows="<";
        break;
      case 1:
      default:
        leftarrows="";
        rightarrows="";
        break;
    }

    if( !altcolor )
    {
      trap_SendServerCommand( -1, va( "cp \"^1Get Ready^7\n^2>%s ^7%i ^2%s<\"", leftarrows, timeleft, rightarrows ) );
      altcolor = qtrue;
    }
    else
    {
      trap_SendServerCommand( -1, va( "cp \"^1Get Ready^7\n^0>^2%s ^7%i ^2%s^0<\"", leftarrows, timeleft, rightarrows ) );
      altcolor = qfalse;
    }
  }
  else if( timeleft == 0 )
  {
    trap_SendServerCommand( -1, "cp \"^2<<<<< GO >>>>>^7\"" );
    level.warmupTime = 0;
  }
}

/*
==================
CheckCvars
==================
*/
void CheckCvars( void )
{
  static int lastPasswordModCount   = -1;
  static int lastMarkDeconModCount  = -1;
  static int lastSDTimeModCount = -1;

  if( g_password.modificationCount != lastPasswordModCount )
  {
    lastPasswordModCount = g_password.modificationCount;

    if( *g_password.string && Q_stricmp( g_password.string, "none" ) )
      trap_Cvar_Set( "g_needpass", "1" );
    else
      trap_Cvar_Set( "g_needpass", "0" );
  }

  // Unmark any structures for deconstruction when
  // the server setting is changed
  if( g_markDeconstruct.modificationCount != lastMarkDeconModCount )
  {
    int       i;
    gentity_t *ent;

    lastMarkDeconModCount = g_markDeconstruct.modificationCount;

    for( i = 1, ent = g_entities + i ; i < level.num_entities ; i++, ent++ )
    {
      if( !ent->inuse )
        continue;

      if( ent->s.eType != ET_BUILDABLE )
        continue;

      ent->deconstruct = qfalse;
    }
  }

  // If we change g_suddenDeathTime during a map, we need to update 
  // when sd will begin 
  if( g_suddenDeathTime.modificationCount != lastSDTimeModCount )
  {
    lastSDTimeModCount = g_suddenDeathTime.modificationCount;
    level.suddenDeathTime= g_suddenDeathTime.integer*60000;
  }

  level.frameMsec = trap_Milliseconds( );
}

/*
==================
CheckDemo
==================
*/
void CheckDemo( void )
{
  int i;

  // Don't do anything if no change
  if( g_demoState.integer == level.demoState )
    return;
  level.demoState = g_demoState.integer;

  // log all connected clients
  if( g_demoState.integer == DS_RECORDING )
  {
    for( i = 0; i < level.maxclients; i++ )
    {
      if( level.clients[ i ].pers.connected != CON_DISCONNECTED )
      {
        char userinfo[ MAX_INFO_STRING ];
        trap_GetConfigstring( CS_PLAYERS + i, userinfo, sizeof(userinfo) );
        G_DemoCommand( DC_CLIENT_SET, va( "%d %s", i, userinfo ) );
      }
    }
  }

  // empty teams and display a message
  else if( g_demoState.integer == DS_PLAYBACK )
  {
    trap_SendServerCommand( -1, "print \"A demo has been started on the server.\n\"" );
    for( i = 0; i < level.maxclients; i++ )
    {
      if( level.clients[ i ].pers.teamSelection != TEAM_NONE )
        G_ChangeTeam( g_entities + i, TEAM_NONE );
    }
  }

  // clear all demo clients
  if( g_demoState.integer == DS_NONE || g_demoState.integer == DS_PLAYBACK )
  {
    int clients = trap_Cvar_VariableIntegerValue( "sv_democlients" );
    for( i = 0; i < clients; i++ )
      trap_SetConfigstring( CS_PLAYERS + i, NULL );
  }
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink( gentity_t *ent )
{
  float thinktime;

  thinktime = ent->nextthink;
  if( thinktime <= 0 )
    return;

  if( thinktime > level.time )
    return;

  ent->nextthink = 0;
  if( !ent->think )
    G_Error( "NULL ent->think" );

  ent->think( ent );
}

/*
=============
G_EvaluateAcceleration

Calculates the acceleration for an entity
=============
*/
void G_EvaluateAcceleration( gentity_t *ent, int msec )
{
  vec3_t  deltaVelocity;
  vec3_t  deltaAccel;

  VectorSubtract( ent->s.pos.trDelta, ent->oldVelocity, deltaVelocity );
  VectorScale( deltaVelocity, 1.0f / (float)msec, ent->acceleration );

  VectorSubtract( ent->acceleration, ent->oldAccel, deltaAccel );
  VectorScale( deltaAccel, 1.0f / (float)msec, ent->jerk );

  VectorCopy( ent->s.pos.trDelta, ent->oldVelocity );
  VectorCopy( ent->acceleration, ent->oldAccel );
}

/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
void G_RunFrame( int levelTime )
{
  int       i;
  gentity_t *ent;
  int       msec;
  int       start, end;

  // if we are waiting for the level to restart, do nothing
  if( level.restarted )
    return;

  if( level.paused )
  {
    level.pausedTime = levelTime - level.time;
    if( ( level.pausedTime % 3000 ) == 0) 
      trap_SendServerCommand( -1, "cp \"The game has been paused. Please wait.\"" );

    for(i=0;i<level.maxclients;i++)
    {
      level.clients[ i ].ps.commandTime = levelTime;
    }

   return;
  }

  CheckMsgTimer( );
  CheckCountdown( );

  level.framenum++;
  level.previousTime = level.time;
  level.time = levelTime;
  msec = level.time - level.previousTime;

  // seed the rng
  srand( level.framenum );

  // get any cvar changes
  G_UpdateCvars( );

  // check demo state
  CheckDemo( );

  //
  // go through all allocated objects
  //
  start = trap_Milliseconds( );
  ent = &g_entities[ 0 ];

  for( i = 0; i < ( level.demoState == DS_PLAYBACK ? g_maxclients.integer : level.num_entities ); i++, ent++ )
  {
    if( !ent->inuse )
      continue;

    // clear events that are too old
    if( level.time - ent->eventTime > EVENT_VALID_MSEC )
    {
      if( ent->s.event )
      {
        ent->s.event = 0; // &= EV_EVENT_BITS;
        if ( ent->client )
        {
          ent->client->ps.externalEvent = 0;
          //ent->client->ps.events[0] = 0;
          //ent->client->ps.events[1] = 0;
        }
      }

      if( ent->freeAfterEvent )
      {
        // tempEntities or dropped items completely go away after their event
        G_FreeEntity( ent );
        continue;
      }
      else if( ent->unlinkAfterEvent )
      {
        // items that will respawn will hide themselves after their pickup event
        ent->unlinkAfterEvent = qfalse;
        trap_UnlinkEntity( ent );
      }
    }

    // temporary entities don't think
    if( ent->freeAfterEvent )
      continue;

    // calculate the acceleration of this entity
    if( ent->evaluateAcceleration )
      G_EvaluateAcceleration( ent, msec );

    if( !ent->r.linked && ent->neverFree )
      continue;

    if( ent->s.eType == ET_MISSILE )
    {
      G_RunMissile( ent );
      continue;
    }

    if( ent->s.eType == ET_BUILDABLE )
    {
      G_BuildableThink( ent, msec );
      continue;
    }

    if( ent->s.eType == ET_CORPSE || ent->physicsObject )
    {
      G_Physics( ent, msec );
      continue;
    }

    if( ent->s.eType == ET_MOVER )
    {
      G_RunMover( ent );
      continue;
    }

    if( i < MAX_CLIENTS )
    {
      G_RunClient( ent );
      continue;
    }

    G_RunThink( ent );
  }
  end = trap_Milliseconds();

  start = trap_Milliseconds();

  // perform final fixups on the players
  ent = &g_entities[ 0 ];

  for( i = 0; i < level.maxclients; i++, ent++ )
  {
    if( ent->inuse )
      ClientEndFrame( ent );
  }

  // save position information for all active clients
  G_UnlaggedStore( );

  end = trap_Milliseconds();

  G_CountSpawns( );
  G_CalculateBuildPoints( );
  G_CalculateStages( );
  G_SpawnClients( TEAM_ALIENS );
  G_SpawnClients( TEAM_HUMANS );
  G_CalculateAvgPlayers( );
  //G_UpdateZaps( msec );

  // see if it is time to end the level
  CheckExitRules( );

  // update to team status?
  CheckTeamStatus( );

  // cancel vote if timed out
  CheckVote( );

  // check team votes
  CheckTeamVote( TEAM_HUMANS );
  CheckTeamVote( TEAM_ALIENS );

  // for tracking changes
  CheckCvars( );

  if( g_listEntity.integer )
  {
    for( i = 0; i < MAX_GENTITIES; i++ )
      G_Printf( "%4i: %s\n", i, g_entities[ i ].classname );

    trap_Cvar_Set( "g_listEntity", "0" );
  }

  level.frameMsec = trap_Milliseconds();
}

