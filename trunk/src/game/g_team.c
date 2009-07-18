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

// NULL for everyone
void QDECL PrintMsg( gentity_t *ent, const char *fmt, ... )
{
  char    msg[ 1024 ];
  va_list argptr;
  char    *p;

  va_start( argptr,fmt );

  if( Q_vsnprintf( msg, sizeof( msg ), fmt, argptr ) > sizeof( msg ) )
    G_Error ( "PrintMsg overrun" );

  va_end( argptr );

  // double quotes are bad
  while( ( p = strchr( msg, '"' ) ) != NULL )
    *p = '\'';

  trap_SendServerCommand( ( ( ent == NULL ) ? -1 : ent-g_entities ), va( "print \"%s\"", msg ) );
}

/*
================
G_TeamFromString

Return the team referenced by a string
================
*/
team_t G_TeamFromString( char *str )
{
  switch( tolower( *str ) )
  {
    case '0': case 's': return TEAM_NONE;
    case '1': case 'a': return TEAM_ALIENS;
    case '2': case 'h': return TEAM_HUMANS;
    default: return NUM_TEAMS;
  }
}

/*
==============
OnSameTeam
==============
*/
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 )
{
  if( !ent1->client || !ent2->client )
    return qfalse;

  if( ent1->client->pers.teamSelection == ent2->client->pers.teamSelection )
    return qtrue;

  return qfalse;
}

/*
==================
G_LeaveTeam
==================
*/
void G_LeaveTeam( gentity_t *self )
{
  team_t    team = self->client->pers.teamSelection;
  gentity_t *ent;
  int       i;

  if( team == TEAM_ALIENS )
    G_RemoveFromSpawnQueue( &level.alienSpawnQueue, self->client->ps.clientNum );
  else if( team == TEAM_HUMANS )
    G_RemoveFromSpawnQueue( &level.humanSpawnQueue, self->client->ps.clientNum );
  else
  {
    if( self->client->sess.spectatorState == SPECTATOR_FOLLOW )
      G_StopFollowing( self );
    return;
  }

  // stop any following clients
  G_StopFromFollowing( self );

  G_TeamVote( self, qfalse );
  self->suicideTime = 0;

  for( i = 0; i < level.num_entities; i++ )
  {
    ent = &g_entities[ i ];
    if( !ent->inuse )
      continue;

    if( ent->client && ent->client->pers.connected == CON_CONNECTED )
    {
      // cure poison
      if( ent->client->ps.stats[ STAT_STATE ] & SS_POISONED &&
          ent->client->lastPoisonClient == self )
        ent->client->ps.stats[ STAT_STATE ] &= ~SS_POISONED;
    }
    else if( ent->s.eType == ET_MISSILE && ent->r.ownerNum == self->s.number )
      G_FreeEntity( ent );
  }
}

