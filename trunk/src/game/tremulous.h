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

/*
 * ALIEN weapons
 *
 * _REPEAT  - time in msec until the weapon can be used again
 * _DMG     - amount of damage the weapon does
 *
 * ALIEN_WDMG_MODIFIER - overall damage modifier for coarse tuning
 *
 */

#define ALIEN_WDMG_MODIFIER         1.0f
#define ADM(d)                      ((int)((float)d*ALIEN_WDMG_MODIFIER))

#define ABUILDER_BUILD_REPEAT       500
#define ABUILDER_CLAW_DMG           ADM(20)
#define ABUILDER_CLAW_RANGE         64.0f
#define ABUILDER_CLAW_WIDTH         4.0f
#define ABUILDER_CLAW_REPEAT        1000
#define ABUILDER_CLAW_K_SCALE       1.0f
#define ABUILDER_BLOB_DMG           ADM(4)
#define ABUILDER_BLOB_REPEAT        1000
#define ABUILDER_BLOB_SPEED         800.0f
#define ABUILDER_BLOB_SPEED_MOD     0.5f
#define ABUILDER_BLOB_TIME          5000

#define ABGRANGER_BUILD_REPEAT       300
#define ABGRANGER_CLAW_DMG           ADM(40)
#define ABGRANGER_CLAW_RANGE         64.0f
#define ABGRANGER_CLAW_WIDTH         4.0f
#define ABGRANGER_CLAW_REPEAT        750
#define ABGRANGER_CLAW_K_SCALE       1.0f
#define ABGRANGER_BLOB_DMG           ADM(8)
#define ABGRANGER_BLOB_REPEAT        1000
#define ABGRANGER_BLOB_SPEED         800.0f
#define ABGRANGER_BLOB_SPEED_MOD     0.5f
#define ABGRANGER_BLOB_TIME          5000

#define LEVEL0_BITE_DMG             ADM(48)
#define LEVEL0_BITE_RANGE           64.0f
#define LEVEL0_BITE_WIDTH           6.0f
#define LEVEL0_BITE_REPEAT          500
#define LEVEL0_BITE_K_SCALE         1.0f
#define LEVEL0_DRILL_TIME           600
#define LEVEL0_DRILL_TIME_MIN       200
#define LEVEL0_DRILL_REPEAT         200      // msec before a new pounce starts
#define LEVEL0_DRILL_SPEED_MOD      0.8f
#define LEVEL0_DRILL_JUMP_MAG       800
// #define LEVEL0_DRILL_SPEED          800
// #define LEVEL0_DRILL_CHARGE_TIME    600

#define SPITFIRE_ZAP_REPEAT          500
#define SPITFIRE_SPITBOMB_THINK_TIME 5000
#define SPITFIRE_SPITBOMB_REPEAT     500
#define SPITFIRE_SPITBOMB_SPEED      1000.0f
#define SPITFIRE_SPITBOMB_RANGE      150.0f
#define SPITFIRE_SPITBOMB_DMG        ADM(20)
#define SPITFIRE_SPITBOMB_REGEN      3000
#define SPITFIRE_EBLOB_BUILDABLE     0.50f

/*
#define LEVEL4_EBLOB_THINK_TIME     5000
#define LEVEL4_EBLOB_REPEAT         2000
#define LEVEL4_EBLOB_SPEED          500.0f
#define LEVEL4_EBLOB_RANGE          200.0f
#define LEVEL4_EBLOB_DMG            ADM(200)
#define LEVEL4_EBLOB_REGEN          10000
#define LEVEL4_EBLOB_BUILDABLE      0.50f    //The percentage of damage done to a buildable by acid
*/


#define LEVEL1_CLAW_DMG             ADM(32)
#define LEVEL1_CLAW_RANGE           96.0f
#define LEVEL1_CLAW_WIDTH           10.0f
#define LEVEL1_CLAW_REPEAT          600
#define LEVEL1_CLAW_U_REPEAT        500
#define LEVEL1_CLAW_K_SCALE         1.0f
#define LEVEL1_CLAW_U_K_SCALE       1.0f
#define LEVEL1_GRAB_RANGE           64.0f
#define LEVEL1_GRAB_TIME            300
#define LEVEL1_GRAB_U_TIME          300
#define LEVEL1_PCLOUD_DMG           ADM(4)
#define LEVEL1_PCLOUD_RANGE         200.0f
#define LEVEL1_PCLOUD_REPEAT        2000
#define LEVEL1_PCLOUD_TIME          10000

#define LEVEL2_CLAW_DMG             ADM(40)
#define LEVEL2_CLAW_RANGE           96.0f
#define LEVEL2_CLAW_WIDTH           12.0f
#define LEVEL2_CLAW_REPEAT          500
#define LEVEL2_CLAW_K_SCALE         1.0f
#define LEVEL2_CLAW_U_REPEAT        400
#define LEVEL2_CLAW_U_K_SCALE       1.0f
#define LEVEL2_AREAZAP_DMG          ADM(20)
#define LEVEL2_AREAZAP_RANGE        200.0f
#define LEVEL2_AREAZAP_CUTOFF       300.0f
#define LEVEL2_AREAZAP_REPEAT       1500
#define LEVEL2_AREAZAP_MAX_TARGETS  3
#define LEVEL2_WALLJUMP_MAXSPEED    1000.0f
#define LEVEL2_WALLJUMP_NORMAL      1.0      // magnitude scale from surface
#define LEVEL2_WALLJUMP_FORWARD     1.5      // magnitude scale in view direction
#define LEVEL2_WALLJUMP_UP          0.0      // magnitude scale up
#define LEVEL2_WALLJUMP_REPEAT      400      // msec before new jump
#define LEVEL2_WALLJUMP_RANGE       8.0      // how far away the wall can be

