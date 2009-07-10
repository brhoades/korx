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

// bg_misc.c -- both games misc functions, all completely stateless

#include "../qcommon/q_shared.h"
#include "bg_public.h"

int  trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void trap_FS_Read( void *buffer, int len, fileHandle_t f );
void trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void trap_FS_FCloseFile( fileHandle_t f );
void trap_FS_Seek( fileHandle_t f, long offset, fsOrigin_t origin ); // fsOrigin_t
int  trap_FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize );

static const buildableAttributes_t bg_buildableList[ ] =
{
  {
    BA_A_SPAWN,            //int       buildNum;
    "eggpod",              //char      *buildName;
    "Egg",                 //char      *humanName;
    "The most basic alien structure. It allows aliens to spawn "
      "and protect the Overmind. Without any of these, the Overmind "
      "is left nearly defenseless and defeat is imminent.",
    "team_alien_spawn",    //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    ASPAWN_BP,             //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    ASPAWN_HEALTH,         //int       health;
    ASPAWN_REGEN,          //int       regenRate;
    ASPAWN_SPLASHDAMAGE,   //int       splashDamage;
    ASPAWN_SPLASHRADIUS,   //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD )|( 1 << WP_ABUILD2 )|( 1 << WP_ABUILD3 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    ASPAWN_BT,             //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.5f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    ASPAWN_CREEPSIZE,      //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    ASPAWN_VALUE,          //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_A_OVERMIND,         //int       buildNum;
    "overmind",            //char      *buildName;
    "Overmind",            //char      *humanName;
    "A collective consciousness that controls all the alien structures "
      "in its vicinity. It must be protected at all costs, since its "
      "death will render alien structures defenseless.",
    "team_alien_overmind", //char      *entityName;
    { -45, -45, -15 },     //vec3_t    mins;
    { 45, 45, 95 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    OVERMIND_BP,           //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    OVERMIND_HEALTH,       //int       health;
    OVERMIND_REGEN,        //int       regenRate;
    OVERMIND_SPLASHDAMAGE, //int       splashDamage;
    OVERMIND_SPLASHRADIUS, //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD )|( 1 << WP_ABUILD2 )|( 1 << WP_ABUILD3 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    OVERMIND_ATTACK_REPEAT,//int       nextthink;
    OVERMIND_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    OVERMIND_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qtrue,                 //qboolean  uniqueTest;
    OVERMIND_VALUE,        //int       value;
    qtrue                //qboolean  replacable;
  },
  {
    BA_A_BARRICADE,        //int       buildNum;
    "barricade",           //char      *buildName;
    "Barricade",           //char      *humanName;
    "Used to obstruct corridors and doorways, hindering humans from "
      "threatening the spawns and Overmind. Barricades will shrink "
      "to allow aliens to pass over them, however.",
    "team_alien_barricade",//char      *entityName;
    { -35, -35, -15 },     //vec3_t    mins;
    { 35, 35, 60 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    BARRICADE_BP,          //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    BARRICADE_HEALTH,      //int       health;
    BARRICADE_REGEN,       //int       regenRate;
    BARRICADE_SPLASHDAMAGE,//int       splashDamage;
    BARRICADE_SPLASHRADIUS,//int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD )|( 1 << WP_ABUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    BARRICADE_BT,          //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    BARRICADE_CREEPSIZE,   //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    BARRICADE_VALUE,       //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_A_ACIDTUBE,         //int       buildNum;
    "acid_tube",           //char      *buildName;
    "Acid Tube",           //char      *humanName;
    "Ejects lethal poisonous acid at an approaching human. These "
      "are highly effective when used in conjunction with a trapper "
      "to hold the victim in place.",
    "team_alien_acid_tube",//char      *entityName;
    { -25, -25, -25 },     //vec3_t    mins;
    { 25, 25, 25 },        //vec3_t    maxs;
    -15.0f,                //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    ACIDTUBE_BP,           //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    ACIDTUBE_HEALTH,       //int       health;
    ACIDTUBE_REGEN,        //int       regenRate;
    ACIDTUBE_SPLASHDAMAGE, //int       splashDamage;
    ACIDTUBE_SPLASHRADIUS, //int       splashRadius;
    MOD_ATUBE, //FIXME: hmm //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD )|( 1 << WP_ABUILD2 ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    200,                   //int       nextthink;
    ACIDTUBE_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    ACIDTUBE_CREEPSIZE,    //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    ACIDTUBE_VALUE,        //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_A_TRAPPER,          //int       buildNum;
    "trapper",             //char      *buildName;
    "Trapper",             //char      *humanName;
    "Fires a blob of adhesive spit at any non-alien in its line of "
      "sight. This hinders their movement, making them an easy target "
      "for other defensive structures or aliens.",
    "team_alien_trapper",  //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 15 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    TRAPPER_BP,            //int       buildPoints;
    ( 1 << S2 )|( 1 << S3 ), //int  stages //NEEDS ADV BUILDER SO S2 AND UP
    TRAPPER_HEALTH,        //int       health;
    TRAPPER_REGEN,         //int       regenRate;
    TRAPPER_SPLASHDAMAGE,  //int       splashDamage;
    TRAPPER_SPLASHRADIUS,  //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD2 ),   //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    TRAPPER_BT,            //int       buildTime;
    qfalse,                //qboolean  usable;
    TRAPPER_RANGE,         //int       turretRange;
    TRAPPER_REPEAT,        //int       turretFireSpeed;
    WP_LOCKBLOB_LAUNCHER,  //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    TRAPPER_CREEPSIZE,     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qtrue,                 //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    TRAPPER_VALUE,         //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_A_BOOSTER,          //int       buildNum;
    "booster",             //char      *buildName;
    "Booster",             //char      *humanName;
    "Laces the attacks of any alien that touches it with a poison "
      "that will gradually deal damage to any humans exposed to it. "
      "The booster also increases the rate of health regeneration for "
      "any nearby aliens. Poisons humans that touch it. ",
    "team_alien_booster",  //char      *entityName;
    { -26, -26, -9 },     //vec3_t     mins;
    { 26, 26, 9 },        //vec3_t     maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    BOOSTER_BP,            //int       buildPoints;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    BOOSTER_HEALTH,        //int       health;
    BOOSTER_REGEN,         //int       regenRate;
    BOOSTER_SPLASHDAMAGE,  //int       splashDamage;
    BOOSTER_SPLASHRADIUS,  //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD2 )|( 1 << WP_ABUILD3 ), //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    BOOSTER_BT,            //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.707f,                //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    BOOSTER_CREEPSIZE,     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qtrue,                 //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    BOOSTER_VALUE,         //int       value;
    qtrue                //qboolean  replacable;
  },
  {
    BA_A_HIVE,             //int       buildNum;
    "hive",                //char      *buildName;
    "Hive",                //char      *humanName;
    "Houses millions of tiny insectoid aliens. When a human "
      "approaches this structure, the insectoids attack.",
    "team_alien_hive",     //char      *entityName;
    { -35, -35, -25 },     //vec3_t    mins;
    { 35, 35, 25 },        //vec3_t    maxs;
    -15.0f,                //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    HIVE_BP,               //int       buildPoints;
    ( 1 << S3 ),           //int  stages
    HIVE_HEALTH,           //int       health;
    HIVE_REGEN,            //int       regenRate;
    HIVE_SPLASHDAMAGE,     //int       splashDamage;
    HIVE_SPLASHRADIUS,     //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD2 ),   //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    500,                   //int       nextthink;
    HIVE_BT,               //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_HIVE,               //weapon_t  turretProjType;
    0.0f,                  //float     minNormal;
    qtrue,                 //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    HIVE_CREEPSIZE,        //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    HIVE_VALUE,            //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_A_HOVEL,            //int       buildNum;
    "hovel",               //char      *buildName;
    "Hovel",               //char      *humanName;
    "An armoured shell used by the Grangers to hide in, while "
      "the alien base is under attack. It may be entered or "
      "exited at any time.",
    "team_alien_hovel",    //char      *entityName;
    { -50, -50, -20 },     //vec3_t    mins;
    { 50, 50, 20 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    HOVEL_BP,              //int       buildPoints;
    ( 1 << S3 ),           //int  stages
    HOVEL_HEALTH,          //int       health;
    HOVEL_REGEN,           //int       regenRate;
    HOVEL_SPLASHDAMAGE,    //int       splashDamage;
    HOVEL_SPLASHRADIUS,    //int       splashRadius;
    MOD_ASPAWN,            //int       meansOfDeath;
    TEAM_ALIENS,           //int       team;
    ( 1 << WP_ABUILD2 )|( 1 << WP_ABUILD3 ), //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    150,                   //int       nextthink;
    HOVEL_BT,              //int       buildTime;
    qtrue,                 //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qtrue,                 //qboolean  creepTest;
    HOVEL_CREEPSIZE,       //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qtrue,                 //qboolean  uniqueTest;
    HOVEL_VALUE,           //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_H_SPAWN,            //int       buildNum;
    "telenode",            //char      *buildName;
    "Telenode",            //char      *humanName;
    "The most basic human structure. It provides a means for humans "
      "to enter the battle arena. Without any of these the humans "
      "cannot spawn and defeat is imminent.",
    "team_human_spawn",    //char      *entityName;
    { -40, -40, -4 },      //vec3_t    mins;
    { 40, 40, 4 },         //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    HSPAWN_BP,             //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    HSPAWN_HEALTH,         //int       health;
    HSPAWN_DCC_REGEN,      //int       regenRate;
    HSPAWN_SPLASHDAMAGE,   //int       splashDamage;
    HSPAWN_SPLASHRADIUS,   //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,           //int       team;
    ( 1 << WP_HBUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    HSPAWN_BT,             //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qtrue,                 //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    HSPAWN_VALUE,          //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_H_MGTURRET,         //int       buildNum;
    "mgturret",            //char      *buildName;
    "Machinegun Turret",   //char      *humanName;
    "Automated base defense that is effective against large targets "
      "but slow to begin firing. Should always be "
      "backed up by physical support.",
    "team_human_mgturret", //char      *entityName;
    { -25, -25, -20 },     //vec3_t    mins;
    { 25, 25, 20 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    MGTURRET_BP,           //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    MGTURRET_HEALTH,       //int       health;
    MGTURRET_DCC_REGEN,    //int       regenRate;
    MGTURRET_SPLASHDAMAGE, //int       splashDamage;
    MGTURRET_SPLASHRADIUS, //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,           //int       team;
    ( 1 << WP_HBUILD ),   //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    50,                    //int       nextthink;
    MGTURRET_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    MGTURRET_RANGE,        //int       turretRange;
    MGTURRET_REPEAT,       //int       turretFireSpeed;
    WP_MGTURRET,           //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qtrue,                 //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    MGTURRET_VALUE,        //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_H_TESLAGEN,         //int       buildNum;
    "tesla",               //char      *buildName;
    "Tesla Generator",     //char      *humanName;
    "A structure equipped with a strong electrical attack that fires "
      "instantly and always hits its target. It is effective against smaller "
      "aliens and for consolidating basic defense.",
    "team_human_tesla",    //char      *entityName;
    { -22, -22, -40 },     //vec3_t    mins;
    { 22, 22, 40 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    TESLAGEN_BP,           //int       buildPoints;
    ( 1 << S3 ),           //int       stages
    TESLAGEN_HEALTH,       //int       health;
    TESLAGEN_DCC_REGEN,    //int       regenRate;
    TESLAGEN_SPLASHDAMAGE, //int       splashDamage;
    TESLAGEN_SPLASHRADIUS, //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,           //int       team;
    ( 1 << WP_HBUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    150,                   //int       nextthink;
    TESLAGEN_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    TESLAGEN_RANGE,        //int       turretRange;
    TESLAGEN_REPEAT,       //int       turretFireSpeed;
    WP_TESLAGEN,           //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qtrue,                 //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    TESLAGEN_VALUE,        //int       value;
    qfalse                //qboolean  replacable;
  },
  {
    BA_H_ARMOURY,          //int       buildNum;
    "arm",                 //char      *buildName;
    "Armoury",             //char      *humanName;
    "An essential part of the human base, providing a means "
      "to upgrade the basic human equipment. A range of upgrades "
      "and weapons are available for sale from the armoury.",
    "team_human_armoury",  //char      *entityName;
    { -40, -40, -13 },     //vec3_t    mins;
    { 40, 40, 50 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    ARMOURY_BP,            //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    ARMOURY_HEALTH,        //int       health;
    ARMOURY_DCC_REGEN,     //int       regenRate;
    ARMOURY_SPLASHDAMAGE,  //int       splashDamage;
    ARMOURY_SPLASHRADIUS,  //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,           //int       team;
    ( 1 << WP_HBUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    ARMOURY_BT,            //int       buildTime;
    qtrue,                 //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    ARMOURY_VALUE,         //int       value;
    qtrue                //qboolean  replacable;
  },
  {
    BA_H_DCC,              //int       buildNum;
    "dcc",                 //char      *buildName;
    "Defence Computer",    //char      *humanName;
    "A structure that enables self-repair functionality in "
      "human structures. Each Defence Computer built "
      "DOES NOT increase repair rate.",
    "team_human_dcc",      //char      *entityName;
    { -35, -35, -13 },     //vec3_t    mins;
    { 35, 35, 47 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    DC_BP,                 //int       buildPoints;
    ( 1 << S2 )|( 1 << S3 ), //int       stages
    DC_HEALTH,             //int       health;
    DC_DCC_REGEN,          //int       regenRate;
    DC_SPLASHDAMAGE,       //int       splashDamage;
    DC_SPLASHRADIUS,       //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,           //int       team;
    ( 1 << WP_HBUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    DC_BT,                 //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    DC_VALUE,              //int       value;
    qtrue                //qboolean  replacable;
  },
  {
    BA_H_MEDISTAT,         //int       buildNum;
    "medistat",            //char      *buildName;
    "Medistation",         //char      *humanName;
    "A structure that automatically restores "
      "the health and stamina of any human that stands on it. "
      "It may only be used by one person at a time. This structure "
      "also issues medkits.",
    "team_human_medistat", //char      *entityName;
    { -35, -35, -7 },      //vec3_t    mins;
    { 35, 35, 7 },         //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    MEDISTAT_BP,           //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    MEDISTAT_HEALTH,       //int       health;
    MEDISTAT_DCC_REGEN,    //int       regenRate;
    MEDISTAT_SPLASHDAMAGE, //int       splashDamage;
    MEDISTAT_SPLASHRADIUS, //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,           //int       team;
    ( 1 << WP_HBUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    MEDISTAT_BT,           //int       buildTime;
    qfalse,                //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qtrue,                 //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    MEDISTAT_VALUE,        //int       value;
    qtrue                //qboolean  replacable;
  },
  {
    BA_H_REACTOR,          //int       buildNum;
    "reactor",             //char      *buildName;
    "Reactor",             //char      *humanName;
    "All structures except the telenode rely on a reactor to operate. "
      "The reactor provides power for all the human structures either "
      "directly or via repeaters. Only one reactor can be built at a time.",
    "team_human_reactor",  //char      *entityName;
    { -50, -50, -15 },     //vec3_t    mins;
    { 50, 50, 95 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    REACTOR_BP,            //int       buildPoints;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    REACTOR_HEALTH,        //int       health;
    REACTOR_DCC_REGEN,     //int       regenRate;
    REACTOR_SPLASHDAMAGE,  //int       splashDamage;
    REACTOR_SPLASHRADIUS,  //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,           //int       team;
    ( 1 << WP_HBUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    REACTOR_ATTACK_DCC_REPEAT, //int       nextthink;
    REACTOR_BT,            //int       buildTime;
    qtrue,                 //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qtrue,                 //qboolean  uniqueTest;
    REACTOR_VALUE,         //int       value;
    qtrue                  //qboolean  replacable;
  },
  {
    BA_H_REPEATER,         //int       buildNum;
    "repeater",            //char      *buildName;
    "Repeater",            //char      *humanName;
    "A power distributor that transmits power from the reactor "
      "to remote locations, so that bases may be built far "
      "from the reactor.",
    "team_human_repeater", //char      *entityName;
    { -15, -15, -15 },     //vec3_t    mins;
    { 15, 15, 25 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    REPEATER_BP,           //int       buildPoints;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    REPEATER_HEALTH,       //int       health;
    REPEATER_DCC_REGEN,    //int       regenRate;
    REPEATER_SPLASHDAMAGE, //int       splashDamage;
    REPEATER_SPLASHRADIUS, //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,           //int       team;
    ( 1 << WP_HBUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    REPEATER_BT,           //int       buildTime;
    qtrue,                 //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  uniqueTest;
    REPEATER_VALUE,        //int       value;
    qtrue                  //qboolean  replacable;
  },
  {
    BA_H_FORCEFIELD,         //int       buildNum;
    "forcefield",            //char      *buildName;
    "Force Field",            //char      *humanName;
    "A barrier that blocks attacking aliens, but permits humans"
    " to pass through. Using the Q key causes the forcefield to lower.",
    "team_human_forcefield", //char      *entityName;
//     { "models/buildables/ffield/ffield.md3", 0, 0, 0 },
//     1.0f,                  //float     modelScale;
    { -20, -20, 0 },     //vec3_t    mins;
    { 20, 20, 60 },        //vec3_t    maxs;
    0.0f,                  //float     zOffset;
    TR_GRAVITY,            //trType_t  traj;
    0.0,                   //float     bounce;
    FORCEFIELD_BP,           //int       buildPoints;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    FORCEFIELD_HEALTH,       //int       health;
    FORCEFIELD_DCC_REGEN,      //int       regenRate;
    FORCEFIELD_SPLASHDAMAGE, //int       splashDamage;
    FORCEFIELD_SPLASHRADIUS, //int       splashRadius;
    MOD_HSPAWN,            //int       meansOfDeath;
    TEAM_HUMANS,            //int       team;
    ( 1 << WP_HBUILD ),    //weapon_t  buildWeapon;
    BANIM_IDLE1,           //int       idleAnim;
    100,                   //int       nextthink;
    FORCEFIELD_BT,           //int       buildTime;
    qfalse,                 //qboolean  usable;
    0,                     //int       turretRange;
    0,                     //int       turretFireSpeed;
    WP_NONE,               //weapon_t  turretProjType;
    0.95f,                 //float     minNormal;
    qfalse,                //qboolean  invertNormal;
    qfalse,                //qboolean  creepTest;
    0,                     //int       creepSize;
    qfalse,                //qboolean  dccTest;
    qfalse,                //qboolean  transparentTest;
    qfalse,                //qboolean  reactorTest;
    FORCEFIELD_VALUE,        //int       value;
    qfalse                  //qboolean  replacable;
  }
};

int   bg_numBuildables = sizeof( bg_buildableList ) / sizeof( bg_buildableList[ 0 ] );

static const buildableAttributes_t nullBuildable = { 0 };

//separate from bg_buildableList to work around char struct init bug
buildableAttributeOverrides_t bg_buildableOverrideList[ BA_NUM_BUILDABLES ];
/*
==============
BG_BuildableByName
==============
*/
const buildableAttributes_t *BG_BuildableByName( const char *name )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( !Q_stricmp( bg_buildableList[ i ].name, name ) )
      return &bg_buildableList[ i ];
  }

  return &nullBuildable;
}

/*
==============
BG_FindHumanNameForBuildNum
==============
*/
char *BG_FindHumanNameForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
      return bg_buildableList[ i ].humanName;
  }

  //wimp out
  return 0;
}

/*
==============
BG_FindNameForBuildNum
==============
*/
char *BG_FindNameForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
      return bg_buildableList[ i ].name;
  }

  //wimp out
  return 0;
}

/*
==============
BG_FindHealthForBuildable
==============
*/
int BG_FindHealthForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
    {
      return bg_buildableList[ i ].health;
    }
  }

  return 1000;
}

/*
==============
BG_FindBBoxForBuildable
==============
*/
void BG_FindBBoxForBuildable( int bclass, vec3_t mins, vec3_t maxs )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
    {
      if( mins != NULL )
      {
        VectorCopy( bg_buildableList[ i ].mins, mins );

        if( VectorLength( bg_buildableOverrideList[ bclass ].mins ) )
          VectorCopy( bg_buildableOverrideList[ bclass ].mins, mins );
      }

      if( maxs != NULL )
      {
        VectorCopy( bg_buildableList[ i ].maxs, maxs );

        if( VectorLength( bg_buildableOverrideList[ bclass ].maxs ) )
          VectorCopy( bg_buildableOverrideList[ bclass ].maxs, maxs );
      }

      return;
    }
  }

  if( mins != NULL )
    VectorCopy( bg_buildableList[ 0 ].mins, mins );

  if( maxs != NULL )
    VectorCopy( bg_buildableList[ 0 ].maxs, maxs );
}

/*
==============
BG_FindNextThinkForBuildable
==============
*/
int BG_FindNextThinkForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
    {
      return bg_buildableList[ i ].nextthink;
    }
  }

  return 100;
}

/*
==============
BG_FindTeamForBuildable
==============
*/
int BG_FindTeamForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
    {
      return bg_buildableList[ i ].team;
    }
  }

  return BIT_NONE;
}

/*
==============
BG_FindUniqueTestForBuildable
==============
*/
int BG_FindUniqueTestForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
    {
      return bg_buildableList[ i ].uniqueTest;
    }
  }

  return qfalse;
}

