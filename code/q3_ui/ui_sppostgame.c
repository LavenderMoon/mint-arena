/*
===========================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Spearmint Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following
the terms and conditions of the GNU General Public License.  If not, please
request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional
terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
//
/*
=============================================================================

SINGLE PLAYER POSTGAME MENU

=============================================================================
*/

#include "ui_local.h"

#define MAX_UI_AWARDS		6

#define MAX_SCOREBOARD_PLAYERS		8

#define AWARD_PRESENTATION_TIME		2000

#define ART_MENU0		"menu/art/menu_0"
#define ART_MENU1		"menu/art/menu_1"
#define ART_REPLAY0		"menu/art/replay_0"
#define ART_REPLAY1		"menu/art/replay_1"
#define ART_NEXT0		"menu/art/next_0"
#define ART_NEXT1		"menu/art/next_1"

#define ID_AGAIN		10
#define ID_NEXT			11
#define ID_MENU			12

typedef struct {
	menuframework_s	menu;
	menubitmap_s	item_again;
	menubitmap_s	item_next;
	menubitmap_s	item_menu;

	int				phase;
	int				ignoreKeysTime;
	int				starttime;
	int				scoreboardtime;
	int				serverId;

	int				playerNums[MAX_SCOREBOARD_PLAYERS];
	int				ranks[MAX_SCOREBOARD_PLAYERS];
	int				scores[MAX_SCOREBOARD_PLAYERS];

	int				level;
	int				numPlayers;
	int				won;
	int				numAwards;
	int				awardsEarned[MAX_UI_AWARDS];
	int				awardsLevels[MAX_UI_AWARDS];
	qboolean		playedSound[MAX_UI_AWARDS];
	int				lastTier;
	sfxHandle_t		winnerSound;
} postgameMenuInfo_t;

static postgameMenuInfo_t	postgameMenuInfo;
static char					arenainfo[MAX_INFO_VALUE];

char	*ui_medalNames[] = {
	"Accuracy",
	"Impressive",
	"Excellent",
	"Gauntlet",
	"Frags",
	"Perfect"
};
char	*ui_medalPicNames[] = {
	"menu/medals/medal_accuracy",
	"menu/medals/medal_impressive",
	"menu/medals/medal_excellent",
	"menu/medals/medal_gauntlet",
	"menu/medals/medal_frags",
	"menu/medals/medal_victory"
};
char	*ui_medalSounds[] = {
	"sound/feedback/accuracy.wav",
	"sound/feedback/impressive_a.wav",
	"sound/feedback/excellent_a.wav",
	"sound/feedback/gauntlet.wav",
	"sound/feedback/frags.wav",
	"sound/feedback/perfect.wav"
};


/*
=================
UI_SPPostgameMenu_AgainEvent
=================
*/
static void UI_SPPostgameMenu_AgainEvent( void* ptr, int event )
{
	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();
	trap_Cmd_ExecuteText( EXEC_APPEND, "map_restart 0\n" );
}


/*
=================
UI_SPPostgameMenu_NextEvent
=================
*/
static void UI_SPPostgameMenu_NextEvent( void* ptr, int event ) {
	int			currentSet;
	int			levelSet;
	int			level;
	int			currentLevel;
	const char	*arenaInfo;

	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();

	// handle specially if we just won the training map
	if( postgameMenuInfo.won == 0 ) {
		level = 0;
	}
	else {
		level = postgameMenuInfo.level + 1;
	}
	levelSet = level / ARENAS_PER_TIER;

	currentLevel = UI_GetCurrentGame();
	if( currentLevel == -1 ) {
		currentLevel = postgameMenuInfo.level;
	}
	currentSet = currentLevel / ARENAS_PER_TIER;

	if( levelSet > currentSet || levelSet == UI_GetNumSPTiers() ) {
		level = currentLevel;
	}

	arenaInfo = UI_GetArenaInfoByNumber( level );
	if ( !arenaInfo ) {
		return;
	}

	UI_SPArena_Start( arenaInfo );
}


/*
=================
UI_SPPostgameMenu_MenuEvent
=================
*/
static void UI_SPPostgameMenu_MenuEvent( void* ptr, int event )
{
	if (event != QM_ACTIVATED) {
		return;
	}
	UI_PopMenu();
	trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect; levelselect\n" );
}


