/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2006 Tim Angus

This file is part of Tremulous.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

// this file holds commands that can be executed by the server console, but not remote clients

#include "g_local.h"


/*
==============================================================================

PACKET FILTERING


You can add or remove addresses from the filter list with:

addip <ip>
removeip <ip>

The ip address is specified in dot format, and you can use '*' to match any value
so you can specify an entire class C network with "addip 192.246.40.*"

Removeip will only remove an address specified exactly the same way.  You cannot addip a subnet, then removeip a single host.

listip
Prints the current list of filters.

g_filterban <0 or 1>

If 1 (the default), then ip addresses matching the current list will be prohibited from entering the game.  This is the default setting.

If 0, then only addresses matching the list will be allowed.  This lets you easily set up a private game, or a game that only allows players from your local network.

TTimo NOTE: for persistence, bans are stored in g_banIPs cvar MAX_CVAR_VALUE_STRING
The size of the cvar string buffer is limiting the banning to around 20 masks
this could be improved by putting some g_banIPs2 g_banIps3 etc. maybe
still, you should rely on PB for banning instead

==============================================================================
*/

// extern vmCvar_t  g_banIPs;
// extern vmCvar_t  g_filterBan;


typedef struct ipFilter_s
{
  unsigned  mask;
  unsigned  compare;
} ipFilter_t;

#define MAX_IPFILTERS 1024

static ipFilter_t ipFilters[ MAX_IPFILTERS ];
static int        numIPFilters;

/*
=================
StringToFilter
=================
*/
static qboolean StringToFilter( char *s, ipFilter_t *f )
{
  char  num[ 128 ];
  int   i, j;
  byte  b[ 4 ];
  byte  m[ 4 ];

  for( i = 0; i < 4; i++ )
  {
    b[ i ] = 0;
    m[ i ] = 0;
  }

  for( i = 0; i < 4; i++ )
  {
    if( *s < '0' || *s > '9' )
    {
      if( *s == '*' ) // 'match any'
      {
        //b[ i ] and m[ i ] to 0
        s++;
        if ( !*s )
          break;

        s++;
        continue;
      }

      G_Printf( "Bad filter address: %s\n", s );
      return qfalse;
    }

    j = 0;
    while( *s >= '0' && *s <= '9' )
      num[ j++ ] = *s++;

    num[ j ] = 0;
    b[ i ] = atoi( num );

    m[ i ] = 255;

    if( !*s )
      break;

    s++;
  }

  f->mask = *(unsigned *)m;
  f->compare = *(unsigned *)b;

  return qtrue;
}

/*
=================
UpdateIPBans
=================
*/
static void UpdateIPBans( void )
{
  byte  b[ 4 ];
  byte  m[ 4 ];
  int    i, j;
  char  iplist_final[ MAX_CVAR_VALUE_STRING ];
  char  ip[ 64 ];

  *iplist_final = 0;

  for( i = 0 ; i < numIPFilters ; i++ )
  {
    if( ipFilters[ i ].compare == 0xffffffff )
      continue;

    *(unsigned *)b = ipFilters[ i ].compare;
    *(unsigned *)m = ipFilters[ i ].mask;
    *ip = 0;

    for( j = 0 ; j < 4 ; j++ )
    {
      if( m[ j ] != 255 )
        Q_strcat( ip, sizeof( ip ), "*" );
      else
        Q_strcat( ip, sizeof( ip ), va( "%i", b[ j ] ) );

      Q_strcat( ip, sizeof( ip ), ( j < 3 ) ? "." : " " );
    }

    if( strlen( iplist_final ) + strlen( ip ) < MAX_CVAR_VALUE_STRING )
      Q_strcat( iplist_final, sizeof( iplist_final ), ip );
    else
    {
      Com_Printf( "g_banIPs overflowed at MAX_CVAR_VALUE_STRING\n" );
      break;
    }
  }

  trap_Cvar_Set( "g_banIPs", iplist_final );
}

/*
=================
G_FilterPacket
=================
*/
qboolean G_FilterPacket( char *from )
{
  int       i;
  unsigned  in;
  byte      m[ 4 ];
  char      *p;

  i = 0;
  p = from;
  while( *p && i < 4 )
  {
    m[ i ] = 0;
    while( *p >= '0' && *p <= '9' )
    {
      m[ i ] = m[ i ] * 10 + ( *p - '0' );
      p++;
    }

    if( !*p || *p == ':' )
      break;

    i++, p++;
  }

  in = *(unsigned *)m;

  for( i = 0; i < numIPFilters; i++ )
    if( ( in & ipFilters[ i ].mask ) == ipFilters[ i ].compare )
      return g_filterBan.integer != 0;

  return g_filterBan.integer == 0;
}

