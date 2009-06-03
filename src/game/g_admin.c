/*
===========================================================================
Copyright (C) 2004-2006 Tony J. White

This file is part of Tremfusion.

This shrubbot implementation is the original work of Tony J. White.

Contains contributions from Wesley van Beelen, Chris Bajumpaa, Josh Menke,
and Travis Maurer.

The functionality of this code mimics the behaviour of the currently
inactive project shrubet (http://www.etstats.com/shrubet/index.php?ver=2)
by Ryan Mannion.   However, shrubet was a closed-source project and
none of it's code has been copied, only it's functionality.

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

// big ugly global buffer for use with buffered printing of long outputs
static char g_bfb[ 32000 ];

// note: list ordered alphabetically
g_admin_cmd_t g_admin_cmds[ ] = 
  {
    {"admintest", G_admin_admintest, "a",
      "display your current admin level",
      ""
    },

    {"listplayers", G_admin_listplayers, "a",
      "display a list of players, their client numbers and their levels",
      ""
    },

    {"info", G_admin_info, "a",
      "read the server's information files",
      "(^5subject^7)"
    },

    {"specme", G_admin_putmespec, "a",
        "moves you to the spectators",
				""
    },

    {"help", G_admin_help, "a",
      "display commands available to you or help on a specific command",
      "(^5command^7)"
    },
    
    {"listmaps", G_admin_listmaps, "a",
      "display a list of available maps on the server",
      "(^5map name^7)"
    },

    {"maplog", G_admin_maplog, "a",
      "show a list of recently played maps",
      "",
    },

    {"rotation", G_admin_listrotation, "a",
       "display a list of maps that are in the active map rotation",
       ""
    },
    
    {"register", G_admin_register, "a",
      "Registers your name to protect it from being used by others or updates your"      
      " admin name to your current name.",
      ""
    },    

    {"listadmins", G_admin_listadmins, "A",
      "display a list of all server admins and their levels",
      "(^5name|start admin#^7) (^5minimum level to display^7)"
    },

    {"subnetban", G_admin_subnetban, "b",
      "Add or change a subnet mask on a ban",
      "[^3ban#^7] [^5CIDR mask^7]"
      "\n ^3Example:^7 '!subnetban 10 16' changes ban #10 to be a ban on XXX.XXX.*.*"
      "\n ^3Example:^7 '!subnetban 10 24' changes ban #10 to be a ban on XXX.XXX.XXX.*"
      "\n ^3Example:^7 '!subnetban 10 32' changes ban #10 to be a regular (non-subnet) ban"
      "\n ^1WARNING:^7 Use of this command may make your admin.dat incompatible with other game.qvms"
    },

    {"ban", G_admin_ban, "b",
      "ban a player by IP and GUID with an optional expiration time and reason."
      "  time is specified as numbers followed by units 'w' (weeks), 'd' "
      "(days), 'h' (hours) or 'm' (minutes), or seconds if no units are "
      "specified",
      "[^3name|slot#|IP^7] (^5time^7) (^5reason^7)"
    },

    {"unban", G_admin_unban, "b",
      "unbans a player specified by the slot as seen in showbans",
      "[^3ban #^7]"
    },

    {"adjustban", G_admin_adjustban, "b",
      "change the length or reason of a ban.  time is specified as numbers "
      "followed by units 'w' (weeks), 'd' (days), 'h' (hours) or 'm' (minutes),"
      " or seconds if no units are specified",
      "[^5ban #^7] (^3length|reason^7)"
    },


    {"showbans", G_admin_showbans, "B",
      "display a (partial) list of active bans",
      "(^5start at ban#^7|name|IP)"
    },

    {"cancelvote", G_admin_cancelvote, "c",
      "cancel a vote taking place",
      ""
    },

    {"time", G_admin_time, "C",
      "show the current local server time",
      ""
    },


    {"allowbuild", G_admin_denybuild, "d",
      "restore a player's ability to build",
      "[^3name|slot#^7]"
    },

    {"denybuild", G_admin_denybuild, "d",
      "take away a player's ability to build",
      "[^3name|slot#^7]"
    },

    {"designate", G_admin_designate, "D",
      "give the player designated builder privileges",
      "[^3name|slot#^7]"
    },

    {"undesignate", G_admin_designate, "D",
      "revoke designated builder privileges",
      "[^3name|slot#^7]"
    },

    {"buildlog", G_admin_buildlog, "e",
      "display a list of recent builds and deconstructs, optionally specifying"
      " a team",
      "(^5starting log index^7)(^5a|h^7)",
    },
    
    {"revert", G_admin_revert, "E",
      "revert one or more buildlog events, optionally of only one team",
      "(^5number^7)(^5a|h^7)"
    },

    {"devmap", G_admin_devmap, "f",
      "load a map with cheats (and optionally force layout)",
      "[^3mapname^7] (^5layout^7)"
    },

    {"allready", G_admin_allready, "F",
      "makes everyone ready in intermission",
      ""
    },

    {"pause", G_admin_pause, "g",
      "buy some time",
      "(^5name|slot^7)"
    },

    {"unpause", G_admin_pause, "g",
      "carry on with the game",
      "(^5name|slot^7)"
    },

    {"lock", G_admin_lock, "G",
      "lock a team to prevent anyone from joining it",
      "[^3a|h^7]"
    },

    {"unlock", G_admin_unlock, "G",
      "unlock a locked team",
      "[^3a|h^7]"
    },

    {"unmute", G_admin_mute, "h",
      "unmute a muted player",
      "[^3name|slot#^7]"
    },

    {"mute", G_admin_mute, "h",
      "mute a player",
      "[^3name|slot#^7]"
    },

    {"report", G_admin_report, "H",
      "report abuse to the server",
      "[detailed but concise description]"
    },

    {"namelog", G_admin_namelog, "i",
      "display a list of names used by recently connected players",
      "(^5name^7)"
    },

    {"readconfig", G_admin_readconfig, "I",
      "reloads the admin config file and refreshes permission flags",
      ""
    },

    {"spec999", G_admin_spec999, "j",
      "move 999 pingers to the spectator team",
      ""},

    {"rename", G_admin_rename, "J",
      "rename a player",
      "[^3name|slot#^7] [^3new name^7]"
    },

    {"layoutsave", G_admin_layoutsave, "k",
      "save a map layout",
      "[^3mapname^7]"
    },
    
    {"listlayouts", G_admin_listlayouts, "k",
      "display a list of all available layouts for a map",
      "(^5mapname^7)"
    },

    {"kick", G_admin_kick, "K",
      "kick a player with an optional reason",
      "(^5reason^7)"
    },

    {"L0", G_admin_L0, "l",
      "Sets a level 1+ to level 0",
       "(^3name|slot#^7)"
    },
    
    {"L1", G_admin_L1, "l",
      "Sets a level 0 to level 1",
      "[^3name|slot#^7]"
    },

    {"L2", G_admin_L2, "l",
      "Sets a level 0 or 1 to level 2",
      "[^3name|slot#^7]"
    },

    {"map", G_admin_map, "L",
      "load a map (and optionally force layout)",
      "[^3mapname^7] (^5layout^7)"
    },

    {"nextmap", G_admin_nextmap, "L",
      "go to the next map in the cycle",
      ""
    },

    {"switch", G_admin_switch, "m",
      "switch places with someone",
      "[^3name|slot#^7]"
    },

    {"drug", G_admin_drug, "M",
      "induce a gas like effect on a player",
      "[^3name|slot#^7]"
    },

    {"speed", G_admin_speed, "n",
      "change the speed level",
      "[^3#^7]"
    },

    {"gravity", G_admin_gravity, "n",
      "change the gravity level",
      "[^3#^7]"
    },

    {"abps", G_admin_abps, "N",
      "change the build points for aliens",
      "[^3#^7]"
    },

    {"hbps", G_admin_hbps, "N",
      "change the build points for humans",
      "[^3#^7]"
    },
  
    {"allowoverride", G_admin_override, "O",
      "override checks for various things",
      "(^5name|slot^7)"
    },

    {"denyoverride", G_admin_override, "O",
      "deny override",
      "(^5name|slot^7)"
    }, 

    {"forcespec", G_admin_forcespec, "p",
      "disable joining of teams for a player",
      "[^3name|slot#^7]"
    },
 
    {"unforcespec", G_admin_unforcespec, "p",
      "enable joining of teams for a player",
      "[^3name|slot#^7]"
    },

    {"restart", G_admin_restart, "r",
      "restart the current map (optionally using named layout or "
      "keeping/switching teams)",
      "(^5layout^7) (^5keepteams|switchteams|keepteamslock|switchteamslock^7)"
    },

    {"passvote", G_admin_passvote, "R",
      "pass a vote currently taking place",
      ""
    },

    {"putteam", G_admin_putteam, "s",
      "move a player to a specified team",
      "[^3name|slot#^7] [^3h|a|s^7]"
    },

    {"slap", G_admin_slap, "S",
      "abuses your authoritay!",
      "[^3name|slot^7] (damage)"
    },

    {"warn", G_admin_warn, "t",
      "Warn a player to cease or face admin intervention",
      "[^3name|slot#^7] [reason]"
    },

    {"hstage", G_admin_hstage, "u",
      "change the stage for humans",
      "[^3#^7]"
    },

    {"astage", G_admin_astage, "u",
      "change the stage for aliens",
      "[^3#^7]"
    },

    {"kill", G_admin_kill, "v",
      "Kill a player.",
      "[^3name|slot^7]"
    },
    
    {"detonate", G_admin_detonate, "v",
      "make a player explode",
      "[^3name|slot#^7]"
    },
  
    {"fireworks", G_admin_fireworks, "v",
      "Play the ending animation for the current map",
      "[^3a|h^7]"
    },

    {"setlevel", G_admin_setlevel, "Z",
      "sets the admin level of a player",
      "[^3name|slot#|admin#^7] [^3level^7]"
    },
    
    {"print",G_admin_print2, "Z",
			"prints text",
			"[text to print]"
    },
      
    {"cp", G_admin_cp, "Z",
      "display a CP message to users, optionally specifying team(s) to send to",
      "(-AHS) [^3message^7]"
    }

  };
static int adminNumCmds = sizeof( g_admin_cmds ) / sizeof( g_admin_cmds[ 0 ] );

static int admin_level_maxname = 0;
g_admin_level_t *g_admin_levels[ MAX_ADMIN_LEVELS ];
g_admin_admin_t *g_admin_admins[ MAX_ADMIN_ADMINS ];
g_admin_ban_t *g_admin_bans[ MAX_ADMIN_BANS ];
g_admin_command_t *g_admin_commands[ MAX_ADMIN_COMMANDS ];
g_admin_namelog_t *g_admin_namelog[ MAX_ADMIN_NAMELOGS ];

// This function should only be used directly when the client is connecting and thus has no GUID.
// Else, use G_admin_permission() 
qboolean G_admin_permission_guid( char *guid, char flag )
{
  int i;
  int l = 0;
  char *flags;

  if(!guid) return qfalse; // since there is a different check for console, here we are just returning false.

  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( !Q_stricmp( guid, g_admin_admins[ i ]->guid ) )
    {
      flags = g_admin_admins[ i ]->flags;
      while( *flags )
      {
        if( *flags == flag )
          return qtrue;
        else if( *flags == '-' )
        {
          while( *flags++ )
          {
            if( *flags == flag )
              return qfalse;
            else if( *flags == '+' )
              break;
          }
        }
        else if( *flags == '*' )
        {
          while( *flags++ )
          {
            if( *flags == flag )
              return qfalse;
          }
          // flags with significance only for individuals (
          // like ADMF_INCOGNITO and ADMF_IMMUTABLE are NOT covered 
          // by the '*' wildcard.  They must be specified manually.
          switch( flag )
          {
          case ADMF_INCOGNITO:
          case ADMF_IMMUTABLE:
          case ADMF_DBUILDER:
            return qfalse; 
          default:
            return qtrue;
          } 
        }
        flags++;
      }
      l = g_admin_admins[ i ]->level;
    }
  }
  for( i = 0; i < MAX_ADMIN_LEVELS && g_admin_levels[ i ]; i++ )
  {
    if( g_admin_levels[ i ]->level == l )
    {
      flags = g_admin_levels[ i ]->flags;
      while( *flags )
      {
        if( *flags == flag )
          return qtrue;
        if( *flags == '*' )
        {
          while( *flags++ )
          {
            if( *flags == flag )
              return qfalse;
          }
          // flags with significance only for individuals (
          // like ADMF_INCOGNITO and ADMF_IMMUTABLE are NOT covered 
          // by the '*' wildcard.  They must be specified manually.
          switch( flag )
          {
          case ADMF_INCOGNITO:
          case ADMF_IMMUTABLE:
          case ADMF_DBUILDER:
            return qfalse; 
          default:
            return qtrue;
          } 
        }
        flags++;
      }
    }
  }
  return qfalse;
}

qboolean G_admin_permission( gentity_t *ent, char flag )
{
  int i;
  int l = 0;
  char *flags;

  // console always wins
  if( !ent )
    return qtrue;

  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( !Q_stricmp( ent->client->pers.guid, g_admin_admins[ i ]->guid ) )
    {
      flags = g_admin_admins[ i ]->flags;
      while( *flags )
      {
        if( *flags == flag )
          return qtrue;
        else if( *flags == '-' )
        {
          while( *flags++ )
          {
            if( *flags == flag )
              return qfalse;
            if( *flags == '+' )
              break;
          }
        }
        else if( *flags == '*' )
        {
          while( *flags++ )
          {
            if( *flags == flag )
              return qfalse;
          }
          // flags with significance only for individuals (
          // like ADMF_INCOGNITO and ADMF_IMMUTABLE are NOT covered
          // by the '*' wildcard.  They must be specified manually.
          switch( flag )
          {
          case ADMF_INCOGNITO:
          case ADMF_IMMUTABLE:
          case ADMF_DBUILDER:
            return qfalse; 
          default:
            return qtrue;
          }
        }
        flags++;
      }
      l = g_admin_admins[ i ]->level;
    }
  }
  for( i = 0; i < MAX_ADMIN_LEVELS && g_admin_levels[ i ]; i++ )
  {
    if( g_admin_levels[ i ]->level == l )
    {
      flags = g_admin_levels[ i ]->flags;
      while( *flags )
      {
        if( *flags == flag )
          return qtrue;
        if( *flags == '*' )
        {
          while( *flags++ )
          {
            if( *flags == flag )
              return qfalse;
          }
          // flags with significance only for individuals (
          // like ADMF_INCOGNITO and ADMF_IMMUTABLE are NOT covered
          // by the '*' wildcard.  They must be specified manually.
          switch( flag )
          {
          case ADMF_INCOGNITO:
          case ADMF_IMMUTABLE:
          case ADMF_DBUILDER:
            return qfalse; 
          default:
            return qtrue;
          } 
        }
        flags++;
      }
    }
  }
  return qfalse;
}

qboolean G_admin_name_check( gentity_t *ent, char *name, char *err, int len )
{
  int i;
  gclient_t *client;
  char testName[ MAX_NAME_LENGTH ] = {""};
  char name2[ MAX_NAME_LENGTH ] = {""};

  G_SanitiseString( name, name2, sizeof( name2 ) );

  if( !Q_stricmp( name2, "UnnamedPlayer" ) )
    return qtrue;

  for( i = 0; i < level.maxclients; i++ )
  {
    client = &level.clients[ i ];
    if( client->pers.connected == CON_DISCONNECTED )
      continue;

    // can rename ones self to the same name using different colors
    if( i == ( ent - g_entities ) )
      continue;

    G_SanitiseString( client->pers.netname, testName, sizeof( testName ) );
    if( !Q_stricmp( name2, testName ) )
    {
      Com_sprintf( err, len, "The name '%s^7' is already in use", name );
      return qfalse;
    }
  }

  if( !g_adminNameProtect.integer )
    return qtrue;

  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( g_admin_admins[ i ]->level < 1 )
      continue;
    G_SanitiseString( g_admin_admins[ i ]->name, testName, sizeof( testName ) );
    if( !Q_stricmp( name2, testName ) &&
      Q_stricmp( ent->client->pers.guid, g_admin_admins[ i ]->guid ) )
    {
      Com_sprintf( err, len, "The name '%s^7' belongs to an admin, "
        "please use another name", name );
      return qfalse;
    }
  }
  return qtrue;
}

static qboolean admin_higher_guid( char *admin_guid, char *victim_guid )
{
  int i;
  int alevel = 0;

  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( !Q_stricmp( admin_guid, g_admin_admins[ i ]->guid ) )
    {
      alevel = g_admin_admins[ i ]->level;
      break;
    }
  }
  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( !Q_stricmp( victim_guid, g_admin_admins[ i ]->guid ) )
    {
      if( alevel < g_admin_admins[ i ]->level )
        return qfalse;
      return !strstr( g_admin_admins[ i ]->flags, va( "%c", ADMF_IMMUTABLE ) );
    }
  }
  return qtrue;
}

static qboolean admin_higher( gentity_t *admin, gentity_t *victim )
{

  // console always wins
  if( !admin )
    return qtrue;
  // just in case
  if( !victim )
    return qtrue;

  return admin_higher_guid( admin->client->pers.guid,
    victim->client->pers.guid );
}

static void admin_writeconfig_string( char *s, fileHandle_t f )
{
  char buf[ MAX_STRING_CHARS ];

  buf[ 0 ] = '\0';
  if( s[ 0 ] )
  {
    //Q_strcat(buf, sizeof(buf), s);
    Q_strncpyz( buf, s, sizeof( buf ) );
    trap_FS_Write( buf, strlen( buf ), f );
  }
  trap_FS_Write( "\n", 1, f );
}

static void admin_writeconfig_int( int v, fileHandle_t f )
{
  char buf[ 32 ];

  Com_sprintf( buf, sizeof(buf), "%d", v );
  trap_FS_Write( buf, strlen( buf ), f );
  trap_FS_Write( "\n", 1, f );
}

static void admin_writeconfig( void )
{
  fileHandle_t f;
  int len, i, j;
  int t;
  char levels[ MAX_STRING_CHARS ] = {""};

  if( !g_admin.string[ 0 ] )
  {
    G_Printf( S_COLOR_YELLOW "WARNING: g_admin is not set. "
      " configuration will not be saved to a file.\n" );
    return;
  }
  t = trap_RealTime( NULL );
  len = trap_FS_FOpenFile( g_admin.string, &f, FS_WRITE );
  if( len < 0 )
  {
    G_Printf( "admin_writeconfig: could not open g_admin file \"%s\"\n",
              g_admin.string );
    return;
  }
  for( i = 0; i < MAX_ADMIN_LEVELS && g_admin_levels[ i ]; i++ )
  {
    trap_FS_Write( "[level]\n", 8, f );
    trap_FS_Write( "level   = ", 10, f );
    admin_writeconfig_int( g_admin_levels[ i ]->level, f );
    trap_FS_Write( "name    = ", 10, f );
    admin_writeconfig_string( g_admin_levels[ i ]->name, f );
    trap_FS_Write( "flags   = ", 10, f );
    admin_writeconfig_string( g_admin_levels[ i ]->flags, f );
    trap_FS_Write( "\n", 1, f );
  }
  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    // don't write level 0 users
    if( g_admin_admins[ i ]->level == 0 )
      continue;

    trap_FS_Write( "[admin]\n", 8, f );
    trap_FS_Write( "name    = ", 10, f );
    admin_writeconfig_string( g_admin_admins[ i ]->name, f );
    trap_FS_Write( "guid    = ", 10, f );
    admin_writeconfig_string( g_admin_admins[ i ]->guid, f );
    trap_FS_Write( "level   = ", 10, f );
    admin_writeconfig_int( g_admin_admins[ i ]->level, f );
    trap_FS_Write( "flags   = ", 10, f );
    admin_writeconfig_string( g_admin_admins[ i ]->flags, f );
    trap_FS_Write( "\n", 1, f );
  }
  for( i = 0; i < MAX_ADMIN_BANS && g_admin_bans[ i ]; i++ )
  {
    // don't write expired bans
    // if expires is 0, then it's a perm ban
    if( g_admin_bans[ i ]->expires != 0 &&
      ( g_admin_bans[ i ]->expires - t ) < 1 )
      continue;

    trap_FS_Write( "[ban]\n", 6, f );
    trap_FS_Write( "name    = ", 10, f );
    admin_writeconfig_string( g_admin_bans[ i ]->name, f );
    trap_FS_Write( "guid    = ", 10, f );
    admin_writeconfig_string( g_admin_bans[ i ]->guid, f );
    trap_FS_Write( "ip      = ", 10, f );
    admin_writeconfig_string( g_admin_bans[ i ]->ip, f );
    trap_FS_Write( "reason  = ", 10, f );
    admin_writeconfig_string( g_admin_bans[ i ]->reason, f );
    trap_FS_Write( "made    = ", 10, f );
    admin_writeconfig_string( g_admin_bans[ i ]->made, f );
    trap_FS_Write( "expires = ", 10, f );
    admin_writeconfig_int( g_admin_bans[ i ]->expires, f );
    trap_FS_Write( "banner  = ", 10, f );
    admin_writeconfig_string( g_admin_bans[ i ]->banner, f );
    trap_FS_Write( "\n", 1, f );
  }
  for( i = 0; i < MAX_ADMIN_COMMANDS && g_admin_commands[ i ]; i++ )
  {
    levels[ 0 ] = '\0';
    trap_FS_Write( "[command]\n", 10, f );
    trap_FS_Write( "command = ", 10, f );
    admin_writeconfig_string( g_admin_commands[ i ]->command, f );
    trap_FS_Write( "exec    = ", 10, f );
    admin_writeconfig_string( g_admin_commands[ i ]->exec, f );
    trap_FS_Write( "desc    = ", 10, f );
    admin_writeconfig_string( g_admin_commands[ i ]->desc, f );
    trap_FS_Write( "levels  = ", 10, f );
    for( j = 0; g_admin_commands[ i ]->levels[ j ] != -1; j++ )
    {
      Q_strcat( levels, sizeof( levels ),
                va( "%i ", g_admin_commands[ i ]->levels[ j ] ) );
    }
    admin_writeconfig_string( levels, f );
    trap_FS_Write( "\n", 1, f );
  }
  trap_FS_FCloseFile( f );
}

static void admin_readconfig_string( char **cnf, char *s, int size )
{
  char *t;

  //COM_MatchToken(cnf, "=");
  s[ 0 ] = '\0';
  t = COM_ParseExt( cnf, qfalse );
  if( strcmp( t, "=" ) )
  {
    COM_ParseWarning( "expected '=' before \"%s\"", t );
    Q_strncpyz( s, t, size );
  }
  while( 1 )
  {
    t = COM_ParseExt( cnf, qfalse );
    if( !*t )
      break;
    if( strlen( t ) + strlen( s ) >= size )
      break;
    if( *s )
      Q_strcat( s, size, " " );
    Q_strcat( s, size, t );
  }
}

static void admin_readconfig_int( char **cnf, int *v )
{
  char *t;

  //COM_MatchToken(cnf, "=");
  t = COM_ParseExt( cnf, qfalse );
  if( !strcmp( t, "=" ) )
  {
    t = COM_ParseExt( cnf, qfalse );
  }
  else
  {
    COM_ParseWarning( "expected '=' before \"%s\"", t );
  }
  *v = atoi( t );
}

// if we can't parse any levels from readconfig, set up default
// ones to make new installs easier for admins
static void admin_default_levels( void )
{
  g_admin_level_t *l;
  int i;

  for( i = 0; i < MAX_ADMIN_LEVELS && g_admin_levels[ i ]; i++ )
  {
    BG_Free( g_admin_levels[ i ] );
    g_admin_levels[ i ] = NULL;
  }
  for( i = 0; i <= 5; i++ )
  {
    l = BG_Alloc( sizeof( g_admin_level_t ) );
    l->level = i;
    *l->name = '\0';
    *l->flags = '\0';
    g_admin_levels[ i ] = l;
  }
  Q_strncpyz( g_admin_levels[ 0 ]->name, "^4Unknown Player",
    sizeof( l->name ) );
  Q_strncpyz( g_admin_levels[ 0 ]->flags, "iahC", sizeof( l->flags ) );

  Q_strncpyz( g_admin_levels[ 1 ]->name, "^5Server Regular",
    sizeof( l->name ) );
  Q_strncpyz( g_admin_levels[ 1 ]->flags, "iahC", sizeof( l->flags ) );

  Q_strncpyz( g_admin_levels[ 2 ]->name, "^6Team Manager",
    sizeof( l->name ) );
  Q_strncpyz( g_admin_levels[ 2 ]->flags, "iahCpP", sizeof( l->flags ) );

  Q_strncpyz( g_admin_levels[ 3 ]->name, "^2Junior Admin",
    sizeof( l->name ) );
  Q_strncpyz( g_admin_levels[ 3 ]->flags, "iahCpPkm$?", sizeof( l->flags ) );

  Q_strncpyz( g_admin_levels[ 4 ]->name, "^3Senior Admin",
    sizeof( l->name ) );
  Q_strncpyz( g_admin_levels[ 4 ]->flags, "iahCpPkmBbe$?", sizeof( l->flags ) );

  Q_strncpyz( g_admin_levels[ 5 ]->name, "^1Server Operator",
    sizeof( l->name ) );
  Q_strncpyz( g_admin_levels[ 5 ]->flags, "*", sizeof( l->flags ) );
  admin_level_maxname = 15;
}

//  return a level for a player entity.
int G_admin_level( gentity_t *ent )
{
  int i;

  if( !ent )
  {
    return MAX_ADMIN_LEVELS;
  }

  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( !Q_stricmp( g_admin_admins[ i ]->guid, ent->client->pers.guid ) )
      return g_admin_admins[ i ]->level;
  }

  return 0;
}

//  set a player's adminname
void G_admin_set_adminname( gentity_t *ent )
{
  int i;
  qboolean found = qfalse;

  if( !ent )
  {
    return;
  }

  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( !Q_stricmp( g_admin_admins[ i ]->guid, ent->client->pers.guid ) )
    {
      found = qtrue;
      break;
    }
  }
 
  if( found )
  {
     Q_strncpyz( ent->client->pers.adminName, g_admin_admins[ i ]->name, sizeof( ent->client->pers.adminName ) );
  }
  else
  {
     Q_strncpyz( ent->client->pers.adminName, "", sizeof( ent->client->pers.adminName ) );
  }
}

char* G_admin_adminPrintName( gentity_t *ent )
{
  char *out;

  if( !ent->client->pers.adminLevel )
  {
    out = "";
    return out;
  }
  
  if( G_admin_permission( ent, ADMF_ADMINSTEALTH ) )
  {
     out = ent->client->pers.adminName;
  }
  else
  {
     out = ent->client->pers.netname;
  }  


  return out;
}

static qboolean admin_command_permission( gentity_t *ent, char *command )
{
  int i, j;
  int level;

  if( !ent )
    return qtrue;
  level = ent->client->pers.adminLevel;
  for( i = 0; i < MAX_ADMIN_COMMANDS && g_admin_commands[ i ]; i++ )
  {
    if( !Q_stricmp( command, g_admin_commands[ i ]->command ) )
    {
      for( j = 0; g_admin_commands[ i ]->levels[ j ] != -1; j++ )
      {
        if( g_admin_commands[ i ]->levels[ j ] == level )
        {
          return qtrue;
        }
      }
    }
  }
  return qfalse;
}

static void admin_log( gentity_t *admin, char *cmd, int skiparg )
{
  fileHandle_t f;
  int len, i, j;
  char string[ MAX_STRING_CHARS ], decoloured[ MAX_STRING_CHARS ];
  int min, tens, sec;
  g_admin_admin_t *a;
  g_admin_level_t *l;
  char flags[ MAX_ADMIN_FLAGS * 2 ];
  gentity_t *victim = NULL;
  int pids[ MAX_CLIENTS ];
  char name[ MAX_NAME_LENGTH ];

  if( !g_adminLog.string[ 0 ] )
    return;


  len = trap_FS_FOpenFile( g_adminLog.string, &f, FS_APPEND );
  if( len < 0 )
  {
    G_Printf( "admin_log: error could not open %s\n", g_adminLog.string );
    return;
  }

  sec = level.time / 1000;
  min = sec / 60;
  sec -= min * 60;
  tens = sec / 10;
  sec -= tens * 10;

  *flags = '\0';
  if( admin )
  {
    for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
    {
      if( !Q_stricmp( g_admin_admins[ i ]->guid , admin->client->pers.guid ) )
      {

        a = g_admin_admins[ i ];
        Q_strncpyz( flags, a->flags, sizeof( flags ) );
        for( j = 0; j < MAX_ADMIN_LEVELS && g_admin_levels[ j ]; j++ )
        {
          if( g_admin_levels[ j ]->level == a->level )
          {
            l = g_admin_levels[ j ];
            Q_strcat( flags, sizeof( flags ), l->flags );
            break;
          }
        }
        break;
      }
    }
  }

  if( G_SayArgc() > 1 + skiparg )
  {
    G_SayArgv( 1 + skiparg, name, sizeof( name ) );
    if( G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) == 1 )
    {
      victim = &g_entities[ pids[ 0 ] ];
    }
  }

  if( victim && Q_stricmp( cmd, "attempted" ) )
  {
    Com_sprintf( string, sizeof( string ),
                 "%3i:%i%i: %i: %s: %s (%s): %s: %s: %s: %s: \"%s\"\n",
                 min,
                 tens,
                 sec,
                 ( admin ) ? admin->s.clientNum : -1,
                 ( admin ) ? admin->client->pers.guid
                 : "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
                 ( admin ) ? admin->client->pers.netname : "console",
                 ( admin ) ? admin->client->pers.adminName : "console",
                 flags,
                 cmd,
                 victim->client->pers.guid,
                 victim->client->pers.netname,
                 G_SayConcatArgs( 2 + skiparg ) );
  }
  else
  {
    Com_sprintf( string, sizeof( string ),
                 "%3i:%i%i: %i: %s: %s (%s): %s: %s: \"%s\"\n",
                 min,
                 tens,
                 sec,
                 ( admin ) ? admin->s.clientNum : -1,
                 ( admin ) ? admin->client->pers.guid
                 : "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
                 ( admin ) ? admin->client->pers.netname : "console",
                 ( admin ) ? admin->client->pers.adminName : "console",
                 flags,
                 cmd,
                 G_SayConcatArgs( 1 + skiparg ) );
  }

  if( g_decolourLogfiles.integer )
  {
    G_DecolorString( string, decoloured, sizeof( decoloured ) );
    trap_FS_Write( decoloured, strlen( decoloured ), f );
  }
  else
  {
     trap_FS_Write( string, strlen( string ), f );
  }

  if ( !Q_stricmp( cmd, "attempted" ) )
  {
    Com_sprintf( string, sizeof( string ),
                 "%s^7 (%i) %s: %s",
                 ( admin ) ? admin->client->pers.netname : "console",
                 ( admin ) ? admin->s.clientNum : -1,
                 cmd,
                 G_SayConcatArgs( 1 + skiparg ) );
    G_AdminMessage( "[SERVER]", "%s", string );
  }
  trap_FS_FCloseFile( f );
  G_LogPrintf("Admin Command: %s^7 (%s): %s %s\n",( admin ) ? admin->client->pers.netname : "console", ( admin ) ? admin->client->pers.adminName : "console", cmd, G_SayConcatArgs( 1 + skiparg ));
}

static int admin_listadmins( gentity_t *ent, int start, char *search )
{
  int drawn = 0;
  char guid_stub[9];
  char name[ MAX_NAME_LENGTH ] = {""};
  char name2[ MAX_NAME_LENGTH ] = {""};
  char lname[ MAX_NAME_LENGTH ] = {""};
  int i, j;
  gentity_t *vic;
  int l = 0;
  qboolean dup = qfalse;

  ADMBP_begin();

  // print out all connected players regardless of level if name searching
  for( i = 0; i < level.maxclients && search[ 0 ]; i++ )
  {
    vic = &g_entities[ i ];

    if( vic->client && vic->client->pers.connected != CON_CONNECTED )
      continue;

    l = vic->client->pers.adminLevel;

    G_SanitiseString( vic->client->pers.netname, name, sizeof( name ) );
    if( !strstr( name, search ) )
      continue;

    for( j = 0; j < 8; j++ )
      guid_stub[ j ] = vic->client->pers.guid[ j + 24 ];
    guid_stub[ j ] = '\0';

    lname[ 0 ] = '\0';
    for( j = 0; j < MAX_ADMIN_LEVELS && g_admin_levels[ j ]; j++ )
    {
      if( g_admin_levels[ j ]->level == l )
      {
        int k, colorlen;

        for( colorlen = k = 0; g_admin_levels[ j ]->name[ k ]; k++ )
          if( Q_IsColorString( &g_admin_levels[ j ]->name[ k ] ) )
            colorlen += 2;
        Com_sprintf( lname, sizeof( lname ), "%*s",
                     admin_level_maxname + colorlen,
                     g_admin_levels[ j ]->name );
        break;
      }
    }
    ADMBP( va( "%4i %4i %s^7 (*%s) %s^7\n",
      i,
      l,
      lname,
      guid_stub,
      vic->client->pers.netname ) );
    drawn++;
  }

  for( i = start; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ] &&
    drawn < MAX_ADMIN_LISTITEMS; i++ )
  {
    if( search[ 0 ] )
    {
      G_SanitiseString( g_admin_admins[ i ]->name, name, sizeof( name ) );
      if( !strstr( name, search ) )
        continue;

      // verify we don't have the same guid/name pair in connected players
      // since we don't want to draw the same player twice
      dup = qfalse;
      for( j = 0; j < level.maxclients; j++ )
      {
        vic = &g_entities[ j ];
        if( !vic->client || vic->client->pers.connected != CON_CONNECTED )
          continue;
        G_SanitiseString( vic->client->pers.netname, name2, sizeof( name2 ) );
        if( !Q_stricmp( vic->client->pers.guid, g_admin_admins[ i ]->guid ) &&
          strstr( name2, search ) )
        {
          dup = qtrue;
          break;
        }
      }
      if( dup )
        continue;
    }
    for( j = 0; j < 8; j++ )
      guid_stub[ j ] = g_admin_admins[ i ]->guid[ j + 24 ];
    guid_stub[ j ] = '\0';

    lname[ 0 ] = '\0';
    for( j = 0; j < MAX_ADMIN_LEVELS && g_admin_levels[ j ]; j++ )
    {
      if( g_admin_levels[ j ]->level == g_admin_admins[ i ]->level )
      {
        int k, colorlen;

        for( colorlen = k = 0; g_admin_levels[ j ]->name[ k ]; k++ )
          if( Q_IsColorString( &g_admin_levels[ j ]->name[ k ] ) )
            colorlen += 2;
        Com_sprintf( lname, sizeof( lname ), "%*s",
                     admin_level_maxname + colorlen,
                     g_admin_levels[ j ]->name );
        break;
      }
    }
    ADMBP( va( "%4i %4i %s^7 (*%s) %s^7\n",
      ( i + MAX_CLIENTS ),
      g_admin_admins[ i ]->level,
      lname,
      guid_stub,
      g_admin_admins[ i ]->name ) );
    drawn++;
  }
  ADMBP_end();
  return drawn;
}

void G_admin_duration( int secs, char *duration, int dursize )
{

  if( secs > ( 60 * 60 * 24 * 365 * 50 ) || secs < 0 )
    Q_strncpyz( duration, "PERMANENT", dursize );
  else if( secs >= ( 60 * 60 * 24 * 365 ) )
    Com_sprintf( duration, dursize, "%1.1f years",
      ( secs / ( 60 * 60 * 24 * 365.0f ) ) );
  else if( secs >= ( 60 * 60 * 24 * 90 ) )
    Com_sprintf( duration, dursize, "%1.1f weeks",
      ( secs / ( 60 * 60 * 24 * 7.0f ) ) );
  else if( secs >= ( 60 * 60 * 24 ) )
    Com_sprintf( duration, dursize, "%1.1f days",
      ( secs / ( 60 * 60 * 24.0f ) ) );
  else if( secs >= ( 60 * 60 ) )
    Com_sprintf( duration, dursize, "%1.1f hours",
      ( secs / ( 60 * 60.0f ) ) );
  else if( secs >= 60 )
    Com_sprintf( duration, dursize, "%1.1f minutes",
      ( secs / 60.0f ) );
  else
    Com_sprintf( duration, dursize, "%i seconds", secs );
}
qboolean G_admin_ban_check( char *userinfo, char *reason, int rlen )
{
  static char lastConnectIP[ 16 ] = {""};
  static int lastConnectTime = 0;
  char guid[ 33 ];
  char ip[ 16 ];
  char *value;
  int i;
  int userIP = 0, intIP = 0, IP[5], k, tempIP, mask, ipscanfcount;
  int t;
  char notice[51];
  qboolean ignoreIP = qfalse;
  
  trap_Cvar_VariableStringBuffer( "g_banNotice", notice, sizeof( notice ) );
 
  *reason = '\0'; 
  
  if( !*userinfo )
    return qfalse;
  
  value = Info_ValueForKey( userinfo, "ip" );
  Q_strncpyz( ip, value, sizeof( ip ) );
  // strip port
  value = strchr( ip, ':' );
  if ( value )
    *value = '\0';
  
  if( !*ip )
    return qfalse;
  
  value = Info_ValueForKey( userinfo, "cl_guid" );
  Q_strncpyz( guid, value, sizeof( guid ) );
  
  t = trap_RealTime( NULL );
  sscanf(ip, "%i.%i.%i.%i", &IP[4], &IP[3], &IP[2], &IP[1]);
  for(k = 4; k >= 1; k--)
  {
    if(!IP[k]) continue;
    userIP |= IP[k] << 8*(k-1);
  }
  ignoreIP = G_admin_permission_guid( guid , 'W');
  for( i = 0; i < MAX_ADMIN_BANS && g_admin_bans[ i ]; i++ )
  {
    // 0 is for perm ban
    if( g_admin_bans[ i ]->expires != 0 &&
         ( g_admin_bans[ i ]->expires - t ) < 1 )
      continue;
    if( !ignoreIP )
    {
      tempIP = userIP;
      intIP = 0;
      mask = -1;

      ipscanfcount = sscanf(g_admin_bans[ i ]->ip, "%d.%d.%d.%d/%d", &IP[4], &IP[3], &IP[2], &IP[1], &IP[0]);

      if(ipscanfcount==5) mask = IP[0]; 
      else mask = -1;

      for(k = 4; k >= 1; k--)
      {
        if(!IP[k]) continue;
        intIP |= IP[k] << 8*(k-1);
      }

      if(mask > 0 && mask <= 32) 
      {
        tempIP &= ~((1 << (32-mask)) - 1); // FIXME: can overflow
        intIP &= ~((1 << (32-mask)) - 1);
      }

      if( intIP == tempIP || mask == 0 )
      {
        char duration[ 32 ];
        G_admin_duration( ( g_admin_bans[ i ]->expires - t ),
          duration, sizeof( duration ) );

        // flood protected
        if( t - lastConnectTime >= 300 ||
            Q_stricmp( lastConnectIP, ip ) )
        {
          lastConnectTime = t;
          Q_strncpyz( lastConnectIP, ip, sizeof( lastConnectIP ) );

          G_AdminsPrintf(
            "Banned player %s^7 (%s^7) tried to connect (ban #%i on %s by %s^7 expires %s reason: %s^7 )\n",
            Info_ValueForKey( userinfo, "name" ),
            g_admin_bans[ i ]->name,
            i+1,
            ip, 
            g_admin_bans[ i ]->banner,
            duration,
            g_admin_bans[ i ]->reason );
        }
            
        Com_sprintf(
          reason,
          rlen,
          "You have been banned by %s^7 reason: %s^7 expires: %s       %s",
          g_admin_bans[ i ]->banner,
          g_admin_bans[ i ]->reason,
          duration,
          notice
          );
        G_LogPrintf("Banned player tried to connect from IP %s\n", ip);
        return qtrue;
      }
    }
    if( *guid && !Q_stricmp( g_admin_bans[ i ]->guid, guid ) )
    {
      char duration[ 32 ];
      G_admin_duration( ( g_admin_bans[ i ]->expires - t ),
        duration, sizeof( duration ) );
      Com_sprintf(
        reason,
        rlen,
        "You have been banned by %s^7 reason: %s^7 expires: %s",
        g_admin_bans[ i ]->banner,
        g_admin_bans[ i ]->reason,
        duration
      );
      G_Printf("Banned player tried to connect with GUID %s\n", guid);
      return qtrue;
    }
  }
  return qfalse;
}
qboolean G_admin_cmd_check( gentity_t *ent, qboolean say )
{
  int i;
  char command[ MAX_ADMIN_CMD_LEN ];
  char *cmd;
  int skip = 0;

  command[ 0 ] = '\0';
  G_SayArgv( 0, command, sizeof( command ) );
  if( !command[ 0 ] )
    return qfalse;
  if( !Q_stricmp( command, "say" ) ||
    ( !Q_stricmp( command, "say_team" ) &&
      G_admin_permission( ent, ADMF_TEAMCHAT_CMD ) ) )
  {
    skip = 1;
    G_SayArgv( 1, command, sizeof( command ) );
  }

  if( command[ 0 ] == '!' )
  {
    cmd = &command[ 1 ];
  }
  else
  {
    return qfalse;
  }

  for( i = 0; i < MAX_ADMIN_COMMANDS && g_admin_commands[ i ]; i++ )
  {
    if( Q_stricmp( cmd, g_admin_commands[ i ]->command ) )
      continue;

    if( admin_command_permission( ent, cmd ) )
    {
      // flooding say will have already been accounted for in ClientCommand
      if( !say && G_FloodLimited( ent ) )
        return qtrue;
      trap_SendConsoleCommand( EXEC_APPEND, g_admin_commands[ i ]->exec );
      admin_log( ent, cmd, skip );
    }
    else
    {
      ADMP( va( "^3!%s: ^7permission denied\n", g_admin_commands[ i ]->command ) );
      admin_log( ent, "attempted", skip - 1 );
    }
    return qtrue;
  }

  for( i = 0; i < adminNumCmds; i++ )
  {
    if( Q_stricmp( cmd, g_admin_cmds[ i ].keyword ) )
      continue;

    if( G_admin_permission( ent, g_admin_cmds[ i ].flag[ 0 ] ) )
    {
      // flooding say will have already been accounted for in ClientCommand
      if( !say && G_FloodLimited( ent ) )
        return qtrue;
      g_admin_cmds[ i ].handler( ent, skip );
      admin_log( ent, cmd, skip );
    }
    else
    {
      ADMP( va( "^3!%s: ^7permission denied\n", g_admin_cmds[ i ].keyword ) );
      admin_log( ent, "attempted", skip - 1 );
    }
    return qtrue;
  }
  return qfalse;
}

void G_admin_namelog_cleanup( )
{
  int i;

  for( i = 0; i < MAX_ADMIN_NAMELOGS && g_admin_namelog[ i ]; i++ )
  {
    BG_Free( g_admin_namelog[ i ] );
    g_admin_namelog[ i ] = NULL;
  }
}

void G_admin_namelog_update( gclient_t *client, qboolean disconnect )
{
  int i, j;
  g_admin_namelog_t *namelog;
  char n1[ MAX_NAME_LENGTH ];
  char n2[ MAX_NAME_LENGTH ];
  int clientNum = ( client - level.clients );

  G_SanitiseString( client->pers.netname, n1, sizeof( n1 ) );
  for( i = 0; i < MAX_ADMIN_NAMELOGS && g_admin_namelog[ i ]; i++ )
  {
    if( disconnect && g_admin_namelog[ i ]->slot != clientNum )
      continue;

    if( !disconnect && !( g_admin_namelog[ i ]->slot == clientNum ||
                          g_admin_namelog[ i ]->slot == -1 ) )
    {
      continue;
    }

    if( !Q_stricmp( client->pers.ip, g_admin_namelog[ i ]->ip ) &&
      !Q_stricmp( client->pers.guid, g_admin_namelog[ i ]->guid ) )
    {
      for( j = 0; j < MAX_ADMIN_NAMELOG_NAMES &&
         g_admin_namelog[ i ]->name[ j ][ 0 ]; j++ )
      {
        G_SanitiseString( g_admin_namelog[ i ]->name[ j ], n2, sizeof( n2 ) );
        if( !Q_stricmp( n1, n2 ) )
          break;
      }
      if( j == MAX_ADMIN_NAMELOG_NAMES )
        j = MAX_ADMIN_NAMELOG_NAMES - 1;
      Q_strncpyz( g_admin_namelog[ i ]->name[ j ], client->pers.netname,
        sizeof( g_admin_namelog[ i ]->name[ j ] ) );
      g_admin_namelog[ i ]->slot = ( disconnect ) ? -1 : clientNum;

      // if this player is connecting, they are no longer banned
      if( !disconnect )
        g_admin_namelog[ i ]->banned = qfalse;

      return;
    }
  }
  if( i >= MAX_ADMIN_NAMELOGS )
  {
    G_Printf( "G_admin_namelog_update: warning, g_admin_namelogs overflow\n" );
    return;
  }
  namelog = BG_Alloc( sizeof( g_admin_namelog_t ) );
  memset( namelog, 0, sizeof( namelog ) );
  for( j = 0; j < MAX_ADMIN_NAMELOG_NAMES; j++ )
    namelog->name[ j ][ 0 ] = '\0';
  Q_strncpyz( namelog->ip, client->pers.ip, sizeof( namelog->ip ) );
  Q_strncpyz( namelog->guid, client->pers.guid, sizeof( namelog->guid ) );
  Q_strncpyz( namelog->name[ 0 ], client->pers.netname,
    sizeof( namelog->name[ 0 ] ) );
  namelog->slot = ( disconnect ) ? -1 : clientNum;
  g_admin_namelog[ i ] = namelog;
}

qboolean G_admin_readconfig( gentity_t *ent, int skiparg )
{
  g_admin_level_t *l = NULL;
  g_admin_admin_t *a = NULL;
  g_admin_ban_t *b = NULL;
  g_admin_command_t *c = NULL;
  int lc = 0, ac = 0, bc = 0, cc = 0;
  fileHandle_t f;
  int len;
  char *cnf, *cnf2;
  char *t;
  qboolean level_open, admin_open, ban_open, command_open;
  int i;

  G_admin_cleanup();

  if( !g_admin.string[ 0 ] )
  {
    ADMP( "^3!readconfig: g_admin is not set, not loading configuration "
      "from a file\n" );
    admin_default_levels();
    return qfalse;
  }

  len = trap_FS_FOpenFile( g_admin.string, &f, FS_READ );
  if( len < 0 )
  {
    G_Printf( "^3!readconfig: ^7could not open admin config file %s\n",
            g_admin.string );
    admin_default_levels();
    return qfalse;
  }
  cnf = BG_Alloc( len + 1 );
  cnf2 = cnf;
  trap_FS_Read( cnf, len, f );
  *( cnf + len ) = '\0';
  trap_FS_FCloseFile( f );

  admin_level_maxname = 0;

  level_open = admin_open = ban_open = command_open = qfalse;
  COM_BeginParseSession( g_admin.string );
  while( 1 )
  {
    t = COM_Parse( &cnf );
    if( !*t )
      break;

    if( !Q_stricmp( t, "[level]" ) )
    {
      if( lc >= MAX_ADMIN_LEVELS )
        return qfalse;
      l = BG_Alloc( sizeof( g_admin_level_t ) );
      g_admin_levels[ lc++ ] = l;
      level_open = qtrue;
      admin_open = ban_open = command_open = qfalse;
    }
    else if( !Q_stricmp( t, "[admin]" ) )
    {
      if( ac >= MAX_ADMIN_ADMINS )
        return qfalse;
      a = BG_Alloc( sizeof( g_admin_admin_t ) );
      g_admin_admins[ ac++ ] = a;
      admin_open = qtrue;
      level_open = ban_open = command_open = qfalse;
    }
    else if( !Q_stricmp( t, "[ban]" ) )
    {
      if( bc >= MAX_ADMIN_BANS )
        return qfalse;
      b = BG_Alloc( sizeof( g_admin_ban_t ) );
      g_admin_bans[ bc++ ] = b;
      ban_open = qtrue;
      level_open = admin_open = command_open = qfalse;
    }
    else if( !Q_stricmp( t, "[command]" ) )
    {
      if( cc >= MAX_ADMIN_COMMANDS )
        return qfalse;
      c = BG_Alloc( sizeof( g_admin_command_t ) );
      g_admin_commands[ cc++ ] = c;
      c->levels[ 0 ] = -1;
      command_open = qtrue;
      level_open = admin_open = ban_open = qfalse;
    }
    else if( level_open )
    {
      if( !Q_stricmp( t, "level" ) )
      {
        admin_readconfig_int( &cnf, &l->level );
      }
      else if( !Q_stricmp( t, "name" ) )
      {
        admin_readconfig_string( &cnf, l->name, sizeof( l->name ) );
      }
      else if( !Q_stricmp( t, "flags" ) )
      {
        admin_readconfig_string( &cnf, l->flags, sizeof( l->flags ) );
      }
      else
      {
        COM_ParseError( "[level] unrecognized token \"%s\"", t );
      }
    }
    else if( admin_open )
    {
      if( !Q_stricmp( t, "name" ) )
      {
        admin_readconfig_string( &cnf, a->name, sizeof( a->name ) );
      }
      else if( !Q_stricmp( t, "guid" ) )
      {
        admin_readconfig_string( &cnf, a->guid, sizeof( a->guid ) );
      }
      else if( !Q_stricmp( t, "level" ) )
      {
        admin_readconfig_int( &cnf, &a->level );
      }
      else if( !Q_stricmp( t, "flags" ) )
      {
        admin_readconfig_string( &cnf, a->flags, sizeof( a->flags ) );
      }
      else
      {
        COM_ParseError( "[admin] unrecognized token \"%s\"", t );
      }

    }
    else if( ban_open )
    {
      if( !Q_stricmp( t, "name" ) )
      {
        admin_readconfig_string( &cnf, b->name, sizeof( b->name ) );
      }
      else if( !Q_stricmp( t, "guid" ) )
      {
        admin_readconfig_string( &cnf, b->guid, sizeof( b->guid ) );
      }
      else if( !Q_stricmp( t, "ip" ) )
      {
        admin_readconfig_string( &cnf, b->ip, sizeof( b->ip ) );
      }
      else if( !Q_stricmp( t, "reason" ) )
      {
        admin_readconfig_string( &cnf, b->reason, sizeof( b->reason ) );
      }
      else if( !Q_stricmp( t, "made" ) )
      {
        admin_readconfig_string( &cnf, b->made, sizeof( b->made ) );
      }
      else if( !Q_stricmp( t, "expires" ) )
      {
        admin_readconfig_int( &cnf, &b->expires );
      }
      else if( !Q_stricmp( t, "banner" ) )
      {
        admin_readconfig_string( &cnf, b->banner, sizeof( b->banner ) );
      }
      else
      {
        COM_ParseError( "[ban] unrecognized token \"%s\"", t );
      }
    }
    else if( command_open )
    {
      if( !Q_stricmp( t, "command" ) )
      {
        admin_readconfig_string( &cnf, c->command, sizeof( c->command ) );
      }
      else if( !Q_stricmp( t, "exec" ) )
      {
        admin_readconfig_string( &cnf, c->exec, sizeof( c->exec ) );
      }
      else if( !Q_stricmp( t, "desc" ) )
      {
        admin_readconfig_string( &cnf, c->desc, sizeof( c->desc ) );
      }
      else if( !Q_stricmp( t, "levels" ) )
      {
        char levels[ MAX_STRING_CHARS ] = {""};
        char *level = levels;
        char *lp;
        int cmdlevel = 0;

        admin_readconfig_string( &cnf, levels, sizeof( levels ) );
        while( cmdlevel < MAX_ADMIN_LEVELS )
        {
          lp = COM_Parse( &level );
          if( !*lp )
            break;
          c->levels[ cmdlevel++ ] = atoi( lp );
        }
        // ensure the list is -1 terminated
        c->levels[ cmdlevel ] = -1;
      }
      else
      {
        COM_ParseError( "[command] unrecognized token \"%s\"", t );
      }
    }
    else
    {
      COM_ParseError( "unexpected token \"%s\"", t );
    }
  }
  BG_Free( cnf2 );
  ADMP( va( "^3!readconfig: ^7loaded %d levels, %d admins, %d bans, %d commands\n",
          lc, ac, bc, cc ) );
  if( lc == 0 )
    admin_default_levels();
  else
  {
    // max printable name length for formatting
    for( i = 0; i < MAX_ADMIN_LEVELS && g_admin_levels[ i ]; i++ )
    {
      len = Q_PrintStrlen( l->name );
      if( len > admin_level_maxname )
        admin_level_maxname = len;
    }
  }
  // reset adminLevel
  for( i = 0; i < level.maxclients; i++ )
    if( level.clients[ i ].pers.connected != CON_DISCONNECTED )
      level.clients[ i ].pers.adminLevel = G_admin_level( &g_entities[ i ] );
  return qtrue;
}

qboolean G_admin_time( gentity_t *ent, int skiparg )
{
  qtime_t qt;

  trap_RealTime( &qt );
  ADMP( va( "^3!time: ^7local time is %02i:%02i:%02i\n",
    qt.tm_hour, qt.tm_min, qt.tm_sec ) );
  return qtrue;
}

qboolean G_admin_setlevel( gentity_t *ent, int skiparg )
{
  char name[ MAX_NAME_LENGTH ] = {""};
  char lstr[ 11 ]; // 10 is max strlen() for 32-bit int
  char adminname[ MAX_NAME_LENGTH ] = {""};
  char testname[ MAX_NAME_LENGTH ] = {""};
  char guid[ 33 ];
  int l, i;
  gentity_t *vic = NULL;
  qboolean updated = qfalse;
  g_admin_admin_t *a;
  qboolean found = qfalse;
  qboolean numeric = qtrue;
  int matches = 0;
  int id = -1;


  if( G_SayArgc() < 3 + skiparg )
  {
    ADMP( "^3!setlevel: ^7usage: !setlevel [name|slot#] [level]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, testname, sizeof( testname ) );
  G_SayArgv( 2 + skiparg, lstr, sizeof( lstr ) );
  l = atoi( lstr );
  G_SanitiseString( testname, name, sizeof( name ) );
  for( i = 0; i < sizeof( name ) && name[ i ]; i++ )
  {
    if( !isdigit( name[ i ] ) )
    {
      numeric = qfalse;
      break;
    }
  }
  if( numeric )
    id = atoi( name );

  if( ent && l > ent->client->pers.adminLevel )
  {
    ADMP( "^3!setlevel: ^7you may not use !setlevel to set a level higher "
      "than your current level\n" );
    return qfalse;
  }

  // if admin is activated for the first time on a running server, we need
  // to ensure at least the default levels get created
  if( !ent && !g_admin_levels[ 0 ] )
    G_admin_readconfig(NULL, 0);

  for( i = 0; i < MAX_ADMIN_LEVELS && g_admin_levels[ i ]; i++ )
  {
    if( g_admin_levels[ i ]->level == l )
    {
      found = qtrue;
      break;
    }
  }
  if( !found )
  {
    ADMP( "^3!setlevel: ^7level is not defined\n" );
    return qfalse;
  }

  if( numeric && id >= 0 && id < level.maxclients )
    vic = &g_entities[ id ];

  if( vic && vic->client && vic->client->pers.connected != CON_DISCONNECTED )
  {
    Q_strncpyz( adminname, vic->client->pers.netname, sizeof( adminname ) );
    Q_strncpyz( guid, vic->client->pers.guid, sizeof( guid ) );
    matches = 1;
  }
  else if( numeric && id >= MAX_CLIENTS && id < MAX_CLIENTS + MAX_ADMIN_ADMINS
    && g_admin_admins[ id - MAX_CLIENTS ] )
  {
    Q_strncpyz( adminname, g_admin_admins[ id - MAX_CLIENTS ]->name,
      sizeof( adminname ) );
    Q_strncpyz( guid, g_admin_admins[ id - MAX_CLIENTS ]->guid,
      sizeof( guid ) );
    matches = 1;
  }
  else
  {
    for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ] && matches < 2; i++ )
    {
      G_SanitiseString( g_admin_admins[ i ]->name, testname, sizeof( testname ) );
      if( strstr( testname, name ) )
      {
        Q_strncpyz( adminname, g_admin_admins[ i ]->name, sizeof( adminname ) );
        Q_strncpyz( guid, g_admin_admins[ i ]->guid, sizeof( guid ) );
        matches++;
      }
    }
    for( i = 0; i < level.maxclients && matches < 2; i++ )
    {
      if( level.clients[ i ].pers.connected == CON_DISCONNECTED )
        continue;
      if( matches && !Q_stricmp( level.clients[ i ].pers.guid, guid ) )
      {
        vic = &g_entities[ i ];
        continue;
      }
      G_SanitiseString( level.clients[ i ].pers.netname, testname,
        sizeof( testname ) );
      if( strstr( testname, name ) )
      {
        vic = &g_entities[ i ];
        matches++;
        Q_strncpyz( guid, vic->client->pers.guid, sizeof( guid ) );
      }
    }
    if( vic )
      Q_strncpyz( adminname, vic->client->pers.netname, sizeof( adminname ) );
  }

  if( matches == 0 )
  {
    ADMP( "^3!setlevel:^7 no match.  use !listplayers or !listadmins to "
      "find an appropriate number to use instead of name.\n" );
    return qfalse;
  }
  if( matches > 1 )
  {
    ADMP( "^3!setlevel:^7 more than one match.  Use the admin number "
      "instead:\n" );
    admin_listadmins( ent, 0, name );
    return qfalse;
  }

  if( !Q_stricmp( guid, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" ) )
  {
    ADMP( va( "^3!setlevel: ^7%s does not have a valid GUID\n", adminname ) );
    return qfalse;
  }
  if( ent && !admin_higher_guid( ent->client->pers.guid, guid ) )
  {
    ADMP( "^3!setlevel: ^7sorry, but your intended victim has a higher"
        " admin level than you\n" );
    return qfalse;
  }

  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ];i++ )
  {
    if( !Q_stricmp( g_admin_admins[ i ]->guid, guid ) )
    {
      g_admin_admins[ i ]->level = l;
      Q_strncpyz( g_admin_admins[ i ]->name, adminname,
                  sizeof( g_admin_admins[ i ]->name ) );
      updated = qtrue;
    }
  }
  if( !updated )
  {
    if( i == MAX_ADMIN_ADMINS )
    {
      ADMP( "^3!setlevel: ^7too many admins\n" );
      return qfalse;
    }
    a = BG_Alloc( sizeof( g_admin_admin_t ) );
    a->level = l;
    Q_strncpyz( a->name, adminname, sizeof( a->name ) );
    Q_strncpyz( a->guid, guid, sizeof( a->guid ) );
    *a->flags = '\0';
    g_admin_admins[ i ] = a;
  }

  AP( va(
    "print \"^3!setlevel: ^7%s^7 was given level %d admin rights by %s\n\"",
    adminname, l, ( ent ) ? ent->client->pers.netname : "console" ) );
  if( vic )
    vic->client->pers.adminLevel = l;
    G_admin_set_adminname( vic );

  if( !g_admin.string[ 0 ] )
    ADMP( "^3!setlevel: ^7WARNING g_admin not set, not saving admin record "
      "to a file\n" );
  else
    admin_writeconfig();
  return qtrue;
}

static qboolean admin_create_ban( gentity_t *ent,
  char *netname,
  char *guid,
  char *ip,
  int seconds,
  char *reason )
{
  g_admin_ban_t *b = NULL;
  qtime_t qt;
  int t;
  int i;

  t = trap_RealTime( &qt );
  b = BG_Alloc( sizeof( g_admin_ban_t ) );

  if( !b )
    return qfalse;

  Q_strncpyz( b->name, netname, sizeof( b->name ) );
  Q_strncpyz( b->guid, guid, sizeof( b->guid ) );
  Q_strncpyz( b->ip, ip, sizeof( b->ip ) );

  //strftime( b->made, sizeof( b->made ), "%m/%d/%y %H:%M:%S", lt );
  Com_sprintf( b->made, sizeof( b->made ), "%02i/%02i/%02i %02i:%02i:%02i",
    qt.tm_mon + 1, qt.tm_mday, qt.tm_year % 100,
    qt.tm_hour, qt.tm_min, qt.tm_sec );

  if( ent )
    Q_strncpyz( b->banner, ent->client->pers.netname, sizeof( b->banner ) );
  else
    Q_strncpyz( b->banner, "console", sizeof( b->banner ) );
  if( !seconds )
    b->expires = 0;
  else
    b->expires = t + seconds;
  if( !*reason )
    Q_strncpyz( b->reason, "banned by admin", sizeof( b->reason ) );
  else
    Q_strncpyz( b->reason, reason, sizeof( b->reason ) );
  for( i = 0; i < MAX_ADMIN_BANS && g_admin_bans[ i ]; i++ )
    ;
  if( i == MAX_ADMIN_BANS )
  {
    ADMP( "^3!ban: ^7too many bans\n" );
    BG_Free( b );
    return qfalse;
  }
  g_admin_bans[ i ] = b;
  return qtrue;
}

int G_admin_parse_time( const char *time )
{
  int seconds = 0, num = 0;
  while( *time )
  {
    if( !isdigit( *time ) )
      return -1;
    while( isdigit( *time ) )
      num = num * 10 + *time++ - '0';

    if( !*time )
      break;
    switch( *time++ )
    {
      case 'w': num *= 7;
      case 'd': num *= 24;
      case 'h': num *= 60;
      case 'm': num *= 60;
      case 's': break;
      default:  return -1;
    }
    seconds += num;
    num = 0;
  }
  if( num )
    seconds += num;
  return seconds;
}

qboolean G_admin_kick( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], *reason, err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;
  char notice[51];

  trap_Cvar_VariableStringBuffer( "g_banNotice", notice, sizeof( notice ) );

  minargc = 3 + skiparg;
  if( G_admin_permission( ent, ADMF_UNACCOUNTABLE ) )
    minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!kick: ^7usage: !kick [name] [reason]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  reason = G_SayConcatArgs( 2 + skiparg );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!kick: ^7%s\n", err ) );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];
  if( !admin_higher( ent, vic ) )
  {
    ADMP( "^3!kick: ^7sorry, but your intended victim has a higher admin"
        " level than you\n" );
    return qfalse;
  }
  if( vic->client->pers.localClient )
  {
    ADMP( "^3!kick: ^7disconnecting the host would end the game\n" );
    return qfalse;
  }
  admin_create_ban( ent,
    vic->client->pers.netname,
    vic->client->pers.guid,
    vic->client->pers.ip,
    G_admin_parse_time( va( "1s%s", g_adminTempBan.string ) ),
    ( *reason ) ? reason : "kicked by admin" );
  if( g_admin.string[ 0 ] )
    admin_writeconfig();

  trap_SendServerCommand( pids[ 0 ],
    va( "disconnect \"You have been kicked.\n%s^7\nreason:\n%s\n%s\"",
      ( ent ) ? va( "admin:\n%s", G_admin_adminPrintName( ent ) ) : "admin\nconsole",
      ( *reason ) ? reason : "kicked by admin", notice ) );

  trap_DropClient( pids[ 0 ], va( "has been kicked%s^7. reason: %s",
    ( ent ) ? va( " by %s", ent->client->pers.netname ) : "",
    ( *reason ) ? reason : "kicked by admin" ) );

  return qtrue;
}

qboolean G_admin_ban( gentity_t *ent, int skiparg )
{
  int seconds;
  char search[ MAX_NAME_LENGTH ];
  char secs[ MAX_TOKEN_CHARS ];
  char *reason;
  int minargc;
  char duration[ 32 ];
  int logmatch = -1, logmatches = 0;
  int i, j;
  qboolean exactmatch = qfalse;
  char n2[ MAX_NAME_LENGTH ];
  char s2[ MAX_NAME_LENGTH ];
  char guid_stub[ 9 ];
  char notice[51];

  trap_Cvar_VariableStringBuffer( "g_banNotice", notice, sizeof( notice ) );

  if( G_admin_permission( ent, ADMF_CAN_PERM_BAN ) &&
       G_admin_permission( ent, ADMF_UNACCOUNTABLE ) )
  {
    minargc = 2 + skiparg;
  }
  else if( ( G_admin_permission( ent, ADMF_CAN_PERM_BAN ) || g_adminMaxBan.integer ) ||
            G_admin_permission( ent, ADMF_UNACCOUNTABLE ) )
  {
    minargc = 3 + skiparg;
  }
  else
  {
    minargc = 4 + skiparg;
  }
  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!ban: ^7usage: !ban [name|slot|ip] [duration] [reason]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, search, sizeof( search ) );
  G_SanitiseString( search, s2, sizeof( s2 ) );
  G_SayArgv( 2 + skiparg, secs, sizeof( secs ) );

  seconds = G_admin_parse_time( secs );
  if( seconds <= 0 )
  {
    if( g_adminMaxBan.integer && !G_admin_permission( ent, ADMF_CAN_PERM_BAN ) )
    {
      ADMP( va( "^3!ban: ^7using your admin level's maximum ban length of %s\n",
        g_adminMaxBan.string ) );
      seconds = G_admin_parse_time( g_adminMaxBan.string );
    }
    else if( G_admin_permission( ent, ADMF_CAN_PERM_BAN ) )
    {
      seconds = 0;
    }
    else
    {
      ADMP( "^3!ban: ^7ban time must be positive\n" );
      return qfalse;
    }
    reason = G_SayConcatArgs( 2 + skiparg );
  }
  else
  {
    reason = G_SayConcatArgs( 3 + skiparg );

    if( g_adminMaxBan.integer &&
        seconds > G_admin_parse_time( g_adminMaxBan.string ) &&
        !G_admin_permission( ent, ADMF_CAN_PERM_BAN ) )
    {
      seconds = G_admin_parse_time( g_adminMaxBan.string );
      ADMP( va( "^3!ban: ^7ban length limited to %s for your admin level\n",
        g_adminMaxBan.string ) );
    }
  }

  for( i = 0; i < MAX_ADMIN_NAMELOGS && g_admin_namelog[ i ]; i++ )
  {
    // skip players in the namelog who have already been banned
    if( g_admin_namelog[ i ]->banned )
      continue;

    // skip disconnected players when banning on slot number
    if( g_admin_namelog[ i ]->slot == -1 )
      continue;

    if( !Q_stricmp( va( "%d", g_admin_namelog[ i ]->slot ), search ) )
    {
      logmatches = 1;
      logmatch = i;
      exactmatch = qtrue;
      break;
    }
  }

  for( i = 0;
       !exactmatch && i < MAX_ADMIN_NAMELOGS && g_admin_namelog[ i ];
       i++ )
  {
    // skip players in the namelog who have already been banned
    if( g_admin_namelog[ i ]->banned )
      continue;

    if( !Q_stricmp( g_admin_namelog[ i ]->ip, search ) )
    {
      logmatches = 1;
      logmatch = i;
      exactmatch = qtrue;
      break;
    }
    for( j = 0; j < MAX_ADMIN_NAMELOG_NAMES &&
       g_admin_namelog[ i ]->name[ j ][ 0 ]; j++ )
    {
      G_SanitiseString( g_admin_namelog[ i ]->name[ j ], n2, sizeof( n2 ) );
      if( strstr( n2, s2 ) )
      {
        if( logmatch != i )
          logmatches++;
        logmatch = i;
      }
    }
  }

  if( !logmatches )
  {
    ADMP( "^3!ban: ^7no player found by that name, IP, or slot number\n" );
    return qfalse;
  }
  if( logmatches > 1 )
  {
    ADMBP_begin();
    ADMBP( "^3!ban: ^7multiple recent clients match name, use IP or slot#:\n" );
    for( i = 0; i < MAX_ADMIN_NAMELOGS && g_admin_namelog[ i ]; i++ )
    {
      for( j = 0; j < 8; j++ )
        guid_stub[ j ] = g_admin_namelog[ i ]->guid[ j + 24 ];
      guid_stub[ j ] = '\0';
      for( j = 0; j < MAX_ADMIN_NAMELOG_NAMES &&
         g_admin_namelog[ i ]->name[ j ][ 0 ]; j++ )
      {
        G_SanitiseString( g_admin_namelog[ i ]->name[ j ], n2, sizeof( n2 ) );
        if( strstr( n2, s2 ) )
        {
          if( g_admin_namelog[ i ]->slot > -1 )
            ADMBP( "^3" );
          ADMBP( va( "%-2s (*%s) %15s ^7'%s^7'\n",
           ( g_admin_namelog[ i ]->slot > -1 ) ?
             va( "%d", g_admin_namelog[ i ]->slot ) : "-",
           guid_stub,
           g_admin_namelog[ i ]->ip,
           g_admin_namelog[ i ]->name[ j ] ) );
        }
      }
    }
    ADMBP_end();
    return qfalse;
  }

  if( ent && !admin_higher_guid( ent->client->pers.guid,
    g_admin_namelog[ logmatch ]->guid ) )
  {

    ADMP( "^3!ban: ^7sorry, but your intended victim has a higher admin"
      " level than you\n" );
    return qfalse;
  }
  if( !strcmp( g_admin_namelog[ logmatch ]->ip, "localhost" ) )
  {
    ADMP( "^3!ban: ^7disconnecting the host would end the game\n" );
    return qfalse;
  }

  G_admin_duration( ( seconds ) ? seconds : -1,
    duration, sizeof( duration ) );

  admin_create_ban( ent,
    g_admin_namelog[ logmatch ]->name[ 0 ],
    g_admin_namelog[ logmatch ]->guid,
    g_admin_namelog[ logmatch ]->ip,
    seconds, reason );

  g_admin_namelog[ logmatch ]->banned = qtrue;

  if( !g_admin.string[ 0 ] )
    ADMP( "^3!ban: ^7WARNING g_admin not set, not saving ban to a file\n" );
  else
    admin_writeconfig();

  if( g_admin_namelog[ logmatch ]->slot == -1 )
  {
    // client is already disconnected so stop here
    AP( va( "print \"^3!ban:^7 %s^7 has been banned by %s^7, "
      "duration: %s, reason: %s\n\"",
      g_admin_namelog[ logmatch ]->name[ 0 ],
      ( ent ) ? ent->client->pers.netname : "console",
      duration,
      ( *reason ) ? reason : "banned by admin" ) );
    return qtrue;
  }

  trap_SendServerCommand( g_admin_namelog[ logmatch ]->slot,
    va( "disconnect \"You have been banned.\n"
      "admin:\n%s^7\nduration:\n%s\nreason:\n%s\n%s\"",
      ( ent ) ? ent->client->pers.netname : "console",
      duration,
      ( *reason ) ? reason : "banned by admin", notice ) );

  trap_DropClient(  g_admin_namelog[ logmatch ]->slot,
    va( "has been banned by %s^7, duration: %s, reason: %s",
      ( ent ) ? ent->client->pers.netname : "console",
      duration,
      ( *reason ) ? reason : "banned by admin" ) );
  return qtrue;
}

qboolean G_admin_unban( gentity_t *ent, int skiparg )
{
  int bnum;
  char bs[ 5 ];

  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( "^3!unban: ^7usage: !unban [ban#]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, bs, sizeof( bs ) );
  bnum = atoi( bs );
  if( bnum < 1 || bnum > MAX_ADMIN_BANS || !g_admin_bans[ bnum - 1 ] )
  {
    ADMP( "^3!unban: ^7invalid ban#\n" );
    return qfalse;
  }
  if( g_admin_bans[ bnum - 1 ]->expires == 0 &&
    !G_admin_permission( ent, ADMF_CAN_PERM_BAN ) )
  {
    ADMP( "^3!unban: ^7you cannot remove permanent bans\n" );
    return qfalse;
  }
  g_admin_bans[ bnum - 1 ]->expires = trap_RealTime( NULL );
  AP( va( "print \"^3!unban: ^7ban #%d for %s^7 has been removed by %s\n\"",
          bnum,
          g_admin_bans[ bnum - 1 ]->name,
          ( ent ) ? ent->client->pers.netname : "console" ) );
  if( g_admin.string[ 0 ] )
    admin_writeconfig();
  return qtrue;
}
qboolean G_admin_adjustban( gentity_t *ent, int skiparg )
{
  int bnum;
  int length;
  int expires;
  char duration[ 32 ] = {""};
  char *reason;
  char bs[ 5 ];
  char secs[ 7 ];

  if( G_SayArgc() < 3 + skiparg )
  {
    ADMP( "^3!adjustban: ^7usage: !adjustban [ban#] [time] [reason]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, bs, sizeof( bs ) );
  bnum = atoi( bs );
  if( bnum < 1 || bnum > MAX_ADMIN_BANS || !g_admin_bans[ bnum - 1] )
  {
    ADMP( "^3!adjustban: ^7invalid ban#\n" );
    return qfalse;
  }

  G_SayArgv( 2 + skiparg, secs, sizeof( secs ) );
  length = G_admin_parse_time( secs );
  
  if( g_adminMaxBan.integer && 
      !G_admin_permission( ent, ADMF_CAN_PERM_BAN ) && 
      ( length > G_admin_parse_time( g_adminMaxBan.string ) || length == 0 ) )
  {
     ADMP( va("^3!adjustban: ^7ban length is limited to %s for your admin level\n", g_adminMaxBan.string ) );
     return qfalse;
  }
  
  if( length < 0 )
    reason = G_SayConcatArgs( 2 + skiparg );
  else
  {
    if( length != 0 )
      expires = trap_RealTime( NULL ) + length;
    else if( G_admin_permission( ent, ADMF_CAN_PERM_BAN ) )
      expires = 0;
    else
    {
      ADMP( "^3!adjustban: ^7ban time must be positive\n" );
      return qfalse;
    }

    g_admin_bans[ bnum - 1 ]->expires = expires;
    G_admin_duration( ( length ) ? length : -1, duration, sizeof( duration ) );
    reason = G_SayConcatArgs( 3 + skiparg );
  }
  if( *reason )
    Q_strncpyz( g_admin_bans[ bnum - 1 ]->reason, reason,
      sizeof( g_admin_bans[ bnum - 1 ]->reason ) );
  AP( va( "print \"^3!adjustban: ^7ban #%d for %s^7 has been updated by %s^7 "
    "%s%s%s%s%s\n\"",
    bnum,
    g_admin_bans[ bnum - 1 ]->name,
    ( ent ) ? G_admin_adminPrintName( ent ) : "console",
    ( length >= 0 ) ? "duration: " : "",
    duration,
    ( length >= 0 && *reason ) ? ", " : "",
    ( *reason ) ? "reason: " : "",
    reason ) );
  if( ent )
    Q_strncpyz( g_admin_bans[ bnum - 1 ]->banner, ent->client->pers.netname,
      sizeof( g_admin_bans[ bnum - 1 ]->banner ) );
  if( g_admin.string[ 0 ] )
    admin_writeconfig();
  return qtrue;
}


qboolean G_admin_putteam( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], team[ 7 ], err[ MAX_STRING_CHARS ];
  gentity_t *vic;
  team_t teamnum = TEAM_NONE;

  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  G_SayArgv( 2 + skiparg, team, sizeof( team ) );
  if( G_SayArgc() < 3 + skiparg )
  {
    ADMP( "^3!putteam: ^7usage: !putteam [name] [h|a|s]\n" );
    return qfalse;
  }

  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!putteam: ^7%s\n", err ) );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( "^3!putteam: ^7sorry, but your intended victim has a higher "
        " admin level than you\n" );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];
  teamnum = G_TeamFromString( team );
  if( teamnum == NUM_TEAMS )
  {
    ADMP( va( "^3!putteam: ^7unknown team %s\n", team ) );
    return qfalse;
  }
  if( vic->client->pers.teamSelection == teamnum )
    return qfalse;
  if( level.demoState == DS_PLAYBACK )
  {
    ADMP( "^3!putteam: ^7cannot join a team while a demo is playing\n" );
    return qfalse;
  }
  G_ChangeTeam( vic, teamnum );

  AP( va( "print \"^3!putteam: ^7%s^7 put %s^7 on to the %s team\n\"",
          ( ent ) ? ent->client->pers.netname : "console",
          vic->client->pers.netname, BG_TeamName( teamnum ) ) );
  return qtrue;
}

qboolean G_admin_map( gentity_t *ent, int skiparg )
{
  char map[ MAX_QPATH ];
  char layout[ MAX_QPATH ] = { "" };

  if( G_SayArgc( ) < 2 + skiparg )
  {
    ADMP( "^3!map: ^7usage: !map [map] (layout)\n" );
    return qfalse;
  }

  G_SayArgv( skiparg + 1, map, sizeof( map ) );

  if( !trap_FS_FOpenFile( va( "maps/%s.bsp", map ), NULL, FS_READ ) )
  {
    ADMP( va( "^3!map: ^7invalid map name '%s'\n", map ) );
    return qfalse;
  }

  if( G_SayArgc( ) > 2 + skiparg )
  {
    G_SayArgv( skiparg + 2, layout, sizeof( layout ) );
    if( !Q_stricmp( layout, "*BUILTIN*" ) ||
      trap_FS_FOpenFile( va( "layouts/%s/%s.dat", map, layout ),
        NULL, FS_READ ) > 0 )
    {
      trap_Cvar_Set( "g_layouts", layout );
    }
    else
    {
      ADMP( va( "^3!map: ^7invalid layout name '%s'\n", layout ) );
      return qfalse;
    }
  }

  trap_SendConsoleCommand( EXEC_APPEND, va( "map %s", map ) );
  level.restarted = qtrue;
  AP( va( "print \"^3!map: ^7map '%s' started by %s %s\n\"", map,
          ( ent ) ? ent->client->pers.netname : "console",
          ( layout[ 0 ] ) ? va( "(forcing layout '%s')", layout ) : "" ) );
  G_admin_maplog_result( "M" );
  return qtrue;
}

qboolean G_admin_devmap( gentity_t *ent, int skiparg )
{
  char map[ MAX_QPATH ];
  char layout[ MAX_QPATH ] = { "" };

  if( G_SayArgc( ) < 2 + skiparg )
  {
    ADMP( "^3!devmap: ^7usage: !devmap [map] (layout)\n" );
    return qfalse;
  }

  G_SayArgv( skiparg + 1, map, sizeof( map ) );

  if( !trap_FS_FOpenFile( va( "maps/%s.bsp", map ), NULL, FS_READ ) )
  {
    ADMP( va( "^3!devmap: ^7invalid map name '%s'\n", map ) );
    return qfalse;
  }

  if( G_SayArgc( ) > 2 + skiparg )
  {
    G_SayArgv( skiparg + 2, layout, sizeof( layout ) );
    if( !Q_stricmp( layout, "*BUILTIN*" ) ||
      trap_FS_FOpenFile( va( "layouts/%s/%s.dat", map, layout ),
        NULL, FS_READ ) > 0 )
    {
      trap_Cvar_Set( "g_layouts", layout );
    }
    else
    {
      ADMP( va( "^3!devmap: ^7invalid layout name '%s'\n", layout ) );
      return qfalse;
    }
  }

  trap_SendConsoleCommand( EXEC_APPEND, va( "devmap %s", map ) );
  level.restarted = qtrue;
  AP( va( "print \"^3!devmap: ^7map '%s' started by %s^7 with cheats %s\n\"", map,
          ( ent ) ? ent->client->pers.netname : "console",
          ( layout[ 0 ] ) ? va( "(forcing layout '%s')", layout ) : "" ) );
  G_admin_maplog_result( "D" );
  return qtrue;
}

qboolean G_admin_mute( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  char command[ MAX_ADMIN_CMD_LEN ], *cmd;
  gentity_t *vic;

  G_SayArgv( skiparg, command, sizeof( command ) );
  cmd = command;
  if( cmd && *cmd == '!' )
    cmd++;
  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( va( "^3!%s: ^7usage: !%s [name|slot#]\n", cmd, cmd ) );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!%s: ^7%s\n", cmd, err ) );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( va( "^3!%s: ^7sorry, but your intended victim has a higher admin"
        " level than you\n", cmd ) );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];
  if( vic->client->pers.muted == qtrue )
  {
    if( !Q_stricmp( cmd, "mute" ) )
    {
      ADMP( "^3!mute: ^7player is already muted\n" );
      return qtrue;
    }
    vic->client->pers.muted = qfalse;
    CPx( pids[ 0 ], "cp \"^1You have been unmuted\"" );
    AP( va( "print \"^3!unmute: ^7%s^7 has been unmuted by %s\n\"",
            vic->client->pers.netname,
            ( ent ) ? ent->client->pers.netname : "console" ) );
  }
  else
  {
    if( !Q_stricmp( cmd, "unmute" ) )
    {
      ADMP( "^3!unmute: ^7player is not currently muted\n" );
      return qtrue;
    }
    vic->client->pers.muted = qtrue;
    CPx( pids[ 0 ], "cp \"^1You've been muted\"" );
    AP( va( "print \"^3!mute: ^7%s^7 has been muted by ^7%s\n\"",
            vic->client->pers.netname,
            ( ent ) ? ent->client->pers.netname : "console" ) );
  }
  return qtrue;
}

qboolean G_admin_denybuild( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  char command[ MAX_ADMIN_CMD_LEN ], *cmd;
  gentity_t *vic;

  G_SayArgv( skiparg, command, sizeof( command ) );
  cmd = command;
  if( cmd && *cmd == '!' )
    cmd++;
  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( va( "^3!%s: ^7usage: !%s [name|slot#]\n", cmd, cmd ) );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!%s: ^7%s\n", cmd, err ) );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( va( "^3!%s: ^7sorry, but your intended victim has a higher admin"
              " level than you\n", cmd ) );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];
  if( vic->client->pers.denyBuild )
  {
    if( !Q_stricmp( cmd, "denybuild" ) )
    {
      ADMP( "^3!denybuild: ^7player already has no building rights\n" );
      return qtrue;
    }
    vic->client->pers.denyBuild = qfalse;
    CPx( pids[ 0 ], "cp \"^1You've regained your building rights\"" );
    AP( va(
      "print \"^3!allowbuild: ^7building rights for ^7%s^7 restored by %s\n\"",
      vic->client->pers.netname,
      ( ent ) ? ent->client->pers.netname : "console" ) );
  }
  else
  {
    if( !Q_stricmp( cmd, "allowbuild" ) )
    {
      ADMP( "^3!allowbuild: ^7player already has building rights\n" );
      return qtrue;
    }
    vic->client->pers.denyBuild = qtrue;
    vic->client->ps.stats[ STAT_BUILDABLE ] = BA_NONE;
    CPx( pids[ 0 ], "cp \"^1You've lost your building rights\"" );
    AP( va(
      "print \"^3!denybuild: ^7building rights for ^7%s^7 revoked by ^7%s\n\"",
      vic->client->pers.netname,
      ( ent ) ? ent->client->pers.netname : "console" ) );
  }
  return qtrue;
}

qboolean G_admin_listadmins( gentity_t *ent, int skiparg )
{
  int i, found = 0;
  char search[ MAX_NAME_LENGTH ] = {""};
  char s[ MAX_NAME_LENGTH ] = {""};
  int start = 0;
  qboolean numeric = qtrue;
  int drawn = 0;

  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( g_admin_admins[ i ]->level == 0 )
      continue;
    found++;
  }
  if( !found )
  {
    ADMP( "^3!listadmins: ^7no admins defined\n" );
    return qfalse;
  }

  if( G_SayArgc() == 2 + skiparg )
  {
    G_SayArgv( 1 + skiparg, s, sizeof( s ) );
    for( i = 0; i < sizeof( s ) && s[ i ]; i++ )
    {
      if( isdigit( s[ i ] ) )
        continue;
      numeric = qfalse;
    }
    if( numeric )
    {
      start = atoi( s );
      if( start > 0 )
        start -= 1;
      else if( start < 0 )
        start = found + start;
    }
    else
      G_SanitiseString( s, search, sizeof( search ) );
  }

  if( start >= found || start < 0 )
    start = 0;

  if( start >= found )
  {
    ADMP( va( "^3!listadmins: ^7listing %d admins\n", found ) );
    return qfalse;
  }

  drawn = admin_listadmins( ent, start, search );

  if( search[ 0 ] )
  {
    ADMP( va( "^3!listadmins:^7 found %d admins matching '%s^7'\n",
      drawn, search ) );
  }
  else
  {
    ADMBP_begin();
    ADMBP( va( "^3!listadmins:^7 showing admin %d - %d of %d.  ",
      ( found ) ? ( start + 1 ) : 0,
      ( ( start + MAX_ADMIN_LISTITEMS ) > found ) ?
       found : ( start + MAX_ADMIN_LISTITEMS ),
      found ) );
    if( ( start + MAX_ADMIN_LISTITEMS ) < found )
    {
      ADMBP( va( "run '!listadmins %d' to see more",
        ( start + MAX_ADMIN_LISTITEMS + 1 ) ) );
    }
    ADMBP( "\n" );
    ADMBP_end();
  }
  return qtrue;
}

qboolean G_admin_listlayouts( gentity_t *ent, int skiparg )
{
  char list[ MAX_CVAR_VALUE_STRING ];
  char map[ MAX_QPATH ];
  int count = 0;
  char *s;
  char layout[ MAX_QPATH ] = { "" };
  int i = 0;

  if( G_SayArgc( ) == 2 + skiparg )
    G_SayArgv( 1 + skiparg, map, sizeof( map ) );
  else
    trap_Cvar_VariableStringBuffer( "mapname", map, sizeof( map ) );

  count = G_LayoutList( map, list, sizeof( list ) );
  ADMBP_begin( );
  ADMBP( va( "^3!listlayouts:^7 %d layouts found for '%s':\n", count, map ) );
  s = &list[ 0 ];
  while( *s )
  {
    if( *s == ' ' )
    {
      ADMBP( va ( " %s\n", layout ) );
      layout[ 0 ] = '\0';
      i = 0;
    }
    else if( i < sizeof( layout ) - 2 )
    {
      layout[ i++ ] = *s;
      layout[ i ] = '\0';
    }
    s++;
  }
  if( layout[ 0 ] )
    ADMBP( va ( " %s\n", layout ) );
  ADMBP_end( );
  return qtrue;
}

qboolean G_admin_listplayers( gentity_t *ent, int skiparg )
{
  int i, j;
  gclient_t *p;
  char c[ 3 ], t[ 2 ]; // color and team letter
  char n[ MAX_NAME_LENGTH ] = {""};
  char n2[ MAX_NAME_LENGTH ] = {""};
  char n3[ MAX_NAME_LENGTH ] = {""};
  char lname[ MAX_NAME_LENGTH ];
  char guid_stub[ 9 ];
  char muted[ 2 ], denied[ 2 ], dbuilder[ 2 ];
  int l;

  ADMBP_begin();
  ADMBP( va( "^3!listplayers: ^7%d players connected:\n",
    level.numConnectedClients ) );
  for( i = 0; i < level.maxclients; i++ )
  {
    p = &level.clients[ i ];
    Q_strncpyz( t, "S", sizeof( t ) );
    Q_strncpyz( c, S_COLOR_YELLOW, sizeof( c ) );
    if( p->pers.teamSelection == TEAM_HUMANS )
    {
      Q_strncpyz( t, "H", sizeof( t ) );
      Q_strncpyz( c, S_COLOR_BLUE, sizeof( c ) );
    }
    else if( p->pers.teamSelection == TEAM_ALIENS )
    {
      Q_strncpyz( t, "A", sizeof( t ) );
      Q_strncpyz( c, S_COLOR_RED, sizeof( c ) );
    }

    if( p->pers.connected == CON_CONNECTING )
    {
      Q_strncpyz( t, "C", sizeof( t ) );
      Q_strncpyz( c, S_COLOR_CYAN, sizeof( c ) );
    }
    else if( p->pers.connected != CON_CONNECTED )
    {
      continue;
    }

    for( j = 0; j < 8; j++ )
      guid_stub[ j ] = p->pers.guid[ j + 24 ];
    guid_stub[ j ] = '\0';

    muted[ 0 ] = '\0';
    if( p->pers.muted )
    {
      Q_strncpyz( muted, "M", sizeof( muted ) );
    }
    denied[ 0 ] = '\0';
    if( p->pers.denyBuild )
    {
      Q_strncpyz( denied, "B", sizeof( denied ) );
    }

    dbuilder[ 0 ] = '\0';
    if( p->pers.designatedBuilder )
    {
      if( !G_admin_permission( &g_entities[ i ], ADMF_INCOGNITO ) &&
          G_admin_permission( &g_entities[ i ], ADMF_DBUILDER ) && 
          G_admin_permission(ent, ADMF_SEESFULLLISTPLAYERS ) )
      {
        Q_strncpyz( dbuilder, "P", sizeof( dbuilder ) );
      }
      else
      {
        Q_strncpyz( dbuilder, "D", sizeof( dbuilder ) );
      }
    }
    l = 0;
    G_SanitiseString( p->pers.netname, n2, sizeof( n2 ) );
    n[ 0 ] = '\0';
    for( j = 0; j < MAX_ADMIN_ADMINS && g_admin_admins[ j ]; j++ )
    {
      if( !Q_stricmp( g_admin_admins[ j ]->guid, p->pers.guid ) )
      {
        // don't gather aka or level info if the admin is incognito
        if( ent && G_admin_permission( &g_entities[ i ], ADMF_INCOGNITO ) )
        {
          break;
        }
        l = g_admin_admins[ j ]->level;
        G_SanitiseString( g_admin_admins[ j ]->name, n3, sizeof( n3 ) );
        if( Q_stricmp( n2, n3 ) )
        {
          Q_strncpyz( n, g_admin_admins[ j ]->name, sizeof( n ) );
        }
        break;
      }
    }
    lname[ 0 ] = '\0';
    for( j = 0; j < MAX_ADMIN_LEVELS && g_admin_levels[ j ]; j++ )
    {
      if( g_admin_levels[ j ]->level == l )
      {
        int k, colorlen;

        for( colorlen = k = 0; g_admin_levels[ j ]->name[ k ]; k++ )
          if( Q_IsColorString( &g_admin_levels[ j ]->name[ k ] ) )
            colorlen += 2;
        Com_sprintf( lname, sizeof( lname ), "%*s",
                     admin_level_maxname + colorlen,
                     g_admin_levels[ j ]->name );
        break;
      }

    }

    if( G_admin_permission( ent, ADMF_SEESFULLLISTPLAYERS ) )
    {
      ADMBP( va( "%2i %s%s^7 %-2i %s^7 (*%s) ^1%1s%1s^7 %s^7 %s%s^7%s\n",
                i,
                c,
                t,
                l,
                lname,
                guid_stub,
                muted,
                denied,
                p->pers.netname,
                ( *n ) ? "(a.k.a. " : "",
                n,
                ( *n ) ? ")" : "" ) );
    }
    else
    {
      ADMBP( va( "%2i %s%s^7 ^1%1s%1s^7 %s^7\n",
                i,
                c,
                t,
                muted,
                denied,
                p->pers.netname ) );
    }
  }
  ADMBP_end();
  return qtrue;
}

qboolean G_admin_showbans( gentity_t *ent, int skiparg )
{
  int i, found = 0;
  int max = -1, count;
  int t;
  char duration[ 32 ];
  int max_name = 1, max_banner = 1, colorlen;
  int len;
  int secs;
  int start = 0;
  char skip[ 11 ];
  char date[ 11 ];
  char *made;
  int j, k;
  char n1[ MAX_NAME_LENGTH ] = {""};
  char n2[ MAX_NAME_LENGTH ] = {""};

  t = trap_RealTime( NULL );

  for( i = 0; i < MAX_ADMIN_BANS && g_admin_bans[ i ]; i++ )
  {
    if( g_admin_bans[ i ]->expires != 0 &&
        ( g_admin_bans[ i ]->expires - t ) < 1 )
    {
      continue;
    }
    found++;
    max = i;
  }

  if( max < 0 )
  {
    ADMP( "^3!showbans: ^7no bans to display\n" );
    return qfalse;
  }

  if( G_SayArgc() == 2 + skiparg )
  {
    G_SayArgv( 1 + skiparg, skip, sizeof( skip ) );
    start = atoi( skip );
    // showbans 1 means start with ban 0
    if( start > 0 )
      start--;
    else if( start < 0 )
    {
      for( i = max, count = 0; i >= 0 && count < -start; i-- )
        if( g_admin_bans[ i ]->expires == 0 ||
          ( g_admin_bans[ i ]->expires - t ) > 0 )
          count++;
      start = i + 1;
    }
  }

  if( start < 0 )
    start = 0;

  if( start > max )
  {
    ADMP( va( "^3!showbans: ^7%d is the last valid ban\n", max + 1 ) );
    return qfalse;
  }

  for( i = start, count = 0; i <= max && count < MAX_ADMIN_SHOWBANS; i++ )
  {
    if( g_admin_bans[ i ]->expires != 0 &&
      ( g_admin_bans[ i ]->expires - t ) < 1 )
      continue;

    count++;

    len = Q_PrintStrlen( g_admin_bans[ i ]->name );
    if( len > max_name )
      max_name = len;
    len = Q_PrintStrlen( g_admin_bans[ i ]->banner );
    if( len > max_banner )
      max_banner = len;
  }

  ADMBP_begin();
  for( i = start, count = 0; i <= max && count < MAX_ADMIN_SHOWBANS; i++ )
  {
    if( g_admin_bans[ i ]->expires != 0 &&
      ( g_admin_bans[ i ]->expires - t ) < 1 )
      continue;

    count++;

    // only print out the the date part of made
    date[ 0 ] = '\0';
    made = g_admin_bans[ i ]->made;
    for( j = 0; made && *made; j++ )
    {
      if( ( j + 1 ) >= sizeof( date ) )
        break;
      if( *made == ' ' )
        break;
      date[ j ] = *made;
      date[ j + 1 ] = '\0';
      made++;
    }

    secs = ( g_admin_bans[ i ]->expires - t );
    G_admin_duration( secs, duration, sizeof( duration ) );

    for( colorlen = k = 0; g_admin_bans[ i ]->name[ k ]; k++ )
      if( Q_IsColorString( &g_admin_bans[ i ]->name[ k ] ) )
        colorlen += 2;
    Com_sprintf( n1, sizeof( n1 ), "%*s", max_name + colorlen,
                 g_admin_bans[ i ]->name );

    for( colorlen = k = 0; g_admin_bans[ i ]->banner[ k ]; k++ )
      if( Q_IsColorString( &g_admin_bans[ i ]->banner[ k ] ) )
        colorlen += 2;
    Com_sprintf( n2, sizeof( n2 ), "%*s", max_banner + colorlen,
                 g_admin_bans[ i ]->banner );

    ADMBP( va( "%4i %s^7 %-15s %-8s %s^7 %-10s\n     \\__ %s\n",
             ( i + 1 ),
             n1,
             g_admin_bans[ i ]->ip,
             date,
             n2,
             duration,
             g_admin_bans[ i ]->reason ) );
  }

  ADMBP( va( "^3!showbans:^7 showing bans %d - %d of %d (%d total).",
           ( found ) ? ( start + 1 ) : 0,
           i,
           max + 1,
           found ) );
  if( i <= max )
    ADMBP( va( "  run !showbans %d to see more", i + 1 ) );
  ADMBP( "\n" );
  ADMBP_end();
  return qtrue;
}
//TODO pull in extra features from lakitu7's qvm for this function
qboolean G_admin_help( gentity_t *ent, int skiparg )
{
  int i;

  if( G_SayArgc() < 2 + skiparg )
  {
    int j = 0;
    int count = 0;

    ADMBP_begin();
    for( i = 0; i < adminNumCmds; i++ )
    {
      if( G_admin_permission( ent, g_admin_cmds[ i ].flag[ 0 ] ) )
      {
        ADMBP( va( "^3!%-12s", g_admin_cmds[ i ].keyword ) );
        j++;
        count++;
      }
      // show 6 commands per line
      if( j == 6 )
      {
        ADMBP( "\n" );
        j = 0;
      }
    }
    for( i = 0; i < MAX_ADMIN_COMMANDS && g_admin_commands[ i ]; i++ )
    {
      if( ! admin_command_permission( ent, g_admin_commands[ i ]->command ) )
        continue;
      ADMBP( va( "^3!%-12s", g_admin_commands[ i ]->command ) );
      j++;
      count++;
      // show 6 commands per line
      if( j == 6 )
      {
        ADMBP( "\n" );
        j = 0;
      }
    }
    if( count )
	ADMBP( "\n" );
    ADMBP( va( "^3!help: ^7%i available commands\n", count ) );
    ADMBP( "run !help [^3command^7] for help with a specific command.\n" );
    ADMBP_end();

    return qtrue;
  }
  else
  {
    //!help param
    char param[ MAX_ADMIN_CMD_LEN ];
    char *cmd;

    G_SayArgv( 1 + skiparg, param, sizeof( param ) );
    cmd = ( param[0] == '!' ) ? &param[1] : &param[0];
    ADMBP_begin();
    for( i = 0; i < adminNumCmds; i++ )
    {
      if( !Q_stricmp( cmd, g_admin_cmds[ i ].keyword ) )
      {
        if( !G_admin_permission( ent, g_admin_cmds[ i ].flag[ 0 ] ) )
        {
          ADMBP( va( "^3!help: ^7you have no permission to use '%s'\n",
                   g_admin_cmds[ i ].keyword ) );
          ADMBP_end();
          return qfalse;
        }
        ADMBP( va( "^3!help: ^7help for '!%s':\n",
          g_admin_cmds[ i ].keyword ) );
        ADMBP( va( " ^3Function: ^7%s\n", g_admin_cmds[ i ].function ) );
        ADMBP( va( " ^3Syntax: ^7!%s %s\n", g_admin_cmds[ i ].keyword,
                 g_admin_cmds[ i ].syntax ) );
        ADMBP( va( " ^3Flag: ^7'%c'\n", g_admin_cmds[ i ].flag[ 0 ] ) );
        ADMBP_end();
        return qtrue;
      }
    }
    for( i = 0; i < MAX_ADMIN_COMMANDS && g_admin_commands[ i ]; i++ )
    {
      if( !Q_stricmp( cmd, g_admin_commands[ i ]->command ) )
      {
        if( !admin_command_permission( ent, g_admin_commands[ i ]->command ) )
        {
          ADMBP( va( "^3!help: ^7you have no permission to use '%s'\n",
                   g_admin_commands[ i ]->command ) );
          ADMBP_end();
          return qfalse;
        }
        ADMBP( va( "^3!help: ^7help for '%s':\n",
          g_admin_commands[ i ]->command ) );
        ADMBP( va( " ^3Description: ^7%s\n", g_admin_commands[ i ]->desc ) );
        ADMBP( va( " ^3Syntax: ^7!%s\n", g_admin_commands[ i ]->command ) );
        ADMBP_end();
        return qtrue;
      }
    }
    ADMBP( va( "^3!help: ^7no help found for '%s'\n", cmd ) );
    ADMBP_end();
    return qfalse;
  }
}

qboolean G_admin_admintest( gentity_t *ent, int skiparg )
{
  int i, l = 0;
  qboolean found = qfalse;
  qboolean lname = qfalse;

  if( !ent )
  {
    ADMP( "^3!admintest: ^7you are on the console.\n" );
    return qtrue;
  }
  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    if( !Q_stricmp( g_admin_admins[ i ]->guid, ent->client->pers.guid ) )
    {
      found = qtrue;
      break;
    }
  }

  if( found )
  {
    l = g_admin_admins[ i ]->level;
    for( i = 0; i < MAX_ADMIN_LEVELS && g_admin_levels[ i ]; i++ )
    {
      if( g_admin_levels[ i ]->level != l )
        continue;
      if( *g_admin_levels[ i ]->name )
      {
        lname = qtrue;
        break;
      }
    }
  }
  AP( va( "print \"^3!admintest: ^7%s^7 is a level %d admin %s%s^7%s\n\"",
          ent->client->pers.netname,
          l,
          ( lname ) ? "(" : "",
          ( lname ) ? g_admin_levels[ i ]->name : "",
          ( lname ) ? ")" : "" ) );
  return qtrue;
}

qboolean G_admin_allready( gentity_t *ent, int skiparg )
{
  int i = 0;
  gclient_t *cl;

  if( !level.intermissiontime )
  {
    ADMP( "^3!allready: ^7this command is only valid during intermission\n" );
    return qfalse;
  }

  for( i = 0; i < g_maxclients.integer; i++ )
  {
    cl = level.clients + i;
    if( cl->pers.connected != CON_CONNECTED )
      continue;

    if( cl->pers.teamSelection == TEAM_NONE )
      continue;

    cl->readyToExit = 1;
  }
  AP( va( "print \"^3!allready:^7 %s^7 says everyone is READY now\n\"",
     ( ent ) ? ent->client->pers.netname : "console" ) );
  return qtrue;
}

qboolean G_admin_cancelvote( gentity_t *ent, int skiparg )
{

  if(!level.voteTime && !level.teamVoteTime[ 0 ] && !level.teamVoteTime[ 1 ] )
  {
    ADMP( "^3!cancelvote: ^7no vote in progress\n" );
    return qfalse;
  }
  level.voteNo = level.numConnectedClients;
  level.voteYes = 0;
  CheckVote( );
  level.teamVoteNo[ 0 ] = level.numConnectedClients;
  level.teamVoteYes[ 0 ] = 0;
  CheckTeamVote( TEAM_HUMANS );
  level.teamVoteNo[ 1 ] = level.numConnectedClients;
  level.teamVoteYes[ 1 ] = 0;
  CheckTeamVote( TEAM_ALIENS );
  AP( va( "print \"^3!cancelvote: ^7%s^7 decided that everyone voted No\n\"",
          ( ent ) ? ent->client->pers.netname : "console" ) );
  return qtrue;
}

qboolean G_admin_passvote( gentity_t *ent, int skiparg )
{
  if(!level.voteTime && !level.teamVoteTime[ 0 ] && !level.teamVoteTime[ 1 ] )
  {
    ADMP( "^3!passvote: ^7no vote in progress\n" );
    return qfalse;
  }
  level.voteYes = level.numConnectedClients;
  level.voteNo = 0;
  CheckVote( );
  level.teamVoteYes[ 0 ] = level.numConnectedClients;
  level.teamVoteNo[ 0 ] = 0;
  CheckTeamVote( TEAM_HUMANS );
  level.teamVoteYes[ 1 ] = level.numConnectedClients;
  level.teamVoteNo[ 1 ] = 0;
  CheckTeamVote( TEAM_ALIENS );
  AP( va( "print \"^3!passvote: ^7%s^7 decided that everyone voted Yes\n\"",
          ( ent ) ? ent->client->pers.netname : "console" ) );
  return qtrue;
}

qboolean G_admin_spec999( gentity_t *ent, int skiparg )
{
  int i;
  gentity_t *vic;

  for( i = 0; i < level.maxclients; i++ )
  {
    vic = &g_entities[ i ];
    if( !vic->client )
      continue;
    if( vic->client->pers.connected != CON_CONNECTED )
      continue;
    if( vic->client->pers.teamSelection == TEAM_NONE )
      continue;
    if( vic->client->ps.ping == 999 )
    {
		  vic->client->pers.saved =  qtrue;									//It isn't fair if they have connection problems and loose their credits & score.
      G_ChangeTeam( vic, TEAM_NONE );
      AP( va( "print \"^3!spec999: ^7%s^7 moved ^7%s^7 to spectators\n\"",
        ( ent ) ? ent->client->pers.netname : "console",
        vic->client->pers.netname ) );
    }
  }
  return qtrue;
}

qboolean G_admin_rename( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ];
  char newname[ MAX_NAME_LENGTH ];
  char oldname[ MAX_NAME_LENGTH ];
  char err[ MAX_STRING_CHARS ];
  char userinfo[ MAX_INFO_STRING ];
  char *s;
  gentity_t *victim = NULL;

  if( G_SayArgc() < 3 + skiparg )
  {
    ADMP( "^3!rename: ^7usage: !rename [name] [newname]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  s = G_SayConcatArgs( 2 + skiparg );
  Q_strncpyz( newname, s, sizeof( newname ) );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!rename: ^7%s\n", err ) );
    return qfalse;
  }
  victim = &g_entities[ pids[ 0 ] ];
  if( !admin_higher( ent, victim ) )
  {
    ADMP( "^3!rename: ^7sorry, but your intended victim has a higher admin"
        " level than you\n" );
    return qfalse;
  }
  if( !G_admin_name_check( victim, newname, err, sizeof( err ) ) )
  {
    ADMP( va( "^3!rename: ^7%s\n", err ) );
    return qfalse;
  }
  level.clients[ pids[ 0 ] ].pers.nameChanges--;
  level.clients[ pids[ 0 ] ].pers.nameChangeTime = 0;
  trap_GetUserinfo( pids[ 0 ], userinfo, sizeof( userinfo ) );
  s = Info_ValueForKey( userinfo, "name" );
  Q_strncpyz( oldname, s, sizeof( oldname ) );
  Info_SetValueForKey( userinfo, "name", newname );
  trap_SetUserinfo( pids[ 0 ], userinfo );
  ClientUserinfoChanged( pids[ 0 ] );
  AP( va( "print \"^3!rename: ^7%s^7 has been renamed to %s^7 by %s\n\"",
          oldname,
          newname,
          ( ent ) ? ent->client->pers.netname : "console" ) );
  return qtrue;
}

qboolean G_admin_restart( gentity_t *ent, int skiparg )
{
  char layout[ MAX_CVAR_VALUE_STRING ] = { "" };
  char teampref[ MAX_CVAR_VALUE_STRING ] = { "" };
  int i;
  gclient_t *cl;

  if( G_SayArgc( ) > 1 + skiparg )
  {
    char map[ MAX_QPATH ];

    trap_Cvar_VariableStringBuffer( "mapname", map, sizeof( map ) );
    G_SayArgv( skiparg + 1, layout, sizeof( layout ) );

    if( Q_stricmp( layout, "keepteams" ) && Q_stricmp( layout, "keepteamslock" ) && Q_stricmp( layout, "switchteams" ) && Q_stricmp( layout, "switchteamslock" ) )
    {
      if( !Q_stricmp( layout, "*BUILTIN*" ) ||
        trap_FS_FOpenFile( va( "layouts/%s/%s.dat", map, layout ),
          NULL, FS_READ ) > 0 )
      {
        trap_Cvar_Set( "g_layouts", layout );
      }
      else
      {
        ADMP( va( "^3!restart: ^7layout '%s' does not exist\n", layout ) );
        return qfalse;
      }
    }
    else 
    {
      strcpy(layout,"");
      G_SayArgv( skiparg + 1, teampref, sizeof( teampref ) );    
    }
  }
  
  if( G_SayArgc( ) > 2 + skiparg ) 
  {
    G_SayArgv( skiparg + 2, teampref, sizeof( teampref ) );      
  }

  if( !Q_stricmp( teampref, "keepteams" ) || !Q_stricmp( teampref, "keepteamslock" ) )
  {
    for( i = 0; i < g_maxclients.integer; i++ )
    {
      cl = level.clients + i;
      if( cl->pers.connected != CON_CONNECTED )
        continue;

      if( cl->pers.teamSelection == TEAM_NONE )
        continue;

      cl->sess.restartTeam = cl->pers.teamSelection;
    }
  } 
  else if(!Q_stricmp( teampref, "switchteams" ) ||  !Q_stricmp( teampref, "switchteamslock" ))
  {
    for( i = 0; i < g_maxclients.integer; i++ )
    {
      cl = level.clients + i;
      if( cl->pers.connected != CON_CONNECTED )
        continue;

      if( cl->pers.teamSelection == TEAM_NONE )
        continue;

      if( cl->pers.teamSelection == TEAM_ALIENS )
        cl->sess.restartTeam = TEAM_HUMANS;
      else if(cl->pers.teamSelection == TEAM_HUMANS )
        cl->sess.restartTeam = TEAM_ALIENS;
    }
  }

  if( !Q_stricmp( teampref, "switchteamslock" ) || !Q_stricmp( teampref, "keepteamslock" ) )
  {
    trap_Cvar_Set( "g_lockTeamsAtStart", "1" );
  }

  trap_SendConsoleCommand( EXEC_APPEND, "map_restart" );

  if(teampref[ 0 ])
    strcpy(teampref,va( "^7(with teams option: '%s^7')", teampref ));

  G_admin_maplog_result( "R" );

  AP( va( "print \"^3!restart: ^7map restarted by %s %s %s\n\"",
          ( ent ) ? ent->client->pers.netname : "console",
          ( layout[ 0 ] ) ? va( "(forcing layout '%s')", layout ) : "",
          teampref ) );
  return qtrue;
}

qboolean G_admin_nextmap( gentity_t *ent, int skiparg )
{
  AP( va( "print \"^3!nextmap: ^7%s^7 decided to load the next map\n\"",
    ( ent ) ? ent->client->pers.netname : "console" ) );
  level.lastWin = TEAM_NONE;
  trap_SetConfigstring( CS_WINNER, "Evacuation" );
  LogExit( va( "nextmap was run by %s",
    ( ent ) ? ent->client->pers.netname : "console" ) );
  G_admin_maplog_result( "N" );
  return qtrue;
}

qboolean G_admin_namelog( gentity_t *ent, int skiparg )
{
  int i, j;
  char search[ MAX_NAME_LENGTH ] = {""};
  char s2[ MAX_NAME_LENGTH ] = {""};
  char n2[ MAX_NAME_LENGTH ] = {""};
  char guid_stub[ 9 ];
  qboolean found = qfalse;
  int printed = 0;

  if( G_SayArgc() > 1 + skiparg )
  {
    G_SayArgv( 1 + skiparg, search, sizeof( search ) );
    G_SanitiseString( search, s2, sizeof( s2 ) );
  }
  ADMBP_begin();
  for( i = 0; i < MAX_ADMIN_NAMELOGS && g_admin_namelog[ i ]; i++ )
  {
    if( search[ 0 ] )
    {
      found = qfalse;
      for( j = 0; j < MAX_ADMIN_NAMELOG_NAMES &&
        g_admin_namelog[ i ]->name[ j ][ 0 ]; j++ )
      {
        G_SanitiseString( g_admin_namelog[ i ]->name[ j ], n2, sizeof( n2 ) );
        if( strstr( n2, s2 ) )
        {
          found = qtrue;
          break;
        }
      }
      if( !found )
        continue;
    }
    printed++;
    for( j = 0; j < 8; j++ )
      guid_stub[ j ] = g_admin_namelog[ i ]->guid[ j + 24 ];
    guid_stub[ j ] = '\0';
    if( g_admin_namelog[ i ]->slot > -1 )
       ADMBP( "^3" );
    ADMBP( va( "%-2s (*%s) %15s^7",
      ( g_admin_namelog[ i ]->slot > -1 ) ?
        va( "%d", g_admin_namelog[ i ]->slot ) : "-",
      guid_stub, g_admin_namelog[ i ]->ip ) );
    for( j = 0; j < MAX_ADMIN_NAMELOG_NAMES &&
      g_admin_namelog[ i ]->name[ j ][ 0 ]; j++ )
    {
      ADMBP( va( " '%s^7'", g_admin_namelog[ i ]->name[ j ] ) );
    }
    ADMBP( "\n" );
  }
  ADMBP( va( "^3!namelog:^7 %d recent clients found\n", printed ) );
  ADMBP_end();
  return qtrue;
}

qboolean G_admin_lock( gentity_t *ent, int skiparg )
{
  char teamName[2] = {""};
  team_t team;

  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( "^3!lock: ^7usage: !lock [a|h]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, teamName, sizeof( teamName ) );
  team = G_TeamFromString( teamName );

  if( team == TEAM_ALIENS )
  {
    if( level.alienTeamLocked )
    {
      ADMP( "^3!lock: ^7the alien team is already locked\n" );
      return qfalse;
    }
    level.alienTeamLocked = qtrue;
  }
  else if( team == TEAM_HUMANS ) {
    if( level.humanTeamLocked )
    {
      ADMP( "^3!lock: ^7the human team is already locked\n" );
      return qfalse;
    }
    level.humanTeamLocked = qtrue;
  }
  else
  {
    ADMP( va( "^3!lock: ^7invalid team\"%c\"\n", teamName[0] ) );
    return qfalse;
  }

  AP( va( "print \"^3!lock: ^7the %s team has been locked by %s\n\"",
    BG_TeamName( team ),
    ( ent ) ? ent->client->pers.netname : "console" ) );
  return qtrue;
}

qboolean G_admin_unlock( gentity_t *ent, int skiparg )
{
  char teamName[2] = {""};
  team_t team;

  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( "^3!unlock: ^7usage: !unlock [a|h]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, teamName, sizeof( teamName ) );
  team = G_TeamFromString( teamName );

  if( team == TEAM_ALIENS )
  {
    if( !level.alienTeamLocked )
    {
      ADMP( "^3!unlock: ^7the alien team is not currently locked\n" );
      return qfalse;
    }
    level.alienTeamLocked = qfalse;
  }
  else if( team == TEAM_HUMANS ) {
    if( !level.humanTeamLocked )
    {
      ADMP( "^3!unlock: ^7the human team is not currently locked\n" );
      return qfalse;
    }
    level.humanTeamLocked = qfalse;
  }
  else
  {
    ADMP( va( "^3!unlock: ^7invalid team\"%c\"\n", teamName[0] ) );
    return qfalse;
  }

  AP( va( "print \"^3!unlock: ^7the %s team has been unlocked by %s\n\"",
    BG_TeamName( team ),
    ( ent ) ? ent->client->pers.netname : "console" ) );
  return qtrue;
}

qboolean G_admin_putmespec( gentity_t *ent, int skiparg )
{
  if( !ent )
  {
	ADMP( "!specme: sorry, but console isn't allowed on the spectators team\n");
	return qfalse;
  }
  G_ChangeTeam( ent, TEAM_NONE );
  AP( va("print \"^3!specme: ^7%s^7 decided to join the spectators\n\"", ent->client->pers.netname ) );
  ent->client->lastspecmeTime = level.time;
  return qtrue;
}

qboolean G_admin_register(gentity_t *ent, int skiparg )
{
  int level = 0;

  level = G_admin_level(ent);

  if( level == 0 )
  level = 1;

  if( !Q_stricmp( ent->client->pers.guid, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" ) )
  {
    ADMP( va( "^3!register: ^7 You cannot register for name protection until you update your client. Please replace your client executable with the one at http://trem.tjw.org/backport/ and reconnect. Updating your client will also allow you to have faster map downloads.\n" ) );
    return qfalse;
  }

  if( g_newbieNumbering.integer
    && g_newbieNamePrefix.string[ 0 ]
    && !Q_stricmpn ( ent->client->pers.netname, g_newbieNamePrefix.string, strlen(g_newbieNamePrefix.string ) ) )
  {
    ADMP( va( "^3!register: ^7 You cannot register names that begin with '%s^7'.\n",
      g_newbieNamePrefix.string ) );
    return qfalse;
  }

  trap_SendConsoleCommand( EXEC_APPEND,va( "!setlevel %d %d;",ent - g_entities, level) );
  ClientUserinfoChanged( ent - g_entities );

  AP( va( "print \"^3!register: ^7%s^7 is now a protected nickname.\n\"", ent->client->pers.netname) );

  return qtrue;
}

qboolean G_admin_speed( gentity_t *ent, int skiparg )
{

  char lvl_chr[ MAX_STRING_CHARS ];
  int minargc;
  int lvl;


  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!speed: ^7speed: !speed [#]\n" );
    return qfalse;
  }

  G_SayArgv( 1 + skiparg, lvl_chr, sizeof( lvl_chr ) );

  lvl = atoi(lvl_chr);

  trap_SendConsoleCommand( EXEC_APPEND, va( "g_speed %i", lvl ) );

  return qtrue;

}

qboolean G_admin_gravity( gentity_t *ent, int skiparg )
{

  char lvl_chr[ MAX_STRING_CHARS ];
  int minargc;
  int lvl;


  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!gravity: ^7gravity: !gravity [#]\n" );
    return qfalse;
  }

  G_SayArgv( 1 + skiparg, lvl_chr, sizeof( lvl_chr ) );

  lvl = atoi(lvl_chr);

  trap_SendConsoleCommand( EXEC_APPEND, va( "g_gravity %i", lvl ) );

  return qtrue;

}


qboolean G_admin_abps( gentity_t *ent, int skiparg )
{

  char lvl_chr[ MAX_STRING_CHARS ];
  int minargc;
  int lvl;


  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!abps: ^7abps: !abps [#]\n" );
    return qfalse;
  }

  G_SayArgv( 1 + skiparg, lvl_chr, sizeof( lvl_chr ) );

  lvl = atoi(lvl_chr);

  trap_SendConsoleCommand( EXEC_APPEND, va( "g_alienBuildPoints %i", lvl ) );

  return qtrue;

}

qboolean G_admin_hbps( gentity_t *ent, int skiparg )
{

  char lvl_chr[ MAX_STRING_CHARS ];
  int minargc;
  int lvl;


  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!hbps: ^7hbps: !hbps [#]\n" );
    return qfalse;
  }

  G_SayArgv( 1 + skiparg, lvl_chr, sizeof( lvl_chr ) );

  lvl = atoi(lvl_chr);

  trap_SendConsoleCommand( EXEC_APPEND, va( "g_humanBuildPoints %i", lvl ) );

  return qtrue;

}

qboolean G_admin_astage( gentity_t *ent, int skiparg )
{

  char lvl_chr[ MAX_STRING_CHARS ];
  int minargc;
  int lvl;


  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!astage: ^7astage: !astage [#]\n" );
    return qfalse;
  }

  G_SayArgv( 1 + skiparg, lvl_chr, sizeof( lvl_chr ) );

  lvl = atoi(lvl_chr);

  lvl -= 1;
  trap_SendConsoleCommand( EXEC_APPEND, va( "g_alienStage %i", lvl ) );

  return qtrue;

}

qboolean G_admin_hstage( gentity_t *ent, int skiparg )
{

  char lvl_chr[ MAX_STRING_CHARS ];
  int minargc;
  int lvl;


  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!hstage: ^7hstage: !hstage [#]\n" );
    return qfalse;
  }

  G_SayArgv( 1 + skiparg, lvl_chr, sizeof( lvl_chr ) );

  lvl = atoi(lvl_chr);

  lvl -= 1;
  trap_SendConsoleCommand( EXEC_APPEND, va( "g_humanStage %i", lvl ) );

  return qtrue;

}

qboolean G_admin_print2(gentity_t *ent, int skiparg )
{
  char *text;
  text = G_SayConcatArgs( 1 + skiparg );
  AP( va( "print \"%s\n\"", text ));
  return qtrue;
}

qboolean G_admin_warn( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], *reason, err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;
  
  minargc = 3 + skiparg;
  if( G_admin_permission( ent, ADMF_UNACCOUNTABLE ) )
    minargc = 2 + skiparg;
  
  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!warn: ^7usage: warn [name] [reason]\n" );
    return qfalse;
  }
  
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  reason = G_SayConcatArgs( 2 + skiparg );
  
  if( (found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS )) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!warn: ^7%s\n", err ) );
    return qfalse;
  }
  
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( "^3!warn: ^7sorry, but your intended victim has a higher admin"
        " level than you.\n" );
    return qfalse;
  }
  
  vic = &g_entities[ pids[ 0 ] ];
  //next line is the onscreen warning
  CPx( pids[ 0 ],va("cp \"^1You have been warned by an administrator.\n ^3Cease imeediately or face admin action!\n^1 %s%s\"",(*reason)? "REASON: " : "" ,(*reason)? reason : "") );
  AP( va( "print \"^3!warn: ^7%s^7 has been warned to cease and desist %s by %s \n\"",
            vic->client->pers.netname, (*reason) ? reason : "his current activity",
            ( ent ) ? ent->client->pers.netname : "console" ) );//console announcement
  ClientUserinfoChanged( pids[ 0 ] );
  return qtrue;
}


qboolean G_admin_kill( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], damage = 0, found;
  char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS], command[MAX_ADMIN_CMD_LEN], *cmd;
  gentity_t *vic;
 
  if( level.intermissiontime ) return qfalse;

  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( "^3!kill: ^7usage: kill [name|slot#]\n" );
    return qfalse;
  }
  G_SayArgv( skiparg, command, sizeof( command ) );
  cmd = command;
  if( cmd && *cmd == '!' )
    cmd++;
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS  ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!kill: ^7%s\n", err ) );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( "^3!kill: ^7sorry, but your intended victim has a higher admin"
        " level than you\n" );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];

  // cant kill spectators
  if( vic->client->pers.teamSelection == TEAM_NONE ||
      vic->client->pers.classSelection == PCL_NONE ) {
    ADMP( "^3!kill: ^7can't kill a ghost\n" );
    return qfalse;
  }

  // cant kill console?
  if( !vic ) {
    ADMP( "^3!kill: ^7bad target\n" );
    return qfalse;
  }

  if( g_killDamage.integer > 0 ) {
    if( G_SayArgc() > 2 + skiparg ) {
      char dmg_str[ MAX_STRING_CHARS ];
      G_SayArgv( 2 + skiparg, dmg_str, sizeof( dmg_str ) );
      damage = atoi(dmg_str);
      if( damage < 0 ) damage = 0;
    } else {
      if( g_killDamage.integer > 100 ) g_killDamage.integer = 100;
      damage = BG_Class( vic->client->ps.stats[ STAT_CLASS ] )->health *
        g_killDamage.integer / 100;
      if( damage < 1 ) damage = 1;
    }


  // play a sound
  //G_AddEvent( vic, EV_SLAP, 0 );

  if( vic != ent )
    trap_SendServerCommand( vic-g_entities,
      va( "cp \"%s^7 is ^1not amused!\n\"",
      ent ? ent->client->pers.netname : "console" ) );

    vic->health -= damage;
    vic->client->ps.stats[ STAT_HEALTH ] = vic->health;
    vic->lastDamageTime = level.time;
    if( vic->health <= 0 )
    {
      vic->enemy = &g_entities[ pids[ 0 ] ];
      vic->die( vic, ent, ent, damage, MOD_KILL );
    } else if( vic->pain ) vic->pain( vic, &g_entities[ pids[ 0 ] ], damage );
  }

  return qtrue;
}

qboolean G_admin_L0(gentity_t *ent, int skiparg ){
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], *reason, err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;

  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!L0: ^7usage: !L0 [name]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  reason = G_SayConcatArgs( 2 + skiparg );
  if( (found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!L0: ^7%s\n", err ) );
    return qfalse;
  }
  if( G_admin_level(&g_entities[ pids[ 0 ] ] )<1 )
  {
    ADMP( "^3!L0: ^7sorry, but the player is already"
        " level 0.\n" );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( "^3!L0: ^7sorry, but the player has a higher admin"
        " level than you\n" );
    return qfalse;
  }

  vic = &g_entities[ pids[ 0 ] ];
  //AP(va( "!setlevel %s 1\n",vic->client->pers.netname));
  trap_SendConsoleCommand( EXEC_APPEND,va( "!setlevel %d 0;", pids[ 0 ] ) );
  ClientUserinfoChanged( pids[ 0 ] );
  return qtrue;
}

qboolean G_admin_L1(gentity_t *ent, int skiparg ){
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], *reason, err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;

  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!L1: ^7usage: !L1 [name]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  reason = G_SayConcatArgs( 2 + skiparg );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!L1: ^7%s\n", err ) );
    return qfalse;
  }
  if( G_admin_level(&g_entities[ pids[ 0 ] ] )>0 )
  {
    ADMP( "^3!L1: ^7sorry, but that person is already higher than"
        " level 0.\n" );
    return qfalse;
  }

  vic = &g_entities[ pids[ 0 ] ];
  //AP(va( "!setlevel %s 1\n",vic->client->pers.netname));
  trap_SendConsoleCommand( EXEC_APPEND,va( "!setlevel %d 1;", pids[ 0 ] ) );
  ClientUserinfoChanged( pids[ 0 ] );
  return qtrue;
}

qboolean G_admin_L2(gentity_t *ent, int skiparg ){
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], *reason, err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;

  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!L2: ^7usage: !L2 [name]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  reason = G_SayConcatArgs( 2 + skiparg );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!L2: ^7%s\n", err ) );
    return qfalse;
  }
  if( G_admin_level(&g_entities[ pids[ 0 ] ] )>2 )
  {
    ADMP( "^3!L2: ^7sorry, but that person is already higher than"
        " level 2.\n" );
    return qfalse;
  }

  vic = &g_entities[ pids[ 0 ] ];
  //AP(va( "!setlevel %s 1\n",vic->client->pers.netname));
  trap_SendConsoleCommand( EXEC_APPEND,va( "!setlevel %d 2;", pids[ 0 ] ) );
  ClientUserinfoChanged( pids[ 0 ] );
  return qtrue;
}


qboolean G_admin_pause( gentity_t *ent, int skiparg )
{ //this is all very similar to denybuild, 
  //it performs an essentially identical function
  int i, j = 0, pids[ MAX_CLIENTS + 1 ], found;
  qboolean everyone = qfalse;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  char command[ MAX_ADMIN_CMD_LEN ], *cmd;
  qboolean targeted = qfalse;
  //targeted is set to ensure we don't get spam when pausing everybody
  gentity_t *vic;
  AP( va( "print \"skiparg:%d G_SayArgc:%d\n\"", skiparg, G_SayArgc()) );
  G_SayArgv( skiparg, command, sizeof( command ) );
  cmd = command;
  if( cmd && *cmd == '!' )
    cmd++;
  if( G_SayArgc() == 1 + skiparg )
  {
		everyone = qtrue;
    for( i = 0; i < MAX_CLIENTS; i++ )
    {
      vic = &g_entities[ i ];
      if( vic && vic->client )
      {
        pids[ j ] = i;
        j++;
      }
    }
      if( Q_stricmp( cmd, "unpause" ) )
        {
          AP( va( "print \"^3!pause: ^7%s^7 has paused the game \n\"",
          ( ent ) ? ent->client->pers.netname : "console" ) );
          level.paused = qfalse;
        }
        else
        {
          AP( va( "print \"^3!unpause: ^7%s^7 has unpaused the game\n\"",
          ( ent ) ? ent->client->pers.netname : "console" ) );
          level.paused = qfalse;
        }
      pids[ j ] = -1;
  }
  else if( G_SayArgc() == 2 + skiparg )
  {
    G_SayArgv( 1 + skiparg, name, sizeof( name ) );
    if( (found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS)) != 1 )
    {
      G_MatchOnePlayer( pids, found, err, sizeof( err ) );
      ADMP( va( "^3!%s: ^7%s\n", cmd, err ) );
      return qfalse;
    }
    targeted = qtrue;
  }
  else if( G_SayArgc() > 2 + skiparg )
  {
    ADMP( va( "^3!%s: ^7usage: ^3!%s ^7(^5name|slot^7)\n", cmd, cmd ) );
    return qfalse;
  }
  for( i = 0; pids[ i ] >= 0; i++ )
  {
    vic = &g_entities[ pids[ i ] ];
    if ( !vic || !vic->client || (!vic->client->pers.connected && (vic->client->pers.connected != CON_CONNECTED ||
        vic->client->pers.connected != CON_CONNECTING))) continue;
    if( !admin_higher( ent, vic ) )
    {
      if( targeted )
	ADMP( va( "^3!%s: ^7sorry, but your intended victim has a higher admin"
		    " level than you\n", cmd ) );
      continue;
    }
    if( vic->client->pers.paused )
    {
      if( !Q_stricmp( cmd, "pause" ) )
      {
        if( targeted )
          ADMP( "^3!pause: ^7player is already paused\n" );
        continue;
      }
      vic->client->pers.paused = qfalse;
      CPx( pids[ i ], "cp \"^2You've been unpaused\"" );
      if( targeted )
	AP( va( "print \"^3!unpause: ^7%s^7 unpaused by %s\n\"",
	    vic->client->pers.netname,
	    ( ent ) ? ent->client->pers.netname : "console" ) );
    }
    else
    {
      if( !Q_stricmp( cmd, "unpause" ) )
      {
	if( targeted )
	  ADMP( "^3!unpause: ^7player is already unpaused\n" );
	continue;
      }
      vic->client->pers.paused = qtrue;
      CPx( pids[ i ], va( "cp \"^1You've been paused by ^7%s\"", 
	  ( ent ) ? ent->client->pers.netname : "console" ) );
      if( targeted )
	AP( va( "print \"^3!pause: ^7%s^7 paused by %s\n\"", 
	    vic->client->pers.netname,
            ( ent ) ? ent->client->pers.netname : "console" ) );
    }
    ClientUserinfoChanged( pids[ i ] );
  }
  return qtrue;
}

qboolean G_admin_forcespec( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;

  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!forcespec: ^7usage: !forcespec [name|slot#]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );

  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!forcespec: ^7%s\n", err ) );
    return qfalse;
  }

  vic = &g_entities[ pids[ 0 ] ];

  if( !admin_higher( ent, vic ) )
  {
    ADMP( "^3!forcespec: ^7sorry, but your intended victim has a higher admin"
        " level than you\n" );
    return qfalse;
  }

  if ( vic->client->pers.specd == qtrue )
  {
  ADMP( "^3!forcespec: ^7player already forcespeced\n" );

  return qfalse;
  }

  //push them to the spec team
  G_ChangeTeam( vic, TEAM_NONE );
  //tell g_cmds that they cannot join teams
  vic->client->pers.specd = qtrue;
  //tell the person they cant join teams
  CPx( pids[ 0 ], "cp \"^1You may no longer join teams\"" );
  //tell everyone that you cant join teams
  AP( va( "print \"^3!forcespec: ^7%s^7 has disallowed joining of teams for ^7%s\n\"", ( ent ) ? ent->client->pers.netname : "console", vic->client->pers.netname ) );
  //tell g_cmds to save their creds
  vic->client->pers.saved = qtrue;
  return qtrue;

 }

qboolean G_admin_unforcespec( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;

  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!unforcespec: ^7usage: !unforcespec [name|slot#]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );

  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!unforcespec: ^7%s\n", err ) );
    return qfalse;
  }

  vic = &g_entities[ pids[ 0 ] ];

  if ( vic->client->pers.specd == qfalse )
  {
  ADMP( "^3!unforcespec: ^7player is not forcespeced\n" );

  return qfalse;
  }

  if( !admin_higher( ent, vic ) )
  {
    ADMP( "^3!unforcespec: ^7sorry, but your intended victim has a higher admin"
        " level than you\n" );
    return qfalse;
  }

  //remove the limitation
  vic->client->pers.specd = qfalse;
  //tell them it was removed
  CPx( pids[ 0 ], "cp \"^1You may now join teams\"" );
  //tell everyone that it was removed
  AP( va( "print \"^3!unforcespec: ^7%s^7 has allowed joining of teams for ^7%s\n\"", ( ent ) ? ent->client->pers.netname : "console", vic->client->pers.netname ) );
  return qtrue;
}

qboolean G_admin_buildlog( gentity_t *ent, int skiparg )
{
#define LOG_DISPLAY_LENGTH 10
  buildHistory_t *ptr;
  gentity_t *builder = NULL;
  int skip = 0, start = 0, lastID = -1, firstID = -1, i, len, matchlen = 0;
  team_t team = TEAM_NONE;
  char message[ MAX_STRING_CHARS ], *teamchar; 
  char *name, *action, *buildablename, markstring[ MAX_STRING_CHARS ]; 
  if( !g_buildLogMaxLength.integer )
  {
    ADMP( "^3!buildlog: ^7build logging is disabled" );
    return qfalse;
  }
  if( G_SayArgc( ) >= 2 + skiparg )
  {
    for( i = 1; i + skiparg < G_SayArgc( ); i++ )
    {
      char argbuf[ 64 ], err[ MAX_STRING_CHARS ];
      int x = 0, pids[ MAX_CLIENTS ];
      G_SayArgv( i + skiparg, argbuf, sizeof argbuf );
      switch( argbuf[ 0 ])
      {
        case 'x':
          x = 1;
        default:
          skip = atoi( argbuf + x );
          start = 0;
          break;
        case '#':
          start = atoi( argbuf + 1 );
          skip = 0;
          break;
        case '-':
          {
            int value;
            if( ( value = G_ClientNumbersFromString( argbuf + 1, pids, MAX_CLIENTS ) ) != 1 )
            {
              G_MatchOnePlayer( pids, value, err, sizeof( err ) );
              ADMP( va( "^3!buildlog: ^7%s\n", err ));
              return qfalse;
            }
            builder = g_entities + *pids;
          }
          break;
        case 'A':
        case 'a':
          team = TEAM_ALIENS;
          break;
        case 'H':
        case 'h':
          team = TEAM_HUMANS;
          break;
      }
    }
  }
  // !buildlog can be abused, so let everyone know when it is used
  AP( va( "print \"^3!buildlog: ^7%s^7 requested a log of recent building"
      " activity\n\"", ( ent ) ? ent->client->pers.netname : "console" ) );
  len = G_CountBuildLog( ); // also clips the log if too long
  if( !len )
  {
    ADMP( "^3!buildlog: ^7no build log found\n" );
    return qfalse;
  }
  if( start )
  {
    // set skip based on start
    for( ptr = level.buildHistory; ptr && ptr->ID != start; 
        ptr = ptr->next, skip++ );
    if( !ptr )
    {
      ADMP( "^3!buildlog: ^7log ID not found\n" );
      skip = 0;
    }
  }
  // ensure skip is a useful value
  if( skip > len - LOG_DISPLAY_LENGTH )
    skip = len - LOG_DISPLAY_LENGTH;
  *message = '\0';
  // skip to start entry
  for( ptr = level.buildHistory, i = len; ptr && i > len - skip; 
      ptr = ptr->next )
  {
    // these checks could perhaps be done more efficiently but they are cheap
    // in processor time so I'm not worrying
    if( team != TEAM_NONE && team != BG_FindTeamForBuildable( ptr->buildable ) )
      continue;
    if( builder && builder != ptr->ent )
      continue;
    matchlen++;
    i--;
  }
  for( ; i + LOG_DISPLAY_LENGTH > len - skip && i > 0; i--, ptr = ptr->next )
  {
    if( !ptr ) 
      break; // run out of log
    *markstring = '\0'; // reinit markstring
    // check team
    if( ( team != TEAM_NONE && team != BG_FindTeamForBuildable( ptr->buildable ) )
        || ( builder && builder != ptr->ent ) )
    {
      skip++; // loop an extra time because we skipped one
      continue; 
    }
    if( lastID < 0 )
      lastID = ptr->ID;
    firstID = ptr->ID;
    matchlen++;
    // set name to the ent's current name or last recorded name
    if( ptr->ent )
    {
      if( ptr->ent->client )
        name = ptr->ent->client->pers.netname;
      else
        name = "<world>"; // any non-client action
    }
    else
      name = ptr->name;
    switch( ptr->fate )
    {
      case BF_BUILT:
        action = "^2built^7 a";
        break;
      case BF_DECONNED:
        action = "^3DECONSTRUCTED^7 a";
        break;
      case BF_DESTROYED:
        action = "destroyed a";
        break;
      case BF_TEAMKILLED:
        action = "^1TEAMKILLED^7 a";
        break;
      default:
        action = "\0"; // erm
        break;
    }
    // handle buildables removed by markdecon
    if( ptr->marked )
    {
      buildHistory_t *mark;
      int j, markdecon[ BA_NUM_BUILDABLES ], and = 2;
      char bnames[ 32 ], *article;
      mark = ptr;
      // count the number of buildables
      memset( markdecon, 0, sizeof( markdecon ) );
      while( ( mark = mark->marked ) )
        markdecon[ mark->buildable ]++;
      // reverse order makes grammar easier
      for( j = BA_NUM_BUILDABLES; j >= 0; j-- )
      {
        buildablename = BG_FindHumanNameForBuildable( j );
        // plural is easy
        if( markdecon[ j ] > 1 )
          Com_sprintf( bnames, 32, "%d %ss", markdecon[ j ], buildablename );
        // use an appropriate article
        else if( markdecon[ j ] == 1 )
        {
          if( BG_FindUniqueTestForBuildable( j ) )
            article = "the"; // if only one
          else if( strchr( "aeiouAEIOU", *buildablename ) )
            article = "an"; // if first char is vowel
          else
            article = "a";
          Com_sprintf( bnames, 32, "%s %s", article, buildablename );
        }
        else
          continue; // none of this buildable
        // C grammar: x, y, and z
        // the integer and is 2 initially, the test means it is used on the
        // second sprintf only, the reverse order makes this second to last
        // the comma is only printed if there is already some markstring i.e.
        // not the first time ( which would put it on the end of the string )
        Com_sprintf( markstring, sizeof( markstring ), "%s%s %s%s", bnames, 
            ( *markstring ) ? "," : "", ( and-- == 1 ) ? "and " : "", markstring );
      }
    }
    buildablename = BG_FindHumanNameForBuildable( ptr->buildable );
    switch( BG_FindTeamForBuildable( ptr->buildable ) )
    {
      case TEAM_ALIENS: 
        teamchar = "^1A"; 
        break;
      case TEAM_HUMANS: 
        teamchar = "^4H"; 
        break;
      default: 
        teamchar = " "; // space so it lines up neatly
        break;
    }
    // prepend the information to the string as we go back in buildhistory
    // so the earliest events are at the top
    Com_sprintf( message, MAX_STRING_CHARS, "%3d %s^7 %s^7 %s%s %s%s%s\n%s", 
        ptr->ID, teamchar, name, action, 
        ( strchr( "aeiouAEIOU", buildablename[ 0 ] ) ) ? "n" : "", 
        buildablename, ( markstring[ 0 ] ) ? ", removing " : "", 
        markstring, message );
  }
  for( ; ptr; ptr = ptr->next )
  {
    if( builder && builder != ptr->ent )
      continue;
    if( team != TEAM_NONE && team != BG_FindTeamForBuildable( ptr->buildable ) )
      continue;
    matchlen++;
  }
  if( matchlen )
    ADMP( va( "%s^3!buildlog: showing log entries %d - %d of %d\n", message,
        firstID, lastID, matchlen ) );
  else
    ADMP( "^3!buildlog: ^7no log entries match those criteria\n" );
  return qtrue;
}

qboolean G_admin_revert( gentity_t *ent, int skiparg )
{
  int i = 0, j = 0, repeat = 1, ID = 0, len;
  team_t team = TEAM_NONE;
  qboolean force = qfalse, reached = qfalse;
  gentity_t *builder = NULL, *targ;
  buildHistory_t *ptr, *tmp, *mark, *prev;
  vec3_t dist;
  char argbuf[ 64 ], *name, *bname, *action, *article;
  len = G_CountBuildLog( );
  if( !len )
  {
    ADMP( "^3!revert: ^7no build log found\n" );
    return qfalse;
  }
  if( G_SayArgc( ) < 2 + skiparg )
  {
    ADMP( "^3!revert: ^7usage: !revert (^5xnum^7) (^5#ID^7) (^5-name|num^7) (^5a|h^7)\n" );
    return qfalse;
  }
  for( i = 1; i + skiparg < G_SayArgc( ); i++ )
  {
    char arg[ 64 ], err[ MAX_STRING_CHARS ];
    int pids[ MAX_CLIENTS ];
    G_SayArgv( i + skiparg, arg, sizeof arg );
    switch( arg[ 0 ])
    {
      case 'x':
        repeat = atoi( arg + 1 );
        break;
      case '#':
        ID = atoi( arg + 1 );
        break;
      case '-':
        {
          int value;
          if( ( value = G_ClientNumbersFromString( arg + 1, pids, MAX_CLIENTS ) ) != 1 )
          {
            G_MatchOnePlayer( pids, value, err, sizeof( err ) );
            ADMP( va( "^3!revert: ^7%s\n", err ));
            return qfalse;
          }
          builder = g_entities + *pids;
        }
        break;
      case 'A':
      case 'a':
        team = TEAM_ALIENS;
        break;
      case 'H':
      case 'h':
        team = TEAM_HUMANS;
        break;
      case '!':
        force = qtrue;
        break;
      default:
        ADMP( "^3!revert: ^7usage: !revert (^5xnum^7) (^5#ID^7) (^5-name|num^7) (^5a|h^7)\n" );
        return qfalse;
    }
  }
  if( repeat > 25 )
  {
    ADMP( "^3!revert: ^7to avoid flooding, can only revert 25 builds at a time\n" );
    repeat = 25;
  }
  for( i = 0, ptr = prev = level.buildHistory; repeat > 0; repeat--, j = 0 )
  {
    if( !ptr ) 
      break; // run out of bhist
    if( !reached && ID )
    {
      if( ptr->ID == ID )
        reached = qtrue;
      else
      {
        prev = ptr;
        ptr = ptr->next;
        repeat++;
        continue;
      }
    }
    if( ( team != TEAM_NONE && 
        team != BG_FindTeamForBuildable( ptr->buildable ) ) || 
        ( builder && builder != ptr->ent ))
    {
      // team doesn't match, so skip this ptr and reset prev
      prev = ptr;
      ptr = ptr->next;
      // we don't want to count this one so counteract the decrement by the for
      repeat++;
      continue;
    }
    // get the ent's current or last recorded name
    if( ptr->ent )
    {
      if( ptr->ent->client )
        name = ptr->ent->client->pers.netname;
      else
        name = "<world>"; // non-client actions
    }
    else
      name = ptr->name;
    bname = BG_FindHumanNameForBuildable( ptr->buildable ); 
    action = "";
    switch( ptr->fate )
    {
      case BF_BUILT:
        action = "build";
        for( j = MAX_CLIENTS, targ = g_entities + j;
            j < level.num_entities; j++, targ++ )
        {
          // easy checks first to save time
          if( targ->s.eType != ET_BUILDABLE )
            continue;
          if( targ->s.modelindex != ptr->buildable )
            continue; 
          VectorSubtract( targ->s.pos.trBase, ptr->origin, dist );
          if( VectorLength( dist ) > 10 )
            continue; // number is somewhat arbitrary, watch for false pos/neg
          // if we didn't continue then it's this one, unlink it but we can't
          // free it yet, because the markdecon buildables might not place
          trap_UnlinkEntity( targ );
          break;
        }
        // if there are marked buildables to replace, and we aren't overriding 
        // space check, check they can fit before acting
        if( ptr->marked && !force )
        {
          for( mark = ptr->marked; mark; mark = mark->marked )
            if( !G_RevertCanFit( mark ) )
            {
              trap_LinkEntity( targ ); // put it back, we failed
              // scariest sprintf ever:
              Com_sprintf( argbuf, sizeof argbuf, "%s%s%s%s%s%s%s!",
                  ( repeat > 1 ) ? "x" : "", ( repeat > 1 ) ? va( "%d ", repeat ) : "",
                  ( ID ) ? "#" : "", ( ID ) ? va( "%d ", ptr->ID ) : "",
                  ( builder ) ? "-" : "", ( builder ) ? va( "%d ", builder - g_entities ) : "", 
                  ( team == TEAM_ALIENS ) ? "a " : ( team == TEAM_HUMANS ) ? "h " : "" );
              ADMP( va( "^3!revert: ^7revert aborted: reverting this %s would conflict with "
                  "another buildable, use ^3!revert %s ^7to override\n", action, argbuf ) );
              return qfalse;
            }
          // if we haven't returned yet then we're good to go, free it
          G_FreeEntity( targ );
          // put the marked buildables back and mark them again
          if( ptr->marked ) // there may be a more efficient way of doing this
          {
            for( mark = ptr->marked; mark; mark = mark->marked )
              G_SpawnRevertedBuildable( mark, qtrue ); 
          }
        }
        break;
      case BF_DECONNED:
        action = "deconstruction";
      case BF_DESTROYED:
      case BF_TEAMKILLED:
        if( !action[ 0 ] ) 
          action = "destruction";
        // if we're not overriding and the replacement can't fit, as before
        if( !force && !G_RevertCanFit( ptr ) )
        {
          Com_sprintf( argbuf, sizeof argbuf, "%s%s%s%s%s%s%s!",
              ( repeat > 1 ) ? "x" : "", ( repeat > 1 ) ? va( "%d ", repeat ) : "",
              ( ID ) ? "#" : "", ( ID ) ? va( "%d ", ptr->ID ) : "",
              ( builder ) ? "-" : "", ( builder ) ? va( "%d ", builder - g_entities ) : "", 
              ( team == TEAM_ALIENS ) ? "a " : ( team == TEAM_HUMANS ) ? "h " : "" );
          ADMP( va( "^3!revert: ^7revert aborted: reverting this %s would "
              "conflict with another buildable, use ^3!revert %s ^7to override\n",
              action, argbuf ) );
          return qfalse;
        }
        // else replace it but don't mark it ( it might have been marked before
        // but it isn't that important )
        G_SpawnRevertedBuildable( ptr, qfalse );
        break;
      default:
        // if this happens something has gone wrong
        ADMP( "^3!revert: ^7incomplete or corrupted build log entry\n" );
        /* quarantine and dispose of the log, it's dangerous
        trap_Cvar_Set( "g_buildLogMaxLength", "0" );
        G_CountBuildLog( );
        */
        return qfalse;
    }
    if( j == level.num_entities )
    {
      ADMP( va( "^3!revert: ^7could not find logged buildable #%d\n", ptr->ID ));
      prev = ptr;
      ptr = ptr->next;
      continue;
    }
    // this is similar to the buildlog stuff
    if( BG_FindUniqueTestForBuildable( ptr->buildable ) )
      article = "the";
    else if( strchr( "aeiouAEIOU", *bname ) )
      article = "an";
    else
      article = "a";
    AP( va( "print \"%s^7 reverted %s^7'%s %s of %s %s\n\"",
        ( ent ) ? ent->client->pers.netname : "console",
        name, strchr( "Ss", name[ strlen( name ) - 1 ] ) ? "" : "s",
        action, article, bname ) ); 
    // remove the reverted entry
    // ptr moves on, prev just readjusts ->next unless it is about to be 
    // freed, in which case it is forced to move on too
    tmp = ptr;
    if( ptr == level.buildHistory )
      prev = level.buildHistory = ptr = ptr->next;
    else
      prev->next = ptr = ptr->next;
    BG_Free( tmp );
  }
  if( ID && !reached )
  {
    ADMP( "^3!revert: ^7no buildlog entry with that ID\n" );
    return qfalse;
  }
  return qtrue;
}



