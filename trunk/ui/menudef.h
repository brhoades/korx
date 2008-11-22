#ifndef MENUDEF_H
#define MENUDEF_H

#define COMMON_HUD_R 1.0
#define COMMON_HUD_G 0.0
#define COMMON_HUD_B 0.0

#define ITEM_TYPE_TEXT                    0     // simple text
#define ITEM_TYPE_BUTTON                  1     // button, basically text with a border
#define ITEM_TYPE_RADIOBUTTON             2     // toggle button, may be grouped
#define ITEM_TYPE_CHECKBOX                3     // check box
#define ITEM_TYPE_EDITFIELD               4     // editable text, associated with a cvar
#define ITEM_TYPE_COMBO                   5     // drop down list
#define ITEM_TYPE_LISTBOX                 6     // scrollable list
#define ITEM_TYPE_MODEL                   7     // model
#define ITEM_TYPE_OWNERDRAW               8     // owner draw, name specs what it is
#define ITEM_TYPE_NUMERICFIELD            9     // editable text, associated with a cvar
#define ITEM_TYPE_SLIDER                  10    // mouse speed, volume, etc.
#define ITEM_TYPE_YESNO                   11    // yes no cvar setting
#define ITEM_TYPE_MULTI                   12    // multiple list setting, enumerated
#define ITEM_TYPE_BIND                    13    // multiple list setting, enumerated

#define ALIGN_CENTER                      1     // center alignment
#define ALIGN_RIGHT                       2     // right alignment
#define ASPECT_NONE                       3     // no aspect compensation

#define VALIGN_BOTTOM                     0     // bottom alignment
#define VALIGN_CENTER                     1     // center alignment
#define VALIGN_TOP                        2     // top alignment

#define ITEM_TEXTSTYLE_NORMAL             0     // normal text
#define ITEM_TEXTSTYLE_BLINK              1     // fast blinking
#define ITEM_TEXTSTYLE_PULSE              2     // slow pulsing
#define ITEM_TEXTSTYLE_SHADOWED           3     // drop shadow ( need a color for this )
#define ITEM_TEXTSTYLE_OUTLINED           4     // drop shadow ( need a color for this )
#define ITEM_TEXTSTYLE_OUTLINESHADOWED    5     // drop shadow ( need a color for this )
#define ITEM_TEXTSTYLE_SHADOWEDMORE       6     // drop shadow ( need a color for this )
#define ITEM_TEXTSTYLE_NEON               7     // drop shadow ( need a color for this )

#define ITEM_ALIGN_LEFT 0                       // left alignment
#define ITEM_ALIGN_CENTER 1                     // center alignment
#define ITEM_ALIGN_RIGHT                  2     // right alignment

#define WINDOW_BORDER_NONE                0     // no border
#define WINDOW_BORDER_FULL                1     // full border based on border color ( single pixel )
#define WINDOW_BORDER_HORZ                2     // horizontal borders only
#define WINDOW_BORDER_VERT                3     // vertical borders only
#define WINDOW_BORDER_KCGRADIENT          4     // horizontal border using the gradient bars

#define WINDOW_STYLE_EMPTY                0     // no background
#define WINDOW_STYLE_FILLED               1     // filled with background color
#define WINDOW_STYLE_GRADIENT             2     // gradient bar based on background color
#define WINDOW_STYLE_SHADER               3     // gradient bar based on background color
#define WINDOW_STYLE_TEAMCOLOR            4     // team color
#define WINDOW_STYLE_CINEMATIC            5     // cinematic

#define MENU_TRUE                         1     // uh.. true
#define MENU_FALSE                        0     // and false

#define HUD_VERTICAL                      0  
#define HUD_HORIZONTAL                    1

// list box element types
#define LISTBOX_TEXT                      0
#define LISTBOX_IMAGE                     1

// list feeders
#define FEEDER_HEADS                    0x00      // model heads
#define FEEDER_MAPS                     0x01      // text maps based on game type
#define FEEDER_SERVERS                  0x02      // servers
#define FEEDER_CLANS                    0x03      // clan names
#define FEEDER_ALLMAPS                  0x04      // all maps available, in graphic format
#define FEEDER_ALIENTEAM_LIST           0x05      // red team members
#define FEEDER_HUMANTEAM_LIST           0x06      // blue team members
#define FEEDER_PLAYER_LIST              0x07      // players
#define FEEDER_TEAM_LIST                0x08      // team members for team voting
#define FEEDER_MODS                     0x09      // team members for team voting
#define FEEDER_DEMOS                    0x0a      // team members for team voting
#define FEEDER_SCOREBOARD               0x0b      // team members for team voting
#define FEEDER_Q3HEADS                  0x0c      // model heads
#define FEEDER_SERVERSTATUS             0x0d      // server status
#define FEEDER_FINDPLAYER               0x0e      // find player
#define FEEDER_CINEMATICS               0x0f      // cinematics