#define LEVEL2_BOUNCEBALL_DMG       ADM(35)
#define LEVEL2_BOUNCEBALL_REPEAT    300
#define LEVEL2_BOUNCEBALL_SPEED     3300.0f
#define LEVEL2_BOUNCEBALL_RADIUS    0
#define LEVEL2_BOUNCEBALL_SPLASH_DMG 0
#define LEVEL2_BOUNCEBALL_TIME      4000
#define LEVEL2_BOUNCEBALL_REGEN     1600

#define LEVEL3_CLAW_DMG             ADM(80)
#define LEVEL3_CLAW_RANGE           96.0f
#define LEVEL3_CLAW_UPG_RANGE       LEVEL3_CLAW_RANGE + 8.0f
#define LEVEL3_CLAW_WIDTH           16.0f
#define LEVEL3_CLAW_REPEAT          700
#define LEVEL3_CLAW_K_SCALE         1.0f
#define LEVEL3_CLAW_U_REPEAT        600
#define LEVEL3_CLAW_U_K_SCALE       1.0f
#define LEVEL3_POUNCE_DMG           ADM(100)
#define LEVEL3_POUNCE_RANGE         72.0f
#define LEVEL3_POUNCE_UPG_RANGE     LEVEL3_POUNCE_RANGE + 6.0f
#define LEVEL3_POUNCE_WIDTH         16.0f
#define LEVEL3_POUNCE_TIME          700      // msec for full Dragoon pounce
#define LEVEL3_POUNCE_TIME_UPG      700      // msec for full Adv. Dragoon pounce
#define LEVEL3_POUNCE_TIME_MIN      200      // msec before which pounce cancels  
#define LEVEL3_POUNCE_REPEAT        400      // msec before a new pounce starts
#define LEVEL3_POUNCE_SPEED_MOD     0.75f    // walking speed modifier for pounce charging
#define LEVEL3_POUNCE_JUMP_MAG      700      // Dragoon pounce jump power
#define LEVEL3_POUNCE_JUMP_MAG_UPG  800      // Adv. Dragoon pounce jump power
#define LEVEL3_BOUNCEBALL_DMG       ADM(110)
#define LEVEL3_BOUNCEBALL_REPEAT    1200
#define LEVEL3_BOUNCEBALL_SPEED     1000.0f
#define LEVEL3_BOUNCEBALL_RADIUS    1       // overpowered, so nerfed from 75->1
#define LEVEL3_BOUNCEBALL_SPLASH_DMG LEVEL3_BOUNCEBALL_DMG
#define LEVEL3_BOUNCEBALL_REGEN     10000    // msec until new barb

#define LEVEL4_CLAW_DMG             ADM(100)
#define LEVEL4_CLAW_RANGE           128.0f
#define LEVEL4_CLAW_WIDTH           14.0f
#define LEVEL4_CLAW_HEIGHT          20.0f
#define LEVEL4_CLAW_REPEAT          750
#define LEVEL4_CLAW_K_SCALE         1.0f
#define LEVEL4_CLAW_U_K_SCALE       1.1f

#define LEVEL4_TRAMPLE_DMG             ADM(55)
#define LEVEL4_TRAMPLE_SPEED           2.0f
#define LEVEL4_TRAMPLE_CHARGE_MIN      375   // minimum msec to start a charge
#define LEVEL4_TRAMPLE_CHARGE_MAX      1000  // msec to maximum charge stored
#define LEVEL4_TRAMPLE_CHARGE_TRIGGER  1000  // msec charge starts on its own
#define LEVEL4_TRAMPLE_DURATION        3000  // msec trample lasts on full charge
//#define LEVEL4_TRAMPLE_STOP_PERCENTAGE 20    // removed from the end of trample when it isn't very useful
#define LEVEL4_TRAMPLE_STOP_PENALTY    1     // charge lost per msec when stopped
#define LEVEL4_TRAMPLE_REPEAT          100 // msec before a trample will rehit a player

#define LEVEL4_CRUSH_DAMAGE_PER_V      0.5f  // damage per falling velocity
#define LEVEL4_CRUSH_DAMAGE            120   // to players only
#define LEVEL4_CRUSH_REPEAT            500   // player damage repeat

#define LEVEL4_EBLOB_THINK_TIME     5000
#define LEVEL4_EBLOB_REPEAT         2000
#define LEVEL4_EBLOB_SPEED          500.0f
#define LEVEL4_EBLOB_RANGE          200.0f
#define LEVEL4_EBLOB_DMG            ADM(200)
#define LEVEL4_EBLOB_REGEN          10000
#define LEVEL4_EBLOB_BUILDABLE      0.50f    //The percentage of damage done to a buildable by acid

/*
 * ALIEN classes
 *
 * _SPEED   - fraction of Q3A run speed the class can move
 * _REGEN   - health per second regained
 *
 * ALIEN_HLTH_MODIFIER - overall health modifier for coarse tuning
 *
 */

#define ALIEN_HLTH_MODIFIER         1.0f
#define AHM(h)                      ((int)((float)h*ALIEN_HLTH_MODIFIER))

#define ALIEN_VALUE_MODIFIER        1.10f
#define AVM(h)                      ((int)((float)h*ALIEN_VALUE_MODIFIER))

#define ALIEN_REGEN_DAMAGE_TIME     1500 //msec since damage that regen starts again
#define ALIEN_REGEN_TIME            50000//msec that it takes for an alien to heal all the way
#define ARMSEC(h)                   ((int)((float)ALIEN_REGEN_TIME/h))

#define ABUILDER_SPEED              0.9f
#define ABUILDER_VALUE              AVM(200)
#define ABUILDER_HEALTH             AHM(50)
#define ABUILDER_REGEN              2
#define ABUILDER_REGEN_MSEC         ARMSEC(ABUILDER_HEALTH)
#define ABUILDER_COST               0

