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

/*
==================
G_SanitiseString

Remove case and control characters from a string
==================
*/
void G_SanitiseString( char *in, char *out, int len )
{
  qboolean skip = qtrue;
  int spaces = 0;

  len--;

  while( *in && len > 0 )
  {
    // strip leading white space
    if( *in == ' ' )
    {
      if( skip )
      {
        in++;
        continue;
      }
      spaces++;
    }
    else
    {
      spaces = 0;
      skip = qfalse;
    }

    if( Q_IsColorString( in ) )
    {
      in += 2;    // skip color code
      continue;
    }

    if( *in < 32 )
    {
      in++;
      continue;
    }

    *out++ = tolower( *in++ );
    len--;
  }
  out -= spaces;
  *out = 0;
}

/*
==================
G_ClientNumberFromString

Returns a player number for either a number or name string
Returns -1 if invalid
==================
*/
int G_ClientNumberFromString( char *s )
{
  gclient_t *cl;
  int       i;
  char      s2[ MAX_NAME_LENGTH ];
  char      n2[ MAX_NAME_LENGTH ];

  // numeric values are just slot numbers
  for( i = 0; s[ i ] && isdigit( s[ i ] ); i++ );
  if( !s[ i ] )
  {
    i = atoi( s );

    if( i < 0 || i >= level.maxclients )
      return -1;

    cl = &level.clients[ i ];

    if( cl->pers.connected == CON_DISCONNECTED )
      return -1;

    return i;
  }

  // check for a name match
  G_SanitiseString( s, s2, sizeof( s2 ) );

  for( i = 0, cl = level.clients; i < level.maxclients; i++, cl++ )
  {
    if( cl->pers.connected == CON_DISCONNECTED )
      continue;

    G_SanitiseString( cl->pers.netname, n2, sizeof( n2 ) );

    if( !strcmp( n2, s2 ) )
      return i;
  }

  return -1;
}


/*
==================
G_MatchOnePlayer

This is a companion function to G_ClientNumbersFromString()

err will be populated with an error message.
==================
*/
void G_MatchOnePlayer( int *plist, int num, char *err, int len )
{
  gclient_t *cl;
  int i;
  char line[ MAX_NAME_LENGTH + 10 ] = {""};

  err[ 0 ] = '\0';
  if( num == 0 )
  {
    Q_strcat( err, len, "no connected player by that name or slot #" );
  }
  else if( num > 1 )
  {
    Q_strcat( err, len, "more than one player name matches. "
            "be more specific or use the slot #:\n" );
    for( i = 0; i < num; i++ )
    {
      cl = &level.clients[ plist[ i ] ];
      if( cl->pers.connected == CON_DISCONNECTED )
        continue;
      Com_sprintf( line, sizeof( line ), "%2i - %s^7\n",
        plist[ i ], cl->pers.netname );
      if( strlen( err ) + strlen( line ) > len )
        break;
      Q_strcat( err, len, line );
    }
  }
}

/*
==================
G_ClientNumbersFromString

Sets plist to an array of integers that represent client numbers that have
names that are a partial match for s.

Returns number of matching clientids up to max.
==================
*/
int G_ClientNumbersFromString( char *s, int *plist, int max )
{
  gclient_t *p;
  int i, found = 0;
  char n2[ MAX_NAME_LENGTH ] = {""};
  char s2[ MAX_NAME_LENGTH ] = {""};

  if( max == 0 )
    return 0;

  // if a number is provided, it is a clientnum
  for( i = 0; s[ i ] && isdigit( s[ i ] ); i++ );
  if( !s[ i ] )
  {
    i = atoi( s );
    if( i >= 0 && i < level.maxclients )
    {
      p = &level.clients[ i ];
      if( p->pers.connected != CON_DISCONNECTED )
      {
        *plist = i;
        return 1;
      }
    }
    // we must assume that if only a number is provided, it is a clientNum
    return 0;
  }

  // now look for name matches
  G_SanitiseString( s, s2, sizeof( s2 ) );
  if( strlen( s2 ) < 1 )
    return 0;
  for( i = 0; i < level.maxclients && found < max; i++ )
  {
    p = &level.clients[ i ];
    if( p->pers.connected == CON_DISCONNECTED )
    {
      continue;
    }
    G_SanitiseString( p->pers.netname, n2, sizeof( n2 ) );
    if( strstr( n2, s2 ) )
    {
      *plist++ = i;
      found++;
    }
  }
  return found;
}

/*
==================
ScoreboardMessage

==================
*/
void ScoreboardMessage( gentity_t *ent )
{
  char      entry[ 1024 ];
  char      string[ 1400 ];
  int       stringlength;
  int       i, j;
  gclient_t *cl;
  int       numSorted;
  weapon_t  weapon = WP_NONE;
  upgrade_t upgrade = UP_NONE;

  // send the latest information on all clients
  string[ 0 ] = 0;
  stringlength = 0;

  numSorted = level.numConnectedClients;

  for( i = 0; i < numSorted; i++ )
  {
    int   ping;

    cl = &level.clients[ level.sortedClients[ i ] ];

    if( cl->pers.connected == CON_CONNECTING )
      ping = -1;
    else
      ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

    if( cl->sess.spectatorState == SPECTATOR_NOT &&
        ( ent->client->pers.teamSelection == TEAM_NONE ||
          cl->pers.teamSelection == ent->client->pers.teamSelection ) )
    {
      weapon = cl->ps.weapon;

      if( BG_InventoryContainsUpgrade( UP_BATTLESUIT, cl->ps.stats ) )
        upgrade = UP_BATTLESUIT;
      else if( BG_InventoryContainsUpgrade( UP_JETPACK, cl->ps.stats ) )
        upgrade = UP_JETPACK;
      else if( BG_InventoryContainsUpgrade( UP_CLOAK, cl->ps.stats ) )
        upgrade = UP_CLOAK;
      else if( BG_InventoryContainsUpgrade( UP_HELMET, cl->ps.stats ) )
        upgrade = UP_HELMET;
      else if( BG_InventoryContainsUpgrade( UP_LIGHTARMOUR, cl->ps.stats ) )
        upgrade = UP_LIGHTARMOUR;
      else
        upgrade = UP_NONE;
    }
    else
    {
      weapon = WP_NONE;
      upgrade = UP_NONE;
    }

    Com_sprintf( entry, sizeof( entry ),
      " %d %d %d %d %d %d", level.sortedClients[ i ], cl->ps.persistant[ PERS_KILLS ],
      ping, cl->ps.persistant[ PERS_KILLED ], weapon, upgrade );

    j = strlen( entry );

    if( stringlength + j > 1024 )
      break;

    strcpy( string + stringlength, entry );
    stringlength += j;
  }

  trap_SendServerCommand( ent-g_entities, va( "scores %i %i %i%s", i,
    level.alienKills, level.humanKills, string ) );
}


/*
==================
ConcatArgs
==================
*/
char *ConcatArgs( int start )
{
  int         i, c, tlen;
  static char line[ MAX_STRING_CHARS ];
  int         len;
  char        arg[ MAX_STRING_CHARS ];

  len = 0;
  c = trap_Argc( );

  for( i = start; i < c; i++ )
  {
    trap_Argv( i, arg, sizeof( arg ) );
    tlen = strlen( arg );

    if( len + tlen >= MAX_STRING_CHARS - 1 )
      break;

    memcpy( line + len, arg, tlen );
    len += tlen;

    if( len == MAX_STRING_CHARS - 1 )
      break;

    if( i != c - 1 )
    {
      line[ len ] = ' ';
      len++;
    }
  }

  line[ len ] = 0;

  return line;
}

/*
==================
Cmd_Give_f

Give items to a client
==================
*/
void Cmd_Give_f( gentity_t *ent )
{
  char      *name;
  qboolean  give_all = qfalse;

  name = ConcatArgs( 1 );
  if( Q_stricmp( name, "all" ) == 0 )
    give_all = qtrue;

  if( give_all || Q_stricmp( name, "health" ) == 0 )
  {
    if(!g_devmapNoGod.integer)
    {
     ent->health = ent->client->ps.stats[ STAT_MAX_HEALTH ];
     BG_AddUpgradeToInventory( UP_MEDKIT, ent->client->ps.stats );
    }
  }

  if( give_all || Q_stricmpn( name, "funds", 5 ) == 0 )
  {
    int credits = atoi( name + 6 );

    if( ent->client->pers.teamSelection == TEAM_ALIENS )
      credits *= ALIEN_CREDITS_PER_FRAG;

    if( give_all )
      credits = ALIEN_MAX_CREDITS;

    G_AddCreditToClient( ent->client, credits, qtrue );
  }

  if( give_all || Q_stricmp( name, "stamina" ) == 0 )
    ent->client->ps.stats[ STAT_STAMINA ] = MAX_STAMINA;

  if( Q_stricmp( name, "poison" ) == 0 )
  {
    if( ent->client->pers.teamSelection == TEAM_HUMANS )
    {
      ent->client->ps.stats[ STAT_STATE ] |= SS_POISONED;
      ent->client->lastPoisonTime = level.time;
      ent->client->lastPoisonClient = ent;
    }
    else
    {
      ent->client->ps.stats[ STAT_STATE ] |= SS_BOOSTED;
      ent->client->boostedTime = level.time;
    }
  }

  if( give_all || Q_stricmp( name, "ammo" ) == 0 )
  {
    gclient_t *client = ent->client;

    if( client->ps.weapon != WP_ALEVEL3_UPG &&
        BG_Weapon( client->ps.weapon )->infiniteAmmo )
      return;

    client->ps.ammo = BG_Weapon( client->ps.weapon )->maxAmmo;
    client->ps.clips = BG_Weapon( client->ps.weapon )->maxClips;

    if( BG_Weapon( client->ps.weapon )->usesEnergy && (
        BG_InventoryContainsUpgrade( UP_BATTPACK, client->ps.stats ) ||
        BG_InventoryContainsUpgrade( UP_BATTLESUIT, client->ps.stats ) ) )
      client->ps.ammo = (int)( (float)client->ps.ammo * BATTPACK_MODIFIER );
    else if( !BG_Weapon( client->ps.weapon )->usesEnergy && (
        BG_InventoryContainsUpgrade( UP_AMMOPACK, client->ps.stats ) ||
        BG_InventoryContainsUpgrade( UP_BATTLESUIT, client->ps.stats ) ) )
      client->ps.clips = (int)( 1 + (float)client->ps.clips * AMMOPACK_MODIFIER );
  }
}


/*
==================
Cmd_God_f

Sets client to godmode

argv(0) god
==================
*/
void Cmd_God_f( gentity_t *ent )
{
  char  *msg;

  if( !g_devmapNoGod.integer )
  {
    ent->flags ^= FL_GODMODE;

    if( !( ent->flags & FL_GODMODE ) )
      msg = "godmode OFF\n";
    else
      msg = "godmode ON\n";
  }
  else
  {
    msg = "Godmode has been disabled.\n";
  }

  trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}


/*
==================
Cmd_Notarget_f

Sets client to notarget

argv(0) notarget
==================
*/
void Cmd_Notarget_f( gentity_t *ent )
{
  char  *msg;

  if( !g_devmapNoGod.integer )
  {
    ent->flags ^= FL_NOTARGET;

    if( !( ent->flags & FL_NOTARGET ) )
      msg = "notarget OFF\n";
    else
      msg = "notarget ON\n";
  }
  else
  {
    msg = "Notarget has been disabled.\n";
  }

  trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}


/*
==================
Cmd_Noclip_f

argv(0) noclip
==================
*/
void Cmd_Noclip_f( gentity_t *ent )
{
  char  *msg;

  if( !g_devmapNoGod.integer )
  {
    if( ent->client->noclip )
      msg = "noclip OFF\n";
    else
      msg = "noclip ON\n";

    ent->client->noclip = !ent->client->noclip;
  } 
  else
  {
    msg = "Noclip has been disabled.\n";
  }

  trap_SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
}


/*
==================
Cmd_LevelShot_f

This is just to help generate the level pictures
for the menus.  It goes to the intermission immediately
and sends over a command to the client to resize the view,
hide the scoreboard, and take a special screenshot
==================
*/
void Cmd_LevelShot_f( gentity_t *ent )
{
  BeginIntermission( );
  trap_SendServerCommand( ent - g_entities, "clientLevelShot" );
}

/*
=================
Cmd_Kill_f
=================
*/
void Cmd_Kill_f( gentity_t *ent )
{
  if( ent->client->ps.stats[ STAT_STATE ] & SS_HOVELING )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Leave the Hovel first (use your destroy key)\n\"" );
    return;
  }

  if( g_cheats.integer || ent->client->pers.override )
  {
    ent->flags &= ~FL_GODMODE;
    ent->client->ps.stats[ STAT_HEALTH ] = ent->health = 0;
    player_die( ent, ent, ent, 100000, MOD_SUICIDE );
  }
  else
  {
    if( ent->suicideTime == 0 )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You will suicide in 20 seconds\n\"" );
      ent->suicideTime = level.time + 20000;
    }
    else if( ent->suicideTime > level.time )
    {
      trap_SendServerCommand( ent-g_entities, "print \"Suicide canceled\n\"" );
      ent->suicideTime = 0;
    }
  }
}

/*
=================
Cmd_Team_f
=================
*/
void Cmd_Team_f( gentity_t *ent )
{
  team_t    team;
  team_t    oldteam = ent->client->pers.teamSelection;
  char      s[ MAX_TOKEN_CHARS ];
  qboolean  force = G_admin_permission(ent, ADMF_FORCETEAMCHANGE);
  int       aliens = level.numAlienClients;
  int       humans = level.numHumanClients;

  // stop team join spam
  if( level.time - ent->client->pers.teamChangeTime < 1000 )
    return;

  if( oldteam == TEAM_ALIENS )
    aliens--;
  else if( oldteam == TEAM_HUMANS )
    humans--;

  // do warm up
  if( g_doWarmup.integer && g_warmupMode.integer == 1 &&
      level.time - level.startTime < g_warmup.integer * 1000 )
  {
    trap_SendServerCommand( ent - g_entities, va( "print \"team: you can't join"
      " a team during warm up (%d seconds remaining)\n\"",
      g_warmup.integer - ( level.time - level.startTime ) / 1000 ) );
    return;
  }

  if( oldteam == TEAM_NONE && G_admin_level(ent)<g_minLevelToJoinTeam.integer )
  {
      trap_SendServerCommand( ent-g_entities,"print \"Sorry, but your admin level is only permitted to spectate.\n\"" );
      return;
  }
  trap_Argv( 1, s, sizeof( s ) );

  if( !s[ 0 ] )
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"team: %i\n\"",
      oldteam ) );
    return;
  }

  if( !Q_stricmp( s, "auto" ) )
  {
    if( ent->client->pers.specd )
    {
      team = TEAM_NONE;
      trap_SendServerCommand( ent-g_entities, va( "print \"You cannot join teams, you are forced to the spectators\n\"" ) );
      return;
    }
    
   	if( ( level.time - ent->client->lastspecmeTime ) < ( g_specmetimeout.value*60000 ) && level.time > ( g_specmetimeout.value*60000 ) )
    {
      trap_SendServerCommand( ent-g_entities,
       va( "print \"You just used !specme a little while ago. Please stay on spectators for %i more second(s).\n\"", 
        ( (int) ( ( g_specmetimeout.value*60000 ) - ( level.time - ent->client->lastspecmeTime ) )/1000 )  ) );
      return;
    } 

    else if( level.humanTeamLocked && level.alienTeamLocked )
      team = TEAM_NONE;
    else if( level.humanTeamLocked || humans > aliens )
      team = TEAM_ALIENS;
    else if( level.alienTeamLocked || aliens > humans )
      team = TEAM_HUMANS;
    else
      team = TEAM_ALIENS + ( rand( ) % 2 );
  }
  else switch( G_TeamFromString( s ) )
  {
    case TEAM_NONE:
      team = TEAM_NONE;
      break;

    case TEAM_ALIENS:
      if( ent->client->pers.specd )
      {
        trap_SendServerCommand( ent-g_entities, va( "print \"You cannot join teams, you are forced to the spectators\n\"" ) );
        return;
      }
      if( g_forceAutoSelect.integer && !G_admin_permission(ent, ADMF_FORCETEAMCHANGE) )
      {
        trap_SendServerCommand( ent-g_entities, "print \"You can only join teams using autoselect\n\"" );
        return;
      }
      if( level.humanTeamLocked && g_extremeSuddenDeath.integer )
      {
        trap_SendServerCommand( ent-g_entities,
          va( "print \"The game is currently in Extreme Sudden Death\n\"" ) );
        return; 
      }
      if( level.alienTeamLocked )
      {
        trap_SendServerCommand( ent-g_entities,
          "print \"Alien team has been ^1LOCKED\n\"" );
        return;
      }
      if( ( level.time - ent->client->lastspecmeTime ) < ( g_specmetimeout.value*60000 ) && level.time > ( g_specmetimeout.value*60000 ) )
      {
        trap_SendServerCommand( ent-g_entities,
        va( "print \"You just used !specme a little while ago. Please stay on spectators for %i more second(s).\n\"", 
          ( (int) ( (g_specmetimeout.value*60000) - ( level.time - ent->client->lastspecmeTime ) )/1000 )  ) );
        return;
      } 
      if( level.humanTeamLocked )
        force = qtrue;

      if( !force && g_teamForceBalance.integer && aliens > humans )
      {
        G_TriggerMenu( ent - g_entities, MN_A_TEAMFULL );
        return;
      }

      team = TEAM_ALIENS;
      break;

    case TEAM_HUMANS:
      if( ent->client->pers.specd )
      {
        trap_SendServerCommand( ent-g_entities, va( "print \"You cannot join teams, you are forced to the spectators\n\"" ) );
        return;
      }
      if( g_forceAutoSelect.integer && !G_admin_permission(ent, ADMF_FORCETEAMCHANGE) )
      {
        trap_SendServerCommand( ent-g_entities, "print \"You can only join teams using autoselect\n\"" );
        return;
      }
      if( level.humanTeamLocked && g_extremeSuddenDeath.integer )
      {
        trap_SendServerCommand( ent-g_entities,
          va( "print \"The game is currently in Extreme Sudden Death\n\"" ) );
        return; 
      }
      if( level.humanTeamLocked )
      {
        trap_SendServerCommand( ent-g_entities,
          "print \"Human team has been ^1LOCKED\n\"" );
        return;
      }
      if( ( level.time - ent->client->lastspecmeTime ) < ( g_specmetimeout.value*60000 ) && level.time > ( g_specmetimeout.value*60000 ) )
      {
        trap_SendServerCommand( ent-g_entities,
        va( "print \"You just used !specme a little while ago. Please stay on spectators for %i more second(s).\n\"", 
          ( (int) ( (g_specmetimeout.value*60000) - ( level.time - ent->client->lastspecmeTime ) )/1000 )  ) );
        return;
      } 
      if( level.alienTeamLocked )
        force = qtrue;

      if( !force && g_teamForceBalance.integer && humans > aliens )
      {
        G_TriggerMenu( ent - g_entities, MN_H_TEAMFULL );
        return;
      }

      team = TEAM_HUMANS;
      break;

    default:
      trap_SendServerCommand( ent-g_entities,
      va( "print \"Unknown team: %s\n\"", s ) );
      return;
  }

  // stop team join spam
  if( oldteam == team )
    return;

  if( team != TEAM_NONE && g_maxGameClients.integer &&
    level.numPlayingClients >= g_maxGameClients.integer )
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"The maximum number of "
      "playing clients has been reached (g_maxGameClients = %d)\n\"",
      g_maxGameClients.integer ) );
    return;
  }

  // guard against build timer exploit
  if( oldteam != TEAM_NONE && ent->client->sess.spectatorState == SPECTATOR_NOT &&
     ( ent->client->ps.stats[ STAT_CLASS ] == PCL_ALIEN_BUILDER0 ||
       ent->client->ps.stats[ STAT_CLASS ] == PCL_ALIEN_BUILDER0_UPG ||
       BG_InventoryContainsWeapon( WP_HBUILD, ent->client->ps.stats ) ) &&
      ent->client->ps.stats[ STAT_MISC ] > 0 )
  {
    if( ent->client->pers.teamSelection == TEAM_ALIENS )
      G_TriggerMenu( ent->client->ps.clientNum, MN_A_TEAMCHANGEBUILDTIMER );
    else
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_TEAMCHANGEBUILDTIMER );
    return;
  }

  // Apply the change
  G_ChangeTeam( ent, team );
}


/*
==================
G_Say
==================
*/
static void G_SayTo( gentity_t *ent, gentity_t *other, int mode, int color, const char *name, const char *message, const char *prefix )
{
  qboolean ignore = qfalse;
  qboolean specAllChat = qfalse;

  if( !other )
    return;

  if( !other->inuse )
    return;

  if( !other->client )
    return;

  if( other->client->pers.connected != CON_CONNECTED )
    return;

  if( ( mode == SAY_TEAM || mode == SAY_ACTION_T ) && !OnSameTeam( ent, other ) )
  {
    if( other->client->pers.teamSelection != TEAM_NONE )
      return;

    specAllChat = G_admin_permission( other, ADMF_SPEC_ALLCHAT );
    if( !specAllChat )
      return;

    // specs with ADMF_SPEC_ALLCHAT flag can see team chat
  }

  if( ent && BG_ClientListTest( &other->client->sess.ignoreList, ent-g_entities ) )
    ignore = qtrue;

  trap_SendServerCommand( other-g_entities, va( "%s \"%s%s%s%c%c%s\"",
    ( mode == SAY_TEAM || mode == SAY_ACTION_T ) ? "tchat" : "chat",
    ( ignore ) ? "[skipnotify]" : "",
    ( specAllChat ) ? prefix : "",
    name, Q_COLOR_ESCAPE, color, message ) );
}

#define EC    "\x19"