/*
==============
BG_FindReplaceableTestForBuildable
==============
*/
qboolean BG_FindReplaceableTestForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
    {
      return bg_buildableList[ i ].replaceable;
    }
  }
  return qfalse;
}

/*
==============
BG_FindBuildPointsForBuildable
==============
*/
int BG_FindBuildPointsForBuildable( int bclass )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( bg_buildableList[ i ].number == bclass )
    {
      return bg_buildableList[ i ].buildPoints;
    }
  }

  return 1000;
}

/*
==============
BG_BuildableByEntityName
==============
*/
const buildableAttributes_t *BG_BuildableByEntityName( const char *name )
{
  int i;

  for( i = 0; i < bg_numBuildables; i++ )
  {
    if( !Q_stricmp( bg_buildableList[ i ].entityName, name ) )
      return &bg_buildableList[ i ];
  }

  return &nullBuildable;
}

/*
==============
BG_Buildable
==============
*/
const buildableAttributes_t *BG_Buildable( buildable_t buildable )
{
  return ( buildable > BA_NONE && buildable < BA_NUM_BUILDABLES ) ?
    &bg_buildableList[ buildable - 1 ] : &nullBuildable;
}

/*
==============
BG_BuildableAllowedInStage
==============
*/
qboolean BG_BuildableAllowedInStage( buildable_t buildable,
                                     stage_t stage )
{
  int stages = BG_Buildable( buildable )->stages;

  if( stages & ( 1 << stage ) )
    return qtrue;
  else
    return qfalse;
}

static buildableConfig_t bg_buildableConfigList[ BA_NUM_BUILDABLES ];

/*
==============
BG_BuildableConfig
==============
*/
buildableConfig_t *BG_BuildableConfig( buildable_t buildable )
{
  return &bg_buildableConfigList[ buildable ];
}

/*
==============
BG_BuildableBoundingBox
==============
*/
void BG_BuildableBoundingBox( buildable_t buildable,
                              vec3_t mins, vec3_t maxs )
{
  buildableConfig_t *buildableConfig = BG_BuildableConfig( buildable );

  if( mins != NULL )
    VectorCopy( buildableConfig->mins, mins );

  if( maxs != NULL )
    VectorCopy( buildableConfig->maxs, maxs );
}

/*
======================
BG_ParseBuildableFile

Parses a configuration file describing a buildable
======================
*/
static qboolean BG_ParseBuildableFile( const char *filename, buildableConfig_t *bc )
{
  char          *text_p;
  int           i;
  int           len;
  char          *token;
  char          text[ 20000 ];
  fileHandle_t  f;
  float         scale;
  int           defined = 0;
  enum
  {
      MODEL         = 1 << 0,
      MODELSCALE    = 1 << 1,
      MINS          = 1 << 2,
      MAXS          = 1 << 3,
      ZOFFSET       = 1 << 4
  };


  // load the file
  len = trap_FS_FOpenFile( filename, &f, FS_READ );
  if( len < 0 )
  {
    Com_Printf( S_COLOR_RED "ERROR: Buildable file %s doesn't exist\n", filename );
    return qfalse;
  }

  if( len == 0 || len >= sizeof( text ) - 1 )
  {
    trap_FS_FCloseFile( f );
    Com_Printf( S_COLOR_RED "ERROR: Buildable file %s is %s\n", filename,
      len == 0 ? "empty" : "too long" );
    return qfalse;
  }

  trap_FS_Read( text, len, f );
  text[ len ] = 0;
  trap_FS_FCloseFile( f );

  // parse the text
  text_p = text;

  // read optional parameters
  while( 1 )
  {
    token = COM_Parse( &text_p );

    if( !token )
      break;

    if( !Q_stricmp( token, "" ) )
      break;

    if( !Q_stricmp( token, "model" ) )
    {
      int index = 0;

      token = COM_Parse( &text_p );
      if( !token )
        break;

      index = atoi( token );

      if( index < 0 )
        index = 0;
      else if( index > 3 )
        index = 3;

      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( bc->models[ index ], token, sizeof( bc->models[ 0 ] ) );

      defined |= MODEL;
      continue;
    }
    else if( !Q_stricmp( token, "modelScale" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      scale = atof( token );

      if( scale < 0.0f )
        scale = 0.0f;

      bc->modelScale = scale;

      defined |= MODELSCALE;
      continue;
    }
    else if( !Q_stricmp( token, "mins" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        bc->mins[ i ] = atof( token );
      }

      defined |= MINS;
      continue;
    }
    else if( !Q_stricmp( token, "maxs" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        bc->maxs[ i ] = atof( token );
      }

      defined |= MAXS;
      continue;
    }
    else if( !Q_stricmp( token, "zOffset" ) )
    {
      float offset;

      token = COM_Parse( &text_p );
      if( !token )
        break;

      offset = atof( token );

      bc->zOffset = offset;

      defined |= ZOFFSET;
      continue;
    }


    Com_Printf( S_COLOR_RED "ERROR: unknown token '%s'\n", token );
    return qfalse;
  }

  if(      !( defined & MODEL      ) )  token = "model";
  else if( !( defined & MODELSCALE ) )  token = "modelScale";
  else if( !( defined & MINS       ) )  token = "mins";
  else if( !( defined & MAXS       ) )  token = "maxs";
  else if( !( defined & ZOFFSET    ) )  token = "zOffset";
  else                                  token = "";

  if( strlen( token ) > 0 )
  {
      Com_Printf( S_COLOR_RED "ERROR: %s not defined in %s\n",
                  token, filename );
      return qfalse;
  }

  return qtrue;
}

/*
===============
BG_InitBuildableConfigs
===============
*/
void BG_InitBuildableConfigs( void )
{
  int               i;
  buildableConfig_t *bc;

  for( i = BA_NONE + 1; i < BA_NUM_BUILDABLES; i++ )
  {
    bc = BG_BuildableConfig( i );
    Com_Memset( bc, 0, sizeof( buildableConfig_t ) );

    BG_ParseBuildableFile( va( "configs/buildables/%s.cfg",
                               BG_Buildable( i )->name ), bc );
  }
}

////////////////////////////////////////////////////////////////////////////////

static const classAttributes_t bg_classList[ ] =
{
  {
    PCL_NONE,                                       //int     classnum;
    "spectator",                                    //char    *className;
    "",
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ),            //int  stages
    0,                                              //int     health;
    0.0f,                                           //float   fallDamage;
    0,                                              //int     regenRate;
    0,                                              //int     regenRateMsec;
    0,                                              //int     abilities;
    WP_NONE,                                        //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.000f,                                         //float   bob;
    1.0f,                                           //float   bobCycle;
    0,                                              //int     steptime;
    600,                                            //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    0,                                              //int     cost;
    0                                               //int     value;
  },
  {
    PCL_ALIEN_BUILDER0,                             //int     classnum;
    "builder",                                      //char    *className;
    "Responsible for building and maintaining all the alien structures. "
      "Has a weak melee slash attack.",
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ),            //int  stages
    ABUILDER_HEALTH,                                //int     health;
    0.2f,                                           //float   fallDamage;
    ABUILDER_REGEN,                                 //int     regenRate;
    ABUILDER_REGEN_MSEC,                            //int     regenRateMsec;
    SCA_TAKESFALLDAMAGE|SCA_FOVWARPS|SCA_ALIENSENSE,//int     abilities;
    WP_ABUILD,                                      //weapon_t  startWeapon
    95.0f,                                          //float   buildDist;
    80,                                             //int     fov;
    0.001f,                                         //float   bob;
    2.0f,                                           //float   bobCycle;
    150,                                            //int     steptime;
    ABUILDER_SPEED,                                 //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    195.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_ALIEN_BUILDER0_UPG, PCL_ALIEN_LEVEL1, PCL_ALIEN_BGRANGER },//int     children[ 3 ];
    ABUILDER_COST,                                  //int     cost;
    ABUILDER_VALUE                                  //int     value;
  },
  {
    PCL_ALIEN_BUILDER0_UPG,                         //int     classnum;
    "builderupg",                                   //char    *classname;
    "Similar to the base Granger, except that in addition to "
      "being able to build structures it has a spit attack "
      "that slows victims and the ability to crawl on walls.",
    ( 1 << S2 )|( 1 << S3 ),                        //int  stages
    ABUILDER_UPG_HEALTH,                            //int     health;
    0.2f,                                           //float   fallDamage;
    ABUILDER_UPG_REGEN,                             //int     regenRate;
    ABUILDER_UPG_REGEN_MSEC,                        //int     regenRateMsec;
    SCA_TAKESFALLDAMAGE|SCA_FOVWARPS|SCA_WALLCLIMBER|SCA_ALIENSENSE,    //int     abilities;
    WP_ABUILD2,                                     //weapon_t  startWeapon
    105.0f,                                         //float   buildDist;
    110,                                            //int     fov;
    0.001f,                                         //float   bob;
    2.0f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    ABUILDER_UPG_SPEED,                             //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL1, PCL_ALIEN_BGRANGER, PCL_NONE },//int     children[ 3 ];
    ABUILDER_UPG_COST,                              //int     cost;
    ABUILDER_UPG_VALUE                              //int     value;
  },
  {
    PCL_ALIEN_LEVEL0,                               //int     classnum;
    "level0",                                       //char    *classname;
    "Has a lethal reflexive bite and the ability to crawl on "
      "walls and ceilings.",
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ),            //int  stages
    LEVEL0_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL0_REGEN,                                   //int     regenRate;
    LEVEL0_REGEN_MSEC,                              //int     regenRateMsec;
    SCA_WALLCLIMBER|SCA_FOVWARPS|SCA_ALIENSENSE,    //int     abilities;
    WP_ALEVEL0,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    140,                                            //int     fov;
    0.0f,                                           //float   bob;
    2.5f,                                           //float   bobCycle;
    25,                                             //int     steptime;
    LEVEL0_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    400.0f,                                         //float   stopSpeed;
    250.0f,                                         //float   jumpMagnitude;
    2.0f,                                           //float   knockbackScale;
    { PCL_ALIEN_BUILDER0, PCL_ALIEN_LEVEL1, PCL_ALIEN_LEVEL0_UPG },       //int     children[ 3 ];
    LEVEL0_COST,                                    //int     cost;
    LEVEL0_VALUE                                    //int     value;
  },
  {
    PCL_ALIEN_LEVEL0_UPG,                           //int     classnum;
    "level0upg",                                    //char    *classname;
    "A more leathal mutaion of the dretch. Known to be a carrier of a pathogen "
    "which humans are prone to contracting. Has the ability to pounce.",  //char    *humanname;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ),            //int  stages
    LEVEL0_UPG_HEALTH,                              //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL0_UPG_REGEN,                               //int     regenRate;
    LEVEL0_UPG_REGEN_MSEC,                          //int     regenRateMsec;
    SCA_WALLCLIMBER|SCA_FOVWARPS|SCA_ALIENSENSE,    //int     abilities;
    WP_ALEVEL0_UPG,                                 //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    140,                                            //int     fov;
    0.0f,                                           //float   bob;
    2.5f,                                           //float   bobCycle;
    25,                                             //int     steptime;
    LEVEL0_UPG_SPEED,                               //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    400.0f,                                         //float   stopSpeed;
    250.0f,                                         //float   jumpMagnitude;
    2.0f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL1, PCL_ALIEN_BUILDER0, PCL_NONE },       //int     children[ 3 ];
    LEVEL0_UPG_COST,                                //int     cost;
    LEVEL0_UPG_VALUE                                //int     value;