#define ABUILDER_UPG_SPEED          1.1f
#define ABUILDER_UPG_VALUE          AVM(250)
#define ABUILDER_UPG_HEALTH         AHM(75)
#define ABUILDER_UPG_REGEN          3
#define ABUILDER_UPG_REGEN_MSEC     ARMSEC(ABUILDER_UPG_HEALTH)
#define ABUILDER_UPG_COST           0

#define ABGRANGER_SPEED             1.3f
#define ABGRANGER_VALUE             AVM(300)
#define ABGRANGER_HEALTH            AHM(100)
#define ABGRANGER_REGEN             4
#define ABGRANGER_REGEN_MSEC        ARMSEC(ABUILDER_UPG_HEALTH)
#define ABGRANGER_COST              1

#define LEVEL0_SPEED                1.4f
#define LEVEL0_VALUE                AVM(175)
#define LEVEL0_HEALTH               AHM(25)
#define LEVEL0_REGEN                1
#define LEVEL0_REGEN_MSEC           ARMSEC(LEVEL0_HEALTH)
#define LEVEL0_COST                 0

#define LEVEL0_UPG_SPEED            1.4f
#define LEVEL0_UPG_VALUE            AVM(200)
#define LEVEL0_UPG_HEALTH           AHM(50)
#define LEVEL0_UPG_REGEN            2
#define LEVEL0_UPG_REGEN_MSEC       ARMSEC(LEVEL0_UPG_HEALTH)
#define LEVEL0_UPG_COST             1

#define SPITFIRE_SPEED              0.8f
#define SPITFIRE_VALUE              AVM(210)
#define SPITFIRE_HEALTH             AHM(65)
#define SPITFIRE_REGEN              2
#define SPITFIRE_REGEN_MSEC         ARMSEC(SPITFIRE_HEALTH)
#define SPITFIRE_COST               1

#define SPITFIRE_UPG_SPEED          0.8f
#define SPITFIRE_UPG_VALUE          AVM(250)
#define SPITFIRE_UPG_HEALTH         AHM(90)
#define SPITFIRE_UPG_REGEN          3
#define SPITFIRE_UPG_REGEN_MSEC     ARMSEC(SPITFIRE_UPG_HEALTH)
#define SPITFIRE_UPG_COST               2

#define LEVEL1_SPEED                1.25f
#define LEVEL1_VALUE                AVM(225)
#define LEVEL1_HEALTH               AHM(75)
#define LEVEL1_REGEN                3
#define LEVEL1_REGEN_MSEC           ARMSEC(LEVEL1_HEALTH)
#define LEVEL1_COST                 1
#define LEVEL1_REGEN_MOD            1.5f

#define LEVEL1_UPG_SPEED            1.25f
#define LEVEL1_UPG_VALUE            AVM(275)
#define LEVEL1_UPG_HEALTH           AHM(100)
#define LEVEL1_UPG_REGEN            3
#define LEVEL1_UPG_REGEN_MSEC       ARMSEC(LEVEL1_UPG_HEALTH)
#define LEVEL1_UPG_COST             1

#define LEVEL2_SPEED                1.2f
#define LEVEL2_VALUE                AVM(350)
#define LEVEL2_HEALTH               AHM(150)
#define LEVEL2_REGEN                4
#define LEVEL2_REGEN_MSEC           ARMSEC(LEVEL2_HEALTH)
#define LEVEL2_COST                 1

#define LEVEL2_UPG_SPEED            1.2f
#define LEVEL2_UPG_VALUE            AVM(450)
#define LEVEL2_UPG_HEALTH           AHM(175)
#define LEVEL2_UPG_REGEN            5
#define LEVEL2_UPG_REGEN_MSEC       ARMSEC(LEVEL2_UPG_HEALTH)
#define LEVEL2_UPG_COST             1

#define LEVEL3_SPEED                1.1f
#define LEVEL3_VALUE                AVM(500)
#define LEVEL3_HEALTH               AHM(200)
#define LEVEL3_REGEN                6
#define LEVEL3_REGEN_MSEC           ARMSEC(LEVEL3_HEALTH)
#define LEVEL3_COST                 1

#define LEVEL3_UPG_SPEED            1.1f
#define LEVEL3_UPG_VALUE            AVM(600)
#define LEVEL3_UPG_HEALTH           AHM(250)
#define LEVEL3_UPG_REGEN            7
#define LEVEL3_UPG_REGEN_MSEC       ARMSEC(LEVEL3_UPG_HEALTH)
#define LEVEL3_UPG_COST             1

#define LEVEL4_SPEED                1.2f
#define LEVEL4_VALUE                AVM(800)
#define LEVEL4_HEALTH               AHM(400)
#define LEVEL4_REGEN                9
#define LEVEL4_REGEN_MSEC           ARMSEC(LEVEL4_HEALTH)
#define LEVEL4_COST                 2
#define LEVEL4_REGEN_RANGE          200.0f

#define LEVEL4_UPG_SPEED            1.2f
#define LEVEL4_UPG_VALUE            AVM(1000)
#define LEVEL4_UPG_HEALTH           AHM(600)
#define LEVEL4_UPG_REGEN            11
#define LEVEL4_UPG_REGEN_MSEC       ARMSEC(LEVEL4_UPG_HEALTH)
#define LEVEL4_UPG_COST             5
#define LEVEL4_UPG_REGEN_MOD        1.5f

/*
 * ALIEN buildables
 *
 * _BP            - build points required for this buildable
 * _BT            - build time required for this buildable
 * _REGEN         - the amount of health per second regained
 * _SPLASHDAMGE   - the amount of damage caused by this buildable when melting
 * _SPLASHRADIUS  - the radius around which it does this damage
 *
 * CREEP_BASESIZE - the maximum distance a buildable can be from an egg/overmind
 * ALIEN_BHLTH_MODIFIER - overall health modifier for coarse tuning
 *
 */

#define ALIEN_BHLTH_MODIFIER        1.0f
#define ABHM(h)                     ((int)((float)h*ALIEN_BHLTH_MODIFIER))
#define ALIEN_BVALUE_MODIFIER       90.0f
#define ABVM(h)                      ((int)((float)h*ALIEN_BVALUE_MODIFIER))