qboolean G_admin_info( gentity_t *ent, int skiparg )
{
  fileHandle_t infoFile;
  int length;
  char filename[ MAX_OSPATH ], message[ MAX_STRING_CHARS ]; 
  if( G_SayArgc() == 2 + skiparg )
    G_SayArgv( 1 + skiparg, filename, sizeof( filename ) );
  else if( G_SayArgc() == 1 + skiparg )
    Q_strncpyz( filename, "default", sizeof( filename ) );
  else
  {
    ADMP( "^3!info: ^7usage: ^3!info ^7(^5subject^7)\n" );
    return qfalse;
  }
  Com_sprintf( filename, sizeof( filename ), "info/info-%s.txt", filename );
  length = trap_FS_FOpenFile( filename, &infoFile, FS_READ );
  if( length <= 0 || !infoFile )
  {
    trap_FS_FCloseFile( infoFile );
    ADMP( "^3!info: ^7no relevant information is available\n" );
    return qfalse;
  }
  else
  {
    int i;
    char *cr;
    trap_FS_Read( message, sizeof( message ), infoFile );
    if( length < sizeof( message ) )
      message[ length ] = '\0';
    else
      message[ sizeof( message ) - 1 ] = '\0';
    trap_FS_FCloseFile( infoFile );
    // strip carriage returns for windows platforms
    while( ( cr = strchr( message, '\r' ) ) )
      memmove( cr, cr + 1, strlen( cr + 1 ) + 1 );
#define MAX_INFO_PARSE_LOOPS 100
    for( i = 0; i < MAX_INFO_PARSE_LOOPS &&
        G_StringReplaceCvars( message, message, sizeof( message ) ); i++ );
    G_Unescape( message, message, sizeof( message ) );
    if( i == MAX_INFO_PARSE_LOOPS )
      G_Printf( S_COLOR_YELLOW "WARNING: %s exceeds MAX_INFO_PARSE_LOOPS\n", filename );
    ADMP( va( "%s\n", message ) );
    return qtrue;
  }
}