/*
=================
G_ChangeTeam
=================
*/
void G_ChangeTeam( gentity_t *ent, team_t newTeam )
{
  team_t  oldTeam = ent->client->pers.teamSelection;
  qboolean isFixingImbalance=qfalse;

  if( oldTeam == newTeam )
    return;

  G_LeaveTeam( ent );
  ent->client->pers.teamSelection = newTeam;

  if ( ( level.numAlienClients - level.numHumanClients > 2 && oldTeam==TEAM_ALIENS && newTeam == TEAM_HUMANS && level.numHumanSpawns>0 ) ||
       ( level.numHumanClients - level.numAlienClients > 2 && oldTeam==TEAM_HUMANS && newTeam == TEAM_ALIENS  && level.numAlienSpawns>0 ) )
  {
    isFixingImbalance=qtrue;
  }

  // under certain circumstances, clients can keep their kills and credits
  // when switching teams
  if( ent->client->pers.saved || G_admin_permission( ent, ADMF_TEAMCHANGEFREE ) ||
    ( g_teamImbalanceWarnings.integer && isFixingImbalance ) ||
    ( ( oldTeam == TEAM_HUMANS || oldTeam == TEAM_ALIENS ) &&
      ( level.time - ent->client->pers.teamChangeTime ) > 60000 ) )
  {
    if( oldTeam == TEAM_NONE )
    {
      // ps.persistant[] from a spectator cannot be trusted
      ent->client->ps.persistant[ PERS_CREDIT ] = ent->client->pers.savedCredit;
    }
    else if( oldTeam == TEAM_HUMANS )
    {
      // always save in alien credits
      ent->client->ps.persistant[ PERS_CREDIT ] *=
        ( ALIEN_MAX_CREDITS / (float)HUMAN_MAX_CREDITS );
    }

    if( newTeam == TEAM_NONE )
    {
      // save values before the client enters the spectator team and their
      // ps.persistant[] values become trashed
      ent->client->pers.savedCredit = ent->client->ps.persistant[ PERS_CREDIT ];
    }
    else if( newTeam == TEAM_HUMANS )
    {
      // convert to alien currency
      ent->client->ps.persistant[ PERS_CREDIT ] *=
        ( HUMAN_MAX_CREDITS / (float)ALIEN_MAX_CREDITS );
    }
  }
  else
  {
    ent->client->ps.persistant[ PERS_CREDIT ] = 0;
    ent->client->ps.persistant[ PERS_KILLS ] = 0;
    ent->client->ps.persistant[ PERS_KILLED ] = 0;
    ent->client->pers.savedCredit = 0;
    ent->client->pers.savedDeaths = 0;
    ent->client->pers.statscounters.kills = 0;
    ent->client->pers.statscounters.structskilled = 0;
    ent->client->pers.statscounters.assists = 0;
    ent->client->pers.statscounters.repairspoisons = 0;
    ent->client->pers.statscounters.headshots = 0;
    ent->client->pers.statscounters.hits = 0;
    ent->client->pers.statscounters.hitslocational = 0;
    ent->client->pers.statscounters.deaths = 0;
    ent->client->pers.statscounters.feeds = 0;
    ent->client->pers.statscounters.suicides = 0;
    ent->client->pers.statscounters.teamkills = 0;
    ent->client->pers.statscounters.dmgdone = 0;
    ent->client->pers.statscounters.structdmgdone = 0;
    ent->client->pers.statscounters.ffdmgdone = 0;
    ent->client->pers.statscounters.structsbuilt = 0;
    ent->client->pers.statscounters.timealive = 0;
    ent->client->pers.statscounters.timeinbase = 0;
    ent->client->pers.statscounters.dretchbasytime = 0;
    ent->client->pers.statscounters.jetpackusewallwalkusetime = 0;
  }

  if( G_admin_permission( ent, ADMF_DBUILDER ) )
  {
    if( !ent->client->pers.designatedBuilder )
    {
      ent->client->pers.designatedBuilder = qtrue;
      trap_SendServerCommand( ent-g_entities,
        "print \"Your designation has been restored\n\"" );
    }
  }
  else if( ent->client->pers.designatedBuilder )
  {
    ent->client->pers.designatedBuilder = qfalse;
    trap_SendServerCommand( ent-g_entities,
     "print \"You have lost designation due to teamchange\n\"" );
  }

  ent->client->pers.classSelection = PCL_NONE;
  ClientSpawn( ent, NULL, NULL, NULL );

  ent->client->pers.joinedATeam = qtrue;
  ent->client->pers.teamChangeTime = level.time;

  //update ClientInfo
  ClientUserinfoChanged( ent->client->ps.clientNum );
  G_CheckDBProtection( );

  // team change from H to A or A to H
  if( oldTeam != TEAM_NONE && newTeam != TEAM_NONE )
    G_LogPrintf(
      "team: %i %i %i: %s" S_COLOR_WHITE " left the %ss and joined the %ss\n",
       ent->s.number, newTeam, oldTeam, ent->client->pers.netname,
       BG_TeamName( oldTeam ), BG_TeamName( newTeam ) );
  // joining spectators
  else if( newTeam == TEAM_NONE )
    G_LogPrintf( "team: %i %i %i: %s" S_COLOR_WHITE " left the %ss\n",
      ent->s.number, newTeam, oldTeam, ent->client->pers.netname,
      BG_TeamName( oldTeam ) );
  else
  // joining a team from spectators
    G_LogPrintf( "team: %i %i %i: %s" S_COLOR_WHITE " joined the %ss\n",
      ent->s.number, newTeam, oldTeam, ent->client->pers.netname,
      BG_TeamName( newTeam ) );
}