//     "jumper",                                       //char    *modelname;
//     0.2f,                                           //float   modelScale;
//     "default",                                      //char    *skinname;
//     0.3f,                                           //float   shadowScale;
//     "alien_general_hud",                            //char    *hudname;
//     { -15, -15, -15 },                              //vec3_t  mins;
//     { 15, 15, 15 },                                 //vec3_t  maxs;
//     { 15, 15, 15 },                                 //vec3_t  crouchmaxs;
//     { -15, -15, -4 },                               //vec3_t  deadmins;
//     { 15, 15, 4 },                                  //vec3_t  deadmaxs;
//     -8.0f,                                          //float   zOffset
//     0, 0,                                           //int     viewheight, crouchviewheight;
  },
  {
    PCL_ALIEN_LEVEL1,                               //int     classnum;
    "level1",                                       //char    *classname;
    "A support class able to crawl on walls and ceilings. Its melee "
      "attack is most effective when combined with the ability to grab "
      "and hold its victims in place. Provides a weak healing aura "
      "that accelerates the healing rate of nearby aliens.",
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ),            //int  stages
    LEVEL1_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL1_REGEN,                                   //int     regenRate;
    LEVEL1_REGEN_MSEC,                              //int     regenRateMsec;
    SCA_FOVWARPS|SCA_WALLCLIMBER|SCA_ALIENSENSE,    //int     abilities;
    WP_ALEVEL1,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    120,                                            //int     fov;
    0.001f,                                         //float   bob;
    1.8f,                                           //float   bobCycle;
    60,                                             //int     steptime;
    LEVEL1_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    300.0f,                                         //float   stopSpeed;
    310.0f,                                         //float   jumpMagnitude;
    1.2f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL2, PCL_ALIEN_LEVEL1_UPG, PCL_NONE },   //int     children[ 3 ];
    LEVEL1_COST,                                     //int     cost;
    LEVEL1_VALUE                                     //int     value;
  },
  {
    PCL_ALIEN_LEVEL1_UPG,                           //int     classnum;
    "level1upg",                                    //char    *classname;
    "In addition to the basic Basilisk abilities, the Advanced "
      "Basilisk sprays a poisonous gas which disorients any "
      "nearby humans. Has a strong healing aura that "
      "that accelerates the healing rate of nearby aliens.",
    ( 1 << S2 )|( 1 << S3 ),                        //int  stages
    LEVEL1_UPG_HEALTH,                              //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL1_UPG_REGEN,                               //int     regenRate;
    LEVEL1_UPG_REGEN_MSEC,                          //int     regenRateMsec;
    SCA_FOVWARPS|SCA_WALLCLIMBER|SCA_ALIENSENSE,    //int     abilities;
    WP_ALEVEL1_UPG,                                 //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    120,                                            //int     fov;
    0.001f,                                         //float   bob;
    1.8f,                                           //float   bobCycle;
    60,                                             //int     steptime;
    LEVEL1_UPG_SPEED,                               //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    300.0f,                                         //float   stopSpeed;
    310.0f,                                         //float   jumpMagnitude;
    1.1f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL2, PCL_NONE, PCL_NONE },       //int     children[ 3 ];
    LEVEL1_UPG_COST,                                //int     cost;
    LEVEL1_UPG_VALUE                                //int     value;
  },
  {
    PCL_ALIEN_LEVEL2,                               //int     classnum;
    "level2",                                       //char    *classname;
    "Has a melee attack and the ability to jump off walls. This "
      "allows the Marauder to gather great speed in enclosed areas.",
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ),            //int  stages
    LEVEL2_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL2_REGEN,                                   //int     regenRate;
    LEVEL2_REGEN_MSEC,                              //int     regenRateMsec;
    SCA_WALLJUMPER|SCA_FOVWARPS|SCA_ALIENSENSE,     //int     abilities;
    WP_ALEVEL2,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.001f,                                         //float   bob;
    1.5f,                                           //float   bobCycle;
    80,                                             //int     steptime;
    LEVEL2_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    2.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    380.0f,                                         //float   jumpMagnitude;
    0.8f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL3, PCL_ALIEN_LEVEL2_UPG, PCL_NONE },   //int     children[ 3 ];
    LEVEL2_COST,                                    //int     cost;
    LEVEL2_VALUE                                    //int     value;
  },
  {
    PCL_ALIEN_LEVEL2_UPG,                           //int     classnum;
    "level2upg",                                    //char    *classname;
    "The Advanced Marauder has all the abilities of the basic Marauder "
      "with the addition of an area effect electric shock attack and three "
      "launchable barbs.",
    ( 1 << S2 )|( 1 << S3 ),                        //int  stages
    LEVEL2_UPG_HEALTH,                              //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL2_UPG_REGEN,                               //int     regenRate;
    LEVEL2_UPG_REGEN_MSEC,                          //int     regenRateMsec;
    SCA_WALLJUMPER|SCA_FOVWARPS|SCA_ALIENSENSE,     //int     abilities;
    WP_ALEVEL2_UPG,                                 //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.001f,                                         //float   bob;
    1.5f,                                           //float   bobCycle;
    80,                                             //int     steptime;
    LEVEL2_UPG_SPEED,                               //float   speed;
    10.0f,                                          //float   acceleration;
    2.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    380.0f,                                         //float   jumpMagnitude;
    0.7f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL3, PCL_NONE, PCL_NONE },       //int     children[ 3 ];
    LEVEL2_UPG_COST,                                //int     cost;
    LEVEL2_UPG_VALUE                                //int     value;
  },
  {
    PCL_ALIEN_LEVEL3,                               //int     classnum;
    "level3",                                       //char    *classname;
    "Possesses a melee attack and the pounce ability, which may "
      "be used as both an attack and a means to reach remote "
      "locations inaccessible from the ground.",
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ),            //int  stages
    LEVEL3_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL3_REGEN,                                   //int     regenRate;
    LEVEL3_REGEN_MSEC,                              //int     regenRateMsec;
    SCA_FOVWARPS|SCA_ALIENSENSE,                    //int     abilities;
    WP_ALEVEL3,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    110,                                            //int     fov;
    0.0005f,                                        //float   bob;
    1.3f,                                           //float   bobCycle;
    90,                                             //int     steptime;
    LEVEL3_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    200.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    0.5f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL4, PCL_ALIEN_LEVEL3_UPG, PCL_NONE },   //int     children[ 3 ];
    LEVEL3_COST,                                    //int     cost;
    LEVEL3_VALUE                                    //int     value;
  },
  {
    PCL_ALIEN_LEVEL3_UPG,                           //int     classnum;
    "level3upg",                                    //char    *classname;
    "In addition to the basic Dragoon abilities, the Advanced "
      "Dragoon has 3 barbs which may be used to attack humans "
      "from a distance.",
    ( 1 << S2 )|( 1 << S3 ),                                    //int  stages
    LEVEL3_UPG_HEALTH,                              //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL3_UPG_REGEN,                               //int     regenRate;
    LEVEL3_UPG_REGEN_MSEC,                          //int     regenRateMsec;
    SCA_FOVWARPS|SCA_ALIENSENSE,                    //int     abilities;
    WP_ALEVEL3_UPG,                                 //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    110,                                            //int     fov;
    0.0005f,                                        //float   bob;
    1.3f,                                           //float   bobCycle;
    90,                                             //int     steptime;
    LEVEL3_UPG_SPEED,                               //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    200.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    0.4f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL4, PCL_NONE, PCL_NONE },       //int     children[ 3 ];
    LEVEL3_UPG_COST,                                //int     cost;
    LEVEL3_UPG_VALUE                                //int     value;
  },
  {
    PCL_ALIEN_LEVEL4,                               //int     classnum;
    "level4",                                       //char    *classname;
    "A large alien with a strong melee attack, this class can "
      "also charge at enemy humans and structures, inflicting "
      "great damage. Any humans or their structures caught under "
      "a falling Tyrant will be crushed by its weight.",
    ( 1 << S3 ),                                    //int  stages
    LEVEL4_HEALTH,                                  //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL4_REGEN,                                   //int     regenRate;
    LEVEL4_REGEN_MSEC,                              //int     regenRateMsec;
    SCA_FOVWARPS|SCA_ALIENSENSE,                    //int     abilities;
    WP_ALEVEL4,                                     //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.001f,                                         //float   bob;
    1.1f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    LEVEL4_SPEED,                                   //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    170.0f,                                         //float   jumpMagnitude;
    0.1f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL4_UPG, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    LEVEL4_COST,                                    //int     cost;
    LEVEL4_VALUE                                    //int     value;
  },
  {
    PCL_ALIEN_LEVEL4_UPG,                           //int     classnum;
    "level4upg",                                    //char    *classname;
    "Stronger and more deadly than the normal Tyrant. The Advanced Tyrant "
    "has a healing aura and can lob an acid blob which explodes on impact",
    ( 1 << S3 ),                                    //int  stages
    LEVEL4_UPG_HEALTH,                              //int     health;
    0.0f,                                           //float   fallDamage;
    LEVEL4_UPG_REGEN,                               //int     regenRate;
    LEVEL4_UPG_REGEN_MSEC,                          //int     regenRateMsec;
    SCA_FOVWARPS|SCA_ALIENSENSE,                    //int     abilities;
    WP_ALEVEL4_UPG,                                 //weapon_t  startWeapon
    0.0f,                                           //float   buildDist;
    90,                                             //int     fov;
    0.001f,                                         //float   bob;
    1.1f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    LEVEL4_UPG_SPEED,                               //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    230.0f,                                         //float   jumpMagnitude;
    0.1f,                                           //float   knockbackScale;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    LEVEL4_UPG_COST,                                //int     cost;
    LEVEL4_UPG_VALUE                                //int     value;
    
    
//     1.0f,                                           //float   modelScale;
//     "default",                                      //char    *skinname;
//     2.0f,                                           //float   shadowScale;
//     "alien_general_hud",                            //char    *hudname;
//     { -32, -32, -22 },                              //vec3_t  mins;
//     { 32, 32, 70 },                                 //vec3_t  maxs;
//     { 32, 32, 70 },                                 //vec3_t  crouchmaxs;
//     { -32, -32, -34 },                               //vec3_t  deadmins;
//     { 32, 32, 34 },                                  //vec3_t  deadmaxs;
//     0.0f,                                           //float   zOffset
//     64, 64,                                         //int     viewheight, crouchviewheight;
  },
  {
    PCL_HUMAN,                                      //int     classnum;
    "human_base",                                   //char    *classname;
    "",
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ),            //int  stages
    100,                                            //int     health;
    1.0f,                                           //float   fallDamage;
    0,                                              //int     regenRate;
    0,                                              //int     regenRateMsec;
    SCA_TAKESFALLDAMAGE|SCA_CANUSELADDERS,          //int     abilities;
    WP_NONE, //special-cased in g_client.c          //weapon_t  startWeapon
    110.0f,                                         //float   buildDist;
    90,                                             //int     fov;
    0.002f,                                         //float   bob;
    1.0f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    1.0f,                                           //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    220.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    0,                                              //int     cost;
    ALIEN_CREDITS_PER_FRAG                          //int     value;
  },
  {
    PCL_HUMAN_BSUIT,                                //int     classnum;
    "human_bsuit",                                  //char    *classname;
    "",
    ( 1 << S3 ),                                    //int  stages
    100,                                            //int     health;
    1.0f,                                           //float   fallDamage;
    0,                                              //int     regenRate;
    0,                                              //int     regenRateMsec;
    SCA_TAKESFALLDAMAGE|
      SCA_CANUSELADDERS,                            //int     abilities;
    WP_NONE, //special-cased in g_client.c          //weapon_t  startWeapon
    110.0f,                                         //float   buildDist;
    90,                                             //int     fov;
    0.002f,                                         //float   bob;
    1.0f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    1.0f,                                           //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    220.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_NONE, PCL_NONE, PCL_NONE },               //int     children[ 3 ];
    0,                                              //int     cost;
    ALIEN_CREDITS_PER_FRAG                          //int     value;
  },
  {
    PCL_ALIEN_BGRANGER,                             //int     classnum;
    "bgranger",                                     //char    *classname;
    "A granger that has specialized in the desolation of human teams. "
      "Its increased HP, unique camouflage, and stronger attacks make it a foe to fear. "
      "Its sticky blobs provide the perfect way to leave any human prone.",
    ( 1 << S3 ),                                    //int  stages
    ABGRANGER_HEALTH,                           //int     health;
    0.0f,                                           //float   fallDamage;
    ABGRANGER_REGEN,                            //int     regenRate;
    ABGRANGER_REGEN_MSEC,                       //int     regenRateMsec;
    SCA_FOVWARPS|SCA_WALLCLIMBER|SCA_ALIENSENSE,    //int     abilities;
    WP_ABUILD3,                                     //weapon_t  startWeapon
    105.0f,                                         //float   buildDist;
    110,                                            //int     fov;
    0.001f,                                         //float   bob;
    2.0f,                                           //float   bobCycle;
    100,                                            //int     steptime;
    ABGRANGER_SPEED,                            //float   speed;
    10.0f,                                          //float   acceleration;
    1.0f,                                           //float   airAcceleration;
    6.0f,                                           //float   friction;
    100.0f,                                         //float   stopSpeed;
    270.0f,                                         //float   jumpMagnitude;
    1.0f,                                           //float   knockbackScale;
    { PCL_ALIEN_LEVEL1, PCL_NONE, PCL_NONE },       //int     children[ 3 ];
    ABGRANGER_COST,                             //int     cost;
    ABGRANGER_VALUE                             //int     value;
  }
};