void G_Unescape( char *input, char *output, int len )
{
  // \n -> newline, \%c -> %c
  // output is terminated at output[len - 1]
  // it's OK for input to equal output, because our position in input is always
  // equal or greater than our position in output
  // however, if output is later in the same string as input, a crash is pretty
  // much inevitable
  int i, j;
  for( i = j = 0; input[i] && j + 1 < len; i++, j++ )
  {
    if( input[i] == '\\' )
    {
      if( !input[++i] )
      {
        output[j] = '\0';
        return;
      }
      else if( input[i] == 'n' )
        output[j] = '\n';
      else
        output[j] = input[i];
    }
    else
      output[j] = input[i];
  }
  output[j] = '\0';
}

qboolean G_StringReplaceCvars( char *input, char *output, int len )
{
  int i, outNum = 0;
  char cvarName[ 64 ], cvarValue[ MAX_CVAR_VALUE_STRING ];
  char *outputBuffer;
  qboolean doneAnything = qfalse;
  if( len <= 0 )
    return qfalse;
  // use our own internal buffer in case output == input
  outputBuffer = BG_Alloc( len );
  len -= 1; // fit in a terminator
  while( *input && outNum < len )
  {
    if( *input == '\\' && input[1] && outNum < len - 1 )
    {
      outputBuffer[ outNum++ ] = *input++;
      outputBuffer[ outNum++ ] = *input++;
    }
    else if( *input == '$' )
    {
      doneAnything = qtrue;
      input++;
      if( *input == '{' ) 
        input++;
      for( i = 0; *input && ( isalnum( *input ) || *input == '_' ) && 
          i < 63; i++ )
        cvarName[ i ] = *input++;
      cvarName[ i ] = '\0';
      if( *input == '}' ) 
        input++;
      trap_Cvar_VariableStringBuffer( cvarName, cvarValue, sizeof( cvarValue ) );
      if( cvarValue[ 0 ] )
      {
        for( i = 0; cvarValue[ i ] && outNum < len; i++ )
          outputBuffer[ outNum++ ] = cvarValue[ i ];
      }
    }
    else
      outputBuffer[ outNum++ ] = *input++;
  }
  outputBuffer[ outNum ] = '\0';
  Q_strncpyz( output, outputBuffer, len );
  BG_Free( outputBuffer );
  return doneAnything;
}