#define ALIEN_DETONATION_DELAY      1500
#define CREEP_BASESIZE              700
#define CREEP_TIMEOUT               1000
#define CREEP_MODIFIER              0.5f
#define CREEP_ARMOUR_MODIFIER       0.75f
#define CREEP_SCALEDOWN_TIME        3000
#define CREEP_REGEN_MOD             1.5f

#define PCLOUD_MODIFIER             0.5f
#define PCLOUD_ARMOUR_MODIFIER      0.75f

#define ASPAWN_BP                   10
#define ASPAWN_BT                   15000
#define ASPAWN_HEALTH               ABHM(250)
#define ASPAWN_REGEN                8
#define ASPAWN_SPLASHDAMAGE         50
#define ASPAWN_SPLASHRADIUS         50
#define ASPAWN_CREEPSIZE            120
#define ASPAWN_VALUE                ABVM(10)

#define BARRICADE_BP                6
#define BARRICADE_BT                20000
#define BARRICADE_HEALTH            ABHM(600)
#define BARRICADE_REGEN             14
#define BARRICADE_SPLASHDAMAGE      50
#define BARRICADE_SPLASHRADIUS      50
#define BARRICADE_CREEPSIZE         120
#define BARRICADE_SHRINKPROP        0.25f
#define BARRICADE_SHRINKTIMEOUT     500
#define BARRICADE_VALUE             ABVM(0)

#define BOOSTER_BP                  14
#define BOOSTER_BT                  15000
#define BOOSTER_HEALTH              ABHM(150)
#define BOOSTER_REGEN               8
#define BOOSTER_SPLASHDAMAGE        50
#define BOOSTER_SPLASHRADIUS        50
#define BOOSTER_CREEPSIZE           120
#define BOOSTER_REGEN_MOD           2.0f
#define BOOSTER_VALUE               ABVM(3)
#define BOOST_TIME                  30000
#define BOOST_WARN_TIME             25000

#define ACIDTUBE_BP                 6
#define ACIDTUBE_BT                 15000
#define ACIDTUBE_HEALTH             ABHM(125)
#define ACIDTUBE_REGEN              10
#define ACIDTUBE_SPLASHDAMAGE       50
#define ACIDTUBE_SPLASHRADIUS       50
#define ACIDTUBE_CREEPSIZE          120
#define ACIDTUBE_DAMAGE             6
#define ACIDTUBE_RANGE              300.0f
#define ACIDTUBE_REPEAT             300
#define ACIDTUBE_REPEAT_ANIM        2000
#define ACIDTUBE_VALUE              ABVM(0)

#define HIVE_BP                     12
#define HIVE_BT                     20000
#define HIVE_HEALTH                 ABHM(175)
#define HIVE_REGEN                  10
#define HIVE_SPLASHDAMAGE           30
#define HIVE_SPLASHRADIUS           200
#define HIVE_CREEPSIZE              120
#define HIVE_SENSE_RANGE            400.0f
#define HIVE_LIFETIME               6000
#define HIVE_REPEAT                 3000
#define HIVE_K_SCALE                1.0f
#define HIVE_DMG                    100
#define HIVE_SPEED                  384.0f
#define HIVE_DIR_CHANGE_PERIOD      500
#define HIVE_VALUE                  ABVM(0)

#define TRAPPER_BP                  8
#define TRAPPER_BT                  12000
#define TRAPPER_HEALTH              ABHM(50)
#define TRAPPER_REGEN               6
#define TRAPPER_SPLASHDAMAGE        15
#define TRAPPER_SPLASHRADIUS        100
#define TRAPPER_CREEPSIZE           30
#define TRAPPER_RANGE               400
#define TRAPPER_REPEAT              1000
#define TRAPPER_VALUE               ABVM(0)
#define LOCKBLOB_SPEED              650.0f
#define LOCKBLOB_LOCKTIME           5000
#define LOCKBLOB_DOT                0.85f // max angle = acos( LOCKBLOB_DOT )
#define LOCKBLOB_K_SCALE            1.0f

#define OVERMIND_BP                 0
#define OVERMIND_BT                 30000
#define OVERMIND_HEALTH             ABHM(750)
#define OVERMIND_REGEN              6
#define OVERMIND_SPLASHDAMAGE       15
#define OVERMIND_SPLASHRADIUS       300
#define OVERMIND_CREEPSIZE          120
#define OVERMIND_ATTACK_RANGE       150.0f
#define OVERMIND_ATTACK_REPEAT      1000
#define OVERMIND_VALUE              ABVM(15)

#define HOVEL_BP                    0
#define HOVEL_BT                    15000
#define HOVEL_HEALTH                ABHM(375)
#define HOVEL_REGEN                 20
#define HOVEL_SPLASHDAMAGE          20
#define HOVEL_SPLASHRADIUS          200
#define HOVEL_CREEPSIZE             120
#define HOVEL_VALUE                 ABVM(0)



/*
 * ALIEN misc
 *
 * ALIENSENSE_RANGE - the distance alien sense is useful for
 *
 */

#define ALIENSENSE_RANGE            1000.0f
#define REGEN_BOOST_RANGE           200.0f

#define ALIEN_POISON_TIME           5000
#define ALIEN_POISON_DMG            5
#define ALIEN_POISON_DIVIDER        (1.0f/1.32f) //about 1.0/(time`th root of damage)

#define ALIEN_SPAWN_REPEAT_TIME     10000

#define ALIEN_MAX_REGEN_MOD         4.0f

#define ALIEN_MAX_FRAGS             9
#define ALIEN_MAX_CREDITS           (ALIEN_MAX_FRAGS*ALIEN_CREDITS_PER_FRAG)
#define ALIEN_CREDITS_PER_FRAG      400
#define ALIEN_TK_SUICIDE_PENALTY    350