void G_Say( gentity_t *ent, gentity_t *target, int mode, const char *chatText )
{
  int         j;
  gentity_t   *other;
  int         color;
  const char  *prefix;
  char        name[ 64 ];
  // don't let text be too long for malicious reasons
  char        text[ MAX_SAY_TEXT ];
  char        location[ 64 ];

  // Bail if the text is blank.
  if( ! chatText[0] )
     return;

  // Flood limit.  If they're talking too fast, determine that and return.
  if( g_floodMinTime.integer )
    if ( G_FloodLimited( ent ) )
    {
      return;
    }

  if( ent && ent->client )
  {
    Com_sprintf( name, sizeof( name ), "test message" );
    switch( ent->client->pers.teamSelection)
    {
      default:
      case TEAM_NONE:
        prefix = "[^3S^7] ";
        break;

      case TEAM_ALIENS:
        prefix = "[^1A^7] ";
        break;

      case TEAM_HUMANS:
        prefix = "[^4H^7] ";
    }
  }
  else
  {
    prefix = "";
  }

  // check if blocked by g_specChat 0
  if( ( !g_specChat.integer ) && ( mode != SAY_TEAM ) &&
      ( ent ) && ( ent->client->pers.teamSelection == TEAM_NONE ) && 
      ( !G_admin_permission( ent, ADMF_NOCENSORFLOOD ) ) ) 
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"Global chatting for "
      "spectators has been disabled. You may only use team chat.\n\"") );
    return;
  }

  switch( mode )
  {
    default:
    case SAY_ALL:
      G_LogPrintf( "say: %s%s^7: " S_COLOR_GREEN "%s\n", prefix,
        ( ent ) ? ent->client->pers.netname : "console", chatText );
      Com_sprintf( name, sizeof( name ), "%s%s" S_COLOR_WHITE ": ", prefix, ent->client->pers.netname );
      color = COLOR_GREEN;
      G_DemoCommand( DC_SERVER_COMMAND, va( "chat \"%s^2%s\"", name, chatText ) );
      break;

    case SAY_TEAM:
      G_LogPrintf( "sayteam: %s%s^7: " S_COLOR_CYAN "%s\n", prefix,
        ent->client->pers.netname, chatText );
      if( Team_GetLocationMsg( ent, location, sizeof( location ) ) )
        Com_sprintf( name, sizeof( name ), "(%s" S_COLOR_WHITE ") (%s): ",
          ( ent ) ? ent->client->pers.netname : "console", location );
      else
        Com_sprintf( name, sizeof( name ), "(%s" S_COLOR_WHITE "): ",
          ( ent ) ? ent->client->pers.netname : "console" );
      color = COLOR_CYAN;
      G_DemoCommand( DC_SERVER_COMMAND, va( "tchat \"%s^5%s\"", name, chatText ) );
      break;

    case SAY_TELL:
      if( target && OnSameTeam( target, ent ) &&
          Team_GetLocationMsg( ent, location, sizeof( location ) ) )
        Com_sprintf( name, sizeof( name ), "[%s" S_COLOR_WHITE "] (%s): ",
          ( ent ) ? ent->client->pers.netname : "console", location );
      else
        Com_sprintf( name, sizeof( name ), "[%s" S_COLOR_WHITE "]: ",
          ( ent ) ? ent->client->pers.netname : "console" );
      color = COLOR_MAGENTA;
      break;

    case SAY_ACTION:
      G_LogPrintf( "action: %s^7: %s^7\n", ent->client->pers.netname, chatText );
      Com_sprintf( name, sizeof( name ), "^2%s^7%s%s%c%c ", g_actionPrefix.string, prefix,
                   ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
      color = COLOR_WHITE;
      break;

    case SAY_ACTION_T:
      G_LogPrintf( "actionteam: %s%s^7: %s^7\n", prefix, ent->client->pers.netname, chatText );
      if( Team_GetLocationMsg( ent, location, sizeof( location ) ) )
        Com_sprintf( name, sizeof( name ), EC"^5%s^7%s%c%c (%s) ", g_actionPrefix.string, 
          ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE, location );
      else
        Com_sprintf( name, sizeof( name ), EC"^5%s^7%s%c%c ", g_actionPrefix.string, 
          ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );
      color = COLOR_WHITE;
      break;

  }

  if( mode != SAY_TEAM && ent && ent->client && ent->client->pers.teamSelection == TEAM_NONE &&
      G_admin_level(ent)<g_minLevelToSpecMM1.integer )
  {
    trap_SendServerCommand( ent-g_entities,va( "print \"Sorry, but your admin level may only use teamchat while spectating.\n\"") ); 
    return;
  }

  Com_sprintf( text, sizeof( text ), "%s^7", chatText );

  if( target )
  {
    G_SayTo( ent, target, mode, color, name, text, prefix );
    return;
  }


  // Ugly hax: if adminsayfilter is off, do the SAY first to prevent text from going out of order
  if( !g_adminSayFilter.integer )
  {
    // send it to all the apropriate clients
    for( j = 0; j < level.maxclients; j++ )
    {
      other = &g_entities[ j ];
      G_SayTo( ent, other, mode, color, name, text, prefix );
    }
  }
   
   if( g_adminParseSay.integer && ( mode== SAY_ALL || mode == SAY_TEAM ) )
   {
     if( G_admin_cmd_check ( ent, qtrue ) && g_adminSayFilter.integer ) 
     {
       return;
     }
   }

  // if it's on, do it here, where it won't happen if it was an admin command
  if( g_adminSayFilter.integer )
  {
    // send it to all the apropriate clients
    for( j = 0; j < level.maxclients; j++ )
    {
      other = &g_entities[ j ];
      G_SayTo( ent, other, mode, color, name, text, prefix );
    }
  }
}


static void Cmd_SayArea_f( gentity_t *ent )
{
  int    entityList[ MAX_GENTITIES ];
  int    num, i;
  int    color = COLOR_BLUE;
  const char  *prefix;
  vec3_t range = { HELMET_RANGE, HELMET_RANGE, HELMET_RANGE };
  vec3_t mins, maxs;
  char   *msg = ConcatArgs( 1 );
  char   name[ 64 ];
  
   if( g_floodMinTime.integer )
   if ( G_FloodLimited( ent ) )
   {
    return;
   }
  
  if ( ent && ent->client )
  {
    switch( ent->client->pers.teamSelection)
    {
      default:
      case TEAM_NONE:
        prefix = "[S] ";
        break;

      case TEAM_ALIENS:
        prefix = "[A] ";
        break;

      case TEAM_HUMANS:
        prefix = "[H] ";
    }
  }
  else
  {
    prefix = "";
  }

  G_LogPrintf( "sayarea: %s%s^7: %s\n", prefix, ent->client->pers.netname, msg );
  Com_sprintf( name, sizeof( name ), EC"<%s%c%c"EC"> ",
    ent->client->pers.netname, Q_COLOR_ESCAPE, COLOR_WHITE );

  VectorAdd( ent->s.origin, range, maxs );
  VectorSubtract( ent->s.origin, range, mins );

  num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
  for( i = 0; i < num; i++ )
    G_SayTo( ent, &g_entities[ entityList[ i ] ], SAY_TEAM, color, name, msg, prefix );
  
  //Send to ADMF_SPEC_ALLCHAT candidates
  for( i = 0; i < level.maxclients; i++ )
  {
    if( (&g_entities[ i ])->client->pers.teamSelection == TEAM_NONE  &&
        G_admin_permission( &g_entities[ i ], ADMF_SPEC_ALLCHAT ) )
    {
      G_SayTo( ent, &g_entities[ i ], SAY_TEAM, color, name, msg, prefix );   
    }
  }
}

/*
==================
Cmd_Say_f
==================
*/
static void Cmd_Say_f( gentity_t *ent )
{
  char    *p;
  char    *args;
  int     mode = SAY_ALL;
  int     skipargs = 0;
  int     offset = 0;

  args = G_SayConcatArgs( 0 );
  if( Q_stricmpn( args, "say_team ", 9 ) == 0 )
    mode = SAY_TEAM;

  // support parsing /m out of say text since some people have a hard
  // time figuring out what the console is.
  if( !Q_stricmpn( args, "say /m ", 7 ) ||
      !Q_stricmpn( args, "say_team /m ", 12 ) ||
      !Q_stricmpn( args, "say /mt ", 8 ) ||
      !Q_stricmpn( args, "say_team /mt ", 13 ) )
  {
    Cmd_PrivateMessage_f( ent );
    return;
  }

  // support parsing /a out of say text for the same reason
   if( !Q_stricmpn( args, "say /a ", 7 ) ||
       !Q_stricmpn( args, "say_team /a ", 12 ) )
   {
      Cmd_AdminMessage_f( ent );
      return;
   }
   
  // support parsing /c out of say text for the same reason
   if( !Q_stricmpn( args, "say /c ", 7 ) ||
       !Q_stricmpn( args, "say_team /c ", 12 ) )
   {
      Cmd_ClanMessage_f( ent );
      return;
   }


  if(!Q_stricmpn( args, "say /me ", 8 ) )
  {
   if( g_actionPrefix.string[0] ) 
   { 
    mode = SAY_ACTION;
    offset = 4;
   } else return;
  }
  else if(!Q_stricmpn( args, "say_team /me ", 13 ) )
  {
   if( g_actionPrefix.string[0] ) 
   { 
    mode = SAY_ACTION_T;
    offset = 4;
   } else return;
  }
  else if( !Q_stricmpn( args, "me ", 3 ) )
  {
   if( g_actionPrefix.string[0] ) 
   { 
    mode = SAY_ACTION;
   } else return;
  }
  else if( !Q_stricmpn( args, "me_team ", 8 ) )
  {
   if( g_actionPrefix.string[0] ) 
   { 
    mode = SAY_ACTION_T;
   } else return;
  }


  if( g_allowShare.integer )
  {
    if( !Q_stricmpn( args, "say /share", 10 ) ||
      !Q_stricmpn( args, "say_team /share", 15 ) )
    {
      Cmd_Share_f( ent );
      return;
    }
   if( !Q_stricmpn( args, "say /donate", 11 ) ||
      !Q_stricmpn( args, "say_team /donate", 16 ) )
    {
      Cmd_Donate_f( ent );
      return;
    }
  }

  if( trap_Argc( ) < 2 )
    return;

  p = ConcatArgs( 1 + skipargs );
  p += offset;
  G_Say( ent, NULL, mode, p );
}

/*
==================
Cmd_Tell_f
==================
*/
static void Cmd_Tell_f( gentity_t *ent )
{
  int     targetNum;
  gentity_t *target;
  char    *p;
  char    arg[MAX_TOKEN_CHARS];

  if( trap_Argc( ) < 2 )
    return;

  trap_Argv( 1, arg, sizeof( arg ) );
  targetNum = atoi( arg );

  if( targetNum < 0 || targetNum >= level.maxclients )
    return;

  target = &g_entities[ targetNum ];
  if( !target || !target->inuse || !target->client )
    return;

  p = ConcatArgs( 2 );

  G_LogPrintf( "tell: %s^7 to %s^7: %s\n", ent->client->pers.netname, target->client->pers.netname, p );
  G_Say( ent, target, SAY_TELL, p );
  // don't tell to the player self if it was already directed to this player
  // also don't send the chat back to a bot
  if( ent != target )
    G_Say( ent, ent, SAY_TELL, p );
}

/*
==================
Cmd_VSay_f
==================
*/
void Cmd_VSay_f( gentity_t *ent )
{
  char            arg[MAX_TOKEN_CHARS];
  voiceChannel_t  vchan;
  voice_t         *voice;
  voiceCmd_t      *cmd;
  voiceTrack_t    *track;
  int             cmdNum = 0;
  int             trackNum = 0;
  char            voiceName[ MAX_VOICE_NAME_LEN ] = {"default"};
  char            voiceCmd[ MAX_VOICE_CMD_LEN ] = {""};
  char            vsay[ 12 ] = {""};
  weapon_t        weapon;

  if( !ent || !ent->client )
    Com_Error( ERR_FATAL, "Cmd_VSay_f() called by non-client entity\n" );

  trap_Argv( 0, arg, sizeof( arg ) );
  if( trap_Argc( ) < 2 )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"usage: %s command [text] \n\"", arg ) );
    return;
  }
  if( !level.voices )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: voice system is not installed on this server\n\"", arg ) );
    return;
  }
  if( !g_voiceChats.integer )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: voice system administratively disabled on this server\n\"",
      arg ) );
    return;
  }
  if( !Q_stricmp( arg, "vsay" ) )
    vchan = VOICE_CHAN_ALL;
  else if( !Q_stricmp( arg, "vsay_team" ) )
    vchan = VOICE_CHAN_TEAM;
  else if( !Q_stricmp( arg, "vsay_local" ) )
    vchan = VOICE_CHAN_LOCAL;
  else
    return;
  Q_strncpyz( vsay, arg, sizeof( vsay ) );

  if( ent->client->pers.voice[ 0 ] )
    Q_strncpyz( voiceName, ent->client->pers.voice, sizeof( voiceName ) );
  voice = BG_VoiceByName( level.voices, voiceName );
  if( !voice )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: voice '%s' not found\n\"", vsay, voiceName ) );
    return;
  }

  trap_Argv( 1, voiceCmd, sizeof( voiceCmd ) ) ;
  cmd = BG_VoiceCmdFind( voice->cmds, voiceCmd, &cmdNum );
  if( !cmd )
  {
    trap_SendServerCommand( ent-g_entities, va(
     "print \"%s: command '%s' not found in voice '%s'\n\"",
      vsay, voiceCmd, voiceName ) );
    return;
  }

  // filter non-spec humans by their primary weapon as well
  weapon = WP_NONE;
  if( ent->client->sess.spectatorState == SPECTATOR_NOT )
  {
    weapon = BG_PrimaryWeapon( ent->client->ps.stats );
  }

  track = BG_VoiceTrackFind( cmd->tracks, ent->client->pers.teamSelection,
    ent->client->pers.classSelection, weapon, (int)ent->client->voiceEnthusiasm,
    &trackNum );
  if( !track )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"%s: no available track for command '%s', team %d, "
      "class %d, weapon %d, and enthusiasm %d in voice '%s'\n\"",
      vsay, voiceCmd, ent->client->pers.teamSelection,
      ent->client->pers.classSelection, weapon,
      (int)ent->client->voiceEnthusiasm, voiceName ) );
    return;
  }

  if( !Q_stricmp( ent->client->lastVoiceCmd, cmd->cmd ) )
    ent->client->voiceEnthusiasm++;

  Q_strncpyz( ent->client->lastVoiceCmd, cmd->cmd,
    sizeof( ent->client->lastVoiceCmd ) );

  // optional user supplied text
  trap_Argv( 2, arg, sizeof( arg ) );

  switch( vchan )
  {
    case VOICE_CHAN_ALL:
    case VOICE_CHAN_LOCAL:
      trap_SendServerCommand( -1, va(
        "voice %d %d %d %d \"%s\"\n",
        (int)(ent-g_entities), vchan, cmdNum, trackNum, arg ) );
      break;
    case VOICE_CHAN_TEAM:
      G_TeamCommand( ent->client->pers.teamSelection, va(
        "voice %d %d %d %d \"%s\"\n",
        (int)(ent-g_entities), vchan, cmdNum, trackNum, arg ) );
      break;
    default:
      break;
  }
}

/*
==================
Cmd_Where_f
==================
*/
void Cmd_Where_f( gentity_t *ent )
{
  if( !ent->client )
    return;
  trap_SendServerCommand( ent - g_entities,
                          va( "print \"origin: %f %f %f\n\"",
                              ent->s.origin[ 0 ], ent->s.origin[ 1 ],
                              ent->s.origin[ 2 ] ) );
}