qboolean G_admin_switch( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;


    minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!switch: ^7usage: !switch [name|slot#]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );

  if( (found = G_ClientNumbersFromString( name, pids , MAX_CLIENTS)) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!switch: ^7%s\n", err ) );
    return qfalse;
  }

  vic = &g_entities[ pids[ 0 ] ];


//put them on noclip
vic->client->noclip = qtrue;
ent->client->noclip = qtrue;

//switch places
  trap_UnlinkEntity( ent );
  VectorCopy( vic->s.origin, ent->client->ps.origin );

//switch places
  trap_UnlinkEntity( vic );
  VectorCopy( ent->s.origin, vic->client->ps.origin );

//spectator fix
  if( ent->client->ps.stats[ STAT_TEAM ] != TEAM_NONE )
    trap_LinkEntity (ent);

  if( vic->client->ps.stats[ STAT_TEAM ] != TEAM_NONE )
    trap_LinkEntity (vic);


//take them off noclip
vic->client->noclip = qfalse;
ent->client->noclip = qfalse;

     trap_SendServerCommand( vic-g_entities, va( "print \"^7%s^7 switched with you\n\"", ent->client->pers.netname ) );
     trap_SendServerCommand( ent-g_entities, va( "print \"^7you switched with ^7%s^7\n\"", vic->client->pers.netname ) );



  return qtrue;

}

qboolean G_admin_report( gentity_t *ent, int skiparg )
{
#define REPORTSIZE ( (1 + MAX_CLIENTS) * ( MAX_NETNAME + 32 + 15 + 6 ) )
//that's name [GUID] (ip) for every client and again for reporter
  char report[ MAX_STRING_CHARS + REPORTSIZE ], buffer[ MAX_STRING_CHARS ];
  int i, length;
  clientPersistant_t *per = &ent->client->pers;
  fileHandle_t reportfile;
  if( !ent || !ent->client ) return qfalse; // console can't report... yet
  if( !g_reportFile.string[0] )
  {
    ADMP( "^3!report: ^7This server does not support reporting\n" );
    return qfalse;
  }
  else if( per->reportCount >= g_reportLimit.integer && 
    g_reportLimit.integer )
  {
    ADMP( "^3!report: ^7you have reached your report submission limit "
    "for this map\n" );
    return qfalse;
  }
  else 
    length = trap_FS_FOpenFile( g_reportFile.string, &reportfile, FS_APPEND );
  if( !reportfile )
  { // if it didn't already exist make one
    length = trap_FS_FOpenFile( g_reportFile.string, &reportfile, FS_WRITE );
  }

  Com_sprintf( report, MAX_STRING_CHARS, "%s [%s] (%s): %s"
                                  "\n\nClients connected:\n", 
          per->netname, per->ip, ( per->guid ) ? per->guid : 
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",  
    G_SayConcatArgs( 1 + skiparg ) );
  for( i = 0; i < MAX_CLIENTS; i++ )
  {
    if( g_entities[ i ].client && 
        g_entities[ i ].client->pers.connected == CON_CONNECTED )
    {
      per = &g_entities[ i ].client->pers;
      Com_sprintf( buffer, MAX_STRING_CHARS, "%s [%s] (%s) %s\n",
              per->netname, per->ip, ( per->guid ) ? per->guid : 
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 
        ( &ent->client->pers == per ) ? "- reporter" : "" );
      Q_strcat( report, REPORTSIZE, buffer );
    }
  }

  Q_strcat( report, REPORTSIZE + MAX_STRING_CHARS, "----------\n\n" );

  trap_FS_Write( report, MAX_STRING_CHARS, reportfile );
  ADMP( "^3!report: ^7your report has been noted for later review\n" );
  ent->client->pers.reportCount++;
  trap_FS_FCloseFile( reportfile );
  return qtrue;
}

qboolean G_admin_drug( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  int minargc;
  gentity_t *vic;


    minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!drug: ^7usage: !drug [name|slot#]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );

  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!drug: ^7%s\n", err ) );
    return qfalse;
  }

  vic = &g_entities[ pids[ 0 ] ];

  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( "^3!drug: ^7sorry, but that player has a higher admin"
        " level than you\n" );
    return qfalse;
  }

        vic->client->ps.eFlags |= EF_POISONCLOUDED;
        vic->client->lastPoisonCloudedTime = level.time;
        trap_SendServerCommand( vic->client->ps.clientNum, "poisoncloud" );

  return qtrue;

}