// Allow Aliens to wallwalk on any entity (buildables, etc) but not players
#define ALIEN_WALLWALK_ENTITIES

/*
 * HUMAN weapons
 *
 * _REPEAT  - time between firings
 * _RELOAD  - time needed to reload
 * _PRICE   - amount in credits weapon costs
 *
 * HUMAN_WDMG_MODIFIER - overall damage modifier for coarse tuning
 *
 */

#define HUMAN_WDMG_MODIFIER         1.0f
#define HDM(d)                      ((int)((float)d*HUMAN_WDMG_MODIFIER))

#define BLASTER_REPEAT              400
#define BLASTER_K_SCALE             1.0f
#define BLASTER_SPREAD              200
#define BLASTER_SPEED               2000
#define BLASTER_DMG                 HDM(9)
#define BLASTER_SIZE                5
#define BLASTER_SURGE_DMG_MOD       1.0f
#define BLASTER_SURGE_TIME_MOD      0.625

#define RIFLE_CLIPSIZE              30
#define RIFLE_MAXCLIPS              6
#define RIFLE_REPEAT                90
#define RIFLE_K_SCALE               1.0f
#define RIFLE_RELOAD                2000
#define RIFLE_PRICE                 0
#define RIFLE_SPREAD                200
#define RIFLE_DMG                   HDM(5)
#define RIFLE_RECOIL_MIN_Y          0.25
#define RIFLE_RECOIL_MAX_Y          1.2
#define RIFLE_RECOIL_MAX_X          0.3
#define RIFLE_RECOIL_SOFTEN         0.85

#define PAINSAW_PRICE               100
#define PAINSAW_REPEAT              75
#define PAINSAW_K_SCALE             1.0f
#define PAINSAW_DAMAGE              HDM(15)
#define PAINSAW_RANGE               40.0f
#define PAINSAW_WIDTH               0.0f
#define PAINSAW_HEIGHT              8.0f

#define GRENADE_PRICE               200
#define GRENADE_REPEAT              0
#define GRENADE_K_SCALE             1.0f
#define GRENADE_DAMAGE              HDM(310)
#define GRENADE_RANGE               192.0f
#define GRENADE_SPEED               400.0f

#define SHOTGUN_PRICE               150
#define SHOTGUN_SHELLS              8
#define SHOTGUN_PELLETS             8 //used to sync server and client side
#define SHOTGUN_MAXCLIPS            3
#define SHOTGUN_REPEAT              1000
#define SHOTGUN_K_SCALE             1.0f
#define SHOTGUN_RELOAD              2000
#define SHOTGUN_SPREAD              600
#define SHOTGUN_DMG                 HDM(7)
#define SHOTGUN_RANGE               (8192 * 12)
#define SHOTGUN_PRI_RECOIL_MIN_Y    4
#define SHOTGUN_PRI_RECOIL_MAX_Y    10
#define SHOTGUN_PRI_RECOIL_MAX_X    7
#define SHOTGUN_PRI_RECOIL_SOFTEN   0.85
#define SHOTGUN_SEC_RECOIL_MIN_Y    8
#define SHOTGUN_SEC_RECOIL_MAX_Y    20
#define SHOTGUN_SEC_RECOIL_MAX_X    7
#define SHOTGUN_SEC_RECOIL_SOFTEN   0.6


#define SHOTGUN_NADE_REPEAT         1000
#define SHOTGUN_NADE_DAMAGE         HDM(40)
#define SHOTGUN_NADE_RANGE          100.0f
#define SHOTGUN_NADE_SPEED          800.0f
#define SHOTGUN_NADE_TAKE           2

#define LASGUN_PRICE                250
#define LASGUN_AMMO                 200
#define LASGUN_REPEAT               200
#define LASGUN_K_SCALE              1.0f
#define LASGUN_RELOAD               2000
#define LASGUN_DAMAGE               HDM(8)
#define LASGUN_SURGE_DMG_MOD        1.25f
#define LASGUN_SURGE_TIME_MOD       0.8

#define MDRIVER_PRICE               350
#define MDRIVER_CLIPSIZE            5
#define MDRIVER_MAXCLIPS            4
#define MDRIVER_DMG                 HDM(40)
#define MDRIVER_REPEAT              1000
#define MDRIVER_K_SCALE             1.0f
#define MDRIVER_RELOAD              2000
#define MDRIVER_MAX_HITS            16
#define MDRIVER_WIDTH               5
#define MDRIVER_SURGE_DMG_MOD       1.2f
#define MDRIVER_SURGE_TIME_MOD      0.75
#define MDRIVER_RECOIL_MIN_Y        20.0
#define MDRIVER_RECOIL_MAX_Y        40.0
#define MDRIVER_RECOIL_MAX_X        20.0
#define MDRIVER_RECOIL_SOFTEN       0.70

#define CHAINGUN_PRICE              400
#define CHAINGUN_BULLETS            300
#define CHAINGUN_REPEAT             80
#define CHAINGUN_K_SCALE            1.0f
#define CHAINGUN_SPREAD             1000
#define CHAINGUN_DMG                HDM(8)
#define CHAINGUN_PRI_RECOIL_MIN_Y   0.15
#define CHAINGUN_PRI_RECOIL_MAX_Y   1.5
#define CHAINGUN_PRI_RECOIL_MAX_X   2
#define CHAINGUN_PRI_RECOIL_SOFTEN  0.75
#define CHAINGUN_SEC_RECOIL_MIN_Y   0.25
#define CHAINGUN_SEC_RECOIL_MAX_Y   1.6
#define CHAINGUN_SEC_RECOIL_MAX_X   3
#define CHAINGUN_SEC_RECOIL_SOFTEN  0.85

#define CHAINGUN_REPEAT_2           60
#define CHAINGUN_SPREAD_2           600
#define CHAINGUN_DMG_2              HDM(4)