int   bg_numClasses = sizeof( bg_classList ) / sizeof( bg_classList[ 0 ] );

static const classAttributes_t nullClass = { 0 };


/*
==============
BG_FindHealthForClass
==============
*/
int BG_FindHealthForClass( int pclass )
{
  int i;

  for( i = 0; i < bg_numClasses; i++ )
  {
    if( bg_classList[ i ].number == pclass )
    {
      return bg_classList[ i ].health;
    }
  }

  Com_Printf( S_COLOR_YELLOW "WARNING: fallthrough in BG_FindHealthForClass\n" );
  return 100;
}


/*
==============
BG_ClassByName
==============
*/
const classAttributes_t *BG_ClassByName( const char *name )
{
  int i;

  for( i = 0; i < bg_numClasses; i++ )
  {
    if( !Q_stricmp( bg_classList[ i ].name, name ) )
      return &bg_classList[ i ];
  }

  return &nullClass;
}

/*
==============
BG_Class
==============
*/
const classAttributes_t *BG_Class( class_t class )
{
  return ( class >= PCL_NONE && class < PCL_NUM_CLASSES ) ?
    &bg_classList[ class ] : &nullClass;
}

/*
==============
BG_ClassAllowedInStage
==============
*/
qboolean BG_ClassAllowedInStage( class_t class,
                                 stage_t stage )
{
  int stages = BG_Class( class )->stages;

  return stages & ( 1 << stage );
}

static classConfig_t bg_classConfigList[ PCL_NUM_CLASSES ];

/*
==============
BG_ClassConfig
==============
*/
classConfig_t *BG_ClassConfig( class_t class )
{
  return &bg_classConfigList[ class ];
}

/*
==============
BG_ClassBoundingBox
==============
*/
void BG_ClassBoundingBox( class_t class,
                          vec3_t mins, vec3_t maxs,
                          vec3_t cmaxs, vec3_t dmins, vec3_t dmaxs )
{
  classConfig_t *classConfig = BG_ClassConfig( class );

  if( mins != NULL )
    VectorCopy( classConfig->mins, mins );

  if( maxs != NULL )
    VectorCopy( classConfig->maxs, maxs );

  if( cmaxs != NULL )
    VectorCopy( classConfig->crouchMaxs, cmaxs );

  if( dmins != NULL )
    VectorCopy( classConfig->deadMins, dmins );

  if( dmaxs != NULL )
    VectorCopy( classConfig->deadMaxs, dmaxs );
}

/*
==============
BG_ClassHasAbility
==============
*/
qboolean BG_ClassHasAbility( class_t class, int ability )
{
  int abilities = BG_Class( class )->abilities;

  return abilities & ability;
}

/*
==============
BG_ClassCanEvolveFromTo
==============
*/
int BG_ClassCanEvolveFromTo( class_t fclass,
                             class_t tclass,
                             int credits, int stage, int cost, int recursecount )
{
  int i, j, best, value;

  if( recursecount > bg_numClasses )
    return -1;

  if( credits < cost || fclass == PCL_NONE || tclass == PCL_NONE ||
      fclass == tclass )
    return -1;
  for( i = 0; i < bg_numClasses; i++ )
  {
    if( bg_classList[ i ].number != fclass )
      continue;
    best = credits + 1;
    for( j = 0; j < 3; j++ )
    {
      int thruClass, evolveCost;

      thruClass = bg_classList[ i ].children[ j ];
      if( thruClass == PCL_NONE || !BG_ClassAllowedInStage( thruClass, stage ) ||
          !BG_ClassIsAllowed( thruClass ) )
        continue;
      evolveCost = BG_Class( thruClass )->cost * ALIEN_CREDITS_PER_FRAG;
      if( thruClass == tclass )
        value = cost + evolveCost;
      else
        value = BG_ClassCanEvolveFromTo( thruClass, tclass, credits, stage,
                                         cost + evolveCost, recursecount);
      if( value >= 0 && value < best )
        best = value;
    }
    return best <= credits ? best : -1;
  }
  Com_Printf( S_COLOR_YELLOW "WARNING: fallthrough in BG_ClassCanEvolveFromTo\n" );
  return -1;
}

/*
==============
BG_AlienCanEvolve
==============
*/
qboolean BG_AlienCanEvolve( class_t pclass, int credits, int stage )
{
  int i, j, tclass;

  for( i = PCL_NONE + 1; i < PCL_NUM_CLASSES; i++ )
  {
    if( bg_classList[ i ].number != pclass )
      continue;
    for( j = 0; j < 3; j++ )
    {
      tclass = bg_classList[ i ].children[ j ];
      if( tclass != PCL_NONE && BG_ClassAllowedInStage( tclass, stage ) &&
          BG_ClassIsAllowed( tclass ) &&
          credits >= BG_Class( tclass )->cost * ALIEN_CREDITS_PER_FRAG )
        return qtrue;
    }
    return qfalse;
  }
  Com_Printf( S_COLOR_YELLOW "WARNING: fallthrough in BG_AlienCanEvolve\n" );
  return qfalse;
}