/*
==================
Cmd_CallVote_f
==================
*/
void Cmd_CallVote_f( gentity_t *ent )
{
  int   i;
  char  arg1[ MAX_STRING_TOKENS ];
  char  arg2[ MAX_STRING_TOKENS ];
  int   clientNum = -1;
  char  name[ MAX_NETNAME ];
  char *arg1plus;
  char *arg2plus;
  char *arg3plus;
  char nullstring[] = "";
  char  message[ MAX_STRING_CHARS ];
  char targetname[ MAX_NAME_LENGTH] = "";
  char reason[ MAX_STRING_CHARS ] = "";
  char *ptr = NULL;

  arg1plus = G_SayConcatArgs( 1 );
  arg2plus = G_SayConcatArgs( 2 );
  arg3plus = G_SayConcatArgs( 3 );

  if( !g_allowVote.integer )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here\n\"" );
    return;
  }
  
  // Flood limit.  If they're talking too fast, determine that and return.
  if( g_floodMinTime.integer )
    if ( G_FloodLimited( ent ) )
    {
      return;
    }

  if( level.teamVoteTime[ 0 ] || level.teamVoteTime[ 1 ] )
  {
    trap_SendServerCommand( ent-g_entities, "print \"A team vote is currently in progress\n\"" );
    return;
  }

  if( g_voteMinTime.integer
    && ent->client->pers.firstConnect 
    && level.time - ent->client->pers.enterTime < g_voteMinTime.integer * 1000
    && !G_admin_permission( ent, ADMF_NO_VOTE_LIMIT ) 
    && ( level.numPlayingClients > 0 && level.numConnectedClients > 1 ) )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"You must wait %d seconds after connecting before calling a vote\n\"",
      g_voteMinTime.integer ) );
    return;
  }

  if( level.voteTime )
  {
    trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress\n\"" );
    return;
  }

  if( g_voteLimit.integer > 0
    && ent->client->pers.voteCount >= g_voteLimit.integer
    && !G_admin_permission( ent, ADMF_NO_VOTE_LIMIT ) )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"You have already called the maximum number of votes (%d)\n\"",
      g_voteLimit.integer ) );
    return;
  }
  
  if( ent->client->pers.muted )
  {
    trap_SendServerCommand( ent - g_entities,
      "print \"You are muted and cannot call votes\n\"" );
    return;
  }

  if( g_doWarmup.integer && g_warmupMode.integer == 1 &&
      level.time - level.startTime < g_warmup.integer * 1000 )
  {
    trap_SendServerCommand( ent - g_entities,
      "print \"No votes are allowed during warmup\n\"" );
    return;
  }

  // make sure it is a valid command to vote on
  trap_Argv( 1, arg1, sizeof( arg1 ) );
  trap_Argv( 2, arg2, sizeof( arg2 ) );

  if( strchr( arg1, ';' ) || strchr( arg2, ';' ) )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string\n\"" );
    return;
  }

  // if there is still a vote to be executed
  if( level.voteExecuteTime )
  {
    if( !Q_stricmp( level.voteString, "map_restart" ) )
    {
      G_admin_maplog_result( "r" );
    }
    else if( !Q_stricmpn( level.voteString, "map", 3 ) )
    {
      G_admin_maplog_result( "m" );
    }

    level.voteExecuteTime = 0;
    trap_SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.voteString ) );
  }
  
  level.votePassThreshold=50;
  
  ptr = strstr(arg1plus, " -");
  if( ptr )
  {
    *ptr = '\0';
    ptr+=2; 

    if( *ptr == 'r' || *ptr=='R' )
    {
      while( *ptr != ' ' )
        ptr++;
      ptr++;
      strcpy(reason, ptr);
    }
    else
    {
      trap_SendServerCommand( ent-g_entities, "print \"callvote: Warning: invalid argument specified \n\"" );
    }
  }

  level.votePassThreshold = 50;

  // detect clientNum for partial name match votes
  if( !Q_stricmp( arg1, "kick" ) ||
    !Q_stricmp( arg1, "denybuild" ) ||
    !Q_stricmp( arg1, "allowbuild" ) ||
    !Q_stricmp( arg1, "mute" ) ||
    !Q_stricmp( arg1, "unmute" ) || 
    !Q_stricmp( arg1, "forcespec" ) ||
    !Q_stricmp( arg1, "unforcespec" ) )
  {
    int clientNums[ MAX_CLIENTS ] = { -1 };
    int numMatches=0;
    char err[ MAX_STRING_CHARS ] = "";
    
    Q_strncpyz(targetname, arg2plus, sizeof(targetname));
    ptr = strstr(targetname, " -");
    if( ptr )
      *ptr = '\0';
    
    if( g_requireVoteReasons.integer && !G_admin_permission( ent, ADMF_UNACCOUNTABLE ) && !Q_stricmp( arg1, "kick" ) && reason[ 0 ]=='\0' )
    {
       trap_SendServerCommand( ent-g_entities, "print \"callvote: You must specify a reason. Use /callvote kick [player] -r [reason] \n\"" );
       return;
    }
    
    if( !targetname[ 0 ] )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: no target\n\"" );
      return;
    }

    numMatches = G_ClientNumbersFromString( targetname, clientNums, MAX_CLIENTS );
    if( numMatches == 1 )
    {
      // there was only one partial name match
      clientNum = clientNums[ 0 ]; 
    }
    else
    {
      // look for an exact name match (sets clientNum to -1 if it fails) 
      clientNum = G_ClientNumberFromString( targetname );
    }
    
    if( clientNum==-1  && numMatches > 1 ) 
    {
      G_MatchOnePlayer( clientNums, numMatches, err, sizeof( err ) );
      ADMP( va( "^3callvote: ^7%s\n", err ) );
      return;
    }
    
    if( clientNum != -1 &&
      level.clients[ clientNum ].pers.connected == CON_DISCONNECTED )
    {
      clientNum = -1;
    }

    if( clientNum != -1 )
    {
      Q_strncpyz( name, level.clients[ clientNum ].pers.netname,
        sizeof( name ) );
      Q_CleanStr( name );
      if ( G_admin_permission ( &g_entities[ clientNum ], ADMF_IMMUNITY ) )
      {
    char reasonprint[ MAX_STRING_CHARS ] = "";

    if( reason[ 0 ] != '\0' )
      Com_sprintf(reasonprint, sizeof(reasonprint), "With reason: %s", reason);

        Com_sprintf( message, sizeof( message ), "%s^7 attempted /callvote %s %s on immune admin %s^7 %s^7",
          ent->client->pers.netname, arg1, targetname, g_entities[ clientNum ].client->pers.netname, reasonprint );
      }
    }
    else
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: invalid player\n\"" );
      return;
    }
  }
 
  if( !Q_stricmp( arg1, "kick" ) )
  {
    if( G_admin_permission( &g_entities[ clientNum ], ADMF_IMMUNITY ) )
    {
      //trap_SendServerCommand( ent-g_entities,
      //  "print \"callvote: admin is immune from vote kick\n\"" );
      //G_AdminsPrintf( "%s\n", message );
      //return;
			Com_sprintf( level.voteString, sizeof( level.voteString ),
				"m %d a kick vote against you passed. it was called by %s", clientNum,
				ent->client->pers.netname );
    }
    else
    {
      // use ip in case this player disconnects before the vote ends
      if( reason[0] != '\0' )
        Com_sprintf( level.voteString, sizeof( level.voteString ),
          "!ban %s \"%s\" vote kick: %s", level.clients[ clientNum ].pers.ip,
          g_adminTempBan.string, reason );
      else
        Com_sprintf( level.voteString, sizeof( level.voteString ),
          "!ban %s \"%s\" vote kick: no reason", level.clients[ clientNum ].pers.ip,
          g_adminTempBan.string );
    }
    Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
      "Kick player \'%s\'", name );
  }
  else if( !Q_stricmp( arg1, "mute" ) )
  {
    
    if( level.clients[ clientNum ].pers.muted )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: player is already muted\n\"" );
      return;
    }

    if( G_admin_permission( &g_entities[ clientNum ], ADMF_IMMUNITY ) )
    {
      // trap_SendServerCommand( ent-g_entities,
      // "print \"callvote: admin is immune from vote mute\n\"" );
      // G_AdminsPrintf("%s\n",message);
      // return;
      Com_sprintf( level.voteString, sizeof( level.voteString ),
        "m %d a mute vote against you passed. it was called by %s", clientNum,
      ent->client->pers.netname );
    }
    else
    {
      Com_sprintf( level.voteString, sizeof( level.voteString ),
        "!mute %i", clientNum );
    }
    Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
      "Mute player \'%s\'", name );
  }
  else if( !Q_stricmp( arg1, "unmute" ) )
  {
    if( !level.clients[ clientNum ].pers.muted )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: player is not currently muted\n\"" );
      return;
    }
    Com_sprintf( level.voteString, sizeof( level.voteString ),
      "!unmute %i", clientNum );
    Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
      "Un-Mute player \'%s\'", name );
  }
  else if( !Q_stricmp( arg1, "map_restart" ) )
  {
    if( g_mapvoteMaxTime.integer 
      && (( level.time - level.startTime ) >= g_mapvoteMaxTime.integer * 1000 )
      && !G_admin_permission( ent, ADMF_NO_VOTE_LIMIT ) 
      && (level.numPlayingClients > 0 && level.numConnectedClients>1) )
    {
       trap_SendServerCommand( ent-g_entities, va(
         "print \"You cannot call for a restart after %d seconds\n\"",
         g_mapvoteMaxTime.integer ) );
       return;
    }
    Com_sprintf( level.voteString, sizeof( level.voteString ), "%s", arg1 );
    Com_sprintf( level.voteDisplayString,
        sizeof( level.voteDisplayString ), "Restart current map" );
    level.votePassThreshold = g_mapVotesPercent.integer;
  }
  else if( !Q_stricmp( arg1, "map" ) )
  {
    if( g_mapvoteMaxTime.integer 
      && (( level.time - level.startTime ) >= g_mapvoteMaxTime.integer * 1000 )
      && !G_admin_permission( ent, ADMF_NO_VOTE_LIMIT ) 
      && (level.numPlayingClients > 0 && level.numConnectedClients>1) )
    {
       trap_SendServerCommand( ent-g_entities, va(
         "print \"You cannot call for a mapchange after %d seconds\n\"",
         g_mapvoteMaxTime.integer ) );
       return;
    }
  
    if( !G_MapExists( arg2 ) )
    {
      trap_SendServerCommand( ent - g_entities, va( "print \"callvote: "
        "'maps/%s.bsp' could not be found on the server\n\"", arg2 ) );
      return;
    }
    if( g_extremeSuddenDeath.integer )
    {
      trap_SendServerCommand( ent-g_entities, "print \"callvote: You cannot call map votes in Extreme Sudden Death\n\"" );
      return;
    } 
    Com_sprintf( level.voteString, sizeof( level.voteString ), "%s %s", arg1, arg2 );
    Com_sprintf( level.voteDisplayString,
        sizeof( level.voteDisplayString ), "Change to map '%s'", arg2 );
    level.votePassThreshold = g_mapVotesPercent.integer;
  }
  else if( !Q_stricmp( arg1, "nextmap" ) )
  {
    if( G_MapExists( g_nextMap.string ) )
    {
      trap_SendServerCommand( ent - g_entities, va( "print \"callvote: "
        "the next map is already set to '%s^7'\n\"", g_nextMap.string ) );
      return;
    }

    if( !trap_FS_FOpenFile( va( "maps/%s.bsp", arg2 ), NULL, FS_READ ) )
    {
      trap_SendServerCommand( ent - g_entities, va( "print \"callvote: "
        "'maps/%s^7.bsp' could not be found on the server\n\"", arg2 ) );
      return;
    }
    Com_sprintf( level.voteString, sizeof( level.voteString ),
      "set g_nextMap %s", arg2 );
    Com_sprintf( level.voteDisplayString,
        sizeof( level.voteDisplayString ), "Set the ^1next map^7 to '%s^7'", arg2 );
    level.votePassThreshold = g_mapVotesPercent.integer;
  }
  else if( !Q_stricmp( arg1, "draw" ) )
  {
    Com_sprintf( level.voteString, sizeof( level.voteString ), "evacuation" );
    Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
        "End match in a draw" );
    level.votePassThreshold = g_mapVotesPercent.integer;
  }
  else if( !Q_stricmp( arg1, "poll" ) )
  {
    if( arg2plus[ 0 ] == '\0' )
    {
      trap_SendServerCommand( ent-g_entities, "print \"callvote: You forgot to specify what people should vote on.\n\"" );
      return;
    }
    Com_sprintf( level.voteString, sizeof( level.voteString ), nullstring );
    Com_sprintf( level.voteDisplayString,
        sizeof( level.voteDisplayString ), "[Poll] \'%s\'", arg2plus );
  }
  else if( !Q_stricmp( arg1, "sudden_death" ) ||
    !Q_stricmp( arg1, "suddendeath" ) )
  {
    if( level.extremeSuddenDeath )
    {
      trap_SendServerCommand( ent - g_entities,
        "print \"callvote: the game is already in extreme sudden death\n\"" );
      return;
    }
    if( !g_suddenDeathVotePercent.integer )
    {
      trap_SendServerCommand( ent-g_entities, "print \"callvote: Sudden Death votes have been disabled\n\"" );
      return;
    } 
    else if( level.suddenDeath ) 
    {
      trap_SendServerCommand( ent - g_entities, va( "print \"callvote: Sudden Death has already begun\n\"") );
      return;
    }
    else if( G_TimeTilSuddenDeath() <= g_suddenDeathVoteDelay.integer * 1000 )
    {
      trap_SendServerCommand( ent - g_entities, va( "print \"callvote: Sudden Death is already immenent\n\"") );
      return;
    }
    else 
    {
      level.votePassThreshold = g_suddenDeathVotePercent.integer;
      Com_sprintf( level.voteString, sizeof( level.voteString ), "suddendeath" );
      Com_sprintf( level.voteDisplayString,
          sizeof( level.voteDisplayString ), "Begin Sudden Death" );

      if( g_suddenDeathVoteDelay.integer )
        Q_strcat( level.voteDisplayString, sizeof( level.voteDisplayString ), va( " in %d seconds", g_suddenDeathVoteDelay.integer ) );

    }
  }
  else if( !Q_stricmp( arg1, "extreme_sudden_death" ) ||
    !Q_stricmp( arg1, "extremesuddendeath" ) ||
    !Q_stricmp( arg1, "esd" ) )
  {
    if( level.extremeSuddenDeath )
    {
      trap_SendServerCommand( ent - g_entities,
        "print \"callvote: Extreme Sudden Death has already begun\n\"" );
      return;
    }
    if( !g_extremeSuddenDeathVotePercent.integer )
    {
      trap_SendServerCommand( ent-g_entities, "print \"callvote: Extreme Sudden Death votes have been disabled\n\"" );
      return;
    } 
    else if( level.extremeSuddenDeathWarning >= TW_CLOSE || g_extremeSuddenDeathVote.integer)
    {
      trap_SendServerCommand( ent - g_entities, va( "print \"callvote: Extreme Sudden Death is already immenent\n\"") );
      return;
    }
    else 
    {
      level.votePassThreshold = g_extremeSuddenDeathVotePercent.integer;
      Com_sprintf( level.voteString, sizeof( level.voteString ), "set g_extremeSuddenDeathVote 1" );
      Com_sprintf( level.voteDisplayString,
          sizeof( level.voteDisplayString ), "Begin Extreme Sudden Death" );

      if( g_extremeSuddenDeathVoteDelay.integer )
        Q_strcat( level.voteDisplayString, sizeof( level.voteDisplayString ), va( " in %d seconds", g_extremeSuddenDeathVoteDelay.integer ) );
    }
  }
  else if( !Q_stricmp( arg1, "forcespec" ) )
  {
    if( level.clients[ clientNum ].pers.specd )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: player is already forced to the spectator team\n\"" );
      return;
    }
    if( G_admin_permission( &g_entities[ clientNum ], ADMF_IMMUNITY ) )
    {
      //trap_SendServerCommand( ent-g_entities,
      //  "print \"callvote: admin is immune from forcespec\n\"" );
      //return;
      Com_sprintf( level.voteString, sizeof( level.voteString ),
        "m %d a forcespec vote against you passed. it was called by %s", clientNum,
      ent->client->pers.netname );
    }
    else
    {
      Com_sprintf( level.voteString, sizeof( level.voteString ),
        "!forcespec %i", clientNum );
    }
   	Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
      "Force player \'%s^7\' to the spectators", name );
  }
  else if( !Q_stricmp( arg1, "unforcespec" ) )
  {
    if( !level.clients[ clientNum ].pers.specd )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: player is not currently forced to be on the spectator team\n\"" );
      return;
    }
    if( G_admin_permission( &g_entities[ clientNum ], ADMF_IMMUNITY ) )
    {
      //trap_SendServerCommand( ent-g_entities,
      //  "print \"callvote: admin is immune from forcespec\n\"" );
      //return;
      Com_sprintf( level.voteString, sizeof( level.voteString ),
        "m %d a denybuild vote against you passed. it was called by %s", clientNum,
      ent->client->pers.netname );
    }
    else
    {
      Com_sprintf( level.voteString, sizeof( level.voteString ),
        "!denybuild %i", clientNum );
   	}
    Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
      "Allow player \'%s^7\' to join teams", name );
  }
  else if( !Q_stricmp( arg1, "denybuild" ) )
  {
    if( level.clients[ clientNum ].pers.denyBuild )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: player is already denybuilded not allowed to build\n\"" );
      return;
    }
    if( G_admin_permission( &g_entities[ clientNum ], ADMF_IMMUNITY ) )
    {
      //trap_SendServerCommand( ent-g_entities,
      //  "print \"callvote: admin is immune from forcespec\n\"" );
      //return;
      Com_sprintf( level.voteString, sizeof( level.voteString ),
        "m %d a denybuild vote against you passed. it was called by %s", clientNum,
      ent->client->pers.netname );
    }
    else
    {
    Com_sprintf( level.voteString, sizeof( level.voteString ),
      "!denybuild %i", clientNum );
    }
   	Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
      "Deny player \'%s^7\' building rights", name );
  }
  else if( !Q_stricmp( arg1, "allowbuild" ) )
  {
    if( !level.clients[ clientNum ].pers.denyBuild )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: player can already build\n\"" );
      return;
    }
    Com_sprintf( level.voteString, sizeof( level.voteString ),
      "!allowbuild %i", clientNum );
   	Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
      "Grant player \'%s^7\' building rights", name );
  }
  else if( !Q_stricmp( arg1, "extend" ) )
  {
    if( g_extendvotetime.value <= 0 )
      trap_SendServerCommand( ent-g_entities, "print \"Extend votes are disabled\n\"" );    
    else
    {
      //The calculations go on in g_main.c
      Com_sprintf( level.voteString, sizeof( level.voteString ),
        "set g_extendvote 1" );
      Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ),
        "Extend game time by %i minutes", g_extendvotetime.integer );
      level.votePassThreshold = g_extendvotepercent.value;
    }
  }
  else
  {
    trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string\n\"" );
    trap_SendServerCommand( ent-g_entities, "print \"Valid vote commands are: "
      "map, map_restart, draw, nextmap, kick, mute, unmute, poll, sudden_death, extreme_sudden_death, and extend\n" );
    return;
  }
  
  if( level.votePassThreshold != 50 )
  {
    Q_strcat( level.voteDisplayString, sizeof( level.voteDisplayString ), va( " (Needs > %d percent)", level.votePassThreshold ) );
  }
  
  if ( reason[0] != '\0' )
    Q_strcat( level.voteDisplayString, sizeof( level.voteDisplayString ), va( " ^7Reason: '%s^7'", reason ) );
  

  trap_SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE
         " called a vote: %s\n\"", ent->client->pers.netname, level.voteDisplayString ) );
         
  //For players who are blind:
  trap_SendServerCommand( -1, va( "cp \"%s" S_COLOR_WHITE
         " has called a vote\n ^2F1 ^7(Yes) - ^1F2 ^7(No)\"", ent->client->pers.netname ) );
  
  G_LogPrintf("Vote: %s^7 called a vote: %s^7\n", ent->client->pers.netname, level.voteDisplayString );
  
  Q_strcat( level.voteDisplayString, sizeof( level.voteDisplayString ), va( " ^7Called by: '%s^7'", ent->client->pers.netname ) );

  ent->client->pers.voteCount++;

  level.voteTime = level.time;
  level.voteNo = 0;
  level.voteYes = 0;

  for( i = 0 ; i < level.maxclients ; i++ )
    level.clients[i].ps.eFlags &= ~EF_VOTED;

  trap_SetConfigstring( CS_VOTE_TIME, va( "%i", level.voteTime ) );
  trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );
  trap_SetConfigstring( CS_VOTE_YES, "0" );
  trap_SetConfigstring( CS_VOTE_NO, "0" );
}

/*
==================
Cmd_CallTeamVote_f
==================
*/
//TODO: Wire: redo this based on lakitu7's code
void Cmd_CallTeamVote_f( gentity_t *ent )
{
  int   i, team, cs_offset = 0;
  char  arg1[ MAX_STRING_TOKENS ];
  char  arg2[ MAX_NAME_LENGTH ];
  int   clientNum = -1;
  char  name[ MAX_NAME_LENGTH ];

  team = ent->client->pers.teamSelection;

  if( team == TEAM_ALIENS )
    cs_offset = 1;

  if( !g_allowVote.integer )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here\n\"" );
    return;
  }

  if( level.teamVoteTime[ cs_offset ] )
  {
    trap_SendServerCommand( ent-g_entities, "print \"A team vote is already in progress\n\"" );
    return;
  }

  if( level.voteTime )
  {
    trap_SendServerCommand( ent-g_entities, "print \"A regular vote is currently in progress\n\"" );
    return;
  }

  if( g_voteLimit.integer > 0 &&
    ent->client->pers.voteCount >= g_voteLimit.integer &&
    !G_admin_permission( ent, ADMF_NO_VOTE_LIMIT ) )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"You have already called the maximum number of votes (%d)\n\"",
      g_voteLimit.integer ) );
    return;
  }

  if( ent->client->pers.muted )
  {
    trap_SendServerCommand( ent - g_entities,
      "print \"You are muted and cannot call teamvotes\n\"" );
    return;
  }

  if( g_voteMinTime.integer
    && ent->client->pers.firstConnect 
    && level.time - ent->client->pers.enterTime < g_voteMinTime.integer * 1000
    && !G_admin_permission( ent, ADMF_NO_VOTE_LIMIT ) 
    && (level.numPlayingClients > 0 && level.numConnectedClients>1) )
  {
    trap_SendServerCommand( ent-g_entities, va(
      "print \"You must wait %d seconds after connecting before calling a vote\n\"",
      g_voteMinTime.integer ) );
    return;
  }
  // make sure it is a valid command to vote on
  trap_Argv( 1, arg1, sizeof( arg1 ) );
  trap_Argv( 2, arg2, sizeof( arg2 ) );

  if( strchr( arg1, ';' ) || strchr( arg2, ';' ) )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Invalid team vote string\n\"" );
    return;
  }

  // detect clientNum for partial name match votes
  if( !Q_stricmp( arg1, "kick" ) ||
    !Q_stricmp( arg1, "denybuild" ) ||
    !Q_stricmp( arg1, "allowbuild" ) || 
    !Q_stricmp( arg1, "designate" ) || 
    !Q_stricmp( arg1, "undesignate" ) )
  {
    int clientNums[ MAX_CLIENTS ];
    int matches = 0;
    char err[ MAX_STRING_CHARS ] = "";

    if( !arg2[ 0 ] )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callteamvote: no target\n\"" );
      return;
    }

    matches = G_ClientNumbersFromString( arg2, clientNums, MAX_CLIENTS );
    if( matches == 1 )
    {
      // there was only one partial name match
      clientNum = clientNums[ 0 ];
    }
    else
    {
      // look for an exact name match (sets clientNum to -1 if it fails)
      clientNum = G_ClientNumberFromString( arg2 );
    }

    // make sure this player is on the same team
    if( clientNum != -1 && level.clients[ clientNum ].pers.teamSelection !=
      team )
      clientNum = -1;

    if( clientNum != -1 )
    {
      Q_strncpyz( name, level.clients[ clientNum ].pers.netname,
        sizeof( name ) );
      Q_CleanStr( name );
    }
    else if( matches > 1 )
    {
      G_MatchOnePlayer( clientNums, matches, err, sizeof( err ) );
      ADMP( va( "^3callvote: ^7%s\n", err ) );
      return;
    }
    else if( matches > 1 )
    {
      G_MatchOnePlayer( clientNums, matches, err, sizeof( err ) );
      ADMP( va( "^3callteamvote: ^7%s\n", err ) );
      return;
    }
    else
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callteamvote: invalid player\n\"" );
      return;
    }
  }

  if( !Q_stricmp( arg1, "kick" ) )
  {
    if( G_admin_permission( &g_entities[ clientNum ], ADMF_IMMUNITY ) )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callteamvote: admin is immune from vote kick\n\"" );
      return;
    }
    if( level.clients[ clientNum ].pers.localClient )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callteamvote: host is immune from vote kick\n\"" );
      return;
    }

    // use ip in case this player disconnects before the vote ends
    Com_sprintf( level.teamVoteString[ cs_offset ],
      sizeof( level.teamVoteString[ cs_offset ] ),
      "!ban %s \"1s%s\" team vote kick", level.clients[ clientNum ].pers.ip,
      g_adminTempBan.string );
    Com_sprintf( level.teamVoteDisplayString[ cs_offset ],
        sizeof( level.teamVoteDisplayString[ cs_offset ] ),
        "Kick player '%s'", name );
  }
  else if( !Q_stricmp( arg1, "denybuild" ) )
  {
    if( level.clients[ clientNum ].pers.denyBuild )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callteamvote: player already lost building rights\n\"" );
      return;
    }

    if( G_admin_permission( &g_entities[ clientNum ], ADMF_IMMUNITY ) )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callteamvote: admin is immune from denybuild\n\"" );
      return;
    }

    Com_sprintf( level.teamVoteString[ cs_offset ],
      sizeof( level.teamVoteString[ cs_offset ] ), "!denybuild %i", clientNum );
    Com_sprintf( level.teamVoteDisplayString[ cs_offset ],
        sizeof( level.teamVoteDisplayString[ cs_offset ] ),
        "Take away building rights from '%s'", name );
  }
  else if( !Q_stricmp( arg1, "allowbuild" ) )
  {
    if( !level.clients[ clientNum ].pers.denyBuild )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callteamvote: player already has building rights\n\"" );
      return;
    }

    Com_sprintf( level.teamVoteString[ cs_offset ],
      sizeof( level.teamVoteString[ cs_offset ] ), "!allowbuild %i", clientNum );
    Com_sprintf( level.teamVoteDisplayString[ cs_offset ],
        sizeof( level.teamVoteDisplayString[ cs_offset ] ),
        "Allow '%s' to build", name );
  }
  else if( !Q_stricmp( arg1, "designate" ) )
  {
    if( !g_designateVotes.integer )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: Designate votes have been disabled.\n\"" );
      return;
    }
	  
    if( level.clients[ clientNum ].pers.designatedBuilder )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: player is already a designated builder\n\"" );
      return;
    }
    Com_sprintf( level.teamVoteString[ cs_offset ],
      sizeof( level.teamVoteString[ cs_offset ] ), "!designate %i", clientNum );
    Com_sprintf( level.teamVoteDisplayString[ cs_offset ],
        sizeof( level.teamVoteDisplayString[ cs_offset ] ),
        "Make '%s' a designated builder", name );
  }
  else if( !Q_stricmp( arg1, "undesignate" ) )
  {
	  
    if( !g_designateVotes.integer )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: Designate votes have been disabled.\n\"" );
      return;
    }
	  
    if( !level.clients[ clientNum ].pers.designatedBuilder )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"callvote: player is not currently a designated builder\n\"" );
      return;
    }
    Com_sprintf( level.teamVoteString[ cs_offset ],
      sizeof( level.teamVoteString[ cs_offset ] ), "!undesignate %i", clientNum );
    Com_sprintf( level.teamVoteDisplayString[ cs_offset ],
        sizeof( level.teamVoteDisplayString[ cs_offset ] ),
        "Remove designated builder status from '%s'", name );
  }
  else if( !Q_stricmp( arg1, "admitdefeat" ) )
  {
    if( team == level.surrenderTeam )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You have already surrendered\n\"");
      return;
    }
    Com_sprintf( level.teamVoteString[ cs_offset ],
      sizeof( level.teamVoteString[ cs_offset ] ), "admitdefeat %i", team );
    Com_sprintf( level.teamVoteDisplayString[ cs_offset ],
        sizeof( level.teamVoteDisplayString[ cs_offset ] ),
        "Admit Defeat" );
  }
  else
  {
    trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string\n\"" );
    trap_SendServerCommand( ent-g_entities,
       "print \"Valid team vote commands are: "
       "kick, denybuild, allowbuild and admitdefeat\n\"" );
    return;
  }
  ent->client->pers.voteCount++;

  G_TeamCommand( team, va( "print \"%s " S_COLOR_WHITE "called a team vote: %s\n\"",
    ent->client->pers.netname, level.teamVoteDisplayString[ cs_offset ] ) );

  G_LogPrintf( "Teamvote: %s^7 called a teamvote (%s): %s\n", 
      ent->client->pers.netname, BG_TeamName(team), 
      level.teamVoteDisplayString[ cs_offset ] );

  G_TeamCommand( team, va( "cp \"%s " S_COLOR_WHITE "has called a team vote\n ^2F1^7 (Yes) - ^1F2 ^7(No)\"",
    ent->client->pers.netname ) );

  G_Printf( "'%s' called a teamvote for '%s'\n", ent->client->pers.netname, 
    level.teamVoteString[ cs_offset ] ) ;

  // start the voting, the caller automatically votes yes (lies)
  level.teamVoteTime[ cs_offset ] = level.time;
  level.teamVoteYes[ cs_offset ] = 0;
  level.teamVoteNo[ cs_offset ] = 0;
  //ent->client->pers.teamVote = qtrue;

  for( i = 0; i < level.maxclients; i++ )
  {
    if( level.clients[ i ].ps.stats[ STAT_TEAM ] == team )
      level.clients[ i ].ps.eFlags &= ~EF_TEAMVOTED;
  }

  //ent->client->ps.eFlags |= EF_TEAMVOTED;

  trap_SetConfigstring( CS_TEAMVOTE_TIME + cs_offset,
    va( "%i", level.teamVoteTime[ cs_offset ] ) );
  trap_SetConfigstring( CS_TEAMVOTE_STRING + cs_offset,
    level.teamVoteDisplayString[ cs_offset ] );
  trap_SetConfigstring( CS_TEAMVOTE_YES + cs_offset, "0" );
  trap_SetConfigstring( CS_TEAMVOTE_NO + cs_offset, "0" );
}