#define PRIFLE_PRICE                400
#define PRIFLE_CLIPS                50
#define PRIFLE_MAXCLIPS             4
#define PRIFLE_REPEAT               100
#define PRIFLE_K_SCALE              1.0f
#define PRIFLE_RELOAD               2000
#define PRIFLE_DMG                  HDM(9)
#define PRIFLE_SPEED                1000
#define PRIFLE_SIZE                 5

#define PRIFLE_SECONDARY_SPEED      400
#define PRIFLE_SECONDARY_REPEAT     500

#define FLAMER_PRICE                450
#define FLAMER_GAS                  150
#define FLAMER_REPEAT               200
#define FLAMER_K_SCALE              1.0f
#define FLAMER_DMG                  HDM(15)
#define FLAMER_RADIUS               50       // splash radius
#define FLAMER_SIZE                 15       // missile bounding box
#define FLAMER_LIFETIME             800.0f
#define FLAMER_SPEED                200.0f
#define FLAMER_LAG                  0.65f  //the amount of player velocity that is added to the fireball

#define LCANNON_PRICE               600
#define LCANNON_AMMO                120
#define LCANNON_K_SCALE             1.0f
#define LCANNON_REPEAT              500
#define LCANNON_RELOAD              0
#define LCANNON_DAMAGE              HDM(250)
#define LCANNON_RADIUS              150      // primary splash damage radius
#define LCANNON_SIZE                5        // missile bounding box radius
#define LCANNON_SECONDARY_DAMAGE    HDM(15)
#define LCANNON_SECONDARY_RADIUS    75       // secondary splash damage radius
#define LCANNON_SECONDARY_SPEED     1700
#define LCANNON_SECONDARY_RELOAD    2000
#define LCANNON_SECONDARY_REPEAT    1000
#define LCANNON_SPEED               425
#define LCANNON_CHARGE_TIME_MAX     2000
#define LCANNON_CHARGE_TIME_MIN     100
#define LCANNON_CHARGE_TIME_WARN    1250
#define LCANNON_CHARGE_AMMO         10       // ammo cost of a full charge shot
#define LCANNON_PRI_RECOIL_MIN_Y    10
#define LCANNON_PRI_RECOIL_MAX_Y    25
#define LCANNON_PRI_RECOIL_MAX_X    20
#define LCANNON_PRI_RECOIL_SOFTEN   0.35f
#define LCANNON_SEC_RECOIL_MIN_Y    5
#define LCANNON_SEC_RECOIL_MAX_Y    10
#define LCANNON_SEC_RECOIL_MAX_X    10
#define LCANNON_SEC_RECOIL_SOFTEN   0.25f
#define LCANNON_SURGE_DMG_MOD       1.2f
#define LCANNON_SURGE_SEC_TIME_MOD 0.75f


#define XAEL_PRICE                  600
#define XAEL_AMMO                   120
#define XAEL_REPEAT                 500
#define XAEL_K_SCALE                1.0f
#define XAEL_CHARGEREPEAT           1000
#define XAEL_RELOAD                 2000
#define XAEL_DAMAGE                 HDM(250)
#define XAEL_RADIUS                 175
#define XAEL_SIZE                   5        // missile bounding box radius
#define XAEL_SECONDARY_DAMAGE       HDM(15)
#define XAEL_SECONDARY_RADIUS       75
#define XAEL_SECONDARY_SPEED        1700
#define XAEL_SECONDARY_REPEAT       1000
#define XAEL_SPEED                  800
#define XAEL_CHARGE_TIME            2000
#define XAEL_TOTAL_CHARGE           255
#define XAEL_MIN_CHARGE             50
#define XAEL_CHARGE_TIME_MAX        2000
#define XAEL_CHARGE_TIME_MIN        150
#define XAEL_CHARGE_TIME_WARN       1250
#define XAEL_CHARGE_AMMO            10
#define XAEL_PRI_RECOIL_MIN_Y       20
#define XAEL_PRI_RECOIL_MAX_Y       50
#define XAEL_PRI_RECOIL_MAX_X       30
#define XAEL_PRI_RECOIL_SOFTEN      0.35
#define XAEL_SEC_RECOIL_MIN_Y	      10
#define XAEL_SEC_RECOIL_MAX_Y       20
#define XAEL_SEC_RECOIL_MAX_X       10
#define XAEL_SEC_RECOIL_SOFTEN      0.25
#define XAEL_SURGE_DMG_MOD          1.2f
#define XAEL_SURGE_PRIM_TIME_MOD     0.75f

#define HBUILD_PRICE                0
#define HBUILD_REPEAT               500
#define HBUILD_HEALRATE             9



/*
 * HUMAN upgrades
 */

#define LIGHTARMOUR_PRICE           0
#define LIGHTARMOUR_POISON_PROTECTION 1
#define LIGHTARMOUR_PCLOUD_PROTECTION 0

#define HELMET_PRICE                90
#define HELMET_RANGE                1000.0f
#define HELMET_POISON_PROTECTION    2
#define HELMET_PCLOUD_PROTECTION    1000

#define MEDKIT_PRICE                0

#define BATTPACK_PRICE              100
#define BATTPACK_MODIFIER           1.5f //modifier for extra energy storage available

#define AMMOPACK_PRICE              100
#define AMMOPACK_MODIFIER           1.4f //modifier for extra ammo storage available

#define REGEN_PRICE                 250
#define REGEN_HEALTH_RATE           2
#define REGEN_STAMINA_RATE          25

#define SURGE_PRICE                 200

#define CLOAK_PRICE                 400
#define CLOAK_TIME                  40