/*
===========
Team_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
gentity_t *Team_GetLocation( gentity_t *ent )
{
  gentity_t   *eloc, *best;
  float       bestlen, len;
  vec3_t      origin;

  best = NULL;
  bestlen = 3.0f * 8192.0f * 8192.0f;

  VectorCopy( ent->r.currentOrigin, origin );

  for( eloc = level.locationHead; eloc; eloc = eloc->nextTrain )
  {
    len = ( origin[ 0 ] - eloc->r.currentOrigin[ 0 ] ) * ( origin[ 0 ] - eloc->r.currentOrigin[ 0 ] )
        + ( origin[ 1 ] - eloc->r.currentOrigin[ 1 ] ) * ( origin[ 1 ] - eloc->r.currentOrigin[ 1 ] )
        + ( origin[ 2 ] - eloc->r.currentOrigin[ 2 ] ) * ( origin[ 2 ] - eloc->r.currentOrigin[ 2 ] );

    if( len > bestlen )
      continue;

    if( !trap_InPVS( origin, eloc->r.currentOrigin ) )
      continue;

    bestlen = len;
    best = eloc;
  }

  return best;
}


/*
===========
Team_GetLocationMsg

Report a location message for the player. Uses placed nearby target_location entities
============
*/
qboolean Team_GetLocationMsg( gentity_t *ent, char *loc, int loclen )
{
  gentity_t *best;

  best = Team_GetLocation( ent );

  if( !best )
    return qfalse;

  if( best->count )
  {
    if( best->count < 0 )
      best->count = 0;

    if( best->count > 7 )
      best->count = 7;

    Com_sprintf( loc, loclen, "%c%c%s" S_COLOR_WHITE, Q_COLOR_ESCAPE, best->count + '0', best->message );
  }
  else
    Com_sprintf( loc, loclen, "%s", best->message );

  return qtrue;
}


/*---------------------------------------------------------------------------*/

static int QDECL SortClients( const void *a, const void *b )
{
  return *(int *)a - *(int *)b;
}


/*
==================
TeamplayLocationsMessage

Format:
  clientNum location health armor weapon misc

==================
*/
void TeamplayInfoMessage( gentity_t *ent )
{
  char      entry[ 1024 ];
  char      string[ 8192 ];
  int       stringlength;
  int       i, j;
  gentity_t *player;
  int       cnt;
  int       h, a = 0;
  int       clients[ TEAM_MAXOVERLAY ];

  if( ! ent->client->pers.teamInfo )
    return;

  // figure out what client should be on the display
  // we are limited to 8, but we want to use the top eight players
  // but in client order (so they don't keep changing position on the overlay)
  for( i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++ )
  {
    player = g_entities + level.sortedClients[ i ];

    if( player->inuse && player->client->ps.stats[ STAT_TEAM ] ==
        ent->client->ps.stats[ STAT_TEAM ] )
      clients[ cnt++ ] = level.sortedClients[ i ];
  }

  // We have the top eight players, sort them by clientNum
  qsort( clients, cnt, sizeof( clients[ 0 ] ), SortClients );

  // send the latest information on all clients
  string[ 0 ] = 0;
  stringlength = 0;

  for( i = 0, cnt = 0; i < g_maxclients.integer && cnt < TEAM_MAXOVERLAY; i++)
  {
    player = g_entities + i;

    if( player->inuse && player->client->ps.stats[ STAT_TEAM ] ==
        ent->client->ps.stats[ STAT_TEAM ] )
    {
      h = player->client->ps.stats[ STAT_HEALTH ];

      if( h < 0 )
        h = 0;

      Com_sprintf( entry, sizeof( entry ),
        " %i %i %i %i %i",
        i, player->client->pers.location, h, a,
        player->client->ps.weapon );

      j = strlen( entry );

      if( stringlength + j > sizeof( string ) )
        break;

      strcpy( string + stringlength, entry );
      stringlength += j;
      cnt++;
    }
  }

  trap_SendServerCommand( ent - g_entities, va( "tinfo %i %s", cnt, string ) );
}