/*
==================
Cmd_TeamVote_f
==================
*/
void Cmd_TeamVote_f( gentity_t *ent )
{
  int     cs_offset = 0;
  char    msg[ 64 ];

  if( ent->client->pers.teamSelection == TEAM_ALIENS )
    cs_offset = 1;

  if( !level.teamVoteTime[ cs_offset ] )
  {
    trap_SendServerCommand( ent-g_entities, "print \"No team vote in progress\n\"" );
    return;
  }

  if( ent->client->ps.eFlags & EF_TEAMVOTED )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Team vote already cast\n\"" );
    return;
  }

  trap_SendServerCommand( ent-g_entities, "print \"Team vote cast\n\"" );

  trap_Argv( 1, msg, sizeof( msg ) );
  ent->client->pers.teamVote = ( tolower( msg[ 0 ] ) == 'y' || msg[ 0 ] == '1' );
  G_TeamVote( ent, qtrue );

  // a majority will be determined in CheckTeamVote, which will also account
  // for players entering or leaving
}

/*
==================
Cmd_Vote_f
==================
*/
void Cmd_Vote_f( gentity_t *ent )
{
  char msg[ 64 ];

  if( !level.voteTime )
  {
    if( ent->client->pers.teamSelection != TEAM_NONE )
    {
      // If there is a teamvote going on but no global vote, forward this vote on as a teamvote
      // (ugly hack for 1.1 cgames + noobs who can't figure out how to use any command that isn't bound by default)
      int     cs_offset = 0;
      if( ent->client->pers.teamSelection == TEAM_ALIENS )
        cs_offset = 1;
    
      if( level.teamVoteTime[ cs_offset ] )
      {
         if( !(ent->client->ps.eFlags & EF_TEAMVOTED ) )
         {
           Cmd_TeamVote_f( ent ); 
	         return;
         }
       }
     }
    trap_SendServerCommand( ent-g_entities, "print \"No vote in progress\n\"" );
    return;
  }

  if( ent->client->ps.eFlags & EF_VOTED )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Vote already cast\n\"" );
    return;
  }

  trap_SendServerCommand( ent-g_entities, "print \"Vote cast\n\"" );

  trap_Argv( 1, msg, sizeof( msg ) );
  ent->client->pers.vote = ( tolower( msg[ 0 ] ) == 'y' || msg[ 0 ] == '1' );
  G_Vote( ent, qtrue );

  // a majority will be determined in CheckVote, which will also account
  // for players entering or leaving
}

/*
=================
Cmd_SetViewpos_f
=================
*/
void Cmd_SetViewpos_f( gentity_t *ent )
{
  vec3_t  origin, angles;
  char    buffer[ MAX_TOKEN_CHARS ];
  int     i;

  if( trap_Argc( ) != 5 )
  {
    trap_SendServerCommand( ent-g_entities, "print \"usage: setviewpos x y z yaw\n\"" );
    return;
  }

  VectorClear( angles );

  for( i = 0; i < 3; i++ )
  {
    trap_Argv( i + 1, buffer, sizeof( buffer ) );
    origin[ i ] = atof( buffer );
  }

  trap_Argv( 4, buffer, sizeof( buffer ) );
  angles[ YAW ] = atof( buffer );

  TeleportPlayer( ent, origin, angles );
}

#define AS_OVER_RT3         ((ALIENSENSE_RANGE*0.5f)/M_ROOT3)

static qboolean G_RoomForClassChange( gentity_t *ent, class_t class,
  vec3_t newOrigin )
{
  vec3_t    fromMins, fromMaxs;
  vec3_t    toMins, toMaxs;
  vec3_t    temp;
  trace_t   tr;
  float     nudgeHeight;
  float     maxHorizGrowth;
  class_t   oldClass = ent->client->ps.stats[ STAT_CLASS ];

  BG_ClassBoundingBox( oldClass, fromMins, fromMaxs, NULL, NULL, NULL );
  BG_ClassBoundingBox( class, toMins, toMaxs, NULL, NULL, NULL );

  VectorCopy( ent->s.origin, newOrigin );

  // find max x/y diff
  maxHorizGrowth = toMaxs[ 0 ] - fromMaxs[ 0 ];
  if( toMaxs[ 1 ] - fromMaxs[ 1 ] > maxHorizGrowth )
    maxHorizGrowth = toMaxs[ 1 ] - fromMaxs[ 1 ];
  if( toMins[ 0 ] - fromMins[ 0 ] > -maxHorizGrowth )
    maxHorizGrowth = -( toMins[ 0 ] - fromMins[ 0 ] );
  if( toMins[ 1 ] - fromMins[ 1 ] > -maxHorizGrowth )
    maxHorizGrowth = -( toMins[ 1 ] - fromMins[ 1 ] );

  if( maxHorizGrowth > 0.0f )
  {
    // test by moving the player up the max required on a 60 degree slope
    nudgeHeight = maxHorizGrowth * 2.0f;
  }
  else
  {
    // player is shrinking, so there's no need to nudge them upwards
    nudgeHeight = 0.0f;
  }

  // find what the new origin would be on a level surface
  newOrigin[ 2 ] -= toMins[ 2 ] - fromMins[ 2 ];

  //compute a place up in the air to start the real trace
  VectorCopy( newOrigin, temp );
  temp[ 2 ] += nudgeHeight;
  trap_Trace( &tr, newOrigin, toMins, toMaxs, temp, ent->s.number, MASK_PLAYERSOLID );

  //trace down to the ground so that we can evolve on slopes
  VectorCopy( newOrigin, temp );
  temp[ 2 ] += ( nudgeHeight * tr.fraction );
  trap_Trace( &tr, temp, toMins, toMaxs, newOrigin, ent->s.number, MASK_PLAYERSOLID );
  VectorCopy( tr.endpos, newOrigin );

  //make REALLY sure
  trap_Trace( &tr, newOrigin, toMins, toMaxs, newOrigin,
    ent->s.number, MASK_PLAYERSOLID );

  //check there is room to evolve
  return ( !tr.startsolid && tr.fraction == 1.0f );
}

/*
=================
Cmd_Class_f
=================
*/
void Cmd_Class_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  int       clientNum;
  int       i;
  vec3_t    infestOrigin;
  class_t   currentClass = ent->client->pers.classSelection;
  class_t   newClass;
  int       entityList[ MAX_GENTITIES ];
  vec3_t    range = { AS_OVER_RT3, AS_OVER_RT3, AS_OVER_RT3 };
  vec3_t    mins, maxs;
  int       num;
  qboolean foundhuman = qfalse, foundovermind = qfalse;
  gentity_t *other;
  
  clientNum = ent->client - level.clients;
  trap_Argv( 1, s, sizeof( s ) );
  newClass = BG_ClassByName( s )->number;

  if( ent->client->sess.spectatorState != SPECTATOR_NOT )
  {
    if( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
      G_StopFollowing( ent );

    ent->client->ps.persistant[ PERS_CREDIT ] = ent->client->pers.savedCredit;
    ent->client->ps.persistant[ PERS_KILLED ] = ent->client->pers.savedDeaths;
  
    if( ent->client->pers.teamSelection == TEAM_ALIENS )
    {
      if( newClass != PCL_ALIEN_LEVEL0 
          && newClass != PCL_ALIEN_BUILDER0 
          && newClass != PCL_ALIEN_BUILDER0_UPG ) 
        {
          G_TriggerMenu2( ent->client->ps.clientNum, MN_A_CLASSNOTSPAWN, newClass );
          return;
        }

      if( !BG_ClassIsAllowed( newClass ) )
      {
        G_TriggerMenu2( ent->client->ps.clientNum, MN_A_CLASSNOTALLOWED, newClass );
        return;
      }

      if( !BG_ClassAllowedInStage( newClass, g_alienStage.integer ) )
      {
        G_TriggerMenu2( ent->client->ps.clientNum, MN_A_CLASSNOTATSTAGE, newClass );
        return;
      }

      // spawn from an egg
      if( G_PushSpawnQueue( &level.alienSpawnQueue, clientNum ) )
      {
        ent->client->pers.classSelection = newClass;
        ent->client->ps.stats[ STAT_CLASS ] = newClass;
      }
    }
    else if( ent->client->pers.teamSelection == TEAM_HUMANS )
    {
      //set the item to spawn with
      if( !Q_stricmp( s, BG_Weapon( WP_MACHINEGUN )->name ) &&
          BG_WeaponIsAllowed( WP_MACHINEGUN ) )
      {
        ent->client->pers.humanItemSelection = WP_MACHINEGUN;
      }
      else if( !Q_stricmp( s, BG_Weapon( WP_HBUILD )->name ) &&
               BG_WeaponIsAllowed( WP_HBUILD ) )
      {
        ent->client->pers.humanItemSelection = WP_HBUILD;
      }
      else
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_UNKNOWNSPAWNITEM );
        return;
      }
      // spawn from a telenode
      if( G_PushSpawnQueue( &level.humanSpawnQueue, clientNum ) )
      {
        ent->client->pers.classSelection = PCL_HUMAN;
        ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN;
      }
    }
    return;
  }

  if( ent->health <= 0 )
    return;

  if( ent->client->pers.teamSelection == TEAM_ALIENS &&
      !( ent->client->ps.stats[ STAT_STATE ] & SS_HOVELING ) )
  {
    if( newClass == PCL_NONE )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_A_UNKNOWNCLASS );
      return;
    }

    //if we are not currently spectating, we are attempting evolution
    if( ent->client->pers.classSelection != PCL_NONE )
    {
      int cost;
    
       if( ent->client->ps.eFlags & EF_WALLCLIMB )
       {
         G_TriggerMenu( clientNum, MN_A_EVOLVEWALLWALK );
         return;
       }

      //check there are no humans nearby
      VectorAdd( ent->client->ps.origin, range, maxs );
      VectorSubtract( ent->client->ps.origin, range, mins );

      num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
      for( i = 0; i < num; i++ )
      {
        other = &g_entities[ entityList[ i ] ];

        if( (( other->client && other->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS ) ||
            ( other->s.eType == ET_BUILDABLE && other->buildableTeam == TEAM_HUMANS ) ) &&
            !ent->client->pers.override )
          foundhuman = qtrue;
        
        if( other->s.modelindex == BA_A_OVERMIND && other->health > 0 && level.overmindPresent )
          foundovermind = qtrue;
      }
      
      if( foundhuman && !foundovermind )
      {
        G_TriggerMenu( clientNum, MN_A_TOOCLOSE );
        return;
      }

      if( !level.overmindPresent &&
          !( newClass == PCL_ALIEN_BUILDER0 || newClass == PCL_ALIEN_BUILDER0_UPG ) &&
          !ent->client->pers.override )
      {
        G_TriggerMenu( clientNum, MN_A_NOOVMND_EVOLVE );
        return;
      }
      
      if( g_nodretchtogranger.integer 
          && ( currentClass == PCL_ALIEN_LEVEL0 || currentClass ==  PCL_ALIEN_LEVEL0_UPG )
          && ( newClass == PCL_ALIEN_BUILDER0 || newClass == PCL_ALIEN_BUILDER0_UPG ) )
      {
        trap_SendServerCommand( ent-g_entities, "print \"Evolving to a granger from a dretch is disabled on this map\n\"" );
        return;
      }
      
      if( newClass == PCL_ALIEN_SPITFIRE && !g_cheats.integer 
          && !ent->client->pers.override )
      {
        trap_SendServerCommand( ent-g_entities, "print \"You cannot evolve into the Spitfire yet\n\"" );
        return;
      }

      //guard against selling the HBUILD weapons exploit
      if( ent->client->sess.spectatorState == SPECTATOR_NOT &&
          ( currentClass == PCL_ALIEN_BUILDER0 ||
            currentClass == PCL_ALIEN_BUILDER0_UPG ) &&
          ent->client->ps.stats[ STAT_MISC ] > 0 &&
          !ent->client->pers.override )
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_A_EVOLVEBUILDTIMER );
        return;
      }

      cost = BG_ClassCanEvolveFromTo( currentClass, newClass,
                                      ent->client->ps.persistant[ PERS_CREDIT ],
                                      g_alienStage.integer, 0 , 0);

      if( G_RoomForClassChange( ent, newClass, infestOrigin ) )
      {
        if( cost >= 0 || ent->client->pers.override )
        {
          int oldBoostTime = -1;
          ent->client->pers.evolveHealthFraction = (float)ent->client->ps.stats[ STAT_HEALTH ] /
            (float)BG_Class( currentClass )->health;

          if( ent->client->pers.evolveHealthFraction < 0.0f )
            ent->client->pers.evolveHealthFraction = 0.0f;
          else if( ent->client->pers.evolveHealthFraction > 1.0f )
            ent->client->pers.evolveHealthFraction = 1.0f;

          //remove credit
          G_AddCreditToClient( ent->client, -cost, qtrue );
          ent->client->pers.classSelection = newClass;
          ClientUserinfoChanged( clientNum );
          VectorCopy( infestOrigin, ent->s.pos.trBase );
          if( ent->client->ps.stats[ STAT_STATE ] & SS_BOOSTED )
            oldBoostTime = ent->client->boostedTime;
          ClientSpawn( ent, ent, ent->s.pos.trBase, ent->s.apos.trBase );
          if( oldBoostTime > 0 )
          {
            ent->client->boostedTime = oldBoostTime;
            ent->client->ps.stats[ STAT_STATE ] |= SS_BOOSTED;
          }
          return;
        }
        else
        {
          G_TriggerMenu2( clientNum, MN_A_CANTEVOLVE, newClass );
          return;
        }
      }
      else
      {
        G_TriggerMenu( clientNum, MN_A_NOEROOM );
        return;
      }
    }
  }
  else if( ent->client->pers.teamSelection == TEAM_HUMANS )
  {
    G_TriggerMenu( clientNum, MN_H_DEADTOCLASS );
    return;
  }
}

/*
=================
DBCommand

Send command to all designated builders of selected team
=================
*/
void DBCommand( team_t team, const char *text )
{
  int i;
  gentity_t *ent;

  for( i = 0, ent = g_entities + i; i < level.maxclients; i++, ent++ )
  {
    if( !ent->client || ( ent->client->pers.connected != CON_CONNECTED ) ||
        ( ent->client->pers.teamSelection != team ) ||
        !ent->client->pers.designatedBuilder )
      continue;

    trap_SendServerCommand( i, text );
  }
}


/*
=================
Cmd_Destroy_f
=================
*/
void Cmd_Destroy_f( gentity_t *ent )
{
  vec3_t      forward, end;
  trace_t     tr;
  gentity_t   *traceEnt;
  char        cmd[ 12 ];
  qboolean    deconstruct = qtrue;

  if( ent->client->pers.paused )
  {
    trap_SendServerCommand( ent-g_entities,
      "print \"You may not deconstruct while paused\n\"" );
    return;
  }

  if( ent->client->pers.denyBuild )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_REVOKED );
    return;
  }

  trap_Argv( 0, cmd, sizeof( cmd ) );
  if( Q_stricmp( cmd, "destroy" ) == 0 )
    deconstruct = qfalse;

  if( ent->client->ps.stats[ STAT_STATE ] & SS_HOVELING )
  {
    if( ( ent->client->hovel->s.eFlags & EF_DBUILDER ) &&
      !ent->client->pers.designatedBuilder )
    {
      trap_SendServerCommand( ent-g_entities, 
        "print \"This structure is protected by designated builder\n\"" );
      DBCommand( ent->client->pers.teamSelection,
        va( "print \"%s^3 has attempted to decon a protected structure!\n\"",
          ent->client->pers.netname ) );
      return;
    }
    G_Damage( ent->client->hovel, ent, ent, forward, ent->s.origin, 10000, 0, MOD_SUICIDE );
  }
  else
  {
    AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
    VectorMA( ent->client->ps.origin, 100, forward, end );

    trap_Trace( &tr, ent->client->ps.origin, NULL, NULL, end, ent->s.number, MASK_PLAYERSOLID );
    traceEnt = &g_entities[ tr.entityNum ];

    if( tr.fraction < 1.0f &&
        ( traceEnt->s.eType == ET_BUILDABLE ) &&
        ( traceEnt->buildableTeam == ent->client->pers.teamSelection ) &&
        ( ent->client->ps.weapon == WP_ABUILD ||
          ent->client->ps.weapon == WP_ABUILD2 ||
          ent->client->ps.weapon == WP_ABUILD3 ||
          ent->client->ps.weapon == WP_HBUILD ) )
    {
      // Always let the builder prevent the explosion 
      if( traceEnt->health <= 0 && g_deconDead.integer )
      {
        G_QueueBuildPoints( traceEnt );
        G_FreeEntity( traceEnt );
        return;
      }

      // Cancel deconstruction
      if( g_markDeconstruct.integer && traceEnt->deconstruct )
      {
        traceEnt->deconstruct = qfalse;
        return;
      }
      if( ( traceEnt->s.eFlags & EF_DBUILDER ) &&
        !ent->client->pers.designatedBuilder )
      {
        trap_SendServerCommand( ent-g_entities, 
          "print \"This structure is protected by designated builder\n\"" );
        DBCommand( ent->client->pers.teamSelection,
          va( "print \"%s^3 has attempted to decon a protected structure!\n\"",
            ent->client->pers.netname ) );
        return;
      }

      // Prevent destruction of the last spawn
      if( !g_markDeconstruct.integer && !g_cheats.integer 
          && !level.suddenDeath && !level.extremeSuddenDeath )
      {
        if( ent->client->pers.teamSelection == TEAM_ALIENS &&
            traceEnt->s.modelindex == BA_A_SPAWN )
        {
          if( level.numAlienSpawns <= 1 )
          {
            G_TriggerMenu( ent->client->ps.clientNum, MN_B_LASTSPAWN );
            return;
          }
        }
        else if( ent->client->pers.teamSelection == TEAM_HUMANS &&
                 traceEnt->s.modelindex == BA_H_SPAWN )
        {
          if( level.numHumanSpawns <= 1 )
          {
            G_TriggerMenu( ent->client->ps.clientNum, MN_B_LASTSPAWN );
            return;
          }
        }
      }

      // Don't allow destruction of hovel with granger inside
      if( traceEnt->s.modelindex == BA_A_HOVEL && traceEnt->active )
        return;

      // Don't allow destruction of buildables that cannot be rebuilt
            if( 
              ( level.suddenDeath || level.extremeSuddenDeath )
              && 
              traceEnt->health > 0 
              &&
              (
                (
                  g_suddenDeathMode.integer == SDMODE_SELECTIVE
                  &&
                  !BG_FindReplaceableTestForBuildable( traceEnt->s.modelindex )
                )
                ||
                (
                  g_suddenDeathMode.integer == SDMODE_BP
                  &&
                  BG_FindBuildPointsForBuildable( traceEnt->s.modelindex )
                )
                ||
                g_suddenDeathMode.integer == SDMODE_NO_BUILD
                ||
                level.extremeSuddenDeath
              )
              )
      {
        trap_SendServerCommand( ent-g_entities,
          "print \"During Sudden Death you can only decon buildings that "
          "can be rebuilt\n\"" );
        return;
      }

      if( !g_markDeconstruct.integer && ent->client->ps.stats[ STAT_MISC ] > 0 )
      {
        G_AddEvent( ent, EV_BUILD_DELAY, ent->client->ps.clientNum );
        return;
      }

      if( g_markDeconstruct.integer )
      {
        if( !deconstruct )
        {
          buildHistory_t *new;

          new = BG_Alloc( sizeof( buildHistory_t ) );
          new->ID = ( ++level.lastBuildID > 1000 ) 
              ? ( level.lastBuildID = 1 ) : level.lastBuildID;
          new->ent = ent;
          new->name[ 0 ] = 0;
          new->buildable = traceEnt->s.modelindex;
          VectorCopy( traceEnt->s.pos.trBase, new->origin );
          VectorCopy( traceEnt->s.angles, new->angles );
          VectorCopy( traceEnt->s.origin2, new->origin2 );
          VectorCopy( traceEnt->s.angles2, new->angles2 );
          new->fate = BF_DECONNED;
          new->next = NULL;
          new->marked = NULL;
          G_LogBuild( new );
          
          G_Damage( traceEnt, ent, ent, forward, tr.endpos, traceEnt->health, 0, MOD_SUICIDE );
        }
        else
        {
          traceEnt->deconstruct     = qtrue; // Mark buildable for deconstruction
          traceEnt->deconstructTime = level.time;
        }
      }
      else
      {
        buildHistory_t *new;

        new = BG_Alloc( sizeof( buildHistory_t ) );
        new->ID = ( ++level.lastBuildID > 1000 ) 
            ? ( level.lastBuildID = 1 ) : level.lastBuildID;
        new->ent = ent;
        new->name[ 0 ] = 0;
        new->buildable = traceEnt->s.modelindex;
        VectorCopy( traceEnt->s.pos.trBase, new->origin );
        VectorCopy( traceEnt->s.angles, new->angles );
        VectorCopy( traceEnt->s.origin2, new->origin2 );
        VectorCopy( traceEnt->s.angles2, new->angles2 );
        new->fate = BF_DECONNED;
        new->next = NULL;
        new->marked = NULL;
        G_LogBuild( new );
        G_LogDestruction( traceEnt, ent, MOD_DECONSTRUCT );
  

				if( !deconstruct )
					G_Damage( traceEnt, ent, ent, forward, tr.endpos, 10000, 0, MOD_SUICIDE );
				else
					G_FreeEntity( traceEnt );

        if( !g_cheats.integer )
          ent->client->ps.stats[ STAT_MISC ] +=
            BG_Buildable( traceEnt->s.modelindex )->buildTime >> 2;
      }
    }
  }
}