#define JETPACK_PRICE               120
#define JETPACK_FLOAT_SPEED         128.0f //up movement speed
#define JETPACK_SINK_SPEED          192.0f //down movement speed
#define JETPACK_DISABLE_TIME        1000   //time to disable the jetpack when player damaged
#define JETPACK_DISABLE_CHANCE      0.3f
#define JETPACK_CHARGE_CAPACITY     90     // JPU (Jetpack Power Units) (Consumption Rate = 1 JPU/s)
#define JETPACK_STD_CHARGE_RATE     1.0   // x JPU/s (Onboard Recharge)
#define JETPACK_RC_CHARGE_RATE      2.0   // x JPU/s (Reactor Charge Bonus) (RC Up = RC Charge Bonus + Onboard Charge Rate)
#define JETPACK_RC_CHARGE_DELAY     10000 // Milliseconds from when jetpack was last deactivated before RC starts giving extra charge
#define JETPACK_FAILURE             10     // Jetpack encounters intermittent failure when its charge is equal to or less than this
#define JETPACK_EXPLODE_MOD         7      // Range and damage multiplier for shotgun nade - used when a jetpack explodes due to mara zap
#define JETPACK_EXPLODE_CHANCE      .001f  // Chance that the jetpack will explode, as a float, not percent

/*
  for 60 JPUs
  standard recharge rate (1 JPU/s) -> 0% to 25% in 10 seconds
  standard recharge rate (1 JPU/s) -> 0% to 100% in 40 seconds
  noRC recharge rate (0.6 JPU/s) -> 0% to 25% in 25 seconds
  noRC recharge rate (0.6 JPU/s -> 0% to 100% in 100 seconds
*/

#define BSUIT_PRICE                 350
#define BSUIT_POISON_PROTECTION     4
#define BSUIT_PCLOUD_PROTECTION     3000

#define MGCLIP_PRICE                0

#define CGAMMO_PRICE                0

#define GAS_PRICE                   0

#define MEDKIT_POISON_IMMUNITY_TIME 0
#define MEDKIT_STARTUP_TIME         4000
#define MEDKIT_STARTUP_SPEED        5


/*
 * HUMAN buildables
 *
 * _BP            - build points required for this buildable
 * _BT            - build time required for this buildable
 * _SPLASHDAMGE   - the amount of damage caused by this buildable when it blows up
 * _SPLASHRADIUS  - the radius around which it does this damage
 *
 * REACTOR_BASESIZE - the maximum distance a buildable can be from an reactor
 * REPEATER_BASESIZE - the maximum distance a buildable can be from a repeater
 * HUMAN_BHLTH_MODIFIER - overall health modifier for coarse tuning
 *
 */

#define HUMAN_BHLTH_MODIFIER        1.0f
#define HBHM(h)                     ((int)((float)h*HUMAN_BHLTH_MODIFIER))
#define HUMAN_BVALUE_MODIFIER       50.0f
#define HBVM(h)                     ((int)((float)h*(float)HUMAN_BVALUE_MODIFIER)) // remember these are measured in credits not frags (c.f. ALIEN_CREDITS_PER_FRAG)

#define REACTOR_BASESIZE            1000
#define REPEATER_BASESIZE           500
#define HUMAN_DETONATION_DELAY      1500

#define HSPAWN_BP                   10
#define HSPAWN_BT                   10000
#define HSPAWN_HEALTH               HBHM(310)
#define HSPAWN_DCC_REGEN            2
#define HSPAWN_SPLASHDAMAGE         50
#define HSPAWN_SPLASHRADIUS         100
#define HSPAWN_VALUE                HBVM(HSPAWN_BP)

#define MEDISTAT_BP                 8
#define MEDISTAT_BT                 10000
#define MEDISTAT_HEALTH             HBHM(190)
#define MEDISTAT_DCC_REGEN          2
#define MEDISTAT_SPLASHDAMAGE       50
#define MEDISTAT_SPLASHRADIUS       100
#define MEDISTAT_VALUE              HBVM(MEDISTAT_BP)

#define MGTURRET_BP                 8
#define MGTURRET_BT                 10000
#define MGTURRET_HEALTH             HBHM(190)
#define MGTURRET_DCC_REGEN          5
#define MGTURRET_SPLASHDAMAGE       100
#define MGTURRET_SPLASHRADIUS       100
#define MGTURRET_ANGULARSPEED       8
#define MGTURRET_ANGULARSPEED_GRAB  3
#define MGTURRET_ANGULARSPEED_DCC   10
#define MGTURRET_ACCURACY_TO_FIRE   7
#define MGTURRET_VERTICALCAP        30  // +/- maximum pitch
#define MGTURRET_REPEAT             100
#define MGTURRET_K_SCALE            1.0f
#define MGTURRET_RANGE              300.0f
#define MGTURRET_SPREAD             200
#define MGTURRET_DMG                HDM(4)
#define MGTURRET_SPINUP_TIME        150 // time between target sighted and fire
#define MGTURRET_DROOPSCALE         0.5f // rate at which turret droops when unpowered

#define MGTURRET_VALUE              HBVM(0)

#define TESLAGEN_BP                 10
#define TESLAGEN_BT                 15000
#define TESLAGEN_HEALTH             HBHM(220)
#define TESLAGEN_DCC_REGEN          6
#define TESLAGEN_SPLASHDAMAGE       50
#define TESLAGEN_SPLASHRADIUS       100
#define TESLAGEN_REPEAT             250
#define TESLAGEN_K_SCALE            4.0f
#define TESLAGEN_RANGE              250
#define TESLAGEN_DMG                HDM(12)
#define TESLAGEN_VALUE              HBVM(0)

#define DC_BP                       8
#define DC_BT                       10000
#define DC_HEALTH                   HBHM(190)
#define DC_DCC_REGEN                4
#define DC_SPLASHDAMAGE             50
#define DC_SPLASHRADIUS             100
#define DC_ATTACK_PERIOD            10000 // how often to spam "under attack"
#define DC_HEALRATE                 6
#define DC_RANGE                    10000
#define DC_VALUE                    HBVM(DC_BP)