qboolean G_admin_fireworks( gentity_t *ent, int skiparg )
{
  char teamName[2] = {""};
  team_t team;
  int i;

  if( G_SayArgc() > 2 + skiparg )
  {
    ADMP( "^3!fireworks: ^7usage: !fireworks [a|h]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, teamName, sizeof( teamName ) );
  if( teamName[ 0 ] == 'a' || teamName[ 0 ] == 'A' )
    team = TEAM_ALIENS;
  else if( teamName[ 0 ] == 'h' || teamName[ 0 ] == 'H' )
    team = TEAM_HUMANS;
  else
    team = TEAM_NONE;
  for( i = 1, ent = g_entities + i ; i < level.num_entities ; i++, ent++ )
  {
    if( !ent->inuse )
      continue;

    if( !Q_stricmp( ent->classname, "trigger_win" ) )
    {
      if( team == TEAM_NONE || team == ent->stageTeam )
        ent->use( ent, ent, ent );
    }
  }
  return qtrue;
} 

qboolean G_admin_slap( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], damage = 0, found;
  char name[MAX_NAME_LENGTH], err[MAX_STRING_CHARS], command[MAX_ADMIN_CMD_LEN], *cmd;
  gentity_t *vic;
  vec3_t dir;
 
  if( level.intermissiontime ) return qfalse;

  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( "^3!slap: ^7usage: slap [name|slot#]\n" );
    return qfalse;
  }
  G_SayArgv( skiparg, command, sizeof( command ) );
  cmd = command;
  if( cmd && *cmd == '!' )
    cmd++;
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS )) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!slap: ^7%s\n", err ) );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( "^3!slap: ^7sorry, but your intended victim has a higher admin"
        " level than you\n" );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];

  // cant slap spectators
  if( vic->client->pers.teamSelection == TEAM_NONE ||
      vic->client->pers.classSelection == PCL_NONE ) {
    ADMP( "^3!slap: ^7can't slap thin air\n" );
    return qfalse;
  }

  // cant slap console?
  if( !vic ) {
    ADMP( "^3!slap: ^7bad target\n" );
    return qfalse;
  }

  // knockback in a random direction
  dir[0] = crandom();
  dir[1] = crandom();
  dir[2] = random();
  G_Knockback( vic, dir, g_slapKnockback.integer );

  // play a sound
  //G_AddEvent( vic, EV_SLAP, 0 );

  if( vic != ent )
    trap_SendServerCommand( vic-g_entities,
      va( "cp \"%s^7 is ^1not amused!\n\"",
      ent ? ent->client->pers.netname : "console" ) );

  if( g_slapDamage.integer > 0 ) {

    // !slap (name) [damage] syntax
    if( G_SayArgc() > 2 + skiparg ) {
      char dmg_str[ MAX_STRING_CHARS ];
      G_SayArgv( 2 + skiparg, dmg_str, sizeof( dmg_str ) );
      damage = atoi(dmg_str);
      if( damage < 0 ) damage = 0;
    } else {
      if( g_slapDamage.integer > 100 ) g_slapDamage.integer = 100;
      damage = BG_FindHealthForClass( vic->client->ps.stats[ STAT_CLASS ] ) *
        g_slapDamage.integer / 100;
      if( damage < 1 ) damage = 1;
    }

    vic->health -= damage;
    vic->client->ps.stats[ STAT_HEALTH ] = vic->health;
    vic->lastDamageTime = level.time;
    if( vic->health <= 0 )
    {
      vic->flags |= FL_NO_KNOCKBACK;
      vic->enemy = &g_entities[ pids[ 0 ] ];
      vic->die( vic, ent, ent, damage, MOD_SLAP );
    } else if( vic->pain ) vic->pain( vic, &g_entities[ pids[ 0 ] ], damage );
  }

  return qtrue;
}