/*
=================
AddIP
=================
*/
static void AddIP( char *str )
{
  int   i;

  for( i = 0 ; i < numIPFilters ; i++ )
    if( ipFilters[ i ].compare == 0xffffffff )
      break;    // free spot

  if( i == numIPFilters )
  {
    if( numIPFilters == MAX_IPFILTERS )
    {
      G_Printf( "IP filter list is full\n" );
      return;
    }

    numIPFilters++;
  }

  if( !StringToFilter( str, &ipFilters[ i ] ) )
    ipFilters[ i ].compare = 0xffffffffu;

  UpdateIPBans( );
}

/*
=================
G_ProcessIPBans
=================
*/
void G_ProcessIPBans( void )
{
  char *s, *t;
  char str[ MAX_CVAR_VALUE_STRING ];

  Q_strncpyz( str, g_banIPs.string, sizeof( str ) );

  for( t = s = g_banIPs.string; *t; /* */ )
  {
    s = strchr( s, ' ' );

    if( !s )
      break;

    while( *s == ' ' )
      *s++ = 0;

    if( *t )
      AddIP( t );

    t = s;
  }
}


/*
=================
Svcmd_AddIP_f
=================
*/
void Svcmd_AddIP_f( void )
{
  char str[ MAX_TOKEN_CHARS ];

  if( trap_Argc( ) < 2 )
  {
    G_Printf( "Usage:  addip <ip-mask>\n" );
    return;
  }

  trap_Argv( 1, str, sizeof( str ) );

  AddIP( str );
}

/*
=================
Svcmd_RemoveIP_f
=================
*/
void Svcmd_RemoveIP_f( void )
{
  ipFilter_t  f;
  int         i;
  char        str[ MAX_TOKEN_CHARS ];

  if( trap_Argc( ) < 2 )
  {
    G_Printf( "Usage:  sv removeip <ip-mask>\n" );
    return;
  }

  trap_Argv( 1, str, sizeof( str ) );

  if( !StringToFilter( str, &f ) )
    return;

  for( i = 0; i < numIPFilters; i++ )
  {
    if( ipFilters[ i ].mask == f.mask &&
        ipFilters[ i ].compare == f.compare)
    {
      ipFilters[ i ].compare = 0xffffffffu;
      G_Printf ( "Removed.\n" );

      UpdateIPBans( );
      return;
    }
  }

  G_Printf ( "Didn't find %s.\n", str );
}

/*
===================
Svcmd_EntityList_f
===================
*/
void  Svcmd_EntityList_f( void )
{
  int       e;
  gentity_t *check;

  check = g_entities + 1;

  for( e = 1; e < level.num_entities; e++, check++ )
  {
    if( !check->inuse )
      continue;

    G_Printf( "%3i:", e );

    switch( check->s.eType )
    {
      case ET_GENERAL:
        G_Printf( "ET_GENERAL          " );
        break;
      case ET_PLAYER:
        G_Printf( "ET_PLAYER           " );
        break;
      case ET_ITEM:
        G_Printf( "ET_ITEM             " );
        break;
      case ET_BUILDABLE:
        G_Printf( "ET_BUILDABLE        " );
        break;
      case ET_MISSILE:
        G_Printf( "ET_MISSILE          " );
        break;
      case ET_MOVER:
        G_Printf( "ET_MOVER            " );
        break;
      case ET_BEAM:
        G_Printf( "ET_BEAM             " );
        break;
      case ET_PORTAL:
        G_Printf( "ET_PORTAL           " );
        break;
      case ET_SPEAKER:
        G_Printf( "ET_SPEAKER          " );
        break;
      case ET_PUSH_TRIGGER:
        G_Printf( "ET_PUSH_TRIGGER     " );
        break;
      case ET_TELEPORT_TRIGGER:
        G_Printf( "ET_TELEPORT_TRIGGER " );
        break;
      case ET_INVISIBLE:
        G_Printf( "ET_INVISIBLE        " );
        break;
      case ET_GRAPPLE:
        G_Printf( "ET_GRAPPLE          " );
        break;
      default:
        G_Printf( "%3i                 ", check->s.eType );
        break;
    }

    if( check->classname )
      G_Printf( "%s", check->classname );

    G_Printf( "\n" );
  }
}