#define ARMOURY_BP                  10
#define ARMOURY_BT                  10000
#define ARMOURY_HEALTH              HBHM(280)
#define ARMOURY_DCC_REGEN           2
#define ARMOURY_SPLASHDAMAGE        50
#define ARMOURY_SPLASHRADIUS        100
#define ARMOURY_VALUE               HBVM(ARMOURY_BP)

#define REACTOR_BP                  0
#define REACTOR_BT                  20000
#define REACTOR_HEALTH              HBHM(930)
#define REACTOR_DCC_REGEN           4
#define REACTOR_SPLASHDAMAGE        200
#define REACTOR_SPLASHRADIUS        300
#define REACTOR_ATTACK_RANGE        100.0f
#define REACTOR_ATTACK_REPEAT       1000
#define REACTOR_ATTACK_DAMAGE       40
#define REACTOR_ATTACK_DCC_REPEAT   1000
#define REACTOR_ATTACK_DCC_RANGE    150.0f
#define REACTOR_ATTACK_DCC_DAMAGE   40
#define REACTOR_VALUE               HBVM(20)

#define REPEATER_BP                 0
#define REPEATER_BT                 10000
#define REPEATER_HEALTH             HBHM(250)
#define REPEATER_DCC_REGEN          2
#define REPEATER_SPLASHDAMAGE       50
#define REPEATER_SPLASHRADIUS       100
#define REPEATER_INACTIVE_TIME      90000
#define REPEATER_VALUE              HBVM(0)

#define FORCEFIELD_BP                 8
#define FORCEFIELD_BT                 10000
#define FORCEFIELD_HEALTH             HBHM(800)
#define FORCEFIELD_SPLASHDAMAGE       50
#define FORCEFIELD_SPLASHRADIUS       100
#define FORCEFIELD_VALUE              HBVM(0)
#define FORCEFIELD_DCC_REGEN          10
#define FORCEFIELD_SHRINKPROP         0.1f
#define FORCEFIELD_SHRINKTIMEOUT      500

/*
 * HUMAN misc
 */

#define HUMAN_SPRINT_MODIFIER       1.2f
#define HUMAN_JOG_MODIFIER          1.0f
#define HUMAN_BACK_MODIFIER         0.8f
#define HUMAN_SIDE_MODIFIER         0.9f
#define HUMAN_DODGE_SIDE_MODIFIER   2.9f
#define HUMAN_DODGE_UP_MODIFIER     0.5f
#define HUMAN_DODGE_TIMEOUT         500
#define HUMAN_LAND_FRICTION         3.0f

#define STAMINA_STOP_RESTORE        25
#define STAMINA_WALK_RESTORE        15
#define STAMINA_MEDISTAT_RESTORE    30 // stacked on STOP or WALK
#define STAMINA_SPRINT_TAKE         8
#define STAMINA_LARMOUR_SPRINT_TAKE 4
#define STAMINA_JUMP_TAKE           125
#define STAMINA_DODGE_TAKE          250
#define STAMINA_BREATHING_LEVEL     0

#define HUMAN_SPAWN_REPEAT_TIME     10000
#define HUMAN_REGEN_DAMAGE_TIME     2000 //msec since damage before dcc repairs

#define HUMAN_MAX_CREDITS           2000
#define HUMAN_TK_SUICIDE_PENALTY    150

#define HUMAN_BUILDER_SCOREINC      50       // builders receive this many points every 10 seconds
#define ALIEN_BUILDER_SCOREINC      AVM(50)  // builders receive this many points every 10 seconds

/*
 * Misc
 */

#define MIN_FALL_DISTANCE           30.0f //the fall distance at which fall damage kicks in
#define MAX_FALL_DISTANCE           120.0f //the fall distance at which maximum damage is dealt
#define AVG_FALL_DISTANCE           ((MIN_FALL_DISTANCE+MAX_FALL_DISTANCE)/2.0f)

#define FREEKILL_PERIOD             120000 //msec
#define FREEKILL_ALIEN              ALIEN_CREDITS_PER_FRAG
#define FREEKILL_HUMAN              LEVEL0_VALUE

#define DEFAULT_ALIEN_BUILDPOINTS   "100"
#define DEFAULT_ALIEN_QUEUE_TIME    "1250"
#define DEFAULT_ALIEN_STAGE2_THRESH "8000"
#define DEFAULT_ALIEN_STAGE3_THRESH "16000"
#define DEFAULT_ALIEN_MAX_STAGE     "2"
#define DEFAULT_HUMAN_BUILDPOINTS   "100"
#define DEFAULT_HUMAN_QUEUE_TIME    "1250"
#define DEFAULT_HUMAN_STAGE2_THRESH "3500"
#define DEFAULT_HUMAN_STAGE3_THRESH "7000"
#define DEFAULT_HUMAN_MAX_STAGE     "2"
/*
  SKIRMISH_TIME
  defines in milliseconds how far to play into the game before stage counters start
  before this time players can earn kills, get killed, and earn credits/evos but
  they will not count towards a new stage
*/
#define SKIRMISH_TIME               180000	// 3 minutes
/*
  base kills and multipliers
  the calculation to determine how many kills a team needs to get to the next stage
  the method is: basekills + (players * multiplier)
  the amount of players means the maximum amount of players that have been on that team for the running game
  kills do not get adjusted live for disparity between number of players on each team
  NOT IMPLEMENTED
*/
#define STAGE1_BASE_KILLS           "5"
#define STAGE1_PLAYER_MULTIPLIER    "3"
#define STAGE2_BASE_KILLS           "5"
#define STAGE2_PLAYER_MULTIPLIER    "4"


#define DAMAGE_FRACTION_FOR_KILL    0.5f //how much damage players (versus structures) need to
                                         //do to increment the stage kill counters

// g_suddenDeathMode settings
#define SDMODE_BP                   0
#define SDMODE_NO_BUILD             1
#define SDMODE_SELECTIVE            2

#define MAXIMUM_BUILD_TIME          20000 // used for pie timer