qboolean G_admin_detonate( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  int oldhealth, damage = 0;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  char command[ MAX_ADMIN_CMD_LEN ], *cmd;
  gentity_t *vic;
  gentity_t *tent;

  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( "^3!detonate: ^7usage: !detonate: [name|slot#]\n" );
    return qfalse;
  }
  G_SayArgv( skiparg, command, sizeof( command ) );
  cmd = command;
  if( cmd && *cmd == '!' )
    cmd++;
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!detonate: ^7%s\n", err ) );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( "^3!detonate: ^7sorry, but your intended victim has a higher admin"
        " level than you\n" );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];

  //make them explode...
  tent = G_TempEntity( vic->s.origin, EV_HUMAN_BUILDABLE_EXPLOSION );
  oldhealth = vic->health;
  damage = BG_FindHealthForClass( vic->client->ps.stats[ STAT_CLASS ] );
  vic->health -= damage;
  vic->client->ps.stats[ STAT_HEALTH ] = vic->health;
  vic->lastDamageTime = level.time;
  if( vic->health <= 0 )
  {
    vic->enemy = ent;
    vic->die( vic, ent, ent, damage, MOD_KILL );
  }
  CPx( pids[ 0 ], "cp \"^1You've been detonated!\"" );
  AP( va( "print \"^3!detonate: ^7%s^7 was detonated by ^7%s\n\"",
          vic->client->pers.netname,
          ( ent ) ? ent->client->pers.netname : "console" ) );

  //ClientUserinfoChanged( pids[ 0 ] );
  return qtrue;
}