/*
======================
BG_ParseClassFile

Parses a configuration file describing a class
======================
*/
static qboolean BG_ParseClassFile( const char *filename, classConfig_t *cc )
{
  char          *text_p;
  int           i;
  int           len;
  char          *token;
  char          text[ 20000 ];
  fileHandle_t  f;
  float         scale = 0.0f;
  int           defined = 0;
  enum
  {
      MODEL         = 1 << 0,
      SKIN          = 1 << 1,
      HUD           = 1 << 2,
      MODELSCALE    = 1 << 3,
      SHADOWSCALE   = 1 << 4,
      MINS          = 1 << 5,
      MAXS          = 1 << 6,
      DEADMINS      = 1 << 7,
      DEADMAXS      = 1 << 8,
      CROUCHMAXS    = 1 << 9,
      VIEWHEIGHT    = 1 << 10,
      CVIEWHEIGHT   = 1 << 11,
      ZOFFSET       = 1 << 12,
      NAME          = 1 << 13
  };

  // load the file
  len = trap_FS_FOpenFile( filename, &f, FS_READ );
  if( len < 0 )
    return qfalse;

  if( len == 0 || len >= sizeof( text ) - 1 )
  {
    trap_FS_FCloseFile( f );
    Com_Printf( S_COLOR_RED "ERROR: Class file %s is %s\n", filename,
      len == 0 ? "empty" : "too long" );
    return qfalse;
  }

  trap_FS_Read( text, len, f );
  text[ len ] = 0;
  trap_FS_FCloseFile( f );

  // parse the text
  text_p = text;

  // read optional parameters
  while( 1 )
  {
    token = COM_Parse( &text_p );

    if( !token )
      break;

    if( !Q_stricmp( token, "" ) )
      break;

    if( !Q_stricmp( token, "model" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( cc->modelName, token, sizeof( cc->modelName ) );

      defined |= MODEL;
      continue;
    }
    else if( !Q_stricmp( token, "skin" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( cc->skinName, token, sizeof( cc->skinName ) );

      defined |= SKIN;
      continue;
    }
    else if( !Q_stricmp( token, "hud" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( cc->hudName, token, sizeof( cc->hudName ) );

      defined |= HUD;
      continue;
    }
    else if( !Q_stricmp( token, "modelScale" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      scale = atof( token );

      if( scale < 0.0f )
        scale = 0.0f;

      cc->modelScale = scale;

      defined |= MODELSCALE;
      continue;
    }
    else if( !Q_stricmp( token, "shadowScale" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      scale = atof( token );

      if( scale < 0.0f )
        scale = 0.0f;

      cc->shadowScale = scale;

      defined |= SHADOWSCALE;
      continue;
    }
    else if( !Q_stricmp( token, "mins" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->mins[ i ] = atof( token );
      }

      defined |= MINS;
      continue;
    }
    else if( !Q_stricmp( token, "maxs" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->maxs[ i ] = atof( token );
      }

      defined |= MAXS;
      continue;
    }
    else if( !Q_stricmp( token, "deadMins" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->deadMins[ i ] = atof( token );
      }

      defined |= DEADMINS;
      continue;
    }
    else if( !Q_stricmp( token, "deadMaxs" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->deadMaxs[ i ] = atof( token );
      }

      defined |= DEADMAXS;
      continue;
    }
    else if( !Q_stricmp( token, "crouchMaxs" ) )
    {
      for( i = 0; i <= 2; i++ )
      {
        token = COM_Parse( &text_p );
        if( !token )
          break;

        cc->crouchMaxs[ i ] = atof( token );
      }

      defined |= CROUCHMAXS;
      continue;
    }
    else if( !Q_stricmp( token, "viewheight" ) )
    {
      token = COM_Parse( &text_p );
      cc->viewheight = atoi( token );
      defined |= VIEWHEIGHT;
      continue;
    }
    else if( !Q_stricmp( token, "crouchViewheight" ) )
    {
      token = COM_Parse( &text_p );
      cc->crouchViewheight = atoi( token );
      defined |= CVIEWHEIGHT;
      continue;
    }
    else if( !Q_stricmp( token, "zOffset" ) )
    {
      float offset;

      token = COM_Parse( &text_p );
      if( !token )
        break;

      offset = atof( token );

      cc->zOffset = offset;

      defined |= ZOFFSET;
      continue;
    }
    else if( !Q_stricmp( token, "name" ) )
    {
      token = COM_Parse( &text_p );
      if( !token )
        break;

      Q_strncpyz( cc->humanName, token, sizeof( cc->humanName ) );

      defined |= NAME;
      continue;
    }


    Com_Printf( S_COLOR_RED "ERROR: unknown token '%s'\n", token );
    return qfalse;
  }

  if(      !( defined & MODEL       ) ) token = "model";
  else if( !( defined & SKIN        ) ) token = "skin";
  else if( !( defined & HUD         ) ) token = "hud";
  else if( !( defined & MODELSCALE  ) ) token = "modelScale";
  else if( !( defined & SHADOWSCALE ) ) token = "shadowScale";
  else if( !( defined & MINS        ) ) token = "mins";
  else if( !( defined & MAXS        ) ) token = "maxs";
  else if( !( defined & DEADMINS    ) ) token = "deadMins";
  else if( !( defined & DEADMAXS    ) ) token = "deadMaxs";
  else if( !( defined & CROUCHMAXS  ) ) token = "crouchMaxs";
  else if( !( defined & VIEWHEIGHT  ) ) token = "viewheight";
  else if( !( defined & CVIEWHEIGHT ) ) token = "crouchViewheight";
  else if( !( defined & ZOFFSET     ) ) token = "zOffset";
  else if( !( defined & NAME        ) ) token = "name";
  else                                  token = "";

  if( strlen( token ) > 0 )
  {
      Com_Printf( S_COLOR_RED "ERROR: %s not defined in %s\n",
                  token, filename );
      return qfalse;
  }

  return qtrue;
}

/*
===============
BG_InitClassConfigs
===============
*/
void BG_InitClassConfigs( void )
{
  int           i;
  classConfig_t *cc;

  for( i = PCL_NONE; i < PCL_NUM_CLASSES; i++ )
  {
    cc = BG_ClassConfig( i );

    BG_ParseClassFile( va( "configs/classes/%s.cfg",
                           BG_Class( i )->name ), cc );
  }
}

////////////////////////////////////////////////////////////////////////////////

static const weaponAttributes_t bg_weapons[ ] =
{
  {
    WP_ALEVEL0,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level0",             //char      *weaponName;
    "Bite",               //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL0_BITE_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL0_BITE_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL0_UPG,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level0upg",             //char      *weaponName;
    "Bite Upgrade",               //char      *weaponHumanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL0_BITE_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL0_BITE_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL1,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level1",             //char      *weaponName;
    "Claws",              //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL1_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL1_CLAW_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL1_UPG,       //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level1upg",          //char      *weaponName;
    "Claws Upgrade",      //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL1_CLAW_U_REPEAT, //int       repeatRate1;
    LEVEL1_PCLOUD_REPEAT, //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL1_CLAW_U_K_SCALE,//float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL2,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level2",             //char      *weaponName;
    "Bite",               //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL2_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL2_CLAW_K_SCALE,        //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL2_UPG,       //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level2upg",          //char      *weaponName;
    "Zap",                //char      *humanName;
    "",
    3,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL2_CLAW_U_REPEAT, //int       repeatRate1;
    LEVEL2_AREAZAP_REPEAT,//int       repeatRate2;
    LEVEL2_BOUNCEBALL_REPEAT,//int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL2_CLAW_U_K_SCALE,//float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qtrue,                //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL3,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level3",             //char      *weaponName;
    "Pounce",             //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL3_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL3_CLAW_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL3_UPG,       //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level3upg",          //char      *weaponName;
    "Pounce (upgrade)",   //char      *humanName;
    "",
    3,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL3_CLAW_U_REPEAT, //int       repeatRate1;
    0,                    //int       repeatRate2;
    LEVEL3_BOUNCEBALL_REPEAT,//int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL3_CLAW_U_K_SCALE,//float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qtrue,                //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL4,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level4",             //char      *weaponName;
    "Charge",             //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL4_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL4_CLAW_K_SCALE,  //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ALEVEL4_UPG,       //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "level4upg",          //char      *weaponName;
    "Charge Upgrade",     //char      *weaponHumanName;
    "",                   //char      info
    1,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    LEVEL4_CLAW_REPEAT,   //int       repeatRate1;
    0,                    //int       repeatRate2;
    LEVEL4_EBLOB_REPEAT,  //int       repeatRate3;
    0,                    //int       reloadTime;
    LEVEL4_CLAW_U_K_SCALE,//float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qtrue,                //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //WUTeam_t  team;
  },
  {
    WP_BLASTER,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    0,                    //int       slots;
    "blaster",            //char      *weaponName;
    "Blaster",            //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    BLASTER_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    BLASTER_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_MACHINEGUN,        //int       weaponNum;
    RIFLE_PRICE,          //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "rifle",              //char      *weaponName;
    "Rifle",              //char      *humanName;
    "A cost effective cased projectile weapon. The rifle has acceptable "
      "accuracy for most tasks. The clip based system is slow but "
      "reliable.",
    RIFLE_CLIPSIZE,       //int       maxAmmo;
    RIFLE_MAXCLIPS,       //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    RIFLE_REPEAT,         //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    RIFLE_RELOAD,         //int       reloadTime;
    RIFLE_K_SCALE,        //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_PAIN_SAW,          //int       weaponNum;
    PAINSAW_PRICE,        //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "psaw",               //char      *weaponName;
    "Pain Saw",           //char      *humanName;
    "Derived from a chainsaw, the Pain Saw uses an electric arc "
      "instead of a chain, which tended to wear out too quickly. "
      "Read the manual carefully before using as improper use may "
      "result in the loss of limbs and or various appendages.",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    PAINSAW_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    PAINSAW_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_SHOTGUN,           //int       weaponNum;
    SHOTGUN_PRICE,        //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "shotgun",            //char      *weaponName;
    "Shotgun",            //char      *humanName;
    "Close range weapon that is useful against larger foes. "
      "Sometimes criticized for having a slow rate of fire and "
      "for being difficult to reload in combat. This model does "
      "include the optional shell cartridge launcher",
    SHOTGUN_SHELLS,       //int       maxAmmo;
    SHOTGUN_MAXCLIPS,     //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    SHOTGUN_REPEAT,       //int       repeatRate1;
    SHOTGUN_NADE_REPEAT,  //int       repeatRate2;
    0,                    //int       repeatRate3;
    SHOTGUN_RELOAD,       //int       reloadTime;
    SHOTGUN_K_SCALE,      //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_LAS_GUN,           //int       weaponNum;
    LASGUN_PRICE,         //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "lgun",               //char      *weaponName;
    "Las Gun",            //char      *humanName;
    "Slightly more powerful than the basic rifle, rapidly fires "
      "small packets of energy. Includes the optional 2x scope.",
    LASGUN_AMMO,          //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    LASGUN_REPEAT,        //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    LASGUN_RELOAD,        //int       reloadTime;
    LASGUN_K_SCALE,       //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qtrue,                //qboolean  canZoom;
    60.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_MASS_DRIVER,       //int       weaponNum;
    MDRIVER_PRICE,        //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "mdriver",            //char      *weaponName;
    "Mass Driver",        //char      *humanName;
    "A portable magnetic accelerator capable of launching "
      "metallic slugs at extremely high velocity. It has a "
      "large payload, but requires some time for the capacitors "
      "to charge between shots.",
    MDRIVER_CLIPSIZE,     //int       maxAmmo;
    MDRIVER_MAXCLIPS,     //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,                //int       usesEnergy;
    MDRIVER_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    MDRIVER_RELOAD,       //int       reloadTime;
    MDRIVER_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qtrue,                //qboolean  canZoom;
    20.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_CHAINGUN,          //int       weaponNum;
    CHAINGUN_PRICE,       //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "chaingun",           //char      *weaponName;
    "Chaingun",           //char      *humanName;
    "Electrically driven cased projectile weapon. It has a high rate "
      "of fire but a wide firing angle and is therefore relatively "
      "inaccurate. Includes optional dual speed gearing.",
    CHAINGUN_BULLETS,     //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    CHAINGUN_REPEAT,      //int       repeatRate1;
    CHAINGUN_REPEAT_2,    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    CHAINGUN_K_SCALE,     //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_PULSE_RIFLE,       //int       weaponNum;
    PRIFLE_PRICE,         //int       price;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "prifle",             //char      *weaponName;
    "Pulse Rifle",        //char      *humanName;
    "An energy weapon that fires rapid pulses of concentrated energy. "
      "Secondary fire slows large aliens, and traps small ones.",
    PRIFLE_CLIPS,         //int       maxAmmo;
    PRIFLE_MAXCLIPS,      //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    PRIFLE_REPEAT,        //int       repeatRate1;
    PRIFLE_SECONDARY_REPEAT, //int       repeatRate2;
    0,                    //int       repeatRate3;
    PRIFLE_RELOAD,        //int       reloadTime;
    PRIFLE_K_SCALE,       //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_FLAMER,            //int       weaponNum;
    FLAMER_PRICE,         //int       price;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "flamer",             //char      *weaponName;
    "Flame Thrower",      //char      *humanName;
    "Sprays fire at its target. It is powered by compressed "
      "gas. The relatively low rate of fire means this weapon is most "
      "effective against static targets.\n\n"
      "CAUTION: DO NOT SMOKE WHILE REFILLING.",
    FLAMER_GAS,           //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    FLAMER_REPEAT,        //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    FLAMER_K_SCALE,       //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_LUCIFER_CANNON,    //int       weaponNum;
    LCANNON_PRICE,        //int       price;
    ( 1 << S3 ),          //int  stages
    SLOT_WEAPON,          //int       slots;
    "lcannon",            //char      *weaponName;
    "Lucifer Cannon",     //char      *humanName;
    "Blaster technology scaled up to deliver devastating power. "
      "Primary fire must be charged before firing. It has a quick "
      "secondary attack that does not require charging. Surge increases damage "
      "by about 20 percent.",
    LCANNON_AMMO,         //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    LCANNON_REPEAT,       //int       repeatRate1;
    LCANNON_SECONDARY_REPEAT, //int       repeatRate2;
    0,                    //int       repeatRate3;
    LCANNON_RELOAD,       //int       reloadTime;
    LCANNON_K_SCALE,      //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_XAEL,              //int       weaponNum;
    XAEL_PRICE,           //int       price;
    ( 1 << S3 ),          //int  stages
    SLOT_WEAPON,          //int       slots;
    "xael",               //char      *weaponName;
    "Xael",               //char      *weaponHumanName;
    "An initial prototype of a second generation Lucifer Cannon. "
      "Similar damage to the lucifer cannon, but please note "
      "that this weapon's shots are very bouncy."
      "Has a higher splash damage with its highly charged shots. "
      "Surge increases damage by about 20 percent. "
      "USE AT YOUR OWN RISK.",
    XAEL_AMMO,            //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    XAEL_REPEAT,          //int       repeatRate1;
    XAEL_SECONDARY_REPEAT,//int       repeatRate2;
    0,                    //int       repeatRate3;
    XAEL_RELOAD,          //int       reloadTime;
    XAEL_K_SCALE,         //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qtrue,                //qboolean  longRanged;
    TEAM_HUMANS           //WUTeam_t  team;
  },
  {
    WP_GRENADE,           //int       weaponNum;
    GRENADE_PRICE,        //int       price;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_NONE,            //int       slots;
    "grenade",            //char      *weaponName;
    "Grenade",            //char      *humanName;
    "The M98 fragmentation grenade is tried and true. "
      "5 second fuse.\n\n CAUTION: DO NOT DROP.",
    1,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qfalse,               //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    GRENADE_REPEAT,       //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    GRENADE_K_SCALE,      //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_LOCKBLOB_LAUNCHER, //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "lockblob",           //char      *weaponName;
    "Lock Blob",          //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    500,                  //int       repeatRate1;
    500,                  //int       repeatRate2;
    500,                  //int       repeatRate3;
    0,                    //int       reloadTime;
    LOCKBLOB_K_SCALE,     //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_HIVE,              //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "hive",               //char      *weaponName;
    "Hive",               //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    500,                  //int       repeatRate1;
    500,                  //int       repeatRate2;
    500,                  //int       repeatRate3;
    0,                    //int       reloadTime;
    HIVE_K_SCALE,         //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_TESLAGEN,          //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "teslagen",           //char      *weaponName;
    "Tesla Generator",    //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qtrue,                //int       usesEnergy;
    500,                  //int       repeatRate1;
    500,                  //int       repeatRate2;
    500,                  //int       repeatRate3;
    0,                    //int       reloadTime;
    TESLAGEN_K_SCALE,     //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_MGTURRET,          //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "mgturret",           //char      *weaponName;
    "Machinegun Turret",  //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    0,                    //int       repeatRate1;
    0,                    //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    MGTURRET_K_SCALE,     //float     knockbackScale;
    qfalse,               //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qfalse,               //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_ABUILD,            //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "abuild",             //char      *weaponName;
    "Alien build weapon", //char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    ABUILDER_BUILD_REPEAT,//int       repeatRate1;
    ABUILDER_CLAW_REPEAT, //int       repeatRate2;
    ABUILDER_BLOB_REPEAT, //int       repeatRate3;
    0,                    //int       reloadTime;
    ABUILDER_CLAW_K_SCALE,//float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qtrue,                //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_ABUILD2,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "abuildupg",          //char      *weaponName;
    "Alien build weapon2",//char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    ABUILDER_BUILD_REPEAT,//int       repeatRate1;
    ABUILDER_CLAW_REPEAT, //int       repeatRate2;
    ABUILDER_BLOB_REPEAT, //int       repeatRate3;
    0,                    //int       reloadTime;
    ABUILDER_CLAW_K_SCALE,//float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qtrue,                //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  },
  {
    WP_HBUILD,            //int       weaponNum;
    HBUILD_PRICE,         //int       price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_WEAPON,          //int       slots;
    "ckit",               //char      *weaponName;
    "Construction Kit",   //char      *humanName;
    "Used for building structures. This includes "
      "spawns, power and basic defense. More structures become "
      "available with new stages.",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    HBUILD_REPEAT,        //int       repeatRate1;
    HBUILD_REPEAT,        //int       repeatRate2;
    0,                    //int       repeatRate3;
    0,                    //int       reloadTime;
    0.0f,                 //float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qfalse,               //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_HUMANS           //team_t  team;
  },
  {
    WP_ABUILD3,           //int       weaponNum;
    0,                    //int       price;
    ( 1 << S3 ),          //int  stages
    SLOT_WEAPON,          //int       slots;
    "abgranger",          //char      *weaponName;
    "Alien battle granger",//char      *humanName;
    "",
    0,                    //int       maxAmmo;
    0,                    //int       maxClips;
    qtrue,                //int       infiniteAmmo;
    qfalse,               //int       usesEnergy;
    ABGRANGER_BUILD_REPEAT,//int       repeatRate1;
    ABGRANGER_CLAW_REPEAT,//int       repeatRate2;
    ABGRANGER_BLOB_REPEAT,//int       repeatRate3;
    0,                    //int       reloadTime;
    ABGRANGER_CLAW_K_SCALE,//float     knockbackScale;
    qtrue,                //qboolean  hasAltMode;
    qtrue,                //qboolean  hasThirdMode;
    qfalse,               //qboolean  canZoom;
    90.0f,                //float     zoomFov;
    qtrue,                //qboolean  purchasable;
    qfalse,               //qboolean  longRanged;
    TEAM_ALIENS           //team_t  team;
  }
};

int   bg_numWeapons = sizeof( bg_weapons ) / sizeof( bg_weapons[ 0 ] );

/*
==============
BG_FindPriceForWeapon
==============
*/
int BG_FindPriceForWeapon( int weapon )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( bg_weapons[ i ].number == weapon )
    {
      return bg_weapons[ i ].price;
    }
  }

  return 100;
}

static const weaponAttributes_t nullWeapon = { 0 };

/*
==============
BG_WeaponByName
==============
*/
const weaponAttributes_t *BG_WeaponByName( const char *name )
{
  int i;

  for( i = 0; i < bg_numWeapons; i++ )
  {
    if( !Q_stricmp( bg_weapons[ i ].name, name ) )
    {
      return &bg_weapons[ i ];
    }
  }

  return &nullWeapon;
}

/*
==============
BG_Weapon
==============
*/
const weaponAttributes_t *BG_Weapon( weapon_t weapon )
{
  return ( weapon > WP_NONE && weapon < WP_NUM_WEAPONS ) ?
    &bg_weapons[ weapon - 1 ] : &nullWeapon;
}

/*
==============
BG_WeaponAllowedInStage
==============
*/
qboolean BG_WeaponAllowedInStage( weapon_t weapon, stage_t stage )
{
  int stages = BG_Weapon( weapon )->stages;

  return stages & ( 1 << stage );
}

////////////////////////////////////////////////////////////////////////////////

static const upgradeAttributes_t bg_upgrades[ ] =
{
  {
    UP_LIGHTARMOUR,         //int   upgradeNum;
    LIGHTARMOUR_PRICE,      //int   price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_TORSO|SLOT_ARMS|SLOT_LEGS, //int   slots;
    "larmour",              //char  *upgradeName;
    "Light Armour",         //char  *humanName;
    "Protective armour that helps to defend against light alien melee "
      "attacks.",
    "icons/iconu_larmour",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_HELMET,              //int   upgradeNum;
    HELMET_PRICE,           //int   price;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_HEAD,              //int   slots;
    "helmet",               //char  *upgradeName;
    "Helmet",               //char  *humanName;
    "In addition to protecting your head, the helmet provides a "
      "scanner indicating the presence of any friendly or hostile "
      "lifeforms and structures in your immediate vicinity.",
    "icons/iconu_helmet",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_MEDKIT,              //int   upgradeNum;
    MEDKIT_PRICE,           //int   price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_NONE,              //int   slots;
    "medkit",               //char  *upgradeName;
    "Medkit",               //char  *humanName;
    "",
    "icons/iconu_atoxin",
    qtrue,                 //qboolean purchasable
    qtrue,                  //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_BATTPACK,            //int   upgradeNum;
    BATTPACK_PRICE,         //int   price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_BACKPACK,          //int   slots;
    "battpack",             //char  *upgradeName;
    "Battery Pack",         //char  *humanName;
    "Back-mounted battery pack that permits storage of one and a half "
      "times the normal energy capacity for energy weapons.",
    "icons/iconu_battpack",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_AMMOPACK,            //int   upgradeNum;
    AMMOPACK_PRICE,         //int   price;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_BACKPACK,          //int   slots;
    "ammopack",             //char  *upgradeName;
    "Ammo Pack",            //char  *upgradeHumanName;
    "A backpack that stores extra clips for non-energy"
     "weapons",
    "icons/iconu_ammopack",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS              //WUTeam_t  team;
  },
  {
    UP_JETPACK,             //int   upgradeNum;
    JETPACK_PRICE,          //int   price;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_BACKPACK,          //int   slots;
    "jetpack",              //char  *upgradeName;
    "Jet Pack",             //char  *humanName;
    "Back-mounted jet pack that enables the user to fly to remote "
      "locations on a rechargable battery. "
      "It is very useful against alien spawns in hard to reach spots. "
      "WARNING: if the capicitors are overloaded explosions may occur.",
    "icons/iconu_jetpack",
    qtrue,                  //qboolean purchasable
    qtrue,                  //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_BATTLESUIT,          //int   upgradeNum;
    BSUIT_PRICE,            //int   price;
    ( 1 << S3 ),            //int  stages
    SLOT_HEAD|SLOT_TORSO|SLOT_ARMS|SLOT_LEGS|SLOT_BACKPACK|SLOT_CLOAK, //int   slots;
    "bsuit",                //char  *upgradeName;
    "Battlesuit",           //char  *humanName;
    "A full body armour that is highly effective at repelling alien attacks. "
      "It allows the user to enter hostile situations with a greater degree "
      "of confidence.",
    "icons/iconu_bsuit",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_GRENADE,             //int   upgradeNum;
    GRENADE_PRICE,          //int   price;
    ( 1 << S2 )|( 1 << S3 ),//int  stages
    SLOT_NONE,              //int   slots;
    "gren",                 //char  *upgradeName;
    "Grenade",              //char  *humanName;
    "The M98 fragmentation grenade is tried and true. "
      "5 second fuse.\n\n CAUTION: DO NOT DROP.",
    0,
    qtrue,                  //qboolean purchasable
    qtrue,                  //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_AMMO,                //int   upgradeNum;
    0,                      //int   price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_NONE,              //int   slots;
    "ammo",                 //char  *upgradeName;
    "Ammunition",           //char  *humanName;
    "Ammunition for the currently held weapon.",
    0,
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS             //team_t  team;
  },
  {
    UP_REGEN,              //int   upgradeNum;
    REGEN_PRICE,           //int   price;
    ( 1 << S1 )|( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_NONE,              //int   slots;
    "regen",               //char  *upgradeName;
    "Biokit",               //char  *upgradeHumanName;
    "An implant that helps its user to regenerate health and "
     "stamina and makes slightly more immune to spreadable infections.",
    "icons/iconu_regen",
    qtrue,                 //qboolean purchasable
    qfalse,                  //qboolean usable
    TEAM_HUMANS              //WUTeam_t  team;
  },
  {
    UP_SURGE,            //int   upgradeNum;
    SURGE_PRICE,         //int   price;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_NONE,          //int   slots;
    "surge",             //char  *upgradeName;
    "Surge",         //char  *upgradeHumanName;
    "An external bank of capacitors which increase the efficiency"
    " of the Las Gun, Mass Driver, Luci, or Xael yeilding a 10 percent"
    " increase in damage while at the same time increasing the "
    "firing speed by 20 percent.",
    "icons/iconu_surge",
    qtrue,                  //qboolean purchasable
    qfalse,                 //qboolean usable
    TEAM_HUMANS              //WUTeam_t  team;
  },
  {
    UP_CLOAK,            //int   upgradeNum;
    CLOAK_PRICE,         //int   price;
    ( 1 << S2 )|( 1 << S3 ), //int  stages
    SLOT_CLOAK,          //int   slots;
    "cloak",             //char  *upgradeName;
    "Cloak",         //char  *upgradeHumanName;
    "An invisibility cloak that can be used to sneak up on aliens "
     "without being seen. The cloak lasts for 30 seconds once it "
     " is activated. You can recharge your cloak by buying ammo."
     " The cloak is now more visible to your allies.",
    "icons/iconu_cloak",
    qtrue,                  //qboolean purchasable
    qtrue,                 //qboolean usable
    TEAM_HUMANS              //WUTeam_t  team;
  }
};

int   bg_numUpgrades = sizeof( bg_upgrades ) / sizeof( bg_upgrades[ 0 ] );

/*
==============
BG_FindPriceForUpgrade
==============
*/
int BG_FindPriceForUpgrade( int upgrade )
{
  int i;

  for( i = 0; i < bg_numUpgrades; i++ )
  {
    if( bg_upgrades[ i ].number == upgrade )
    {
      return bg_upgrades[ i ].price;
    }
  }

  return 100;
}

static const upgradeAttributes_t nullUpgrade = { 0 };

/*
==============
BG_UpgradeByName
==============
*/
const upgradeAttributes_t *BG_UpgradeByName( const char *name )
{
  int i;

  for( i = 0; i < bg_numUpgrades; i++ )
  {
    if( !Q_stricmp( bg_upgrades[ i ].name, name ) )
    {
      return &bg_upgrades[ i ];
    }
  }

  return &nullUpgrade;
}

/*
==============
BG_Upgrade
==============
*/
const upgradeAttributes_t *BG_Upgrade( upgrade_t upgrade )
{
  return ( upgrade > UP_NONE && upgrade < UP_NUM_UPGRADES ) ?
    &bg_upgrades[ upgrade - 1 ] : &nullUpgrade;
}

/*
==============
BG_UpgradeAllowedInStage
==============
*/
qboolean BG_UpgradeAllowedInStage( upgrade_t upgrade, stage_t stage )
{
  int stages = BG_Upgrade( upgrade )->stages;

  return stages & ( 1 << stage );
}

////////////////////////////////////////////////////////////////////////////////

/*
================
BG_EvaluateTrajectory

================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result )
{
  float   deltaTime;
  float   phase;

  switch( tr->trType )
  {
    case TR_STATIONARY:
    case TR_INTERPOLATE:
      VectorCopy( tr->trBase, result );
      break;

    case TR_LINEAR:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
      break;

    case TR_SINE:
      deltaTime = ( atTime - tr->trTime ) / (float)tr->trDuration;
      phase = sin( deltaTime * M_PI * 2 );
      VectorMA( tr->trBase, phase, tr->trDelta, result );
      break;

    case TR_LINEAR_STOP:
      if( atTime > tr->trTime + tr->trDuration )
        atTime = tr->trTime + tr->trDuration;

      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      if( deltaTime < 0 )
        deltaTime = 0;

      VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
      break;

    case TR_GRAVITY:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
      result[ 2 ] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;   // FIXME: local gravity...
      break;

    case TR_BUOYANCY:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
      result[ 2 ] += 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;   // FIXME: local gravity...
      break;

    default:
      Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trTime );
      break;
  }
}

/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result )
{
  float deltaTime;
  float phase;

  switch( tr->trType )
  {
    case TR_STATIONARY:
    case TR_INTERPOLATE:
      VectorClear( result );
      break;

    case TR_LINEAR:
      VectorCopy( tr->trDelta, result );
      break;

    case TR_SINE:
      deltaTime = ( atTime - tr->trTime ) / (float)tr->trDuration;
      phase = cos( deltaTime * M_PI * 2 );  // derivative of sin = cos
      phase *= 0.5;
      VectorScale( tr->trDelta, phase, result );
      break;

    case TR_LINEAR_STOP:
      if( atTime > tr->trTime + tr->trDuration )
      {
        VectorClear( result );
        return;
      }
      VectorCopy( tr->trDelta, result );
      break;

    case TR_GRAVITY:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorCopy( tr->trDelta, result );
      result[ 2 ] -= DEFAULT_GRAVITY * deltaTime;   // FIXME: local gravity...
      break;

    case TR_BUOYANCY:
      deltaTime = ( atTime - tr->trTime ) * 0.001;  // milliseconds to seconds
      VectorCopy( tr->trDelta, result );
      result[ 2 ] += DEFAULT_GRAVITY * deltaTime;   // FIXME: local gravity...
      break;

    default:
      Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime );
      break;
  }
}

char *eventnames[ ] =
{
  "EV_NONE",

  "EV_FOOTSTEP",
  "EV_FOOTSTEP_METAL",
  "EV_FOOTSTEP_SQUELCH",
  "EV_FOOTSPLASH",
  "EV_FOOTWADE",
  "EV_SWIM",

  "EV_STEP_4",
  "EV_STEP_8",
  "EV_STEP_12",
  "EV_STEP_16",

  "EV_STEPDN_4",
  "EV_STEPDN_8",
  "EV_STEPDN_12",
  "EV_STEPDN_16",

  "EV_FALL_SHORT",
  "EV_FALL_MEDIUM",
  "EV_FALL_FAR",
  "EV_FALLING",

  "EV_JUMP",
  "EV_WATER_TOUCH", // foot touches
  "EV_WATER_LEAVE", // foot leaves
  "EV_WATER_UNDER", // head touches
  "EV_WATER_CLEAR", // head leaves

  "EV_NOAMMO",
  "EV_CHANGE_WEAPON",
  "EV_FIRE_WEAPON",
  "EV_FIRE_WEAPON2",
  "EV_FIRE_WEAPON3",

  "EV_PLAYER_RESPAWN", // for fovwarp effects
  "EV_PLAYER_TELEPORT_IN",
  "EV_PLAYER_TELEPORT_OUT",

  "EV_GRENADE_BOUNCE",    // eventParm will be the soundindex

  "EV_GENERAL_SOUND",
  "EV_GLOBAL_SOUND",    // no attenuation

  "EV_BULLET_HIT_FLESH",
  "EV_BULLET_HIT_WALL",

  "EV_SHOTGUN",
  "EV_MASS_DRIVER",

  "EV_MISSILE_HIT",
  "EV_MISSILE_MISS",
  "EV_MISSILE_MISS_METAL",
  "EV_TESLATRAIL",
  "EV_BULLET",        // otherEntity is the shooter

  "EV_LEV1_GRAB",
  "EV_LEV4_TRAMPLE_PREPARE",
  "EV_LEV4_TRAMPLE_START",

  "EV_PAIN",
  "EV_DEATH1",
  "EV_DEATH2",
  "EV_DEATH3",
  "EV_OBITUARY",

  "EV_GIB_PLAYER",      // gib a previously living player

  "EV_BUILD_CONSTRUCT",
  "EV_BUILD_DESTROY",
  "EV_BUILD_DELAY",     // can't build yet
  "EV_BUILD_REPAIR",    // repairing buildable
  "EV_BUILD_REPAIRED",  // buildable has full health
  "EV_HUMAN_BUILDABLE_EXPLOSION",
  "EV_ALIEN_BUILDABLE_EXPLOSION",
  "EV_ALIEN_ACIDTUBE",

  "EV_MEDKIT_USED",

  "EV_ALIEN_EVOLVE",
  "EV_ALIEN_EVOLVE_FAILED",

  "EV_DEBUG_LINE",
  "EV_STOPLOOPINGSOUND",
  "EV_TAUNT",

  "EV_OVERMIND_ATTACK", // overmind under attack
  "EV_OVERMIND_DYING",  // overmind close to death
  "EV_OVERMIND_SPAWNS", // overmind needs spawns

  "EV_DCC_ATTACK",      // dcc under attack

  "EV_MGTURRET_SPINUP", // turret spinup sound should play

  "EV_RPTUSE_SOUND",    // trigger a sound

  "EV_ALIEN_HIT",       // Alien feedback for hitting an enemy
  "EV_ALIEN_MISS",      // Alien feedback for missing a strike
  "EV_ALIEN_TEAMHIT",   // Alien feedback for hitting a teammate

  "EV_ALIENRANGED_HIT",       // Alien ranged attack feedback for hitting an enemy
  "EV_ALIENRANGED_MISS",      // Alien ranged attack feedback for missing a strike
  "EV_ALIENRANGED_TEAMHIT",   // Alien ranged attack feedback for hitting a teammate

  "EV_LEV2_ZAP"
};

/*
===============
BG_EventName
===============
*/
const char *BG_EventName( int num )
{
  if( num < 0 || num >= sizeof( eventnames ) / sizeof( char * ) )
    return "UNKNOWN";
  return eventnames[ num ];
}

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
*/

void  trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );

void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps )
{
#ifdef _DEBUG
  {
    char buf[ 256 ];
    trap_Cvar_VariableStringBuffer( "showevents", buf, sizeof( buf ) );

    if( atof( buf ) != 0 )
    {
#ifdef GAME
      Com_Printf( " game event svt %5d -> %5d: num = %20s parm %d\n",
                  ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence,
                  BG_EventName( newEvent ), eventParm );
#else
      Com_Printf( "Cgame event svt %5d -> %5d: num = %20s parm %d\n",
                  ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence,
                  BG_EventName( newEvent ), eventParm );
#endif
    }
  }
#endif
  ps->events[ ps->eventSequence & ( MAX_PS_EVENTS - 1 ) ] = newEvent;
  ps->eventParms[ ps->eventSequence & ( MAX_PS_EVENTS - 1 ) ] = eventParm;
  ps->eventSequence++;
}


/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap )
{
  int     i;

  if( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR || ps->pm_type == PM_FREEZE )
    s->eType = ET_INVISIBLE;
  else if( ps->persistant[ PERS_SPECSTATE ] != SPECTATOR_NOT )
    s->eType = ET_INVISIBLE;
  else
    s->eType = ET_PLAYER;

  s->number = ps->clientNum;

  s->pos.trType = TR_INTERPOLATE;
  VectorCopy( ps->origin, s->pos.trBase );

  // Snapping player origins causes more problems than it solves
  //if( snap )
  //  SnapVector( s->pos.trBase );

  //set the trDelta for flag direction
  VectorCopy( ps->velocity, s->pos.trDelta );

  s->apos.trType = TR_INTERPOLATE;
  VectorCopy( ps->viewangles, s->apos.trBase );

  if( snap )
    SnapVector( s->apos.trBase );

  s->time2 = ps->movementDir;
  s->legsAnim = ps->legsAnim;
  s->torsoAnim = ps->torsoAnim;
  //FIXME: gotta work out a way to transmit this in the entitystate
  //s->weaponAnim |= ps->weaponAnim;
  s->clientNum = ps->clientNum;   // ET_PLAYER looks here instead of at number
                    // so corpses can also reference the proper config
  s->eFlags = ps->eFlags;
  if( ps->stats[STAT_HEALTH] <= 0 )
    s->eFlags |= EF_DEAD;
  else
    s->eFlags &= ~EF_DEAD;

  if( ps->stats[ STAT_STATE ] & SS_BLOBLOCKED )
    s->eFlags |= EF_BLOBLOCKED;
  else
    s->eFlags &= ~EF_BLOBLOCKED;

  if( ps->externalEvent )
  {
    s->event = ps->externalEvent;
    s->eventParm = ps->externalEventParm;
  }
  else if( ps->entityEventSequence < ps->eventSequence )
  {
    int   seq;

    if( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS )
      ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;

    seq = ps->entityEventSequence & ( MAX_PS_EVENTS - 1 );
    s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
    s->eventParm = ps->eventParms[ seq ];
    ps->entityEventSequence++;
  }

  s->weapon = ps->weapon;
  s->groundEntityNum = ps->groundEntityNum;

  //store items held and active items in modelindex and modelindex2
  s->modelindex = 0;
  s->modelindex2 = 0;
  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
  {
    if( BG_InventoryContainsUpgrade( i, ps->stats ) )
    {
      s->modelindex |= 1 << i;

      if( BG_UpgradeIsActive( i, ps->stats ) )
        s->modelindex2 |= 1 << i;
    }
  }

  // use misc field to store team/class info:
  s->misc = ps->stats[ STAT_TEAM ] | ( ps->stats[ STAT_CLASS ] << 8 );

  // have to get the surfNormal through somehow...
  VectorCopy( ps->grapplePoint, s->angles2 );

  s->loopSound = ps->loopSound;
  s->generic1 = ps->generic1;

  if( s->generic1 <= WPM_NONE || s->generic1 >= WPM_NUM_WEAPONMODES )
    s->generic1 = WPM_PRIMARY;

  s->otherEntityNum = ps->otherEntityNum;
}


/*
========================
BG_PlayerStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap )
{
  int     i;

  if( ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR || ps->pm_type == PM_FREEZE )
    s->eType = ET_INVISIBLE;
  else if( ps->persistant[ PERS_SPECSTATE ] != SPECTATOR_NOT )
    s->eType = ET_INVISIBLE;
  else
    s->eType = ET_PLAYER;

  s->number = ps->clientNum;

  s->pos.trType = TR_LINEAR_STOP;
  VectorCopy( ps->origin, s->pos.trBase );

  // Snapping player origins causes more problems than it solves
  //if( snap )
  //  SnapVector( s->pos.trBase );

  // set the trDelta for flag direction and linear prediction
  VectorCopy( ps->velocity, s->pos.trDelta );
  // set the time for linear prediction
  s->pos.trTime = time;
  // set maximum extra polation time
  s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

  s->apos.trType = TR_INTERPOLATE;
  VectorCopy( ps->viewangles, s->apos.trBase );
  if( snap )
    SnapVector( s->apos.trBase );

  s->time2 = ps->movementDir;
  s->legsAnim = ps->legsAnim;
  s->torsoAnim = ps->torsoAnim;
  // FIXME: see previous instance of weaponAnim
  //s->weaponAnim = ps->weaponAnim;
  s->clientNum = ps->clientNum;   // ET_PLAYER looks here instead of at number
                    // so corpses can also reference the proper config
  s->eFlags = ps->eFlags;

  if( ps->stats[STAT_HEALTH] <= 0 )
    s->eFlags |= EF_DEAD;
  else
    s->eFlags &= ~EF_DEAD;

  if( ps->stats[ STAT_STATE ] & SS_BLOBLOCKED )
    s->eFlags |= EF_BLOBLOCKED;
  else
    s->eFlags &= ~EF_BLOBLOCKED;

  if( ps->externalEvent )
  {
    s->event = ps->externalEvent;
    s->eventParm = ps->externalEventParm;
  }
  else if( ps->entityEventSequence < ps->eventSequence )
  {
    int   seq;

    if( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS )
      ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;

    seq = ps->entityEventSequence & ( MAX_PS_EVENTS - 1 );
    s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
    s->eventParm = ps->eventParms[ seq ];
    ps->entityEventSequence++;
  }

  s->weapon = ps->weapon;
  s->groundEntityNum = ps->groundEntityNum;

  //store items held and active items in modelindex and modelindex2
  s->modelindex = 0;
  s->modelindex2 = 0;

  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
  {
    if( BG_InventoryContainsUpgrade( i, ps->stats ) )
    {
      s->modelindex |= 1 << i;

      if( BG_UpgradeIsActive( i, ps->stats ) )
        s->modelindex2 |= 1 << i;
    }
  }

  // use misc field to store team/class info:
  s->misc = ps->stats[ STAT_TEAM ] | ( ps->stats[ STAT_CLASS ] << 8 );

  // have to get the surfNormal through somehow...
  VectorCopy( ps->grapplePoint, s->angles2 );

  s->loopSound = ps->loopSound;
  s->generic1 = ps->generic1;

  if( s->generic1 <= WPM_NONE || s->generic1 >= WPM_NUM_WEAPONMODES )
    s->generic1 = WPM_PRIMARY;

  s->otherEntityNum = ps->otherEntityNum;
}

/*
========================
BG_WeaponIsFull

Check if a weapon has full ammo
========================
*/
qboolean BG_WeaponIsFull( weapon_t weapon, int stats[ ], int ammo, int clips )
{
  int maxAmmo, maxClips;

  maxAmmo = BG_Weapon( weapon )->maxAmmo;
  maxClips = BG_Weapon( weapon )->maxClips;

  if( BG_InventoryContainsUpgrade( UP_BATTPACK, stats ) || BG_InventoryContainsUpgrade( UP_BATTLESUIT, stats ))
    maxAmmo = (int)( (float)maxAmmo * BATTPACK_MODIFIER );
  else if( ( BG_InventoryContainsUpgrade( UP_AMMOPACK, stats ) ||
      BG_InventoryContainsUpgrade( UP_BATTLESUIT, stats ) ) )
    maxClips = (int)( 1 + (float)(maxClips) * AMMOPACK_MODIFIER );

  return ( maxAmmo == ammo ) && ( maxClips == clips );
}

/*
========================
BG_InventoryContainsWeapon

Does the player hold a weapon?
========================
*/
qboolean BG_InventoryContainsWeapon( int weapon, int stats[ ] )
{
  // humans always have a blaster
  if( stats[ STAT_TEAM ] == TEAM_HUMANS && weapon == WP_BLASTER )
    return qtrue;

  return ( stats[ STAT_WEAPON ] == weapon );
}

/*
========================
BG_CalculateSlotsForInventory

Calculate the slots used by an inventory and warn of conflicts
========================
*/
int BG_CalculateSlotsForInventory( int stats[ ] )
{
  int i, slot, slots;

  slots = BG_Weapon( stats[ STAT_WEAPON ] )->slots;
  if( stats[ STAT_TEAM ] == TEAM_HUMANS )
    slots |= BG_Weapon( WP_BLASTER )->slots;
  for( i = UP_NONE; i < UP_NUM_UPGRADES; i++ )
    if( BG_InventoryContainsUpgrade( i, stats ) )
    {
      slot = BG_Upgrade( i )->slots;
      // this check should never be true
      if( slots & slot )
        Com_Printf( S_COLOR_YELLOW "WARNING: held item %d conflicts with "
                    "inventory slot %d\n", i, slot );
      slots |= slot;
    }

  return slots;
}

/*
========================
BG_AddUpgradeToInventory

Give the player an upgrade
========================
*/
void BG_AddUpgradeToInventory( int item, int stats[ ] )
{
  stats[ STAT_ITEMS ] |= ( 1 << item );
}

/*
========================
BG_RemoveUpgradeFromInventory

Take an upgrade from the player
========================
*/
void BG_RemoveUpgradeFromInventory( int item, int stats[ ] )
{
  stats[ STAT_ITEMS ] &= ~( 1 << item );
}

/*
========================
BG_InventoryContainsUpgrade

Does the player hold an upgrade?
========================
*/
qboolean BG_InventoryContainsUpgrade( int item, int stats[ ] )
{
  return( stats[ STAT_ITEMS ] & ( 1 << item ) );
}

/*
========================
BG_ActivateUpgrade

Activates an upgrade
========================
*/
void BG_ActivateUpgrade( int item, int stats[ ] )
{
  stats[ STAT_ACTIVEITEMS ] |= ( 1 << item );
}

/*
========================
BG_DeactivateUpgrade

Deactivates an upgrade
========================
*/
void BG_DeactivateUpgrade( int item, int stats[ ] )
{
  stats[ STAT_ACTIVEITEMS ] &= ~( 1 << item );
}

/*
========================
BG_UpgradeIsActive

Is this upgrade active?
========================
*/
qboolean BG_UpgradeIsActive( int item, int stats[ ] )
{
  return( stats[ STAT_ACTIVEITEMS ] & ( 1 << item ) );
}

/*
===============
BG_RotateAxis

Shared axis rotation function
===============
*/
qboolean BG_RotateAxis( vec3_t surfNormal, vec3_t inAxis[ 3 ],
                        vec3_t outAxis[ 3 ], qboolean inverse, qboolean ceiling )
{
  vec3_t  refNormal = { 0.0f, 0.0f, 1.0f };
  vec3_t  ceilingNormal = { 0.0f, 0.0f, -1.0f };
  vec3_t  localNormal, xNormal;
  float   rotAngle;

  //the grapplePoint being a surfNormal rotation Normal hack... see above :)
  if( ceiling )
  {
    VectorCopy( ceilingNormal, localNormal );
    VectorCopy( surfNormal, xNormal );
  }
  else
  {
    //cross the reference normal and the surface normal to get the rotation axis
    VectorCopy( surfNormal, localNormal );
    CrossProduct( localNormal, refNormal, xNormal );
    VectorNormalize( xNormal );
  }

  //can't rotate with no rotation vector
  if( VectorLength( xNormal ) != 0.0f )
  {
    rotAngle = RAD2DEG( acos( DotProduct( localNormal, refNormal ) ) );

    if( inverse )
      rotAngle = -rotAngle;

    AngleNormalize180( rotAngle );

    //hmmm could get away with only one rotation and some clever stuff later... but i'm lazy
    RotatePointAroundVector( outAxis[ 0 ], xNormal, inAxis[ 0 ], -rotAngle );
    RotatePointAroundVector( outAxis[ 1 ], xNormal, inAxis[ 1 ], -rotAngle );
    RotatePointAroundVector( outAxis[ 2 ], xNormal, inAxis[ 2 ], -rotAngle );
  }
  else
    return qfalse;

  return qtrue;
}

/*
===============
BG_GetClientNormal

Get the normal for the surface the client is walking on
===============
*/
void BG_GetClientNormal( const playerState_t *ps, vec3_t normal )
{
  if( ps->stats[ STAT_STATE ] & SS_WALLCLIMBING )
  {
    if( ps->eFlags & EF_WALLCLIMBCEILING )
      VectorSet( normal, 0.0f, 0.0f, -1.0f );
    else
      VectorCopy( ps->grapplePoint, normal );
  }
  else
    VectorSet( normal, 0.0f, 0.0f, 1.0f );
}

/*
===============
BG_PositionBuildableRelativeToPlayer

Find a place to build a buildable
===============
*/
void BG_PositionBuildableRelativeToPlayer( const playerState_t *ps,
                                           const vec3_t mins, const vec3_t maxs,
                                           void (*trace)( trace_t *, const vec3_t, const vec3_t,
                                                          const vec3_t, const vec3_t, int, int ),
                                           vec3_t outOrigin, vec3_t outAngles, trace_t *tr )
{
  vec3_t  forward, entityOrigin, targetOrigin;
  vec3_t  angles, playerOrigin, playerNormal;
  float   buildDist;

  BG_GetClientNormal( ps, playerNormal );

  VectorCopy( ps->viewangles, angles );
  VectorCopy( ps->origin, playerOrigin );
  buildDist = BG_Class( ps->stats[ STAT_CLASS ] )->buildDist;

  AngleVectors( angles, forward, NULL, NULL );
  ProjectPointOnPlane( forward, forward, playerNormal );
  VectorNormalize( forward );

  VectorMA( playerOrigin, buildDist, forward, entityOrigin );

  VectorCopy( entityOrigin, targetOrigin );

  //so buildings can be placed facing slopes
  VectorMA( entityOrigin, 32, playerNormal, entityOrigin );

  //so buildings drop to floor
  VectorMA( targetOrigin, -128, playerNormal, targetOrigin );

  // The mask is MASK_DEADSOLID on purpose to avoid collisions with other entities
  (*trace)( tr, entityOrigin, mins, maxs, targetOrigin, ps->clientNum, MASK_DEADSOLID );
  VectorCopy( tr->endpos, entityOrigin );
  VectorMA( entityOrigin, 0.1f, playerNormal, outOrigin );
  vectoangles( forward, outAngles );
}

/*
===============
BG_GetValueOfEquipment

Returns the equipment value of some human player's gear
===============
*/
int BG_GetValueOfEquipment( playerState_t *ps ) {
int i, worth = 0;

for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
{
  if( BG_InventoryContainsUpgrade( i, ps->stats ) )
    worth += BG_FindPriceForUpgrade( i );
}

for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
{
  if( BG_InventoryContainsWeapon( i, ps->stats ) )
    worth += BG_FindPriceForWeapon( i );
}

  return worth;
}
/*
===============
BG_GetValueOfPlayer

Returns the credit value of a player
===============
*/
int BG_GetValueOfPlayer( playerState_t *ps )
{
  int     i, worth = BG_Class( ps->stats[ STAT_CLASS ] )->value;

  if( ps->stats[ STAT_TEAM ] == TEAM_HUMANS )
  {
    for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
    {
      if( BG_InventoryContainsUpgrade( i, ps->stats ) )
        worth += BG_Upgrade( i )->price;
    }

    for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
    {
      if( BG_InventoryContainsWeapon( i, ps->stats ) )
        worth += BG_Weapon( i )->price;
    }
  }

  return worth;
}

/*
=================
BG_PlayerPoisonCloudTime
=================
*/
int BG_PlayerPoisonCloudTime( playerState_t *ps )
{
  int time = LEVEL1_PCLOUD_TIME;

  if( BG_InventoryContainsUpgrade( UP_BATTLESUIT, ps->stats ) )
    time -= BSUIT_PCLOUD_PROTECTION;
  if( BG_InventoryContainsUpgrade( UP_HELMET, ps->stats ) )
    time -= HELMET_PCLOUD_PROTECTION;
  if( BG_InventoryContainsUpgrade( UP_LIGHTARMOUR, ps->stats ) )
    time -= LIGHTARMOUR_PCLOUD_PROTECTION;
    
  return time;
}

/*
=================
BG_GetPlayerWeapon

Returns the players current weapon or the weapon they are switching to.
Only needs to be used for human weapons.
=================
*/
weapon_t BG_GetPlayerWeapon( playerState_t *ps )
{
  if( ps->persistant[ PERS_NEWWEAPON ] )
    return ps->persistant[ PERS_NEWWEAPON ];
  return ps->weapon;
}

/*
===============
BG_PlayerCanChangeWeapon
===============
*/
qboolean BG_PlayerCanChangeWeapon( playerState_t *ps )
{
  // prevent lcannon ammo conservation exploit
  if( ( ps->weapon == WP_LUCIFER_CANNON &&
      ps->stats[ STAT_MISC ] > LCANNON_CHARGE_TIME_MIN ) ||
      //not pretty, allow players to switch weapons while ckit is repairing
      ( ps->weapon != WP_HBUILD &&
        ( ps->weaponTime > 0 || ps->weaponstate == WEAPON_FIRING ) ) )
    return qfalse;
  if( ( ps->weapon == WP_XAEL &&
      ps->stats[ STAT_MISC ] > XAEL_CHARGE_TIME_MIN ) ||
      ps->weaponTime > 0 || ps->weaponstate == WEAPON_FIRING )
    return qfalse;
  return qtrue;
}

/*
===============
atof_neg

atof with an allowance for negative values
===============
*/
float atof_neg( char *token, qboolean allowNegative )
{
  float value;

  value = atof( token );

  if( !allowNegative && value < 0.0f )
    value = 1.0f;

  return value;
}

/*
===============
atoi_neg

atoi with an allowance for negative values
===============
*/
int atoi_neg( char *token, qboolean allowNegative )
{
  int value;

  value = atoi( token );

  if( !allowNegative && value < 0 )
    value = 1;

  return value;
}

/*
===============
BG_ParseCSVEquipmentList
===============
*/
void BG_ParseCSVEquipmentList( const char *string, weapon_t *weapons, int weaponsSize,
    upgrade_t *upgrades, int upgradesSize )
{
  char      buffer[ MAX_STRING_CHARS ];
  int       i = 0, j = 0;
  char      *p, *q;
  qboolean  EOS = qfalse;

  Q_strncpyz( buffer, string, MAX_STRING_CHARS );

  p = q = buffer;

  while( *p != '\0' )
  {
    //skip to first , or EOS
    while( *p != ',' && *p != '\0' )
      p++;

    if( *p == '\0' )
      EOS = qtrue;

    *p = '\0';

    //strip leading whitespace
    while( *q == ' ' )
      q++;

    if( weaponsSize )
      weapons[ i ] = BG_WeaponByName( q )->number;

    if( upgradesSize )
      upgrades[ j ] = BG_UpgradeByName( q )->number;

    if( weaponsSize && weapons[ i ] == WP_NONE &&
        upgradesSize && upgrades[ j ] == UP_NONE )
      Com_Printf( S_COLOR_YELLOW "WARNING: unknown equipment %s\n", q );
    else if( weaponsSize && weapons[ i ] != WP_NONE )
      i++;
    else if( upgradesSize && upgrades[ j ] != UP_NONE )
      j++;

    if( !EOS )
    {
      p++;
      q = p;
    }
    else
      break;

    if( i == ( weaponsSize - 1 ) || j == ( upgradesSize - 1 ) )
      break;
  }

  if( weaponsSize )
    weapons[ i ] = WP_NONE;

  if( upgradesSize )
    upgrades[ j ] = UP_NONE;
}

/*
===============
BG_ParseCSVClassList
===============
*/
void BG_ParseCSVClassList( const char *string, class_t *classes, int classesSize )
{
  char      buffer[ MAX_STRING_CHARS ];
  int       i = 0;
  char      *p, *q;
  qboolean  EOS = qfalse;

  Q_strncpyz( buffer, string, MAX_STRING_CHARS );

  p = q = buffer;

  while( *p != '\0' )
  {
    //skip to first , or EOS
    while( *p != ',' && *p != '\0' )
      p++;

    if( *p == '\0' )
      EOS = qtrue;

    *p = '\0';

    //strip leading whitespace
    while( *q == ' ' )
      q++;

    classes[ i ] = BG_ClassByName( q )->number;

    if( classes[ i ] == PCL_NONE )
      Com_Printf( S_COLOR_YELLOW "WARNING: unknown class %s\n", q );
    else
      i++;

    if( !EOS )
    {
      p++;
      q = p;
    }
    else
      break;
  }

  classes[ i ] = PCL_NONE;
}

/*
===============
BG_ParseCSVBuildableList
===============
*/
void BG_ParseCSVBuildableList( const char *string, buildable_t *buildables, int buildablesSize )
{
  char      buffer[ MAX_STRING_CHARS ];
  int       i = 0;
  char      *p, *q;
  qboolean  EOS = qfalse;

  Q_strncpyz( buffer, string, MAX_STRING_CHARS );

  p = q = buffer;

  while( *p != '\0' )
  {
    //skip to first , or EOS
    while( *p != ',' && *p != '\0' )
      p++;

    if( *p == '\0' )
      EOS = qtrue;

    *p = '\0';

    //strip leading whitespace
    while( *q == ' ' )
      q++;

    buildables[ i ] = BG_BuildableByName( q )->number;

    if( buildables[ i ] == BA_NONE )
      Com_Printf( S_COLOR_YELLOW "WARNING: unknown buildable %s\n", q );
    else
      i++;

    if( !EOS )
    {
      p++;
      q = p;
    }
    else
      break;
  }

  buildables[ i ] = BA_NONE;
}

typedef struct gameElements_s
{
  buildable_t       buildables[ BA_NUM_BUILDABLES ];
  class_t           classes[ PCL_NUM_CLASSES ];
  weapon_t          weapons[ WP_NUM_WEAPONS ];
  upgrade_t         upgrades[ UP_NUM_UPGRADES ];
} gameElements_t;

static gameElements_t bg_disabledGameElements;

/*
============
BG_InitAllowedGameElements
============
*/
void BG_InitAllowedGameElements( void )
{
  char cvar[ MAX_CVAR_VALUE_STRING ];

  trap_Cvar_VariableStringBuffer( "g_disabledEquipment",
      cvar, MAX_CVAR_VALUE_STRING );

  BG_ParseCSVEquipmentList( cvar,
      bg_disabledGameElements.weapons, WP_NUM_WEAPONS,
      bg_disabledGameElements.upgrades, UP_NUM_UPGRADES );

  trap_Cvar_VariableStringBuffer( "g_disabledClasses",
      cvar, MAX_CVAR_VALUE_STRING );

  BG_ParseCSVClassList( cvar,
      bg_disabledGameElements.classes, PCL_NUM_CLASSES );

  trap_Cvar_VariableStringBuffer( "g_disabledBuildables",
      cvar, MAX_CVAR_VALUE_STRING );

  BG_ParseCSVBuildableList( cvar,
      bg_disabledGameElements.buildables, BA_NUM_BUILDABLES );
}

/*
============
BG_WeaponIsAllowed
============
*/
qboolean BG_WeaponIsAllowed( weapon_t weapon )
{
  int i;

  for( i = 0; i < WP_NUM_WEAPONS &&
      bg_disabledGameElements.weapons[ i ] != WP_NONE; i++ )
  {
    if( bg_disabledGameElements.weapons[ i ] == weapon )
      return qfalse;
  }

  return qtrue;
}

/*
============
BG_UpgradeIsAllowed
============
*/
qboolean BG_UpgradeIsAllowed( upgrade_t upgrade )
{
  int i;

  for( i = 0; i < UP_NUM_UPGRADES &&
      bg_disabledGameElements.upgrades[ i ] != UP_NONE; i++ )
  {
    if( bg_disabledGameElements.upgrades[ i ] == upgrade )
      return qfalse;
  }

  return qtrue;
}

/*
============
BG_ClassIsAllowed
============
*/
qboolean BG_ClassIsAllowed( class_t class )
{
  int i;

  for( i = 0; i < PCL_NUM_CLASSES &&
      bg_disabledGameElements.classes[ i ] != PCL_NONE; i++ )
  {
    if( bg_disabledGameElements.classes[ i ] == class )
      return qfalse;
  }

  return qtrue;
}

/*
============
BG_BuildableIsAllowed
============
*/
qboolean BG_BuildableIsAllowed( buildable_t buildable )
{
  int i;

  for( i = 0; i < BA_NUM_BUILDABLES &&
      bg_disabledGameElements.buildables[ i ] != BA_NONE; i++ )
  {
    if( bg_disabledGameElements.buildables[ i ] == buildable )
      return qfalse;
  }

  return qtrue;
}

/*
============
BG_ClientListTest
============
*/
qboolean BG_ClientListTest( clientList_t *list, int clientNum )
{
  if( clientNum < 0 || clientNum >= MAX_CLIENTS || !list )
    return qfalse;
  if( clientNum < 32 )
    return ( ( list->lo & ( 1 << clientNum ) ) != 0 );
  else
    return ( ( list->hi & ( 1 << ( clientNum - 32 ) ) ) != 0 );
}

/*
============
BG_ClientListAdd
============
*/
void BG_ClientListAdd( clientList_t *list, int clientNum )
{
  if( clientNum < 0 || clientNum >= MAX_CLIENTS || !list )
    return;
  if( clientNum < 32 )
    list->lo |= ( 1 << clientNum );
  else
    list->hi |= ( 1 << ( clientNum - 32 ) );
}

/*
============
BG_ClientListRemove
============
*/
void BG_ClientListRemove( clientList_t *list, int clientNum )
{
  if( clientNum < 0 || clientNum >= MAX_CLIENTS || !list )
    return;
  if( clientNum < 32 )
    list->lo &= ~( 1 << clientNum );
  else
    list->hi &= ~( 1 << ( clientNum - 32 ) );
}

/*
============
BG_ClientListString
============
*/
char *BG_ClientListString( clientList_t *list )
{
  static char s[ 17 ];

  s[ 0 ] = '\0';
  if( !list )
    return s;
  Com_sprintf( s, sizeof( s ), "%08x%08x", list->hi, list->lo );
  return s;
}

/*
============
BG_ClientListParse
============
*/
void BG_ClientListParse( clientList_t *list, const char *s )
{
  if( !list )
    return;
  list->lo = 0;
  list->hi = 0;
  if( !s )
    return;
  if( strlen( s ) != 16 )
    return;
  sscanf( s, "%x%x", &list->hi, &list->lo );
}

/*
============
BG_PrimaryWeapon
============
*/
weapon_t BG_PrimaryWeapon( int stats[ ] )
{
  int i;

  for( i = WP_NONE; i < WP_NUM_WEAPONS; i++ )
  {
    if( BG_Weapon( i )->slots != SLOT_WEAPON )
      continue;
    if( BG_InventoryContainsWeapon( i, stats ) )
      return i;
  }
  if( BG_InventoryContainsWeapon( WP_BLASTER, stats ) )
    return WP_BLASTER;
  return WP_NONE;
}

/*
============
BG_LoadEmoticons
============
*/
int BG_LoadEmoticons( char names[ ][ MAX_EMOTICON_NAME_LEN ], int widths[ ] )
{
  int numFiles;
  char fileList[ MAX_EMOTICONS * ( MAX_EMOTICON_NAME_LEN + 9 ) ] = {""};
  int i;
  char *filePtr;
  int fileLen;
  char emoticon[ MAX_EMOTICON_NAME_LEN + 9 ] = {""};
  int loaded = 0;
  int count;
  int width = 0;

  numFiles = trap_FS_GetFileList( "emoticons", "x1.tga", fileList,
    sizeof( fileList ) );

  if( numFiles < 1 )
    return 0;

  filePtr = fileList;
  fileLen = 0;
  count = 0;
  for( i = 0; i < numFiles; i++, filePtr += fileLen + 1 )
  {
    if( count >= MAX_EMOTICONS )
    {
      count++;
      continue;
    }

    fileLen = strlen( filePtr );
    if( fileLen > MAX_EMOTICON_NAME_LEN + 8 )
    {
      Com_Printf( S_COLOR_YELLOW "WARNING: MAX_EMOTICON_NAME_LEN is %d. "
        "skipping \"%s\", filename too long", MAX_EMOTICON_NAME_LEN, filePtr );
      continue;
    }
    if( fileLen < 9 )
    {
      Com_Printf( S_COLOR_YELLOW "WARNING: skipping \"%s\", filename too short",
        filePtr );
      continue;
    }
    if( !trap_FS_FOpenFile( va( "emoticons/%s", filePtr ), NULL, FS_READ ) )
    {
      Com_Printf( S_COLOR_YELLOW "WARNING: BG_LoadEmoticons(): detected "
        " an unreadable .tga file name \"emoticons/%s\" in emoticon detection",
        filePtr );
      break;
    }

    Q_strncpyz( emoticon, filePtr, sizeof( emoticon ) );

    if( emoticon[ fileLen - 8 ] != '_' )
    {
      Com_Printf( S_COLOR_YELLOW "WARNING: skipping \"%s\", filename invalid",
        filePtr );
      continue;
    }

    width = emoticon[ fileLen - 7 ] - '0';

    if( width < 1 || width > 9 )
    {
      Com_Printf( S_COLOR_YELLOW "WARNING: skipping \"%s\", invalid width",
        filePtr );
      continue;
    }

    emoticon[ fileLen - 8 ] = '\0';

    Q_strncpyz( names[ count ], emoticon, sizeof( names[ count ] ) );
    if( widths )
      widths[ count ] = width;
    count++;
    loaded = count;
  }

  Com_Printf( "Loaded %d of %d emoticons (MAX_EMOTICONS is %d)\n",
    loaded, count, MAX_EMOTICONS );
  return loaded;
}

/*
============
BG_TeamName
============
*/
char *BG_TeamName( team_t team )
{
  if( team == TEAM_NONE )
    return "spectator";
  if( team == TEAM_ALIENS )
    return "alien";
  if( team == TEAM_HUMANS )
    return "human";
  return "<team>";
}