void CheckTeamStatus( void )
{
  int i;
  gentity_t *loc, *ent;

  if( level.time - level.lastTeamLocationTime > TEAM_LOCATION_UPDATE_TIME )
  {
    level.lastTeamLocationTime = level.time;

    for( i = 0; i < g_maxclients.integer; i++ )
    {
      ent = g_entities + i;
      if( ent->client->pers.connected != CON_CONNECTED )
        continue;

      if( ent->inuse && ( ent->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS ||
                          ent->client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS ) )
      {

        loc = Team_GetLocation( ent );

        if( loc )
          ent->client->pers.location = loc->health;
        else
          ent->client->pers.location = 0;
      }
    }

    for( i = 0; i < g_maxclients.integer; i++ )
    {
      ent = g_entities + i;
      if( ent->client->pers.connected != CON_CONNECTED )
        continue;

      if( ent->inuse && ( ent->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS ||
                          ent->client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS ) )
        TeamplayInfoMessage( ent );
    }
  }

  if( g_tkmap.integer )
    return;
    
  if( g_teamImbalanceWarnings.integer && !level.intermissiontime 
      && level.time - level.lastTeamUnbalancedTime > ( g_teamImbalanceWarnings.integer * 1000 ) )
  {
    //Warn on unbalanced teams
    if( level.numTeamWarnings < 3 )
    {
      level.lastTeamUnbalancedTime = level.time;
      if( level.numAlienSpawns > 0 && level.numHumanClients - level.numAlienClients > 2
          && !level.alienTeamLocked && level.numAlienClients > 0 )
      {
        trap_SendServerCommand( -1, "print \"Teams are unbalanced. Humans have more players.\n Humans will keep their points when switching teams.\n\"");
        level.numTeamWarnings++;
      }
      else if( level.numHumanSpawns > 0 && level.numAlienClients - level.numHumanClients > 2
               && !level.humanTeamLocked && level.numHumanClients > 0 )
      {
        trap_SendServerCommand( -1, "print \"Teams are unbalanced. Aliens have more players.\n Aliens will keep their points when switching teams.\n\"");
        level.numTeamWarnings++;
      }
      else
        level.numTeamWarnings = 0;
    }
    else if( level.numTeamWarnings == 3 )
    {
      level.lastTeamUnbalancedTime = level.time;
      if( level.numAlienSpawns > 0 && level.numHumanClients - level.numAlienClients > 2
          && !level.alienTeamLocked && level.numAlienClients > 0 )
      {
        trap_SendServerCommand( -1, "print \"Teams are unbalanced. Humans have more players.\n After this warning all aliens will be given one evo until the balance is corrected.\n\"");
        level.numTeamWarnings++;
      }
      else if( level.numHumanSpawns > 0 && level.numAlienClients - level.numHumanClients > 2
                && !level.humanTeamLocked && level.numHumanClients > 0 )
      {
        trap_SendServerCommand( -1, "print \"Teams are unbalanced. Aliens have more players.\n After this warning all humans will be given 250 credits until the balance is corrected.\n\"");
        level.numTeamWarnings++;
      }
      else
        level.numTeamWarnings = 0;
    }
    else if( level.numTeamWarnings > 3 )
    {
      level.lastTeamUnbalancedTime = level.time;
      if( level.numAlienSpawns > 0 && level.numHumanClients - level.numAlienClients > 2
          && !level.alienTeamLocked && level.numAlienClients > 0 )
      {
        trap_SendServerCommand( -1, "print \"Teams are unbalanced. Humans have more players.\n All Aliens have been given 1 evo, this will continue until the balance is corrected.\n\"" );
        for( i = 0; i < MAX_CLIENTS; i++ )
        {
          if( level.clients[ i ].ps.stats[ STAT_TEAM ] == TEAM_ALIENS )
          {
            G_AddCreditToClient( &(level.clients[ i ] ), ALIEN_CREDITS_PER_FRAG, qtrue );
            trap_Cvar_Set( "g_alienCredits", va( "%d", g_alienCredits.integer + FREEKILL_ALIEN ) );
          }
        }
        level.numTeamWarnings = 2;
      }
      else if( level.numHumanSpawns > 0 && level.numAlienClients - level.numHumanClients > 2
                && !level.humanTeamLocked && level.numHumanClients > 0 )
      {
        int i;
        trap_SendServerCommand( -1, "print \"Teams are unbalanced. Aliens have more players.\n All Humans have been given 250 credits, this will continue until the balance is corrected.\n\"" );
        for( i = 0; i < MAX_CLIENTS; i++ )
        {
          if( level.clients[ i ].ps.stats[ STAT_TEAM ] == TEAM_HUMANS )
          {
            G_AddCreditToClient( &(level.clients[ i ] ), 250, qtrue );
            trap_Cvar_Set( "g_humanCredits", va( "%d", g_humanCredits.integer + 250 ) );
          }
        }
        level.numTeamWarnings = 2;
      }
      else
        level.numTeamWarnings = 0;
    }
  }
}