/*
=================
UI_SPPostgameMenu_MenuKey
=================
*/
static sfxHandle_t UI_SPPostgameMenu_MenuKey( int key ) {
	if ( uis.realtime < postgameMenuInfo.ignoreKeysTime ) {
		return 0;
	}

	if( postgameMenuInfo.phase == 1 ) {
		postgameMenuInfo.phase = 2;
		postgameMenuInfo.starttime = uis.realtime;
		postgameMenuInfo.ignoreKeysTime	= uis.realtime + 250;
		return 0;
	}

	if( postgameMenuInfo.phase == 2 ) {
		postgameMenuInfo.phase = 3;
		postgameMenuInfo.starttime = uis.realtime;
		postgameMenuInfo.ignoreKeysTime	= uis.realtime + 250;
		return 0;
	}

	if( key == K_ESCAPE || key == K_MOUSE2 ) {
		return 0;
	}

	return Menu_DefaultKey( &postgameMenuInfo.menu, key );
}


static int medalLocations[MAX_UI_AWARDS] = {144, 448, 88, 504, 32, 560};

static void UI_SPPostgameMenu_DrawAwardsMedals( int max ) {
	int		n;
	int		medal;
	int		amount;
	int		x, y;
	char	buf[16];

	for( n = 0; n < max; n++ ) {
		x = medalLocations[n];
		y = 8;
		medal = postgameMenuInfo.awardsEarned[n];
		amount = postgameMenuInfo.awardsLevels[n];

		CG_DrawNamedPic( x, y, 48, 48, ui_medalPicNames[medal] );

		if( medal == AWARD_ACCURACY ) {
			Com_sprintf( buf, sizeof(buf), "%i%%", amount );
		}
		else {
			if( amount == 1 ) {
				continue;
			}
			Com_sprintf( buf, sizeof(buf), "%i", amount );
		}

		UI_DrawString( x + 24, y + 52, buf, UI_CENTER, color_yellow );
	}
}


static void UI_SPPostgameMenu_DrawAwardsPresentation( int timer ) {
	int		awardNum;
	int		atimer;
	vec4_t	color;

	awardNum = timer / AWARD_PRESENTATION_TIME;
	atimer = timer % AWARD_PRESENTATION_TIME;

	color[0] = color[1] = color[2] = 1.0f;
	color[3] = (float)( AWARD_PRESENTATION_TIME - atimer ) / (float)AWARD_PRESENTATION_TIME;
	UI_DrawProportionalString( 320, 8, ui_medalNames[postgameMenuInfo.awardsEarned[awardNum]], UI_CENTER, color );

	UI_SPPostgameMenu_DrawAwardsMedals( awardNum + 1 );

	if( !postgameMenuInfo.playedSound[awardNum] ) {
		postgameMenuInfo.playedSound[awardNum] = qtrue;
		trap_S_StartLocalSound( trap_S_RegisterSound( ui_medalSounds[postgameMenuInfo.awardsEarned[awardNum]], qfalse ), CHAN_ANNOUNCER );
	}
}

// !TODO: Make some sort of cg-ui_shared.h file or something for cg_ things shared to ui_
extern qboolean CG_DrawOldScoreboard( void );