qboolean G_admin_designate( gentity_t *ent, int skiparg )
{
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  char command[ MAX_ADMIN_CMD_LEN ], *cmd;
  gentity_t *vic;

  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( "^3!designate: ^7usage: designate [name|slot#]\n" );
    return qfalse;
  }
  G_SayArgv( skiparg, command, sizeof( command ) );
  cmd = command;
  if( cmd && *cmd == '!' )
    cmd++;
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  if( (found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS )) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!designate: ^7%s\n", err ) );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) &&
    !Q_stricmp( cmd, "undesignate" ) )
  {
    ADMP( "^3!mute: ^7sorry, but your intended victim has a higher admin"
        " level than you\n" );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];
  if( vic->client->pers.designatedBuilder == qtrue )
  {
    if( !Q_stricmp( cmd, "designate" ) )
    {
      ADMP( "^3!designate: ^7player is already designated builder\n" );
      return qtrue;
    }
    vic->client->pers.designatedBuilder = qfalse;
    CPx( pids[ 0 ], "cp \"^1Your designation has been revoked\"" );
    AP( va(
      "print \"^3!designate: ^7%s^7's designation has been revoked by %s\n\"",
       vic->client->pers.netname,
       ( ent ) ? ent->client->pers.netname : "console" ) );
    G_CheckDBProtection( );
  }
  else
  {
    if( !Q_stricmp( cmd, "undesignate" ) )
    {
      ADMP( "^3!undesignate: ^7player is not currently designated builder\n" );
      return qtrue;
    }
    vic->client->pers.designatedBuilder = qtrue;
    CPx( pids[ 0 ], "cp \"^1You've been designated\"" );
    AP( va( "print \"^3!designate: ^7%s^7 has been designated by ^7%s\n\"",
      vic->client->pers.netname,
      ( ent ) ? ent->client->pers.netname : "console" ) );
  }
  ClientUserinfoChanged( pids[ 0 ] );
  return qtrue;
}