/*
=================
Cmd_ActivateItem_f

Activate an item
=================
*/
void Cmd_ActivateItem_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  int   upgrade, weapon;

  trap_Argv( 1, s, sizeof( s ) );
  
  // "weapon" aliased to whatever weapon you have
  if( !Q_stricmp( "weapon", s ) )
  {
    if( ent->client->ps.weapon == WP_BLASTER &&
        BG_PlayerCanChangeWeapon( &ent->client->ps ) )
      G_ForceWeaponChange( ent, WP_NONE );  
    return;
  }
  
  upgrade = BG_UpgradeByName( s )->number;
  weapon = BG_WeaponByName( s )->number;

  if( upgrade != UP_NONE && BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
  {
    BG_ActivateUpgrade( upgrade, ent->client->ps.stats );
  }
  else if( upgrade == UP_JETPACK && weapon == WP_SPITFIRE )
  {
    BG_ActivateUpgrade( UP_SPITPACK, ent->client->ps.stats );
  }
  else if( weapon != WP_NONE && BG_InventoryContainsWeapon( weapon, ent->client->ps.stats ) )
  {
    if( ent->client->ps.weapon != weapon && BG_PlayerCanChangeWeapon( &ent->client->ps ) )
    {
      G_ForceWeaponChange( ent, weapon );
    }
  }
  else
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"You don't have the %s\n\"", s ) );
  }
}


/*
=================
Cmd_DeActivateItem_f

Deactivate an item
=================
*/
void Cmd_DeActivateItem_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  int   upgrade;

  trap_Argv( 1, s, sizeof( s ) );
  upgrade = BG_UpgradeByName( s )->number;

  if( BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
    BG_DeactivateUpgrade( upgrade, ent->client->ps.stats );
  else
    trap_SendServerCommand( ent-g_entities, va( "print \"You don't have the %s\n\"", s ) );
}


/*
=================
Cmd_ToggleItem_f
=================
*/
void Cmd_ToggleItem_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  int   upgrade, weapon;

  trap_Argv( 1, s, sizeof( s ) );
  upgrade = BG_UpgradeByName( s )->number;
  weapon = BG_WeaponByName( s )->number;

  if( upgrade == UP_JETPACK )
  {
    if( ent->client->ps.stats[ STAT_CLASS ] == PCL_ALIEN_SPITFIRE )
      upgrade = UP_SPITPACK;
    else
      ent->client->ps.stats[ STAT_JPRCDELAY ] = level.time + JETPACK_RC_CHARGE_DELAY;
  }

  if( weapon != WP_NONE )
  {
    if( !BG_PlayerCanChangeWeapon( &ent->client->ps ) )
      return;

    //special case to allow switching between
    //the blaster and the primary weapon
    if( ent->client->ps.weapon != WP_BLASTER )
      weapon = WP_BLASTER;
    else
      weapon = WP_NONE;

    G_ForceWeaponChange( ent, weapon );
  }
  else if( BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
  {
    if( BG_UpgradeIsActive( upgrade, ent->client->ps.stats ) )
      BG_DeactivateUpgrade( upgrade, ent->client->ps.stats );
    else
      BG_ActivateUpgrade( upgrade, ent->client->ps.stats );
  }
  else
    trap_SendServerCommand( ent-g_entities, va( "print \"You don't have the %s\n\"", s ) );
}

/*
=================
Cmd_Buy_f
=================
*/
void Cmd_Buy_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  weapon_t  weapon;
  upgrade_t upgrade;
  qboolean armAvailable = qfalse;
  qboolean powerAvailable = qfalse;
  qboolean energyWeapon;
  qboolean override;

  trap_Argv( 1, s, sizeof( s ) );

  weapon = BG_WeaponByName( s )->number;
  upgrade = BG_UpgradeByName( s )->number;
  override = ent->client->pers.override;

  // only humans can buy
  if( ent->client->pers.teamSelection != TEAM_HUMANS && !override )
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"Only humans can buy weapons or items.\n\"" ) );
    return;
  }

  // determine if there is a nearby arm
  if( G_BuildableRange( ent->client->ps.origin, 100, BA_H_ARMOURY ) && !override )
  {
    gentity_t *armEnt;
    int       entityList[ MAX_GENTITIES ];
    vec3_t    range = { 100, 100, 100 };
    vec3_t    mins, maxs;
    int       i, num;

    VectorAdd( ent->client->ps.origin, range, maxs );
    VectorSubtract( ent->client->ps.origin, range, mins );
    
    num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
    for( i = 0; i < num; i++ )
    {
      armEnt = &g_entities[ entityList[ i ] ];

      //checked for the range in the if clause, no need to do the other things again
      if( armEnt->s.eType != ET_BUILDABLE 
          || armEnt->s.modelindex != BA_H_ARMOURY )
      continue;
      
      if( !G_Visible( ent, armEnt, CONTENTS_SOLID ) )
        armAvailable = qfalse;
      else
        armAvailable = qtrue;
        
      break;
    }
  }
  else if( override )
    armAvailable = qtrue;
    
  // determine if there is a nearby reactor or repeater
  if( G_BuildableRange( ent->client->ps.origin, 100, BA_H_REACTOR ) || G_BuildableRange( ent->client->ps.origin, 100, BA_H_REPEATER) )
    powerAvailable = qtrue;

  // buying ammo (also buys jetpack charge and cloak charge automatically)
  if( upgrade == UP_AMMO )
  {
    energyWeapon = BG_Weapon( ent->client->ps.weapon )->usesEnergy;
    if( energyWeapon && ( armAvailable || powerAvailable ) )
      G_GiveClientMaxAmmo( ent, qtrue );
    else if( !energyWeapon && armAvailable )
      G_GiveClientMaxAmmo( ent, qfalse );
    else
    {
      if( !energyWeapon )
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOARMOURYHERE );
      else
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOENERGYAMMOHERE );
    }
    if( armAvailable || powerAvailable )
    {
      if( BG_InventoryContainsUpgrade( UP_JETPACK,ent->client->ps.stats ) )
        ent->client->ps.stats[ STAT_JPCHARGE ] = JETPACK_CHARGE_CAPACITY;
      if( BG_InventoryContainsUpgrade( UP_CLOAK, ent->client->ps.stats ) )
      {
        ent->client->cloakReady = qtrue;
        ent->client->ps.eFlags &= ~EF_MOVER_STOP;
        BG_DeactivateUpgrade( UP_CLOAK, ent->client->ps.stats );
        ent->client->ps.stats[ STAT_CLOAK ] = CLOAK_TIME;
      }
    }
    return;
  }

  if( weapon != WP_NONE )
  {
    // check if we're trying to buy what we already have
    if( BG_InventoryContainsWeapon( weapon, ent->client->ps.stats ) )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_ITEMHELD );
      return;
    }

    // don't allow humans to buy aliens
    if( BG_Weapon( weapon )->team != TEAM_HUMANS && !override )
    {
      trap_SendServerCommand(ent-g_entities,"print \"You can't buy alien items\n\"");
      return;
    }

    // determine if it can be bought
    if( !BG_Weapon( weapon )->purchasable && !override )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't buy this item\n\"" );
      return;
    }

    // determine if buying it is allowed at this stage
    if( !BG_WeaponAllowedInStage( weapon, g_humanStage.integer ) && !override )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't buy this item yet\n\"" );
      return;
    }

    // determine if buying it is allowed    
    if( !BG_WeaponIsAllowed( weapon ) && !override )
    {
      trap_SendServerCommand( ent-g_entities, "print \"Purchasing of this item has been disabled\n\"" );
      return;
    }

    // determine if it costs more than we have
    if( BG_Weapon( weapon )->price > (short)ent->client->ps.persistant[ PERS_CREDIT ] && !override )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOFUNDS );
      return;
    }

    // determine if we have room to carry it
    if( BG_Weapon( weapon )->slots & BG_CalculateSlotsForInventory( ent->client->ps.stats ) && !override )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOSLOTS );
      return;
    }

    // can't buy something new while building (or charging xael/luci)
    if( !BG_PlayerCanChangeWeapon( &ent->client->ps ) && !override )
      return;
    
    // check to see if we are near an arm
    if( !armAvailable && !override )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOARMOURYHERE );
      return;
    }

    ent->client->ps.stats[ STAT_WEAPON ] = weapon;
    ent->client->ps.ammo = BG_Weapon( weapon )->maxAmmo;
    ent->client->ps.clips = BG_Weapon( weapon )->maxClips;

    if( BG_Weapon( weapon )->usesEnergy )
    {
      if( BG_InventoryContainsUpgrade( UP_BATTPACK, ent->client->ps.stats ) || BG_InventoryContainsUpgrade( UP_BATTLESUIT, ent->client->ps.stats ) )
      {
        ent->client->ps.ammo *= BATTPACK_MODIFIER;
      }
    }
    else if( !BG_Weapon( weapon )->usesEnergy )
    {
      if( BG_InventoryContainsUpgrade( UP_AMMOPACK, ent->client->ps.stats ) || BG_InventoryContainsUpgrade( UP_BATTLESUIT, ent->client->ps.stats ) )
      {
        ent->client->ps.clips = (int)(1 + (float)(ent->client->ps.clips) * AMMOPACK_MODIFIER );
      }
    }

    // finished with all the checks: change weapon, reset STAT_MISC, subtract cost of item from credits
    G_ForceWeaponChange( ent, weapon );
    ent->client->ps.stats[STAT_MISC] = 0;
    G_AddCreditToClient( ent->client,-(short)BG_Weapon( weapon )->price, qfalse);
  }
  else if( upgrade != UP_NONE )
  {
    // check if we're trying to buy what we already have
    if( BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) && !override )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_ITEMHELD );
      return;
    }

    // determine if it costs more than we have
    if( BG_Upgrade( upgrade )->price > (short)ent->client->ps.persistant[ PERS_CREDIT ] && !override )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOFUNDS );
      return;
    }

    // determine if we have room to carry it
    if( BG_Upgrade( upgrade )->slots & BG_CalculateSlotsForInventory( ent->client->ps.stats ) && !override )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOSLOTS );
      return;
    }

    // determine if it can be bought
    if( !BG_Upgrade( upgrade )->purchasable && !override )
    {
      trap_SendServerCommand( ent-g_entities, va("print \"You can't buy this item\n\"" ) );
      return;
    }

    // determine if buying it is allowed at this stage
    if( !BG_UpgradeAllowedInStage( upgrade, g_humanStage.integer) && !override )
    {
      trap_SendServerCommand( ent-g_entities, va( "print \"You can't buy this item yet\n\"" ) );
      return;
    }
    
    // check to see if we are near an arm
    if( !armAvailable && !override )
    {
      G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOARMOURYHERE );
      return;
    }

    // determine if buying it is allowed
    if( !BG_UpgradeIsAllowed( upgrade ) && !override )
    {
      trap_SendServerCommand(ent-g_entities,"print \"Purchasing of this item has been disabled\n\"");
      return;
    }
    else
    {
      if( upgrade == UP_BATTLESUIT )
      {
        vec3_t newOrigin;

        if( !G_RoomForClassChange( ent,PCL_HUMAN_BSUIT, newOrigin ) )
        {
          G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOROOMBSUITON );
          return;
        }
        VectorCopy( newOrigin, ent->client->ps.origin );
        ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN_BSUIT;
        ent->client->pers.classSelection = PCL_HUMAN_BSUIT;
        ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
      }

      //add to inventory
      BG_AddUpgradeToInventory( upgrade, ent->client->ps.stats );
    }

    if( upgrade == UP_BATTLESUIT )
    {
      G_GiveClientMaxAmmo( ent, qtrue );
      G_GiveClientMaxAmmo( ent, qfalse );
    }
    else
    {
      if( upgrade == UP_BATTPACK )
        G_GiveClientMaxAmmo( ent, qtrue );

      if( upgrade == UP_AMMOPACK )
        G_GiveClientMaxAmmo( ent, qfalse );
      if( upgrade == UP_CLOAK )
      {
        ent->client->cloakReady = qtrue;
        ent->client->ps.eFlags &= ~EF_MOVER_STOP;
        ent->client->ps.stats[ STAT_CLOAK ] = CLOAK_TIME;
      }
      if( upgrade == UP_JETPACK )
        ent->client->ps.stats[ STAT_JPCHARGE ] = JETPACK_CHARGE_CAPACITY;
    }
      
    //subtract from funds
    G_AddCreditToClient( ent->client, -(short)BG_Upgrade( upgrade )->price, qfalse );
  }
  else
    G_TriggerMenu( ent->client->ps.clientNum, MN_H_UNKNOWNITEM );

  //update ClientInfo
  ClientUserinfoChanged( ent->client->ps.clientNum );

  //make sure it doesnt let them save all the time
  if( !ent->client->pers.teamSelection == TEAM_NONE && ent->client->pers.saved )
    ent->client->pers.saved = qfalse;
}


/*
=================
Cmd_Sell_f
=================
*/
void Cmd_Sell_f( gentity_t *ent )
{
  char      s[ MAX_TOKEN_CHARS ];
  int       i;
  int       weapon, upgrade;
  qboolean armAvailable = qfalse;
  qboolean override = ent->client->pers.override;
  
  trap_Argv( 1, s, sizeof( s ) );

  if( G_BuildableRange( ent->client->ps.origin, 100, BA_H_ARMOURY ) && !override )
  {
    int       entityList[ MAX_GENTITIES ];
    vec3_t    range = { 100, 100, 100 };
    vec3_t    mins, maxs;
    int       i, num;
    gentity_t *armEnt;
    
    VectorAdd( ent->client->ps.origin, range, maxs );
    VectorSubtract( ent->client->ps.origin, range, mins );
    
    num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
    
    for( i = 0; i < num; i++ )
    {
      armEnt = &g_entities[ entityList[ i ] ];

      //checked for range in the first clause, no need to do that again
      if( armEnt->s.eType != ET_BUILDABLE 
          || armEnt->s.modelindex != BA_H_ARMOURY )
      continue;
      
      if( G_Visible( ent, armEnt, CONTENTS_SOLID ) )
        armAvailable = qtrue;
      else
        armAvailable = qfalse;
        
      break;
    }
  }
  else if( override )
    armAvailable = qtrue;
  else
    armAvailable = qfalse;

  //no armoury nearby
  if( !armAvailable )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOARMOURYHERE );
    return;
  }

  weapon = BG_WeaponByName( s )->number;
  upgrade = BG_UpgradeByName( s )->number;

  if( !Q_stricmpn( s, "weapon", 6 ) )
    weapon = ent->client->ps.stats[ STAT_WEAPON ];

  if( weapon != WP_NONE )
  {
    weapon_t selected = BG_GetPlayerWeapon( &ent->client->ps );
  
    if( !BG_PlayerCanChangeWeapon( &ent->client->ps )  &&
        !ent->client->pers.override )
      return;
  
    //are we /allowed/ to sell this?
    if( !BG_Weapon( weapon )->purchasable  &&
        !ent->client->pers.override ) 
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't sell this weapon\n\"" );
      return;
    }

    //remove weapon if carried
    if( BG_InventoryContainsWeapon( weapon, ent->client->ps.stats ) )
    {
      //guard against selling the HBUILD weapons exploit
      if( weapon == WP_HBUILD && ent->client->ps.stats[ STAT_MISC ] > 0 &&
          !ent->client->pers.override )
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_ARMOURYBUILDTIMER );
        return;
      }

      ent->client->ps.stats[ STAT_WEAPON ] = WP_NONE;

      //add to funds --- Causes issues with overflow and 2000 credits, doing it manually
      G_AddCreditToClient( ent->client, (short)BG_Weapon( weapon )->price, qfalse );
      //ent->client->ps.persistant[ PERS_CREDIT ] += (short)BG_Weapon( weapon )->price;
    }

    //if we have this weapon selected, force a new selection
    if( weapon == selected )
      G_ForceWeaponChange( ent, WP_NONE );
  }
  else if( upgrade != UP_NONE )
  {
    //are we /allowed/ to sell this?
    if( !BG_Upgrade( upgrade )->purchasable &&
        !ent->client->pers.override )
    {
      trap_SendServerCommand( ent-g_entities, "print \"You can't sell this item\n\"" );
      return;
    }
    //remove upgrade if carried
    if( BG_InventoryContainsUpgrade( upgrade, ent->client->ps.stats ) )
    {
      // shouldn't really need to test for this, but just to be safe
      if( upgrade == UP_BATTLESUIT )
      {
        vec3_t newOrigin;

        if( !G_RoomForClassChange( ent, PCL_HUMAN, newOrigin ) )
        {
          G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOROOMBSUITOFF );
          return;
        }
        VectorCopy( newOrigin, ent->client->ps.origin );
        ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN;
        ent->client->pers.classSelection = PCL_HUMAN;
        ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
      }

      //add to inventory
      BG_RemoveUpgradeFromInventory( upgrade, ent->client->ps.stats );

      if( upgrade == UP_BATTPACK )
        G_GiveClientMaxAmmo( ent, qtrue );
      if( upgrade == UP_AMMOPACK )
        G_GiveClientMaxAmmo( ent, qfalse );
      if( upgrade == UP_BATTLESUIT )
      {
        G_GiveClientMaxAmmo( ent, qtrue );
        G_GiveClientMaxAmmo( ent, qfalse );
      }
      if( upgrade == UP_CLOAK )
      {
        ent->client->cloakReady = qfalse;
        ent->client->ps.eFlags &= ~EF_MOVER_STOP;
        ent->client->ps.stats[ STAT_CLOAK ] = 0;
      }
      //add to funds
      G_AddCreditToClient( ent->client, (short)BG_Upgrade( upgrade )->price, qfalse );
    }
  }
  else if( !Q_stricmp( s, "weapons" ) )
  {
    weapon_t selected = BG_GetPlayerWeapon( &ent->client->ps );

    if( !BG_PlayerCanChangeWeapon( &ent->client->ps ) &&
        !ent->client->pers.override )
      return;

    for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
    {
      //guard against selling the HBUILD weapons exploit
      if( i == WP_HBUILD && ent->client->ps.stats[ STAT_MISC ] > 0 &&
          !ent->client->pers.override )
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_ARMOURYBUILDTIMER );
        continue;
      }

      if( BG_InventoryContainsWeapon( i, ent->client->ps.stats ) &&
          BG_Weapon( i )->purchasable )
      {
        ent->client->ps.stats[ STAT_WEAPON ] = WP_NONE;

        //add to funds
        G_AddCreditToClient( ent->client, (short)BG_Weapon( i )->price, qfalse );
      }

      //if we have this weapon selected, force a new selection
      if( i == selected )
        G_ForceWeaponChange( ent, WP_NONE );
    }
  }
  else if( !Q_stricmp( s, "upgrades" ) )
  {
    for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
    {
      //remove upgrade if carried
      if( BG_InventoryContainsUpgrade( i, ent->client->ps.stats ) &&
          BG_Upgrade( i )->purchasable )
      {

        // shouldn't really need to test for this, but just to be safe
        if( i == UP_BATTLESUIT )
        {
          vec3_t newOrigin;

          if( !G_RoomForClassChange( ent, PCL_HUMAN, newOrigin ) )
          {
            G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOROOMBSUITOFF );
            continue;
          }
          VectorCopy( newOrigin, ent->client->ps.origin );
          ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN;
          ent->client->pers.classSelection = PCL_HUMAN;
          ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
        }

        BG_RemoveUpgradeFromInventory( i, ent->client->ps.stats );

        if( i == UP_BATTPACK || i == UP_AMMOPACK || i == UP_BATTLESUIT )
          G_GiveClientMaxAmmo( ent, qtrue );

        if( i == UP_CLOAK )
        {
          ent->client->cloakReady = qfalse;
          ent->client->ps.eFlags &= ~EF_MOVER_STOP;
        }

        //add to funds
        G_AddCreditToClient( ent->client, (short)BG_Upgrade( i )->price, qfalse );
      }
    }
  }
  else if( !Q_stricmp( s, "all" ) || !Q_stricmp( s, "everything" ) )
  {
    weapon_t selected = BG_GetPlayerWeapon( &ent->client->ps );

    if( !BG_PlayerCanChangeWeapon( &ent->client->ps ) &&
        !ent->client->pers.override )
      return;

    for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
    {
      //guard against selling the HBUILD weapons exploit
      if( i == WP_HBUILD && ent->client->ps.stats[ STAT_MISC ] > 0 &&
          !ent->client->pers.override )
      {
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_ARMOURYBUILDTIMER );
        continue;
      }

      if( BG_InventoryContainsWeapon( i, ent->client->ps.stats ) &&
          BG_Weapon( i )->purchasable )
      {
        ent->client->ps.stats[ STAT_WEAPON ] = WP_NONE;

        //add to funds
        G_AddCreditToClient( ent->client, (short)BG_Weapon( i )->price, qfalse );
      }

      //if we have this weapon selected, force a new selection
      if( i == selected )
        G_ForceWeaponChange( ent, WP_NONE );
    }
    for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
    {
      //remove upgrade if carried
      if( BG_InventoryContainsUpgrade( i, ent->client->ps.stats ) &&
          BG_Upgrade( i )->purchasable )
      {

        // shouldn't really need to test for this, but just to be safe
        if( i == UP_BATTLESUIT )
        {
          vec3_t newOrigin;

          if( !G_RoomForClassChange( ent, PCL_HUMAN, newOrigin ) )
          {
            G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOROOMBSUITOFF );
            continue;
          }
          VectorCopy( newOrigin, ent->client->ps.origin );
          ent->client->ps.stats[ STAT_CLASS ] = PCL_HUMAN;
          ent->client->pers.classSelection = PCL_HUMAN;
          ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
        }

        BG_RemoveUpgradeFromInventory( i, ent->client->ps.stats );

        if( i == UP_BATTPACK || i == UP_AMMOPACK || i == UP_BATTLESUIT )
          G_GiveClientMaxAmmo( ent, qtrue );

        if( i == UP_CLOAK )
        {
          ent->client->cloakReady = qfalse;
          ent->client->ps.eFlags &= ~EF_MOVER_STOP;
        }

        //add to funds
        G_AddCreditToClient( ent->client, (short)BG_Upgrade( i )->price, qfalse );
      }
    }
  }
  else
    G_TriggerMenu( ent->client->ps.clientNum, MN_H_UNKNOWNITEM );

  //update ClientInfo
  ClientUserinfoChanged( ent->client->ps.clientNum );
}