/*
=================
UI_SPPostgameMenu_MenuDraw
=================
*/
static void UI_SPPostgameMenu_MenuDraw( void ) {
	int		timer;
	int		serverId;
	char	info[MAX_INFO_STRING];

	trap_GetConfigString( CS_SYSTEMINFO, info, sizeof(info) );
	serverId = atoi( Info_ValueForKey( info, "sv_serverid" ) );
	if( serverId != postgameMenuInfo.serverId ) {
		UI_PopMenu();
		return;
	}

	CG_DrawOldScoreboard();

	// phase 1
	if( postgameMenuInfo.phase == 1 ) {
		timer = uis.realtime - postgameMenuInfo.starttime;

		if( timer >= 1000 && postgameMenuInfo.winnerSound ) {
			trap_S_StartLocalSound( postgameMenuInfo.winnerSound, CHAN_ANNOUNCER );
			postgameMenuInfo.winnerSound = 0;
		}

		if( timer < 5000 ) {
			return;
		}
		postgameMenuInfo.phase = 2;
		postgameMenuInfo.starttime = uis.realtime;
	}

	// phase 2
	if( postgameMenuInfo.phase == 2 ) {
		timer = uis.realtime - postgameMenuInfo.starttime;
		if( timer >= ( postgameMenuInfo.numAwards * AWARD_PRESENTATION_TIME ) ) {

			if( timer < 5000 ) {
				return;
			}

			postgameMenuInfo.phase = 3;
			postgameMenuInfo.starttime = uis.realtime;
		}
		else {
			UI_SPPostgameMenu_DrawAwardsPresentation( timer );
		}
	}

	// phase 3
	if( postgameMenuInfo.phase == 3 ) {
		if( uis.demoversion ) {
			if( postgameMenuInfo.won == 1 && UI_ShowTierVideo( 8 )) {
				trap_Cvar_Set( "nextmap", "" );
				trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect; cinematic demoEnd.RoQ\n" );
				return;
			}
		}
		else if( postgameMenuInfo.won > -1 && UI_ShowTierVideo( postgameMenuInfo.won + 1 )) {
			if( postgameMenuInfo.won == postgameMenuInfo.lastTier ) {
				trap_Cvar_Set( "nextmap", "" );
				trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect; cinematic end.RoQ\n" );
				return;
			}

			trap_Cvar_SetValue( "ui_spSelection", postgameMenuInfo.won * ARENAS_PER_TIER );
			trap_Cvar_Set( "nextmap", "levelselect" );
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "disconnect; cinematic tier%i.RoQ\n", postgameMenuInfo.won + 1 ) );
			return;
		}

		postgameMenuInfo.item_again.generic.flags &= ~QMF_INACTIVE;
		postgameMenuInfo.item_next.generic.flags &= ~QMF_INACTIVE;
		postgameMenuInfo.item_menu.generic.flags &= ~QMF_INACTIVE;

		UI_SPPostgameMenu_DrawAwardsMedals( postgameMenuInfo.numAwards );

		Menu_Draw( &postgameMenuInfo.menu );
	}
}


/*
=================
UI_SPPostgameMenu_Cache
=================
*/
void UI_SPPostgameMenu_Cache( void ) {
	int			n;
	qboolean	buildscript;

	buildscript = trap_Cvar_VariableValue("com_buildscript");

	trap_R_RegisterShaderNoMip( ART_MENU0 );
	trap_R_RegisterShaderNoMip( ART_MENU1 );
	trap_R_RegisterShaderNoMip( ART_REPLAY0 );
	trap_R_RegisterShaderNoMip( ART_REPLAY1 );
	trap_R_RegisterShaderNoMip( ART_NEXT0 );
	trap_R_RegisterShaderNoMip( ART_NEXT1 );
	for( n = 0; n < MAX_UI_AWARDS; n++ ) {
		trap_R_RegisterShaderNoMip( ui_medalPicNames[n] );
		trap_S_RegisterSound( ui_medalSounds[n], qfalse );
	}

	if( buildscript ) {
		trap_S_RegisterSound( "music/loss.wav", qfalse );
		trap_S_RegisterSound( "music/win.wav", qfalse );
		trap_S_RegisterSound( "sound/player/announce/youwin.wav", qfalse );
	}
}


/*
=================
UI_SPPostgameMenu_Init
=================
*/
static void UI_SPPostgameMenu_Init( void ) {
	postgameMenuInfo.menu.wrapAround	= qtrue;
	postgameMenuInfo.menu.key			= UI_SPPostgameMenu_MenuKey;
	postgameMenuInfo.menu.draw			= UI_SPPostgameMenu_MenuDraw;
	postgameMenuInfo.ignoreKeysTime		= uis.realtime + 1500;

	UI_SPPostgameMenu_Cache();

	postgameMenuInfo.item_menu.generic.type			= MTYPE_BITMAP;
	postgameMenuInfo.item_menu.generic.name			= ART_MENU0;
	postgameMenuInfo.item_menu.generic.flags		= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_INACTIVE;
	postgameMenuInfo.item_menu.generic.x			= 0;
	postgameMenuInfo.item_menu.generic.y			= 480-64;
	postgameMenuInfo.item_menu.generic.callback		= UI_SPPostgameMenu_MenuEvent;
	postgameMenuInfo.item_menu.generic.id			= ID_MENU;
	postgameMenuInfo.item_menu.width				= 128;
	postgameMenuInfo.item_menu.height				= 64;
	postgameMenuInfo.item_menu.focuspic				= ART_MENU1;

	postgameMenuInfo.item_again.generic.type		= MTYPE_BITMAP;
	postgameMenuInfo.item_again.generic.name		= ART_REPLAY0;
	postgameMenuInfo.item_again.generic.flags		= QMF_CENTER_JUSTIFY|QMF_PULSEIFFOCUS|QMF_INACTIVE;
	postgameMenuInfo.item_again.generic.x			= 320;
	postgameMenuInfo.item_again.generic.y			= 480-64;
	postgameMenuInfo.item_again.generic.callback	= UI_SPPostgameMenu_AgainEvent;
	postgameMenuInfo.item_again.generic.id			= ID_AGAIN;
	postgameMenuInfo.item_again.width				= 128;
	postgameMenuInfo.item_again.height				= 64;
	postgameMenuInfo.item_again.focuspic			= ART_REPLAY1;

	postgameMenuInfo.item_next.generic.type			= MTYPE_BITMAP;
	postgameMenuInfo.item_next.generic.name			= ART_NEXT0;
	postgameMenuInfo.item_next.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS|QMF_INACTIVE;
	postgameMenuInfo.item_next.generic.x			= 640;
	postgameMenuInfo.item_next.generic.y			= 480-64;
	postgameMenuInfo.item_next.generic.callback		= UI_SPPostgameMenu_NextEvent;
	postgameMenuInfo.item_next.generic.id			= ID_NEXT;
	postgameMenuInfo.item_next.width				= 128;
	postgameMenuInfo.item_next.height				= 64;
	postgameMenuInfo.item_next.focuspic				= ART_NEXT1;

	Menu_AddItem( &postgameMenuInfo.menu, ( void * )&postgameMenuInfo.item_menu );
	Menu_AddItem( &postgameMenuInfo.menu, ( void * )&postgameMenuInfo.item_again );
	Menu_AddItem( &postgameMenuInfo.menu, ( void * )&postgameMenuInfo.item_next );
}