//TA: tremulous menus
#define FEEDER_TREMTEAMS                0x10      //teams
#define FEEDER_TREMALIENCLASSES         0x11      //alien classes
#define FEEDER_TREMHUMANITEMS           0x12      //human items
#define FEEDER_TREMHUMANARMOURYBUY      0x13      //human buy
#define FEEDER_TREMHUMANARMOURYSELL     0x14      //human sell
#define FEEDER_TREMALIENUPGRADE         0x15      //alien upgrade
#define FEEDER_TREMALIENBUILD           0x16      //alien buildables
#define FEEDER_TREMHUMANBUILD           0x17      //human buildables
//TA: tremulous menus
#define FEEDER_IGNORE_LIST              0x18      //ignored players

// display flags
#define UI_SHOW_FAVORITESERVERS           0x00000001
#define UI_SHOW_NOTFAVORITESERVERS        0x00000002

#define UI_SHOW_VOTEACTIVE                0x00000004
#define UI_SHOW_CANVOTE                   0x00000008
#define UI_SHOW_TEAMVOTEACTIVE            0x00000010
#define UI_SHOW_CANTEAMVOTE               0x00000020

#define UI_SHOW_NOTSPECTATING             0x00000040

// owner draw types
#define CG_PLAYER_HEALTH                  0
#define CG_PLAYER_HEALTH_CROSS            1
#define CG_PLAYER_AMMO_VALUE              2
#define CG_PLAYER_CLIPS_VALUE             3
#define CG_PLAYER_BUILD_TIMER             4
#define CG_PLAYER_CREDITS_VALUE           7
#define CG_PLAYER_BANK_VALUE              8
#define CG_PLAYER_CREDITS_VALUE_NOPAD     9
#define CG_PLAYER_BANK_VALUE_NOPAD        10
#define CG_PLAYER_STAMINA                 11
#define CG_PLAYER_STAMINA_1               12
#define CG_PLAYER_STAMINA_2               13
#define CG_PLAYER_STAMINA_3               14
#define CG_PLAYER_STAMINA_4               15
#define CG_PLAYER_STAMINA_BOLT            16
#define CG_PLAYER_BOOST_BOLT              17
#define CG_PLAYER_CLIPS_RING              18
#define CG_PLAYER_BUILD_TIMER_RING        19
#define CG_PLAYER_SELECT                  20
#define CG_PLAYER_SELECTTEXT              21
#define CG_PLAYER_WEAPONICON              22
#define CG_PLAYER_WALLCLIMBING            23
#define CG_PLAYER_BOOSTED                 24
#define CG_PLAYER_POISON_BARBS            25
#define CG_PLAYER_ALIEN_SENSE             26
#define CG_PLAYER_HUMAN_SCANNER           27
#define CG_PLAYER_USABLE_BUILDABLE        28
#define CG_KILLER                         29
#define CG_SPECTATORS                     30

// loading screen
#define CG_LOAD_LEVELSHOT                 31
#define CG_LOAD_MEDIA                     32
#define CG_LOAD_MEDIA_LABEL               33
#define CG_LOAD_BUILDABLES                34
#define CG_LOAD_BUILDABLES_LABEL          35
#define CG_LOAD_CHARMODEL                 36
#define CG_LOAD_CHARMODEL_LABEL           37
#define CG_LOAD_OVERALL                   38
#define CG_LOAD_LEVELNAME                 39
#define CG_LOAD_MOTD                      40
#define CG_LOAD_HOSTNAME                  41

#define CG_FPS                            42
#define CG_FPS_FIXED                      43
#define CG_TIMER                          44
#define CG_TIMER_MINS                     45
#define CG_TIMER_SECS                     46
#define CG_SNAPSHOT                       47
#define CG_LAGOMETER                      48
#define CG_PLAYER_CROSSHAIRNAMES          49
#define CG_STAGE_REPORT_TEXT              50
#define CG_ALIENS_SCORE_LABEL             51
#define CG_HUMANS_SCORE_LABEL             52
#define CG_DEMO_PLAYBACK                  53
#define CG_DEMO_RECORDING                 54

#define CG_CONSOLE                        55
#define CG_TUTORIAL                       56
#define CG_CLOCK                          57

#define UI_NETSOURCE                      58
#define UI_NETMAPPREVIEW                  59
#define UI_NETMAPCINEMATIC                60
#define UI_SERVERREFRESHDATE              61
#define UI_SERVERMOTD                     62
#define UI_GLINFO                         63
#define UI_KEYBINDSTATUS                  64
#define UI_SELECTEDMAPPREVIEW             65
#define UI_SELECTEDMAPNAME                66

#define UI_TEAMINFOPANE                   67
#define UI_ACLASSINFOPANE                 68
#define UI_AUPGRADEINFOPANE               69
#define UI_HITEMINFOPANE                  70
#define UI_HBUYINFOPANE                   71
#define UI_HSELLINFOPANE                  72
#define UI_ABUILDINFOPANE                 73
#define UI_HBUILDINFOPANE                 74
#define UI_HELPINFOPANE                   75

#define CG_PLAYER_CHARGE_BAR_BG           76
#define CG_PLAYER_CHARGE_BAR              77
#define CG_PLAYER_HEALTH_BAR              78

#define UI_ALLMAPS_SELECTION              79

#define UI_PLAYERLIST_SELECTION           80
#define UI_TEAMLIST_SELECTION             81

#endif