/*
=================
Cmd_Build_f
=================
*/
void Cmd_Build_f( gentity_t *ent )
{
  char          s[ MAX_TOKEN_CHARS ];
  buildable_t   buildable;
  float         dist;
  vec3_t        origin;
  team_t        team;

  if( ent->client->pers.paused )
  {
    trap_SendServerCommand( ent-g_entities,
      "print \"You may not build while paused\n\"" );
    return;
  }

  if( ent->client->pers.denyBuild )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_REVOKED );
    return;
  }

  if( ent->client->pers.teamSelection == level.surrenderTeam )
  {
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_SURRENDER );
    return;
  }

  trap_Argv( 1, s, sizeof( s ) );

  buildable = BG_BuildableByName( s )->number;

  if( level.suddenDeath )
  {
    //FIXME: Wire: need to implement these messages properly
    //G_TriggerMenu( ent->client->ps.clientNum, MN_B_SUDDENDEATH );
    if( g_suddenDeathMode.integer == SDMODE_SELECTIVE )
    {
      if( !BG_FindReplaceableTestForBuildable( buildable ) )
      {
        trap_SendServerCommand( ent-g_entities,
          "print \"This building type cannot be rebuilt during Sudden Death\n\"" );
        return;
      }
      if( G_BuildingExists( buildable ) )
      {
        trap_SendServerCommand( ent-g_entities,
          "print \"You can only rebuild one of each type of rebuildable building during Sudden Death.\n\"" );
        return;
      }
    }
    else if( g_suddenDeathMode.integer == SDMODE_NO_BUILD )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"Building is not allowed during Sudden Death\n\"" );
      return;
    }
  }

  team = ent->client->ps.stats[ STAT_TEAM ];

  if( buildable != BA_NONE &&
      ( ( 1 << ent->client->ps.weapon ) & BG_Buildable( buildable )->buildWeapon ) &&
      !( ent->client->ps.stats[ STAT_STATE ] & SS_HOVELING ) &&
      BG_BuildableIsAllowed( buildable ) &&
      ( ( team == TEAM_ALIENS && BG_BuildableAllowedInStage( buildable, g_alienStage.integer ) ) ||
        ( team == TEAM_HUMANS && BG_BuildableAllowedInStage( buildable, g_humanStage.integer ) ) ) )
  {
    dist = BG_Class( ent->client->ps.stats[ STAT_CLASS ] )->buildDist;

    //these are the errors displayed when the builder first selects something to use
    switch( G_CanBuild( ent, buildable, dist, origin ) )
    {
      // can place right away, set the blueprint and the valid togglebit
      case IBE_NONE:
      case IBE_TNODEWARN:
      case IBE_RPTNOREAC:
      case IBE_RPTPOWERHERE:
      case IBE_SPWNWARN:
        ent->client->ps.stats[ STAT_BUILDABLE ] = ( buildable | SB_VALID_TOGGLEBIT );
        break;

      // can't place yet but maybe soon: start with valid togglebit off
      case IBE_NORMAL:
      case IBE_HOVELEXIT:
      case IBE_NOCREEP:
      case IBE_NOROOM:
      case IBE_NOOVERMIND:
      case IBE_NOPOWERHERE:
        ent->client->ps.stats[ STAT_BUILDABLE ] = buildable;
        break;

      // more serious errors just pop a menu
      case IBE_NOALIENBP:
        if( g_suddenDeath.integer )
          ent->client->ps.stats[ STAT_BUILDABLE ] = ( buildable | SB_VALID_TOGGLEBIT );
        else
          G_TriggerMenu( ent->client->ps.clientNum, MN_A_NOBP );
        break;

      case IBE_ONEOVERMIND:
        G_TriggerMenu( ent->client->ps.clientNum, MN_A_ONEOVERMIND );
        break;

      case IBE_ONEHOVEL:
        G_TriggerMenu( ent->client->ps.clientNum, MN_A_ONEHOVEL );
        break;

      case IBE_ONEREACTOR:
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_ONEREACTOR );
        break;

      case IBE_NOHUMANBP:
        if( g_suddenDeath.integer )
          ent->client->ps.stats[ STAT_BUILDABLE ] = ( buildable | SB_VALID_TOGGLEBIT );
        else
          G_TriggerMenu( ent->client->ps.clientNum, MN_H_NOBP);
        break;

      case IBE_NODCC:
        G_TriggerMenu( ent->client->ps.clientNum, MN_H_NODCC );
        break;

      default:
        break;
    }
  }
  else
    G_TriggerMenu( ent->client->ps.clientNum, MN_B_CANNOT );
}

/*
=================
Cmd_Protect_f
=================
*/
void Cmd_Protect_f( gentity_t *ent )
{
  vec3_t      forward, end;
  trace_t     tr;
  gentity_t   *traceEnt;

  if( !ent->client->pers.designatedBuilder )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Only designated"
        " builders can toggle structure protection.\n\"" );
    return;
  }

  AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
  VectorMA( ent->client->ps.origin, 100, forward, end );

  trap_Trace( &tr, ent->client->ps.origin, NULL, NULL, end, ent->s.number,
    MASK_PLAYERSOLID );
  traceEnt = &g_entities[ tr.entityNum ];

  if( tr.fraction < 1.0f && ( traceEnt->s.eType == ET_BUILDABLE ) &&
      ( traceEnt->buildableTeam == ent->client->pers.teamSelection ) )
  {
    if( traceEnt->s.eFlags & EF_DBUILDER )
    {
      trap_SendServerCommand( ent-g_entities,
        "print \"Structure protection removed\n\"" );
      traceEnt->s.eFlags &= ~EF_DBUILDER;
    }
    else
    {
      trap_SendServerCommand( ent-g_entities, 
        "print \"Structure protection applied\n\"" );
      traceEnt->s.eFlags |= EF_DBUILDER;
    }
  }
}

 /*
 =================
 Cmd_Resign_f
 =================
 */
 void Cmd_Resign_f( gentity_t *ent )
 {
   if( !ent->client->pers.designatedBuilder )
   {
     trap_SendServerCommand( ent-g_entities,
       "print \"You are not a designated builder\n\"" );
     return;
   }
 
   ent->client->pers.designatedBuilder = qfalse;
   trap_SendServerCommand( -1, va(
     "print \"%s" S_COLOR_WHITE " has resigned\n\"",
     ent->client->pers.netname ) );
   G_CheckDBProtection( );
 }
 
/*
=================
Cmd_Reload_f
=================
*/
void Cmd_Reload_f( gentity_t *ent )
{
  playerState_t *ps = &ent->client->ps;
  int ammo;

  // weapon doesn't ever need reloading
  if( BG_Weapon( ps->weapon )->infiniteAmmo )
    return;

  if( ps->clips <= 0 )
    return;

  if( BG_Weapon( ps->weapon )->usesEnergy &&
      BG_InventoryContainsUpgrade( UP_BATTPACK, ps->stats ) )
    ammo = BG_Weapon( ps->weapon )->maxAmmo * BATTPACK_MODIFIER;
  else
    ammo = BG_Weapon( ps->weapon )->maxAmmo;

  // don't reload when full
  if( ps->ammo >= ammo )
    return;
  
  // the animation, ammo refilling etc. is handled by PM_Weapon
  if( ent->client->ps.weaponstate != WEAPON_RELOADING )
    ent->client->ps.pm_flags |= PMF_WEAPON_RELOAD;
}

/*
=================
Cmd_MyStats_f
=================
*/
void Cmd_MyStats_f( gentity_t *ent )
{

   if(!ent) return;


   if( !level.intermissiontime && ent->client->pers.statscounters.timeLastViewed && (level.time - ent->client->pers.statscounters.timeLastViewed) <60000 ) 
   {   
     ADMP( "You may only check your stats once per minute and during intermission.\n");
     return;
   }
   
   if( !g_myStats.integer )
   {
    ADMP( "myStats has been disabled\n");
    return;
   }
   
   ADMP( G_statsString( &ent->client->pers.statscounters, &ent->client->pers.teamSelection ) );
   ent->client->pers.statscounters.timeLastViewed = level.time;
  
  return;
}

char *G_statsString( statsCounters_t *sc, team_t *pt )
{
  char *s;

  int percentNearBase=0;
  int percentJetpackWallwalk=0;
  int percentHeadshots=0;
  double avgTimeAlive=0;
  int avgTimeAliveMins = 0;
  int avgTimeAliveSecs = 0;

  if( sc->timealive )
   percentNearBase = (int)(100 *  (float) sc->timeinbase / ((float) (sc->timealive ) ) );

  if( sc->timealive && sc->deaths )
  {
    avgTimeAlive = sc->timealive / sc->deaths;
  }

  avgTimeAliveMins = (int) (avgTimeAlive / 60.0f);
  avgTimeAliveSecs = (int) (avgTimeAlive - (60.0f * avgTimeAliveMins));

  if( *pt == TEAM_ALIENS )
  {
    if( sc->dretchbasytime > 0 )
     percentJetpackWallwalk = (int)(100 *  (float) sc->jetpackusewallwalkusetime / ((float) ( sc->dretchbasytime) ) );

    if( sc->hitslocational )
      percentHeadshots = (int)(100 * (float) sc->headshots / ((float) (sc->hitslocational) ) );

    s = va( "^3Kills:^7 %3i ^3StructKills:^7 %3i ^3Assists:^7 %3i^7 ^3Poisons:^7 %3i ^3Headshots:^7 %3i (%3i)\n^3Deaths:^7 %3i ^3Feeds:^7 %3i ^3Suicides:^7 %3i ^3TKs:^7 %3i ^3Avg Lifespan:^7 %4d:%02d\n^3Damage to:^7 ^3Enemies:^7 %5i ^3Structs:^7 %5i ^3Friendlies:^7 %3i \n^3Structs Built:^7 %3i ^3Time Near Base:^7 %3i ^3Time wallwalking:^7 %3i\n",
     sc->kills,
     sc->structskilled,
     sc->assists,
     sc->repairspoisons,
     sc->headshots,
     percentHeadshots,
     sc->deaths,
     sc->feeds,
     sc->suicides,
     sc->teamkills,
     avgTimeAliveMins,
     avgTimeAliveSecs,
     sc->dmgdone,
     sc->structdmgdone,
     sc->ffdmgdone,
     sc->structsbuilt,
     percentNearBase,
     percentJetpackWallwalk
         );
  }
  else if( *pt == TEAM_HUMANS )
  {
    if( sc->timealive )
     percentJetpackWallwalk = (int)(100 *  (float) sc->jetpackusewallwalkusetime / ((float) ( sc->timealive ) ) );
    s = va( "^3Kills:^7 %3i ^3StructKills:^7 %3i ^3Assists:^7 %3i \n^3Deaths:^7 %3i ^3Feeds:^7 %3i ^3Suicides:^7 %3i ^3TKs:^7 %3i ^3Avg Lifespan:^7 %4d:%02d\n^3Damage to:^7 ^3Enemies:^7 %5i ^3Structs:^7 %5i ^3Friendlies:^7 %3i \n^3Structs Built:^7 %3i ^3Repairs:^7 %4i ^3Time Near Base:^7 %3i ^3Time Jetpacking:^7 %3i\n",
     sc->kills,
     sc->structskilled,
     sc->assists,
     sc->deaths,
     sc->feeds,
     sc->suicides,
     sc->teamkills,
     avgTimeAliveMins,
     avgTimeAliveSecs,
     sc->dmgdone,
     sc->structdmgdone,
     sc->ffdmgdone,
     sc->structsbuilt,
     sc->repairspoisons,
     percentNearBase,
     percentJetpackWallwalk
         );
  }
  else s="No stats available\n";

  return s;
}

/*
=================
G_StopFromFollowing

stops any other clients from following this one
called when a player leaves a team or dies
=================
*/
void G_StopFromFollowing( gentity_t *ent )
{
  int i;

  for( i = 0; i < level.maxclients; i++ )
  {
    if( level.clients[ i ].sess.spectatorState != SPECTATOR_NOT &&
        level.clients[ i ].sess.spectatorState == SPECTATOR_FOLLOW &&
        level.clients[ i ].sess.spectatorClient == ent->client->ps.clientNum )
    {
      if( !G_FollowNewClient( &g_entities[ i ], 1 ) )
        G_StopFollowing( &g_entities[ i ] );
    }
  }
}

/*
=================
G_StopFollowing

If the client being followed leaves the game, or you just want to drop
to free floating spectator mode
=================
*/
void G_StopFollowing( gentity_t *ent )
{
  ent->client->ps.stats[ STAT_TEAM ] = ent->client->pers.teamSelection;

  if( ent->client->pers.teamSelection == TEAM_NONE )
  {
    ent->client->sess.spectatorState = 
      ent->client->ps.persistant[ PERS_SPECSTATE ] = SPECTATOR_FREE;
  }
  else
  {
    vec3_t spawn_origin, spawn_angles;

    ent->client->sess.spectatorState =
      ent->client->ps.persistant[ PERS_SPECSTATE ] = SPECTATOR_LOCKED;
    if( ent->client->pers.teamSelection == TEAM_ALIENS )
      G_SelectAlienLockSpawnPoint( spawn_origin, spawn_angles );
    else if( ent->client->pers.teamSelection == TEAM_HUMANS )
      G_SelectHumanLockSpawnPoint( spawn_origin, spawn_angles );
    G_SetOrigin( ent, spawn_origin );
    VectorCopy( spawn_origin, ent->client->ps.origin );
    G_SetClientViewAngle( ent, spawn_angles );
  }
  ent->client->sess.spectatorClient = -1;
  ent->client->ps.pm_flags &= ~PMF_FOLLOW;
  ent->client->ps.stats[ STAT_STATE ] &= ~SS_WALLCLIMBING;
  ent->client->ps.eFlags &= ~( EF_WALLCLIMB | EF_WALLCLIMBCEILING );
  ent->client->ps.stats[ STAT_VIEWLOCK ] = 0;
  ent->client->ps.viewangles[ PITCH ] = 0.0f;
  ent->client->ps.clientNum = ent - g_entities;
  
  CalculateRanks( );
}

/*
=================
G_FollowLockView

Client is still following a player, but that player has gone to spectator
mode and cannot be followed for the moment
=================
*/
void G_FollowLockView( gentity_t *ent )
{
  vec3_t spawn_origin, spawn_angles;
  int clientNum;
  
  clientNum = ent->client->sess.spectatorClient;
  ent->client->sess.spectatorState =
    ent->client->ps.persistant[ PERS_SPECSTATE ] = SPECTATOR_FOLLOW;
  ent->client->ps.clientNum = clientNum;
  ent->client->ps.pm_flags &= ~PMF_FOLLOW;
  ent->client->ps.stats[ STAT_TEAM ] = ent->client->pers.teamSelection;
  ent->client->ps.stats[ STAT_STATE ] &= ~SS_WALLCLIMBING;
  ent->client->ps.eFlags &= ~( EF_WALLCLIMB | EF_WALLCLIMBCEILING );
  ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
  ent->client->ps.stats[ STAT_VIEWLOCK ] = 0;
  ent->client->ps.viewangles[ PITCH ] = 0.0f;

  // Put the view at the team spectator lock position
  if( level.clients[ clientNum ].pers.teamSelection == TEAM_ALIENS )
    G_SelectAlienLockSpawnPoint( spawn_origin, spawn_angles );
  else if( level.clients[ clientNum ].pers.teamSelection == TEAM_HUMANS )
    G_SelectHumanLockSpawnPoint( spawn_origin, spawn_angles );
  G_SetOrigin( ent, spawn_origin );
  VectorCopy( spawn_origin, ent->client->ps.origin );
  G_SetClientViewAngle( ent, spawn_angles );
}

/*
=================
G_FollowNewClient

This was a really nice, elegant function. Then I fucked it up.
=================
*/
qboolean G_FollowNewClient( gentity_t *ent, int dir )
{
  int       clientnum = ent->client->sess.spectatorClient;
  int       original = clientnum;
  qboolean  selectAny = qfalse;

  if( dir > 1 )
    dir = 1;
  else if( dir < -1 )
    dir = -1;
  else if( dir == 0 )
    return qtrue;

  if( ent->client->sess.spectatorState == SPECTATOR_NOT )
    return qfalse;

  // select any if no target exists
  if( clientnum < 0 || clientnum >= level.maxclients )
  {
    clientnum = original = 0;
    selectAny = qtrue;
  }

  do
  {
    clientnum += dir;

    if( clientnum >= level.maxclients )
      clientnum = 0;

    if( clientnum < 0 )
      clientnum = level.maxclients - 1;

    // can't follow self
    if( &g_entities[ clientnum ] == ent )
      continue;

    // avoid selecting existing follow target
    if( clientnum == original && !selectAny )
      continue; //effectively break;

    // can only follow connected clients
    if( level.clients[ clientnum ].pers.connected != CON_CONNECTED &&
        !level.clients[ clientnum ].pers.demoClient )
      continue;

    // can't follow a spectator
    if( level.clients[ clientnum ].pers.teamSelection == TEAM_NONE &&
        !level.clients[ clientnum ].pers.demoClient )
      continue;
    
    // if stickyspec is disabled, can't follow someone in queue either
    if( !ent->client->pers.stickySpec &&
        level.clients[ clientnum ].sess.spectatorState != SPECTATOR_NOT )
      continue;
      
    // can't follow someone who is following someone else
    if( level.clients[ clientnum ].sess.spectatorState == SPECTATOR_FOLLOW )
      continue;
      
    // can only follow teammates when dead and on a team
    if( ent->client->pers.teamSelection != TEAM_NONE && 
        ( level.clients[ clientnum ].pers.teamSelection != 
          ent->client->pers.teamSelection ) )
      continue;

    // this is good, we can use it
    ent->client->sess.spectatorClient = clientnum;
    ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
    
    // if this client is in the spawn queue, we need to do something special
    if( level.clients[ clientnum ].sess.spectatorState != SPECTATOR_NOT )
      G_FollowLockView( ent );
    
    return qtrue;

  } while( clientnum != original );

  return qfalse;
}