/*
===================
UI_SPPostgameMenu_f
===================
*/
void UI_SPPostgameMenu_f( void ) {
	int			playerGameRank;
	int			playerNum;
	int			n;
	int			oldFrags, newFrags;
	const char	*arena;
	int			awardValues[MAX_UI_AWARDS];
	char		map[MAX_QPATH];
	char		info[MAX_INFO_STRING];
	int			tempRank;

	memset( &postgameMenuInfo, 0, sizeof(postgameMenuInfo) );

	trap_GetConfigString( CS_SYSTEMINFO, info, sizeof(info) );
	postgameMenuInfo.serverId = atoi( Info_ValueForKey( info, "sv_serverid" ) );

	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	Q_strncpyz( map, Info_ValueForKey( info, "mapname" ), sizeof(map) );
	arena = UI_GetArenaInfoByMap( map );
	if ( !arena ) {
		return;
	}
	Q_strncpyz( arenainfo, arena, sizeof(arenainfo) );

	postgameMenuInfo.level = atoi( Info_ValueForKey( arenainfo, "num" ) );

	postgameMenuInfo.numPlayers = atoi( CG_Argv( 1 ) );
	playerNum = atoi( CG_Argv( 2 ) );
	playerGameRank = 8;		// in case they ended game as a spectator

	if( postgameMenuInfo.numPlayers > MAX_SCOREBOARD_PLAYERS ) {
		postgameMenuInfo.numPlayers = MAX_SCOREBOARD_PLAYERS;
	}

	for( n = 0; n < postgameMenuInfo.numPlayers; n++ ) {
		tempRank = ( atoi( CG_Argv( 8 + n * 3 + 2 ) ) & ~RANK_TIED_FLAG ) + 1;
		
		postgameMenuInfo.playerNums[n] = atoi( CG_Argv( 8 + n * 3 + 1 ) );
		postgameMenuInfo.ranks[n] = tempRank;
		postgameMenuInfo.scores[n] = atoi( CG_Argv( 8 + n * 3 + 3 ) );
		
		CG_Printf( "\n" );
		if( postgameMenuInfo.playerNums[n] == playerNum )
		{
			CG_Printf( "[PLAYER]\n" );
		}
		CG_Printf( "playerNums[%i]: %i\n", n, postgameMenuInfo.playerNums[n] );
		CG_Printf( "ranks[%i]: %i\n", n, tempRank );
		CG_Printf( "scores[%i]: %i\n", n, postgameMenuInfo.scores[n] );
		
		if( postgameMenuInfo.playerNums[n] == playerNum ) {
			playerGameRank = tempRank;
		}
	}

	UI_SetBestScore( postgameMenuInfo.level, playerGameRank );

	// process award stats and prepare presentation data
	awardValues[AWARD_ACCURACY] = atoi( CG_Argv( 3 ) );
	awardValues[AWARD_IMPRESSIVE] = atoi( CG_Argv( 4 ) );
	awardValues[AWARD_EXCELLENT] = atoi( CG_Argv( 5 ) );
	awardValues[AWARD_GAUNTLET] = atoi( CG_Argv( 6 ) );
	awardValues[AWARD_FRAGS] = atoi( CG_Argv( 7 ) );
	awardValues[AWARD_PERFECT] = atoi( CG_Argv( 8 ) );

	postgameMenuInfo.numAwards = 0;

	if( awardValues[AWARD_ACCURACY] >= 50 ) {
		UI_LogAwardData( AWARD_ACCURACY, 1 );
		postgameMenuInfo.awardsEarned[postgameMenuInfo.numAwards] = AWARD_ACCURACY;
		postgameMenuInfo.awardsLevels[postgameMenuInfo.numAwards] = awardValues[AWARD_ACCURACY];
		postgameMenuInfo.numAwards++;
	}

	if( awardValues[AWARD_IMPRESSIVE] ) {
		UI_LogAwardData( AWARD_IMPRESSIVE, awardValues[AWARD_IMPRESSIVE] );
		postgameMenuInfo.awardsEarned[postgameMenuInfo.numAwards] = AWARD_IMPRESSIVE;
		postgameMenuInfo.awardsLevels[postgameMenuInfo.numAwards] = awardValues[AWARD_IMPRESSIVE];
		postgameMenuInfo.numAwards++;
	}

	if( awardValues[AWARD_EXCELLENT] ) {
		UI_LogAwardData( AWARD_EXCELLENT, awardValues[AWARD_EXCELLENT] );
		postgameMenuInfo.awardsEarned[postgameMenuInfo.numAwards] = AWARD_EXCELLENT;
		postgameMenuInfo.awardsLevels[postgameMenuInfo.numAwards] = awardValues[AWARD_EXCELLENT];
		postgameMenuInfo.numAwards++;
	}

	if( awardValues[AWARD_GAUNTLET] ) {
		UI_LogAwardData( AWARD_GAUNTLET, awardValues[AWARD_GAUNTLET] );
		postgameMenuInfo.awardsEarned[postgameMenuInfo.numAwards] = AWARD_GAUNTLET;
		postgameMenuInfo.awardsLevels[postgameMenuInfo.numAwards] = awardValues[AWARD_GAUNTLET];
		postgameMenuInfo.numAwards++;
	}

	oldFrags = UI_GetAwardLevel( AWARD_FRAGS ) / 100;
	UI_LogAwardData( AWARD_FRAGS, awardValues[AWARD_FRAGS] );
	newFrags = UI_GetAwardLevel( AWARD_FRAGS ) / 100;
	if( newFrags > oldFrags ) {
		postgameMenuInfo.awardsEarned[postgameMenuInfo.numAwards] = AWARD_FRAGS;
		postgameMenuInfo.awardsLevels[postgameMenuInfo.numAwards] = newFrags * 100;
		postgameMenuInfo.numAwards++;
	}

	if( awardValues[AWARD_PERFECT] ) {
		UI_LogAwardData( AWARD_PERFECT, 1 );
		postgameMenuInfo.awardsEarned[postgameMenuInfo.numAwards] = AWARD_PERFECT;
		postgameMenuInfo.awardsLevels[postgameMenuInfo.numAwards] = 1;
		postgameMenuInfo.numAwards++;
	}

	if ( playerGameRank == 1 ) {
		postgameMenuInfo.won = UI_TierCompleted( postgameMenuInfo.level );
	}
	else {
		postgameMenuInfo.won = -1;
	}

	postgameMenuInfo.starttime = uis.realtime;
	postgameMenuInfo.scoreboardtime = uis.realtime;

	uis.menusp = 0;

	UI_SPPostgameMenu_Init();
	UI_PushMenu( &postgameMenuInfo.menu );

	if ( playerGameRank == 1 ) {
		Menu_SetCursorToItem( &postgameMenuInfo.menu, &postgameMenuInfo.item_next );
	}
	else {
		Menu_SetCursorToItem( &postgameMenuInfo.menu, &postgameMenuInfo.item_again );
	}

	if ( playerGameRank != 1 ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "music music/loss\n" );
	}
	else {
		trap_Cmd_ExecuteText( EXEC_APPEND, "music music/win\n" );
	}

	postgameMenuInfo.phase = 1;

	postgameMenuInfo.lastTier = UI_GetNumSPTiers();
	if ( UI_GetSpecialArenaInfo( "final" ) ) {
		postgameMenuInfo.lastTier++;
	}
}