qboolean G_admin_layoutsave( gentity_t *ent, int skiparg )
{
  char layout[ MAX_QPATH ];

  if( G_SayArgc( ) < 2 + skiparg )
  {
    ADMP( "^3!layoutsave: ^7usage: !layoutsave [layout]\n" );
    return qfalse;
  }

  G_SayArgv( skiparg + 1, layout, sizeof( layout ) );

  trap_SendConsoleCommand( EXEC_APPEND, va( "layoutsave %s", layout ) );
  AP( va( "print \"^3!layoutsave: ^7layout saved as '%s' by %s\n\"", layout,
          ( ent ) ? ent->client->pers.netname : "console" ) );
  return qtrue;
}

qboolean G_admin_cp( gentity_t *ent, int skiparg )
{
  int minargc;
  char *s;

  minargc = 2 + skiparg;

  if( G_SayArgc() < minargc )
  {
    ADMP( "^3!cp: ^7usage: !cp [message]\n" );
    return qfalse;
  }

  s = G_SayConcatArgs( 1 + skiparg );
  G_CP(ent);
  return qtrue;
}


#define MAX_LISTMAPS_MAPS 128

static int SortMaps(const void *a, const void *b)
{
  return strcmp(*(char **)a, *(char **)b);
}

qboolean G_admin_listmaps( gentity_t *ent, int skiparg )
{
  char fileList[ 4096 ] = {""};
  char *fileSort[ MAX_LISTMAPS_MAPS ];
  char search[ 16 ] = {""};
  int numFiles;
  int i;
  int fileLen = 0;
  int  count = 0;
  char *filePtr;
  int rows;

  if( G_SayArgc( ) > 1 + skiparg )
  {
    G_SayArgv( skiparg + 1, search, sizeof( search ) );
  }

  numFiles = trap_FS_GetFileList( "maps/", ".bsp",
    fileList, sizeof( fileList ) );
  filePtr = fileList;
  for( i = 0; i < numFiles && count < MAX_LISTMAPS_MAPS; i++, filePtr += fileLen + 1 )
  {
    fileLen = strlen( filePtr );
    if (fileLen < 5)
      continue;

    filePtr[ fileLen - 4 ] = '\0';

    if( search[ 0 ] && !strstr( filePtr, search ) )
      continue;

    fileSort[ count ] = filePtr;
    count++;
  }

  qsort(fileSort, count, sizeof(fileSort[ 0 ]), SortMaps);

  rows = count / 3;
  if ( rows * 3 < count ) rows++;

  ADMBP_begin();
  for( i = 0; i < rows; i++ )
  {
    ADMBP( va( "^7%20s %20s %20s\n",
      fileSort[ i ],
      ( rows + i < count ) ? fileSort[ rows + i ] : "",
      ( rows * 2 + i < count ) ? fileSort[ rows * 2 + i ] : "" ) );
  }
  if ( search[ 0 ] )
    ADMBP( va( "^3!listmaps: ^7found %d maps matching '%s^7'.\n", count, search ) );
  else
    ADMBP( va( "^3!listmaps: ^7listing %d maps.\n", count ) );

  ADMBP_end();

  return qtrue;
}


void G_admin_maplog_update( void )
{
  char map[ 64 ];
  char maplog[ MAX_CVAR_VALUE_STRING ];
  char *ptr;
  int count = 0;

  trap_Cvar_VariableStringBuffer( "mapname", map, sizeof( map ) );

  Q_strncpyz( maplog, g_adminMapLog.string, sizeof( maplog ) );
  ptr = maplog;
  while( *ptr && count < MAX_ADMIN_MAPLOG_LENGTH ) 
  {
    while( *ptr != ' ' && *ptr != '\0' ) ptr++;

    count++;
    if( count >= MAX_ADMIN_MAPLOG_LENGTH )
    {
      *ptr = '\0';
    }

    if( *ptr == ' ' ) ptr++;
  }

  trap_Cvar_Set( "g_adminMapLog", va( "%s%s%s",
    map,
    ( maplog[0] != '\0'  ) ? " " : "",
    maplog ) );
}

void G_admin_maplog_result( char *flag )
{
  char maplog[ MAX_CVAR_VALUE_STRING ];
  int t;

  if( !flag )
    return;

  if( g_adminMapLog.string[ 0 ] &&
    g_adminMapLog.string[ 1 ] == ';' )
  {
    // only one result allowed
    return;
  }
  
  if ( level.surrenderTeam != TEAM_NONE )
  {
    if( flag[ 0 ] == 'a' )
    {
      if( level.surrenderTeam == TEAM_HUMANS )
        flag = "A";
    }
    else if( flag[ 0 ] == 'h' )
    {
      if( level.surrenderTeam == TEAM_ALIENS )
        flag = "H";
    }
  }

  t = ( level.time - level.startTime ) / 1000;
  Q_strncpyz( maplog, g_adminMapLog.string, sizeof( maplog ) );
  trap_Cvar_Set( "g_adminMapLog", va( "%1s;%03d:%02d;%s",
    flag,
    t / 60, t % 60,
    maplog ) );
}


qboolean G_admin_maplog( gentity_t *ent, int skiparg )
{
  char maplog[ MAX_CVAR_VALUE_STRING ];
  char *ptr;
  int count = 0;

  Q_strncpyz( maplog, g_adminMapLog.string, sizeof( maplog ) );

  ADMBP_begin( );
  ptr = maplog;
  while( *ptr != '\0' && count < MAX_ADMIN_MAPLOG_LENGTH + 1 )
  {
    char *end;
    const char *result = NULL;
    char *clock = NULL;
    char *colon;

    end = ptr;
    while( *end != ' ' && *end != '\0' ) end++;
    if( *end == ' ' )
    {
      *end = '\0';
      end++;
    }

     if( ptr[ 0 ] && ptr[ 1 ] == ';' )
     {
       switch( ptr[ 0 ] )
       {
         case 't':
           result = "^7tie";
           break;
         case 'a':
           result = "^1Alien win";
           break;
         case 'A':
           result = "^1Alien win ^7/ Humans admitted defeat";
           break;
         case 'h':
           result = "^4Human win";
           break;
         case 'H':
           result = "^4Human win ^7/ Aliens admitted defeat";
           break;
         case 'd':
           result = "^5draw vote";
           break;
         case 'N':
           result = "^6admin loaded next map";
           break;
         case 'r':
           result = "^2restart vote";
           break;
         case 'R':
           result = "^6admin restarted map";
           break;
         case 'm':
           result = "^2map vote";
           break;
         case 'M':
           result = "^6admin changed map";
           break;
         case 'D':
           result = "^6admin loaded devmap";
           break;
         default:
           result = "";
           break;
       }
       ptr += 2;
       colon = strchr( ptr, ';' );
       if ( colon )
       {
         clock = ptr;
         ptr = colon + 1;
         *colon = '\0';
 
         // right justification with -6%s doesnt work..
         if( clock[ 0 ] == '0' && clock[ 1 ] != ':' )
         {
           if( clock[ 1 ] == '0' && clock[ 2 ] != ':' )
             clock[ 1 ] = ' ';
           clock[ 0 ] = ' ';
         }
       }
     }
     else if( count == 0 )
     {
       result = "^3current map";
       clock = "  -:--";
     }
 
     ADMBP( va( "%s%20s %-6s %s^7\n",
       ( count == 0 ) ? "^3" : "^7",
       ptr,
       ( clock ) ? clock : "",
       ( result ) ? result : "" ) );

    ptr = end;
    count++;
  }
  ADMBP_end( );

  return qtrue;
}

qboolean G_admin_listrotation( gentity_t *ent, int skiparg )
{
  int i, j, statusColor;
  char *status = '\0';

  extern mapRotations_t mapRotations;

  // Check for an active map rotation
  if ( !G_MapRotationActive() ||
       g_currentMapRotation.integer == NOT_ROTATING )
  {
    trap_SendServerCommand( ent-g_entities, "print \"^3!rotation: ^7There is no active map rotation on this server\n\"" );
    return qfalse;
  }

  // Locate the active map rotation and output its contents
  for( i = 0; i < mapRotations.numRotations; i++ )
  {
    if ( i == g_currentMapRotation.integer )
    {
      ADMBP_begin();
      ADMBP( va( "^3!rotation: ^7%s\n", mapRotations.rotations[ i ].name ) );

      for( j = 0; j < mapRotations.rotations[ i ].numMaps; j++ )
      {
        if( j - 1 == G_GetCurrentMap( i ) && strcmp( g_nextMap.string, "0" ) )
          ADMBP( va( "  ^%iN^7  %-20s ^%i%s\n", 1, g_nextMap.string, 1, "next map vote" ) );
        if ( G_GetCurrentMap( i ) == j )
        {
          if( mapRotations.rotations[ i ].maps[ j ].name != g_currentMap.string )
            status = "map vote";
          else
            status = "current slot";
          statusColor = 3;
        }
        else if ( !G_MapExists( mapRotations.rotations[ i ].maps[ j ].name ) )
        {
          statusColor = 1;
          status = "missing";
        }
        else
        {
          statusColor = 7;
          status = "";
        }
        ADMBP( va( "^%i%3i %-20s ^%i%s\n", statusColor, j + 1, mapRotations.rotations[ i ].maps[ j ].name, statusColor, status ) );
      }

      ADMBP_end();

      // No maps were found in the active map rotation
      if ( mapRotations.rotations[ i ].numMaps < 1 )
      {
        trap_SendServerCommand( ent-g_entities, "print \"  - ^7Empty!\n\"" );
        return qfalse;
      }
    }
  }
  return qtrue;
}


qboolean G_admin_subnetban( gentity_t *ent, int skiparg )
{
  int bnum;
  int mask;
  int IPRlow = 0, IPRhigh = 0;
  char cIPRlow[ 32 ], cIPRhigh[ 32 ];
  char bs[ 5 ];
  char strmask[ 5 ];
  char exl[2];
  int k, IP[5];
  
  if( G_SayArgc() < 3 + skiparg )
  {
    ADMP( "^3!subnetban: ^7usage: !subnetban [ban#] [mask]\n" );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, bs, sizeof( bs ) );
  bnum = atoi( bs );
  if( bnum < 1 || bnum > MAX_ADMIN_BANS || !g_admin_bans[ bnum - 1] )
  {
    ADMP( "^3!subnetban: ^7invalid ban#\n" );
    return qfalse;
  }

  G_SayArgv( 2 + skiparg, strmask, sizeof( strmask ) );
  mask = atoi( strmask );
  
  if( mask >= 0 && mask <= 32)
  {
    G_SayArgv( 3 + skiparg, exl, sizeof( exl ) );
    if( mask >= 0 && mask < 16 && strcmp(exl, "!") )
    {
      if( ent )
      {
        ADMP( "^3!subnetban: ^7Only console may ban such a large network. Regular admins may only ban >=16.\n" );
        return qfalse;
      }

      ADMP( "^3!subnetban: ^1WARNING:^7 you are about to ban a large network, use !subnetban [ban] [mask] ! to force^7\n" );
      return qfalse;
    }
    else
    {
      sscanf(g_admin_bans[ bnum - 1 ]->ip, "%d.%d.%d.%d/%d", &IP[4], &IP[3], &IP[2], &IP[1], &IP[0]);
      for(k = 4; k >= 1; k--)
      {
        if(!IP[k]) IP[k] = 0;
        IPRlow |= IP[k] << 8*(k-1);
      }
      IPRhigh = IPRlow;
      if( mask == 32 )
      {
        Q_strncpyz( 
          g_admin_bans[ bnum - 1 ]->ip, 
          va("%i.%i.%i.%i", IP[4], IP[3], IP[2], IP[1]), 
          sizeof( g_admin_bans[ bnum - 1 ]->ip ) 
        );
      }
      else
      {
        Q_strncpyz( 
          g_admin_bans[ bnum - 1 ]->ip, 
          va("%i.%i.%i.%i/%i", IP[4], IP[3], IP[2], IP[1], mask ), 
          sizeof( g_admin_bans[ bnum - 1 ]->ip )
        );
        IPRlow &= ~((1 << (32-mask)) - 1);
        IPRhigh |= ((1 << (32-mask)) - 1);
      }
    }
  }
  else
  {
    ADMP( "^3!subnetban: ^7mask is out of range, please use 0-32 inclusive\n" );
    return qfalse;
  }
  if( mask > 0 )
  {
    Q_strncpyz( 
      cIPRlow, 
      va("%i.%i.%i.%i", (IPRlow & (255 << 24)) >> 24, (IPRlow & (255 << 16)) >> 16, (IPRlow & (255 << 8)) >> 8, IPRlow & 255), 
      sizeof( cIPRlow ) 
    );
    Q_strncpyz( 
      cIPRhigh, 
      va("%i.%i.%i.%i", (IPRhigh & (255 << 24)) >> 24, (IPRhigh & (255 << 16)) >> 16, (IPRhigh & (255 << 8)) >> 8, IPRhigh & 255), 
      sizeof( cIPRhigh ) 
    );
  }
  else
  {
    Q_strncpyz( cIPRlow, "0.0.0.0", sizeof( cIPRlow ) );
    Q_strncpyz( cIPRhigh, "255.255.255.255", sizeof( cIPRhigh ) );
    
  }
  
  AP( va( "print \"^3!subnetban: ^7ban #%d for %s^7 has been updated by %s^7 "
    "%s (%s - %s)\n\"",
    bnum,
    g_admin_bans[ bnum - 1 ]->name,
    ( ent ) ? ent->client->pers.netname : "console",
    g_admin_bans[ bnum - 1 ]->ip,
    cIPRlow,
    cIPRhigh) );
  if( ent )
    Q_strncpyz( g_admin_bans[ bnum - 1 ]->banner, ent->client->pers.netname,
      sizeof( g_admin_bans[ bnum - 1 ]->banner ) );
  if( g_admin.string[ 0 ] )
    admin_writeconfig();
  return qtrue;
}


qboolean G_admin_override( gentity_t *ent, int skiparg )
{ 
  //this is all very similar to denybuild, 
  //it performs an essentially identical function
  int pids[ MAX_CLIENTS ], found;
  char name[ MAX_NAME_LENGTH ], err[ MAX_STRING_CHARS ];
  char command[ MAX_ADMIN_CMD_LEN ], *cmd;
  gentity_t *vic;

  G_SayArgv( skiparg, command, sizeof( command ) );
  cmd = command;
  if( cmd && *cmd == '!' )
    cmd++;
  if( G_SayArgc() < 2 + skiparg )
  {
    ADMP( va( "^3!%s: ^7usage: !%s [name|slot#]\n", cmd, cmd ) );
    return qfalse;
  }
  G_SayArgv( 1 + skiparg, name, sizeof( name ) );
  if( ( found = G_ClientNumbersFromString( name, pids, MAX_CLIENTS ) ) != 1 )
  {
    G_MatchOnePlayer( pids, found, err, sizeof( err ) );
    ADMP( va( "^3!%s: ^7%s\n", cmd, err ) );
    return qfalse;
  }
  if( !admin_higher( ent, &g_entities[ pids[ 0 ] ] ) )
  {
    ADMP( va( "^3!%s: ^7sorry, but your intended victim has a higher admin"
              " level than you\n", cmd ) );
    return qfalse;
  }
  vic = &g_entities[ pids[ 0 ] ];
  //FIXME A: It's backwards, sorry
  if( !vic->client->pers.override )
  {
    if( !Q_stricmp( cmd, "denyoverride" ) )
    {
      ADMP( "^3!denyoverride: ^7player cannot currently override\n" );
      return qtrue;
    }
    vic->client->pers.override = qtrue;
    CPx( pids[ 0 ], "cp \"^1You've been allowed to override\"" );
    AP( va(
      "print \"^3!allowoverride: ^7^7%s^7 has been allowed to override by %s\n\"",
      vic->client->pers.netname,
      ( ent ) ? ent->client->pers.netname : "console" ) );
  }
  else
  {
    if( !Q_stricmp( cmd, "allowoverride" ) )
    {
      ADMP( "^3!allowoverride: ^7player can already override\n" );
      return qtrue;
    }
    vic->client->pers.denyBuild = qfalse;
    vic->client->ps.stats[ STAT_BUILDABLE ] = BA_NONE;
    CPx( pids[ 0 ], "cp \"^1You've lost your ability to override\"" );
    AP( va(
      "print \"^3!denyoverride: ^7^7%s^7 has been denied override by ^7%s\n\"",
      vic->client->pers.netname,
      ( ent ) ? ent->client->pers.netname : "console" ) );
  }
  return qtrue;
}

/*
================
 G_admin_print

 This function facilitates the ADMP define.  ADMP() is similar to CP except
 that it prints the message to the server console if ent is not defined.
================
*/
void G_admin_print( gentity_t *ent, char *m )
{
  if( ent )
    trap_SendServerCommand( ent - level.gentities, va( "print \"%s\"", m ) );
  else
  {
    char m2[ MAX_STRING_CHARS ];
    if( !trap_Cvar_VariableIntegerValue( "com_ansiColor" ) )
    {
      G_DecolorString( m, m2, sizeof( m2 ) );
      trap_Print( m2 );
    }
    else
      trap_Print( m );
  }
}

void G_admin_buffer_begin()
{
  g_bfb[ 0 ] = '\0';
}

void G_admin_buffer_end( gentity_t *ent )
{
  ADMP( g_bfb );
}

void G_admin_buffer_print( gentity_t *ent, char *m )
{
  // 1022 - strlen("print 64 \"\"") - 1
  if( strlen( m ) + strlen( g_bfb ) >= 1009 )
  {
    ADMP( g_bfb );
    g_bfb[ 0 ] = '\0';
  }
  Q_strcat( g_bfb, sizeof( g_bfb ), m );
}


void G_admin_cleanup()
{
  int i = 0;

  for( i = 0; i < MAX_ADMIN_LEVELS && g_admin_levels[ i ]; i++ )
  {
    BG_Free( g_admin_levels[ i ] );
    g_admin_levels[ i ] = NULL;
  }
  for( i = 0; i < MAX_ADMIN_ADMINS && g_admin_admins[ i ]; i++ )
  {
    BG_Free( g_admin_admins[ i ] );
    g_admin_admins[ i ] = NULL;
  }
  for( i = 0; i < MAX_ADMIN_BANS && g_admin_bans[ i ]; i++ )
  {
    BG_Free( g_admin_bans[ i ] );
    g_admin_bans[ i ] = NULL;
  }
  for( i = 0; i < MAX_ADMIN_COMMANDS && g_admin_commands[ i ]; i++ )
  {
    BG_Free( g_admin_commands[ i ] );
    g_admin_commands[ i ] = NULL;
  }
}