gclient_t *ClientForString( const char *s )
{
  gclient_t *cl;
  int       i;
  int       idnum;

  // numeric values are just slot numbers
  if( s[ 0 ] >= '0' && s[ 0 ] <= '9' )
  {
    idnum = atoi( s );

    if( idnum < 0 || idnum >= level.maxclients )
    {
      Com_Printf( "Bad client slot: %i\n", idnum );
      return NULL;
    }

    cl = &level.clients[ idnum ];

    if( cl->pers.connected == CON_DISCONNECTED )
    {
      G_Printf( "Client %i is not connected\n", idnum );
      return NULL;
    }

    return cl;
  }

  // check for a name match
  for( i = 0; i < level.maxclients; i++ )
  {
    cl = &level.clients[ i ];
    if( cl->pers.connected == CON_DISCONNECTED )
      continue;

    if( !Q_stricmp( cl->pers.netname, s ) )
      return cl;
  }

  G_Printf( "User %s is not on the server\n", s );

  return NULL;
}

/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
void  Svcmd_ForceTeam_f( void )
{
  gclient_t *cl;
  char      str[ MAX_TOKEN_CHARS ];

  // find the player
  trap_Argv( 1, str, sizeof( str ) );
  cl = ClientForString( str );

  if( !cl )
    return;

  // set the team
  trap_Argv( 2, str, sizeof( str ) );
  /*SetTeam( &g_entities[cl - level.clients], str );*/
  //FIXME: tremulise this
}

/*
===================
Svcmd_LayoutSave_f

layoutsave <name>
===================
*/
void  Svcmd_LayoutSave_f( void )
{
  char str[ MAX_QPATH ];
  char str2[ MAX_QPATH - 4 ];
  char *s;
  int i = 0;

  if( trap_Argc( ) != 2 )
  {
    G_Printf( "usage: layoutsave LAYOUTNAME\n" );
    return;
  }
  trap_Argv( 1, str, sizeof( str ) );

  // sanitize name
  s = &str[ 0 ];
  while( *s && i < sizeof( str2 ) - 1 )
  {
    if( ( *s >= '0' && *s <= '9' ) ||
      ( *s >= 'a' && *s <= 'z' ) ||
      ( *s >= 'A' && *s <= 'Z' ) || *s == '-' || *s == '_' )
    {
      str2[ i++ ] = *s;
      str2[ i ] = '\0';
    }
    s++;
  }

  if( !str2[ 0 ] )
  {
    G_Printf("layoutsave: invalid name \"%s\"\n", str );
    return;
  }

  G_LayoutSave( str2 );
}

char  *ConcatArgs( int start );

/*
===================
Svcmd_LayoutLoad_f

layoutload [<name> [<name2> [<name3 [...]]]]

This is just a silly alias for doing:
 set g_layouts "name name2 name3"
 map_restart
===================
*/
void  Svcmd_LayoutLoad_f( void )
{
  char layouts[ MAX_CVAR_VALUE_STRING ];
  char *s;

  s = ConcatArgs( 1 );
  Q_strncpyz( layouts, s, sizeof( layouts ) );
  trap_Cvar_Set( "g_layouts", layouts ); 
  trap_SendConsoleCommand( EXEC_APPEND, "map_restart\n" );
  level.restarted = qtrue;
}

static void Svcmd_AdmitDefeat_f( void )
{
  int  team;
  char teamNum[ 2 ];

  if( trap_Argc( ) != 2 )
  {
    G_Printf("admitdefeat: must provide a team\n");
    return;
  }
  trap_Argv( 1, teamNum, sizeof( teamNum ) );
  team = atoi( teamNum );
  if( team == PTE_ALIENS || teamNum[ 0 ] == 'a' )
  {
    level.surrenderTeam = PTE_ALIENS;
    G_BaseSelfDestruct( PTE_ALIENS );
    G_TeamCommand( PTE_ALIENS, "cp \"Hivemind Link Broken\" 1");
    trap_SendServerCommand( -1, "print \"Alien team has admitted defeat\n\"" );
  }
  else if( team == PTE_HUMANS || teamNum[ 0 ] == 'h' )
  {
    level.surrenderTeam = PTE_HUMANS;
    G_BaseSelfDestruct( PTE_HUMANS );
    G_TeamCommand( PTE_HUMANS, "cp \"Life Support Terminated\" 1");
    trap_SendServerCommand( -1, "print \"Human team has admitted defeat\n\"" );
  }
  else
  {
    G_Printf("admitdefeat: invalid team\n");
  } 
}