/*
=================
G_ToggleFollow
=================
*/
void G_ToggleFollow( gentity_t *ent )
{
  if( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
    G_StopFollowing( ent );
  else
    G_FollowNewClient( ent, 1 );
}

/*
=================
Cmd_Follow_f
=================
*/
void Cmd_Follow_f( gentity_t *ent )
{
  int   i;
  int   pids[ MAX_CLIENTS ];
  char  arg[ MAX_NAME_LENGTH ];

  if( trap_Argc( ) != 2 )
  {
    G_ToggleFollow( ent );
  }
  else 
  {
    trap_Argv( 1, arg, sizeof( arg ) );
    if( G_ClientNumbersFromString( arg, pids, MAX_CLIENTS ) == 1 )
    {
      i = pids[ 0 ];
    }
    else
    {
      i = G_ClientNumberFromString( arg );

      if( i == -1 )
      {
        trap_SendServerCommand( ent - g_entities,
          "print \"follow: invalid player\n\"" );
        return;
      }
    }

    // can't follow self
    if( &level.clients[ i ] == ent->client )
      return;

    // can't follow another spectator if sticky spec is off
    if( !ent->client->pers.stickySpec &&
        level.clients[ i ].sess.spectatorState != SPECTATOR_NOT )
      return;

    // can only follow teammates when dead and on a team
    if( ent->client->pers.teamSelection != TEAM_NONE && 
        ( level.clients[ i ].pers.teamSelection != 
          ent->client->pers.teamSelection ) )
      return;

    ent->client->sess.spectatorState = SPECTATOR_FOLLOW;
    ent->client->sess.spectatorClient = i;
  }
}

/*
=================
Cmd_FollowCycle_f
=================
*/
void Cmd_FollowCycle_f( gentity_t *ent )
{
  char args[ 11 ];
  int  dir = 1;

  trap_Argv( 0, args, sizeof( args ) );
  if( Q_stricmp( args, "followprev" ) == 0 )
    dir = -1;

  // won't work unless spectating
  if( ent->client->sess.spectatorState == SPECTATOR_NOT )
    return;

  G_FollowNewClient( ent, dir );
}

/*
=================
Cmd_PTRCVerify_f

Check a PTR code is valid
=================
*/
void Cmd_PTRCVerify_f( gentity_t *ent )
{
  connectionRecord_t  *connection;
  char                s[ MAX_TOKEN_CHARS ] = { 0 };
  int                 code;

  if( ent->client->pers.connection )
    return;

  trap_Argv( 1, s, sizeof( s ) );

  if( !s[ 0 ] )
    return;

  code = atoi( s );

  connection = G_FindConnectionForCode( code );
  if( connection )
  {
    // valid code
    if( connection->clientTeam != TEAM_NONE )
      trap_SendServerCommand( ent->client->ps.clientNum, "ptrcconfirm" );

    // restore mapping
    ent->client->pers.connection = connection;
    connection->clientNum = ent->client->ps.clientNum;
  }
  else
  {
    // invalid code -- generate a new one
    connection = G_GenerateNewConnection( ent->client );

    if( connection )
    {
      trap_SendServerCommand( ent->client->ps.clientNum,
        va( "ptrcissue %d", connection->ptrCode ) );
    }
  }
}

/*
=================
Cmd_PTRCRestore_f

Restore against a PTR code
=================
*/
void Cmd_PTRCRestore_f( gentity_t *ent )
{
  char                s[ MAX_TOKEN_CHARS ] = { 0 };
  int                 code;
  connectionRecord_t  *connection;

  if( ent->client->pers.joinedATeam )
  {
    trap_SendServerCommand( ent - g_entities,
      "print \"You cannot use a PTR code after joining a team\n\"" );
    return;
  }

  trap_Argv( 1, s, sizeof( s ) );

  if( !s[ 0 ] )
    return;

  code = atoi( s );

  connection = ent->client->pers.connection;
  if( connection && connection->ptrCode == code )
  {
    // set the correct team
    G_ChangeTeam( ent, connection->clientTeam );

    // set the correct credit
    ent->client->ps.persistant[ PERS_CREDIT ] = 0;
    G_AddCreditToClient( ent->client, connection->clientCredit, qtrue );
    if ( connection->oldClient != ent - g_entities )
        G_AddCreditToClient( &level.clients[ connection->oldClient ], -connection->clientCredit, qtrue );
    connection->oldClient = ent - g_entities;
  }
  else
  {
    trap_SendServerCommand( ent - g_entities,
      va( "print \"'%d' is not a valid PTR code\n\"", code ) );
  }
}

static void Cmd_Ignore_f( gentity_t *ent )
{
  int pids[ MAX_CLIENTS ];
  char name[ MAX_NAME_LENGTH ];
  char cmd[ 9 ];
  int matches = 0;
  int i;
  qboolean ignore = qfalse;

  trap_Argv( 0, cmd, sizeof( cmd ) );
  if( Q_stricmp( cmd, "ignore" ) == 0 )
    ignore = qtrue;

  if( trap_Argc() < 2 )
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
      "usage: %s [clientNum | partial name match]\n\"", cmd ) );
    return;
  }

  Q_strncpyz( name, ConcatArgs( 1 ), sizeof( name ) );
  matches = G_ClientNumbersFromString( name, pids, MAX_CLIENTS );
  if( matches < 1 )
  {
    trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
      "%s: no clients match the name '%s'\n\"", cmd, name ) );
    return;
  }

  for( i = 0; i < matches; i++ )
  {
    if( ignore )
    {
      if( !BG_ClientListTest( &ent->client->sess.ignoreList, pids[ i ] ) )
      {
        BG_ClientListAdd( &ent->client->sess.ignoreList, pids[ i ] );
        ClientUserinfoChanged( ent->client->ps.clientNum );
        trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
          "ignore: added %s^7 to your ignore list\n\"",
          level.clients[ pids[ i ] ].pers.netname ) );
      }
      else
      {
        trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
          "ignore: %s^7 is already on your ignore list\n\"",
          level.clients[ pids[ i ] ].pers.netname ) );
      }
    }
    else
    {
      if( BG_ClientListTest( &ent->client->sess.ignoreList, pids[ i ] ) )
      {
        BG_ClientListRemove( &ent->client->sess.ignoreList, pids[ i ] );
        ClientUserinfoChanged( ent->client->ps.clientNum );
        trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
          "unignore: removed %s^7 from your ignore list\n\"",
          level.clients[ pids[ i ] ].pers.netname ) );
      }
      else
      {
        trap_SendServerCommand( ent-g_entities, va( "print \"[skipnotify]"
          "unignore: %s^7 is not on your ignore list\n\"",
          level.clients[ pids[ i ] ].pers.netname ) );
      }
    }
  }
}

/*
=================
Cmd_Share_f
=================
*/
void Cmd_Share_f( gentity_t *ent )
{
  int   i, clientNum = 0, creds = 0, skipargs = 0;
  int   clientNums[ MAX_CLIENTS ] = { -1 };
  int   playerCredit = 0, targetCredit = 0;
  char  cmd[ 12 ];
  char  arg1[ MAX_STRING_TOKENS ];
  char  arg2[ MAX_STRING_TOKENS ];
  team_t team;
  
  if( !ent || !ent->client || ( ent->client->pers.teamSelection == TEAM_NONE ) )
  {
    return;
  }
  
  if( !g_allowShare.integer )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Share has been disabled.\n\"" );
    return;
  }
  
  if( g_floodMinTime.integer )
    if ( G_FloodLimited( ent ) )
    {
      trap_SendServerCommand( ent-g_entities, "print \"Your shares are flood-limited; wait before shareing again\n\"" );
      return;
    }

  team = ent->client->pers.teamSelection;

  G_SayArgv( 0, cmd, sizeof( cmd ) );
  if( !Q_stricmp( cmd, "say" ) || !Q_stricmp( cmd, "say_team" ) )
  {
    skipargs = 1;
    G_SayArgv( 1, cmd, sizeof( cmd ) );
  }

  // target player name is in arg1
  G_SayArgv( 1+skipargs, arg1, sizeof( arg1 ) );
  // amount to be shared is in arg2
  G_SayArgv( 2+skipargs, arg2, sizeof( arg2 ) );

  if( arg1[0] && !strchr( arg1, ';' ) && Q_stricmp( arg1, "target_in_aim" ) )
  {
    //check arg1 is a number
    for( i = 0; arg1[ i ]; i++ )
    {
      if( arg1[ i ] < '0' || arg1[ i ] > '9' )
      {
        clientNum = -1;
        break;
      }
    }

    if( clientNum >= 0 )
    {
      clientNum = atoi( arg1 );
    }
    else if( G_ClientNumbersFromString( arg1, clientNums, MAX_CLIENTS ) == 1 )
    {
      // there was one partial name match
      clientNum = clientNums[ 0 ]; 
    }
    else
    {
      // look for an exact name match before bailing out
      clientNum = G_ClientNumberFromString( arg1 );
      if( clientNum == -1 )
      {
        trap_SendServerCommand( ent-g_entities,
          "print \"share: invalid player name specified.\n\"" );
        return;
      }
    }
  }
  else // arg1 not set
  {
    vec3_t      forward, end;
    trace_t     tr;
    gentity_t   *traceEnt;

    
    // trace a teammate
    AngleVectors( ent->client->ps.viewangles, forward, NULL, NULL );
    VectorMA( ent->client->ps.origin, 8192 * 16, forward, end );

    trap_Trace( &tr, ent->client->ps.origin, NULL, NULL, end, ent->s.number, MASK_PLAYERSOLID );
    traceEnt = &g_entities[ tr.entityNum ];

    if( tr.fraction < 1.0f && traceEnt->client &&
      ( traceEnt->client->pers.teamSelection == team ) )
      clientNum = traceEnt - g_entities;
    else
    {
      trap_SendServerCommand( ent-g_entities,
        va( "print \"share: aim at a teammate to share %s.\n\"",
        ( team == TEAM_HUMANS ) ? "credits" : "evolvepoints" ) );
      return;
    }
  }

  // verify target player team
  if( ( clientNum < 0 ) || ( clientNum >= level.maxclients ) ||
      ( level.clients[ clientNum ].pers.teamSelection != team ) )
  {
    trap_SendServerCommand( ent-g_entities,
      "print \"share: not a valid player of your team.\n\"" );
    return;
  }

  if( !arg2[0] || strchr( arg2, ';' ) )
  {
    // default credit count
    if( team == TEAM_HUMANS )
      creds = FREEKILL_HUMAN;
    else if( team == TEAM_ALIENS )
      creds = FREEKILL_ALIEN;
  }
  else
  {
    //check arg2 is a number
    for( i = 0; arg2[ i ]; i++ )
    {
      if( arg2[ i ] < '0' || arg2[ i ] > '9' )
      {
        trap_SendServerCommand( ent-g_entities,
          "print \"usage: share [name|slot#] [amount]\n\"" );
        return;
      }
    }

    // credit count from parameter
    creds = atoi( arg2 );
  }

  // player specified "0" to transfer
  if( creds <= 0 )
  {
    trap_SendServerCommand( ent-g_entities,
      "print \"Ooh, you are a generous one, indeed!\n\"" );
    return;
  }

  if( ent->client->pers.teamSelection == TEAM_NONE )
    playerCredit = ent->client->pers.savedCredit;
  else
    playerCredit = ent->client->ps.persistant[ PERS_CREDIT ];
  if( level.clients[ clientNum ].pers.teamSelection == TEAM_NONE )
    targetCredit = level.clients[ clientNum ].pers.savedCredit;
  else
    targetCredit = level.clients[ clientNum ].ps.persistant[ PERS_CREDIT ];

  // convert alien share amount
  if( team == TEAM_ALIENS )
    creds = ALIEN_CREDITS_PER_FRAG*creds;

  // transfer only credits the player really has
  if( creds > playerCredit )
  {
    creds = playerCredit;
  }

  // player has no credits
  if( creds <= 0 )
  {
    trap_SendServerCommand( ent-g_entities,
      "print \"Earn some first, lazy gal!\n\"" );
    return;
  }
  
  // allow transfers only up to the credit/evo limit
  if( ( team == TEAM_HUMANS ) && 
      ( creds > HUMAN_MAX_CREDITS - targetCredit ) )
    creds = HUMAN_MAX_CREDITS - targetCredit;
  else if( ( team == TEAM_ALIENS ) && 
      ( creds > ALIEN_MAX_CREDITS - targetCredit ) )
    creds = ALIEN_MAX_CREDITS - targetCredit;

  // target cannot take any more credits
  if( creds <= 0 )
  {
    trap_SendServerCommand( ent-g_entities,
      va( "print \"share: player cannot receive any more %s.\n\"",
        ( team == TEAM_HUMANS ) ? "credits" : "evolvepoints" ) );
    return;
  }

  // transfer credits
  G_AddCreditToClient( ent->client, -creds, qfalse );
  G_AddCreditToClient( &(level.clients[ clientNum ]), creds, qtrue );

  // convert alien share amount
  if( team == TEAM_ALIENS )
    creds = creds/ALIEN_CREDITS_PER_FRAG;

  // tell the players involved
  trap_SendServerCommand( ent-g_entities,
    va( "print \"share: transferred %d %s to %s^7.\n\"", creds,
      ( team == TEAM_HUMANS ) ? "credits" : "evolvepoints",
      level.clients[ clientNum ].pers.netname ) );
  trap_SendServerCommand( clientNum,
    va( "print \"You have received %d %s from %s^7.\n\"", creds,
      ( team == TEAM_HUMANS ) ? "credits" : "evolvepoints",
      ent->client->pers.netname ) );

  G_LogPrintf( "Share: %i %i %i %d: %s^7 transferred %d%s to %s^7\n",
    ent->client->ps.clientNum,
    clientNum,
    team,
    creds,
    ent->client->pers.netname,
    creds,
    ( team == TEAM_HUMANS ) ? "c" : "e",
    level.clients[ clientNum ].pers.netname );
}

#define MAX_DONATE_LOOPS 15

/*
=================
Cmd_Donate_f

Alms for the poor
=================
*/
void Cmd_Donate_f( gentity_t *ent ) 
{
  char s[ MAX_TOKEN_CHARS ] = "", *type = "evo(s)";
  int i, value, divisor, portion, total=0, pwm=0,
    max, minportion=1, times;
  qboolean donated = qfalse;
  gentity_t *thisclient;

  //annoying warning prevention
  value = 0;
  times = 0;
  divisor = 0;
  
  if( !ent->client )
    return;

  if( !g_allowShare.integer )
  {
    trap_SendServerCommand( ent-g_entities, "print \"Donate has been disabled.\n\"" );
    return;
  }
  
  if( level.extremeSuddenDeath ) 
  {
    trap_SendServerCommand( ent-g_entities,
    "print \"Donate is disabled during ESD.\n\"" );
    return;
  }
  
  if( g_floodMinTime.integer )
    if ( G_FloodLimited( ent ) )
    {
      trap_SendServerCommand( ent-g_entities, "print \"Your donations are flood-limited; wait before donating again\n\"" );
      return;
    }

  trap_Argv( 1, s, sizeof( s ) );
  value = atoi(s);
  if( value <= 0 ) 
  {
    trap_SendServerCommand( ent-g_entities,
      "print \"donate: very funny\n\"" );
    return;
  }
  
  if( ent->client->pers.teamSelection == TEAM_ALIENS )
    value = value * ALIEN_CREDITS_PER_FRAG;

  if( value > ent->client->ps.persistant[ PERS_CREDIT ] )
    value = ent->client->ps.persistant[ PERS_CREDIT ];

  if( ent->client->pers.teamSelection == TEAM_ALIENS )
  {
    minportion = ALIEN_CREDITS_PER_FRAG;
    divisor = level.numAlienClients-1;
    max = ALIEN_MAX_CREDITS;
    if( value > divisor )
      portion = value/divisor;
    else
      portion = value;
  }
  else if( ent->client->pers.teamSelection == TEAM_HUMANS ) 
  {
    divisor = level.numHumanClients-1;
    max = HUMAN_MAX_CREDITS;
    type = "credit(s)";
    if( value > divisor )
      portion = value/divisor;
    else
      portion = value;
  } 
  else 
  {
    trap_SendServerCommand( ent-g_entities,
      va( "print \"donate: spectators cannot be so gracious\n\"" ) );
    return;
  }
  
  if( divisor < 1 ) 
  {
    trap_SendServerCommand( ent-g_entities,
      "print \"donate: get yourself some teammates first\n\"" );
    return;
  }

  total = value;
  
  while( !donated && value > 0 )
  {
    //loops so that if we have spares we keep donating
    for( i = 0; i < level.maxclients; i++ )
    {
      int thisportion = portion;    //by default we try to donate what we calculated above
      thisclient = g_entities + i;
      
      //check if a client
      if( !thisclient->client )
        continue;
        
      //check team
      if( thisclient->client->pers.teamSelection != ent->client->pers.teamSelection )
        continue;
      
      //check if self
      if( thisclient->client == ent->client )
        continue;
      
      //check for max credits
      if( thisclient->client->ps.persistant[ PERS_CREDIT ] >= max )
      {
        pwm++;
        continue;
      }
      
      //if we're out break out of the loop
      if( value <= 0 )
      {
        donated = qtrue;
        break;
      }
      
      //Give them stuff until they pop
      if( portion + thisclient->client->ps.persistant[ PERS_CREDIT ] >= max )
      {
        thisportion = max - thisclient->client->ps.persistant[ PERS_CREDIT ];
        if( thisportion < minportion )
          continue;
      }
      else if( portion < value )
        thisportion = value;
      
      value -= thisportion;
      
      //give it out
      G_AddCreditToClient( thisclient->client, thisportion, qtrue );

      //spam them
      if( thisportion > 0 )
      {
        trap_SendServerCommand( i,
          va( "print \"%s^7 donated %d %s to you, don't forget to say 'thank you'!\n\"",
          ent->client->pers.netname, (thisportion/minportion), type ) );
      }
    }
    //prevents issues with infinite loops
    if( pwm == divisor )
      break;
    else if( times >= MAX_DONATE_LOOPS )
      break;
  
    pwm = 0;
    times++;
  }
  
  //take away what we've used minus any left over
  G_AddCreditToClient( ent->client, -(total-value), qtrue );
  
  //tell the donator
  trap_SendServerCommand( ent-g_entities,
    va( "print \"Donated %d %s to the cause.\n\"",
    ((total-value)/minportion), type ) );
}

/*
=================
Cmd_Test_f

Hopefully will be able to catch that bizarro turret bug with this one
=================
*/
static void Cmd_Test_f( gentity_t *ent )
{
  trap_SendServerCommand( ent - g_entities, va( "print \""
    "  pointcontents = %x\n  r.contents = %x\n  targeted = %d\n\"", 
    trap_PointContents( ent->s.origin, ent - g_entities ),
    ent->r.contents, ( ent->targeted ) ? (int)(ent->targeted - g_entities) : 0 ) );
}

/*
=================
Cmd_Damage_f

Deals damage to you (for testing), arguments: [damage] [dx] [dy] [dz]
The dx/dy arguments describe the damage point's offset from the entity origin
=================
*/
void Cmd_Damage_f( gentity_t *ent )
{
  vec3_t point;
  char arg[ 16 ];
  float dx = 0.f, dy = 0.f, dz = 100.f;
  int damage = 100;
  qboolean nonloc = qtrue;

  if( trap_Argc() > 1 )
  {
    trap_Argv( 1, arg, sizeof( arg ) );
    damage = atoi( arg );
  }
  if( trap_Argc() > 4 )
  {
    trap_Argv( 2, arg, sizeof( arg ) );
    dx = atof( arg );
    trap_Argv( 3, arg, sizeof( arg ) );
    dy = atof( arg );
    trap_Argv( 4, arg, sizeof( arg ) );
    dz = atof( arg );
    nonloc = qfalse;
  }
  VectorCopy( ent->s.origin, point );
  point[ 0 ] += dx;
  point[ 1 ] += dy;
  point[ 2 ] += dz;
  G_Damage( ent, NULL, NULL, NULL, point, damage,
            ( nonloc ? DAMAGE_NO_LOCDAMAGE : 0 ), MOD_TARGET_LASER );
}

/*
==================
G_FloodLimited

Determine whether a user is flood limited, and adjust their flood demerits
Print them a warning message if they are over the limit
Return is time in msec until the user can speak again
==================
*/
int G_FloodLimited( gentity_t *ent )
{
  int deltatime, ms;

  if( g_floodMinTime.integer <= 0 )
    return 0;

  // handles !ent
  if( G_admin_permission( ent, ADMF_NOCENSORFLOOD ) )
    return 0;

  deltatime = level.time - ent->client->pers.floodTime;

  ent->client->pers.floodDemerits += g_floodMinTime.integer - deltatime;
  if( ent->client->pers.floodDemerits < 0 )
    ent->client->pers.floodDemerits = 0;
  ent->client->pers.floodTime = level.time;

  ms = ent->client->pers.floodDemerits - g_floodMaxDemerits.integer;
  if( ms <= 0 )
    return 0;
  trap_SendServerCommand( ent - g_entities, va( "print \"You are flooding: "
                          "please wait %d second%s before trying again\n",
                          ( ms + 999 ) / 1000, ( ms > 1000 ) ? "s" : "" ) );
  return ms;
}