/*
=================
ConsoleCommand

=================
*/
qboolean  ConsoleCommand( void )
{
  char cmd[ MAX_TOKEN_CHARS ];

  trap_Argv( 0, cmd, sizeof( cmd ) );

  if( Q_stricmp( cmd, "entitylist" ) == 0 )
  {
    Svcmd_EntityList_f( );
    return qtrue;
  }

  if( Q_stricmp( cmd, "forceteam" ) == 0 )
  {
    Svcmd_ForceTeam_f( );
    return qtrue;
  }

  if( Q_stricmp( cmd, "game_memory" ) == 0 )
  {
    Svcmd_GameMem_f( );
    return qtrue;
  }

  if( Q_stricmp( cmd, "addip" ) == 0 )
  {
    Svcmd_AddIP_f( );
    return qtrue;
  }

  if( Q_stricmp( cmd, "removeip" ) == 0 )
  {
    Svcmd_RemoveIP_f( );
    return qtrue;
  }

  if( Q_stricmp( cmd, "listip" ) == 0 )
  {
    trap_SendConsoleCommand( EXEC_NOW, "g_banIPs\n" );
    return qtrue;
  }

  if( Q_stricmp( cmd, "mapRotation" ) == 0 )
  {
    char *rotationName = ConcatArgs( 1 );

    if( !G_StartMapRotation( rotationName, qfalse ) )
      G_Printf( "Can't find map rotation %s\n", rotationName );

    return qtrue;
  }

  if( Q_stricmp( cmd, "stopMapRotation" ) == 0 )
  {
    G_StopMapRotation( );

    return qtrue;
  }

  if( Q_stricmp( cmd, "advanceMapRotation" ) == 0 )
  {
    G_AdvanceMapRotation( );

    return qtrue;
  }

  if( Q_stricmp( cmd, "alienWin" ) == 0 )
  {
    int       i;
    gentity_t *e;

    for( i = 1, e = g_entities + i; i < level.num_entities; i++, e++ )
    {
      if( e->s.modelindex == BA_H_SPAWN )
        G_Damage( e, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
    }

    return qtrue;
  }

  if( Q_stricmp( cmd, "humanWin" ) == 0 )
  {
    int       i;
    gentity_t *e;

    for( i = 1, e = g_entities + i; i < level.num_entities; i++, e++ )
    {
      if( e->s.modelindex == BA_A_SPAWN )
        G_Damage( e, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
    }

    return qtrue;
  }

  if( !Q_stricmp( cmd, "layoutsave" ) )
  {
    Svcmd_LayoutSave_f( );
    return qtrue;
  }
  
  if( !Q_stricmp( cmd, "layoutload" ) )
  {
    Svcmd_LayoutLoad_f( );
    return qtrue;
  }
  
  if( !Q_stricmp( cmd, "admitdefeat" ) )
  {
    Svcmd_AdmitDefeat_f( );
    return qtrue;
  }

  if( !Q_stricmp( cmd, "evacuation" ) )
  {
    trap_SendServerCommand( -1, "print \"Evacuation ordered\n\"" );
    level.lastWin = PTE_NONE;
    trap_SetConfigstring( CS_WINNER, "Evacuation" );
    LogExit( "Evacuation." );
    return qtrue;
  }
  
  // see if this is a a admin command
  if( G_admin_cmd_check( NULL, qfalse ) )
    return qtrue;

  if( g_dedicated.integer )
  {
    if( Q_stricmp( cmd, "say" ) == 0 )
    {
      trap_SendServerCommand( -1, va( "print \"server: %s\n\"", ConcatArgs( 1 ) ) );
      return qtrue;
    }
    else if( !Q_stricmp( cmd, "chat" ) )
    {
      trap_SendServerCommand( -1, va( "chat \"%s\" -1 0", ConcatArgs( 1 ) ) );
      G_Printf( "chat: %s\n", ConcatArgs( 1 ) );
      return qtrue;
    }
    else if( !Q_stricmp( cmd, "cp" ) )
    {
      trap_SendServerCommand( -1, va( "cp \"%s\"", ConcatArgs( 1 ) ) );
      G_Printf( "cp: %s\n", ConcatArgs( 1 ) );
      return qtrue;
    }
    else if( !Q_stricmp( cmd, "m" ) )
    {
      G_PrivateMessage( NULL );
      return qtrue;
    }
    else if( !Q_stricmp( cmd, "a " ) || !Q_stricmp( cmd, "say_admins " ) )
    {
      G_Say( NULL, NULL, SAY_ADMINS, ConcatArgs( 1 )  );
      return qtrue;
		}
    else if( !Q_stricmp( cmd, "c " ) || !Q_stricmp( cmd, "say_clan " ) )
    {
      G_Say( NULL, NULL, SAY_CLAN, ConcatArgs( 1 )  );
      return qtrue;
    }
    else if( !Q_stricmp( cmd, "me " ) || !Q_stricmp( cmd, "say_me " ) || !Q_stricmp( cmd, "me_team " ) )
    {
      G_Say( NULL, NULL, SAY_ALL, NULL );
      return qtrue;
    }


    G_Printf( "unknown command: %s\n", cmd );
    return qtrue;
  }

  return qfalse;
}