commands_t cmds[ ] = {
  // normal commands
  { "team", 0, Cmd_Team_f },
  { "vote", 0, Cmd_Vote_f },
  { "ignore", 0, Cmd_Ignore_f },
  { "unignore", 0, Cmd_Ignore_f },

  // communication commands
  { "tell", CMD_MESSAGE, Cmd_Tell_f },
  { "callvote", CMD_MESSAGE, Cmd_CallVote_f },
  { "callteamvote", CMD_MESSAGE|CMD_TEAM, Cmd_CallTeamVote_f },
  { "say_area", CMD_MESSAGE|CMD_TEAM, Cmd_SayArea_f },
  // can be used even during intermission
  { "say", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Say_f },
  { "say_team", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Say_f },
  { "vsay", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VSay_f },
  { "vsay_team", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VSay_f },
  { "vsay_local", CMD_MESSAGE|CMD_INTERMISSION, Cmd_VSay_f },
  { "a", CMD_MESSAGE|CMD_INTERMISSION, Cmd_AdminMessage_f },
  { "c", CMD_MESSAGE|CMD_INTERMISSION, Cmd_ClanMessage_f },
  { "m", CMD_MESSAGE|CMD_INTERMISSION, Cmd_PrivateMessage_f },
  { "mt", CMD_MESSAGE|CMD_INTERMISSION, Cmd_PrivateMessage_f },
  { "me", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Say_f },
  { "me_team", CMD_MESSAGE|CMD_INTERMISSION, Cmd_Say_f },

  { "score", CMD_INTERMISSION, ScoreboardMessage },
  { "mystats", CMD_TEAM|CMD_INTERMISSION, Cmd_MyStats_f },

  // cheats
  { "give", CMD_CHEAT|CMD_TEAM|CMD_LIVING, Cmd_Give_f },
  { "god", CMD_CHEAT|CMD_TEAM|CMD_LIVING, Cmd_God_f },
  { "notarget", CMD_CHEAT|CMD_TEAM|CMD_LIVING, Cmd_Notarget_f },
  { "levelshot", CMD_CHEAT, Cmd_LevelShot_f },
  { "setviewpos", CMD_CHEAT_TEAM, Cmd_SetViewpos_f },
  { "noclip", CMD_CHEAT_TEAM, Cmd_Noclip_f },
  { "destroy", CMD_CHEAT|CMD_TEAM|CMD_LIVING, Cmd_Destroy_f },
  { "test", CMD_CHEAT, Cmd_Test_f },
  { "damage", CMD_CHEAT|CMD_LIVING, Cmd_Damage_f },
  { "where", 0, Cmd_Where_f },

  // game commands
  { "ptrcverify", 0, Cmd_PTRCVerify_f },
  { "ptrcrestore", 0, Cmd_PTRCRestore_f },

  { "follow", 0, Cmd_Follow_f },
  { "follownext", 0, Cmd_FollowCycle_f },
  { "followprev", 0, Cmd_FollowCycle_f },

  { "teamvote", CMD_TEAM, Cmd_TeamVote_f },
  { "class", CMD_TEAM, Cmd_Class_f },
  { "kill", CMD_TEAM|CMD_LIVING, Cmd_Kill_f },

  { "build", CMD_TEAM|CMD_LIVING, Cmd_Build_f },
  { "deconstruct", CMD_TEAM|CMD_LIVING, Cmd_Destroy_f },

  { "buy", CMD_HUMAN|CMD_LIVING, Cmd_Buy_f },
  { "sell", CMD_HUMAN|CMD_LIVING, Cmd_Sell_f },
  { "itemact", CMD_HUMAN|CMD_LIVING, Cmd_ActivateItem_f },
  { "itemdeact", CMD_HUMAN|CMD_LIVING, Cmd_DeActivateItem_f },
  { "itemtoggle", CMD_HUMAN|CMD_LIVING, Cmd_ToggleItem_f },
  { "reload", CMD_HUMAN|CMD_LIVING, Cmd_Reload_f },
  { "share", CMD_TEAM, Cmd_Share_f },
  { "donate", CMD_TEAM, Cmd_Donate_f },
  { "protect", CMD_TEAM|CMD_LIVING, Cmd_Protect_f },
  { "resign", CMD_TEAM, Cmd_Resign_f }
};
static int numCmds = sizeof( cmds ) / sizeof( cmds[ 0 ] );

/*
=================
ClientCommand
=================
*/
void ClientCommand( int clientNum )
{
  gentity_t *ent;
  char      cmd[ MAX_TOKEN_CHARS ];
  int       i;

  ent = g_entities + clientNum;
  if( !ent->client )
    return;   // not fully in game yet

  trap_Argv( 0, cmd, sizeof( cmd ) );

  for( i = 0; i < numCmds; i++ )
  {
    if( Q_stricmp( cmd, cmds[ i ].cmdName ) == 0 )
      break;
  }

  if( i == numCmds )
  {
    if( !G_admin_cmd_check( ent, qfalse ) )
      trap_SendServerCommand( clientNum,
        va( "print \"Unknown command %s\n\"", cmd ) );
    return;
  }

  // do tests here to reduce the amount of repeated code

  if( !( cmds[ i ].cmdFlags & CMD_INTERMISSION ) && level.intermissiontime )
    return;

  if( cmds[ i ].cmdFlags & CMD_CHEAT && !g_cheats.integer 
      && !ent->client->pers.override )
  {
    G_TriggerMenu( clientNum, MN_CMD_CHEAT );
    return;
  }

  if( cmds[ i ].cmdFlags & CMD_MESSAGE && ( ent->client->pers.muted ||
      G_FloodLimited( ent ) ) )
    return;

  if( cmds[ i ].cmdFlags & CMD_TEAM &&
      ent->client->pers.teamSelection == TEAM_NONE )
  {
    G_TriggerMenu( clientNum, MN_CMD_TEAM );
    return;
  }

  if( cmds[ i ].cmdFlags & CMD_CHEAT_TEAM && !g_cheats.integer &&
      ent->client->pers.teamSelection != TEAM_NONE 
      && !ent->client->pers.override )
  {
    G_TriggerMenu( clientNum, MN_CMD_CHEAT_TEAM );
    return;
  }

  if( cmds[ i ].cmdFlags & CMD_SPEC &&
      ent->client->sess.spectatorState == SPECTATOR_NOT
      && !ent->client->pers.override )
  {
    G_TriggerMenu( clientNum, MN_CMD_SPEC );
    return;
  }

  if( cmds[ i ].cmdFlags & CMD_ALIEN &&
      ent->client->pers.teamSelection != TEAM_ALIENS )
  {
    G_TriggerMenu( clientNum, MN_CMD_ALIEN );
    return;
  }

  if( cmds[ i ].cmdFlags & CMD_HUMAN && ent->client->pers.teamSelection != TEAM_HUMANS && !ent->client->pers.override )
  {
    // small hack to allow spitfires to use /itemact /itemtoggle /itemdeact
    // seems safe as they still can't buy human items/weapons/etc nor activate subsequent items they can't buy as they don't have them
    if( ent->client->ps.stats[ STAT_CLASS ] != PCL_ALIEN_SPITFIRE )
    {
      G_TriggerMenu( clientNum, MN_CMD_HUMAN );
      return;
    }
  }

  if( cmds[ i ].cmdFlags & CMD_LIVING &&
    ( ent->client->ps.stats[ STAT_HEALTH ] <= 0 ||
      ent->client->sess.spectatorState != SPECTATOR_NOT ) 
      && !ent->client->pers.override )
  {
    G_TriggerMenu( clientNum, MN_CMD_LIVING );
    return;
  }

  cmds[ i ].cmdHandler( ent );
}

/*
=================
G_SayArgc
G_SayArgv
G_SayConcatArgs

trap_Argc, trap_Argv, and ConcatArgs consider say text as a single argument
These functions assemble the text and re-parse it on word boundaries
=================
*/
int G_SayArgc( void )
{
  int c = 0;
  char *s;

  s = ConcatArgs( 0 );
  while( 1 )
  {
    while( *s == ' ' )
      s++;
    if( !*s )
      break;
    c++;
    while( *s && *s != ' ' )
      s++;
  }
  return c;
}

qboolean G_SayArgv( int n, char *buffer, int bufferLength )
{
  char *s;

  if( bufferLength < 1 )
    return qfalse;
  if( n < 0 )
    return qfalse;
  s = ConcatArgs( 0 );
  while( 1 )
  {
    while( *s == ' ' )
      s++;
    if( !*s || n == 0 )
      break;
    n--;
    while( *s && *s != ' ' )
      s++;
  }
  if( n > 0 )
    return qfalse;
  //memccpy( buffer, s, ' ', bufferLength );
  while( *s && *s != ' ' && bufferLength > 1 )
  {
    *buffer++ = *s++;
    bufferLength--;
  }
  *buffer = 0;
  return qtrue;
}

char *G_SayConcatArgs( int start )
{
  char *s;

  s = ConcatArgs( 0 );
  while( 1 )
  {
    while( *s == ' ' )
      s++;
    if( !*s || start == 0 )
      break;
    start--;
    while( *s && *s != ' ' )
      s++;
  }
  return s;
}

void G_DecolorString( char *in, char *out, int len )
{
  len--;

  while( *in && len > 0 ) {
    if( Q_IsColorString( in ) ) {
      in++;
      if( *in )
        in++;
      continue;
    }
    *out++ = *in++;
    len--;
  }
  *out = '\0';
}

void G_ParseEscapedString( char *buffer )
{
  int i = 0;
  int j = 0;

  while( buffer[i] )
  {
    if(!buffer[i]) break;

    if(buffer[i] == '\\')
    {
      if(buffer[i + 1] == '\\')
        buffer[j] = buffer[++i];
      else if(buffer[i + 1] == 'n')
      {
        buffer[j] = '\n';
        i++;
      }
      else
        buffer[j] = buffer[i];
    }
    else
      buffer[j] = buffer[i];

    i++;
    j++;
  }
  buffer[j] = 0;
}

void G_WordWrap( char *buffer, int maxwidth )
{
  char out[ MAX_STRING_CHARS ];
  int i = 0;
  int j = 0;
  int k;
  int linecount = 0;
  int currentcolor = 7;

  while ( buffer[ j ]!='\0' )
  {
     if( i == ( MAX_STRING_CHARS - 1 ) )
       break;

     //If it's the start of a new line, copy over the color code,
     //but not if we already did it, or if the text at the start of the next line is also a color code
     if( linecount == 0 && i>2 && out[ i-2 ] != Q_COLOR_ESCAPE && out[ i-1 ] != Q_COLOR_ESCAPE )
     {
       out[ i ] = Q_COLOR_ESCAPE;
       out[ i + 1 ] = '0' + currentcolor; 
       i+=2;
       continue;
     }

     if( linecount < maxwidth )
     {
       out[ i ] = buffer[ j ];
       if( out[ i ] == '\n' ) 
       {
         linecount = 0;
       }
       else if( Q_IsColorString( &buffer[j] ) )
       {
         currentcolor = buffer[j+1] - '0';
       }
       else
         linecount++;
       
       //If we're at a space and getting close to a line break, look ahead and make sure that there isn't already a \n or a closer space coming. If not, break here.
      if( out[ i ] == ' ' && linecount >= (maxwidth - 10 ) ) 
      {
        qboolean foundbreak = qfalse;
        for( k = i+1; k < maxwidth; k++ )
        {
          if( !buffer[ k ] )
            continue;
          if( buffer[ k ] == '\n' || buffer[ k ] == ' ' )
            foundbreak = qtrue;
        }
        if( !foundbreak )
        {
          out [ i ] = '\n';
          linecount = 0;
        }
      }
       
      i++;
      j++;
     }
     else
     {
       out[ i ] = '\n';
       i++;
       linecount = 0;
     }
  }
  out[ i ] = '\0';


  strcpy( buffer, out );
}

void Cmd_PrivateMessage_f( gentity_t *ent )
{
  int pids[ MAX_CLIENTS ];
  int ignoreids[ MAX_CLIENTS ];
  char name[ MAX_NAME_LENGTH ];
  char cmd[ 12 ];
  char str[ MAX_STRING_CHARS ];
  char *msg;
  char color;
  int pcount, matches, ignored = 0;
  int i;
  int skipargs = 0;
  qboolean teamonly = qfalse;
  gentity_t *tmpent;
  qboolean nteammater = qfalse;

  if( !g_privateMessages.integer && ent )
  {
    ADMP( "Sorry, but private messages have been disabled\n" );
    return;
  }

  G_SayArgv( 0, cmd, sizeof( cmd ) );
  if( !Q_stricmp( cmd, "say" ) || !Q_stricmp( cmd, "say_team" ) )
  {
    skipargs = 1;
    G_SayArgv( 1, cmd, sizeof( cmd ) );
  }
  if( G_SayArgc( ) < 3+skipargs )
  {
    ADMP( va( "usage: %s [name|slot#] [message]\n", cmd ) );
    return;
  }

  if( !Q_stricmp( cmd, "mt" ) || !Q_stricmp( cmd, "/mt" ) )
    teamonly = qtrue;

  G_SayArgv( 1+skipargs, name, sizeof( name ) );
  msg = G_SayConcatArgs( 2+skipargs );
  pcount = G_ClientNumbersFromString( name, pids, MAX_CLIENTS );

  if( ent )
  {
    int count = 0;

    for( i = 0; i < pcount; i++ )
    {
      tmpent = &g_entities[ pids[ i ] ];

      if( teamonly && !OnSameTeam( ent, tmpent ) )
        continue;

      if( !OnSameTeam( ent, tmpent ) )
        nteammater = qtrue;

      if( BG_ClientListTest( &tmpent->client->sess.ignoreList,
        ent-g_entities ) )
      {
        ignoreids[ ignored++ ] = pids[ i ];
        continue;
      }

      pids[ count ] = pids[ i ];
      count++;
    }
    matches = count;
  }
  else
  {
    matches = pcount;
  }

  if( g_smartesd.integer && g_extremeSuddenDeath.integer && nteammater == qtrue )
  {
   ADMP( "Sorry, but you cannot pm anyone who isn't a teammate during ESD.\n" );
   return;
  }

  color = teamonly ? COLOR_CYAN : COLOR_YELLOW;

  Com_sprintf( str, sizeof( str ), "^%csent to %i player%s: ^7", color, matches,
    ( matches == 1 ) ? "" : "s" );

  for( i=0; i < matches; i++ )
  {
    tmpent = &g_entities[ pids[ i ] ];

    if( i > 0 )
      Q_strcat( str, sizeof( str ), "^7, " );
    Q_strcat( str, sizeof( str ), tmpent->client->pers.netname );
    trap_SendServerCommand( pids[ i ], va(
      "chat \"%s^%c -> ^7%s^7: (%d recipient%s): ^%c%s^7\" %i",
      ( ent ) ? ent->client->pers.netname : "console",
      color,
      name,
      matches,
      ( matches == 1 ) ? "" : "s",
      color,
      msg,
      ent ? (int)(ent-g_entities) : -1 ) );
    if( ent )
    {
      trap_SendServerCommand( pids[ i ], va(
        "print \">> to reply, say: /m %d [your message] <<\n\"",
        (int)( ent - g_entities ) ) );
    }
    trap_SendServerCommand( pids[ i ], va(
      "cp \"^%cprivate message from ^7%s^7\"", color,
      ( ent ) ? ent->client->pers.netname : "console" ) );
  }

  if( !matches )
    ADMP( va( "^3No player matching ^7\'%s^7\' ^3to send message to.\n",
      name ) );
  else
  {
    ADMP( va( "^%cPrivate message: ^7%s\n", color, msg ) );
    ADMP( va( "%s\n", str ) );

    G_LogPrintf( "%s: %s^7: %s^7: %s\n",
      ( teamonly ) ? "tprivmsg" : "privmsg",
      ( ent ) ? ent->client->pers.netname : "console",
      name, msg );
  }

  if( ignored )
  {
    Com_sprintf( str, sizeof( str ), "^%cignored by %i player%s: ^7", color,
      ignored, ( ignored == 1 ) ? "" : "s" );
    for( i=0; i < ignored; i++ )
    {
      tmpent = &g_entities[ ignoreids[ i ] ];
      if( i > 0 )
        Q_strcat( str, sizeof( str ), "^7, " );
      Q_strcat( str, sizeof( str ), tmpent->client->pers.netname );
    }
    ADMP( va( "%s\n", str ) );
  }
}

/*
=================
Cmd_AdminMessage_f

Send a message to all active admins
=================
*/
void Cmd_AdminMessage_f( gentity_t *ent )
{
  char cmd[ sizeof( "say_team" ) ];
  char prefix[ 50 ];
  char postprefix[ 50 ];
  char *msg;
  int skiparg = 0;

  // Check permissions and add the appropriate user [prefix]
  if( !ent )
  {
    Com_sprintf( postprefix, sizeof( postprefix ), "[CONSOLE]" S_COLOR_WHITE ": " S_COLOR_MAGENTA );
    Com_sprintf( prefix, sizeof( prefix ), "[ADMIN]" );
  }
  else if( !G_admin_permission( ent, ADMF_ADMINCHAT ) )
  {
    if( !g_publicAdminMessages.integer )
    {
      ADMP( "Sorry, but use of /a by non-admins has been disabled.\n" );
      return;
    }
    else
    {
      Com_sprintf( postprefix, sizeof( postprefix ), "[PLAYER]%s" S_COLOR_WHITE ": " S_COLOR_MAGENTA,
                   ent->client->pers.netname );
      Com_sprintf( prefix, sizeof( prefix ), "[A]" );
      ADMP( "Your message has been sent to any available admins "
            "and to the server logs.\n" );
    }
  }
  else
  {
    Com_sprintf( postprefix, sizeof( postprefix ), "%s" S_COLOR_WHITE ": " S_COLOR_MAGENTA,
                 ent->client->pers.netname );
    Com_sprintf( prefix, sizeof( prefix ), "[ADMIN]" );
  }

  // Skip say/say_team if this was used from one of those
  G_SayArgv( 0, cmd, sizeof( cmd ) );
  if( !Q_stricmp( cmd, "say" ) || !Q_stricmp( cmd, "say_team" ) )
  {
    skiparg = 1;
    G_SayArgv( 1, cmd, sizeof( cmd ) );
  }
  if( G_SayArgc( ) < 2 + skiparg )
  {
    ADMP( va( "usage: %s [message]\n", cmd ) );
    return;
  }

  msg = G_SayConcatArgs( 1 + skiparg );

  // Send it
  G_AdminMessage( prefix, "%s%s", postprefix, msg );
}

void G_CP( gentity_t *ent )
{ 
  int i;
  char buffer[MAX_STRING_CHARS];
  char prefixes[MAX_STRING_CHARS] = "";
  char wrappedtext[ MAX_STRING_CHARS ] = "";
  char *ptr;
  char *text;
  qboolean sendAliens = qtrue;
  qboolean sendHumans = qtrue;
  qboolean sendSpecs = qtrue;
  Q_strncpyz( buffer, ConcatArgs( 1 ), sizeof( buffer ) );
  G_ParseEscapedString( buffer );

  if( strstr( buffer, "!cp" ) )
  {
    ptr = buffer;
    while( *ptr != '!' )
      ptr++;
    ptr+=4;

    Q_strncpyz( buffer, ptr, sizeof(buffer) );
  }

  text = buffer;

  ptr = buffer;
  while( *ptr == ' ' )
    ptr++;
  if( *ptr == '-' )
  {
      sendAliens = qfalse;
      sendHumans = qfalse;
      sendSpecs = qfalse;
      Q_strcat( prefixes, sizeof( prefixes ), " " );
      ptr++;

      while( *ptr != ' ' )
      {
        if( *ptr == 'a' || *ptr == 'A' )
        {
          sendAliens = qtrue;
          Q_strcat( prefixes, sizeof( prefixes ), "[A]" );
        }
        if( *ptr == 'h' || *ptr == 'H' )
        {
          sendHumans = qtrue;
          Q_strcat( prefixes, sizeof( prefixes ), "[H]" );
        }
        if( *ptr == 's' || *ptr == 'S' )
        {
          sendSpecs = qtrue;
          Q_strcat( prefixes, sizeof( prefixes ), "[S]" );
        }
        ptr++;
      }
      text = ptr+1;
  }
  
  strcpy( wrappedtext, text );
  G_WordWrap( wrappedtext, 50 );

  for( i = 0; i < level.maxclients; i++ )
  {
    if( level.clients[ i ].pers.connected == CON_DISCONNECTED )
      continue;

    if( ( !sendAliens && level.clients[ i ].pers.teamSelection == TEAM_ALIENS ) ||
        ( !sendHumans && level.clients[ i ].pers.teamSelection == TEAM_HUMANS ) ||
        ( !sendSpecs && level.clients[ i ].pers.teamSelection == TEAM_NONE ) )
    {
      if( G_admin_permission( &g_entities[ i ], ADMF_ADMINCHAT ) )
      {
        trap_SendServerCommand( i, va("print \"^6[Admins]^7 CP to other team%s: %s \n\"", prefixes, text ) );
      }
      continue;
    }

      trap_SendServerCommand( i, va( "cp \"%s\"", wrappedtext ) );
      trap_SendServerCommand( i, va( "print \"%s^7 CP%s: %s\n\"", ( ent ) ? ent->client->pers.netname : "console", prefixes, text ) );
    }

    G_Printf( "cp: %s\n", ConcatArgs( 1 ) );
}

/*
=================
Cmd_ClanMessage_f

Send a message to all active clanmates
=================
*/
void Cmd_ClanMessage_f( gentity_t *ent )
{
  char cmd[ sizeof( "say_team" ) ];
  char prefix[ 50 ];
  char postprefix[ 50 ];
  char *msg;
  int skiparg = 0;

  // Check permissions and add the appropriate user [prefix]
  if( !ent )
  {
    Com_sprintf( postprefix, sizeof( postprefix ), "[CONSOLE]" S_COLOR_WHITE ": " );
    Com_sprintf( prefix, sizeof( prefix ), "[CLAN]" );
  }
  else if( !G_admin_permission( ent, ADMF_CLANCHAT ) )
  {
    if( !g_publicClanMessages.integer )
    {
      ADMP( "Sorry, but use of /c by non clan members has been disabled.\n" );
      return;
    }
    else
    {
      Com_sprintf( postprefix, sizeof( postprefix ), "[PLAYER]%s" S_COLOR_WHITE ": ",
                   ent->client->pers.netname );
      Com_sprintf( prefix, sizeof( prefix ), "[C]" );
      ADMP( "Your message has been sent to any available clan members "
            "and to the server logs.\n" );
    }
  }
  else
  {
    Com_sprintf( postprefix, sizeof( postprefix ), "%s" S_COLOR_WHITE ": ",
                 ent->client->pers.netname );
    Com_sprintf( prefix, sizeof( prefix ), "[CLAN]" );
  }

  // Skip say/say_team if this was used from one of those
  G_SayArgv( 0, cmd, sizeof( cmd ) );
  if( !Q_stricmp( cmd, "say" ) || !Q_stricmp( cmd, "say_team" ) )
  {
    skiparg = 1;
    G_SayArgv( 1, cmd, sizeof( cmd ) );
  }
  if( G_SayArgc( ) < 2 + skiparg )
  {
    ADMP( va( "usage: %s [message]\n", cmd ) );
    return;
  }

  msg = G_SayConcatArgs( 1 + skiparg );

  // Send it
  G_AdminMessage( prefix, "%s%s", postprefix, msg );
}
