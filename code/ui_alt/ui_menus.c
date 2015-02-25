/*
===========================================================================
Copyright (C) 2014 Zack Middleton

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
// defines the game menus

#include "ui_local.h"

/*

	Menu action functions

*/

#ifdef MISSIONPACK
void launchQ3( int item ) {
	trap_Cvar_Set( "fs_game", BASEQ3 );
	trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
}
#else
void launchTeamArena( int item ) {
	trap_Cvar_Set( "fs_game", BASETA );
	trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
}
#endif

void demoHandler( int item ) {
	CG_Printf("DEBUG: TODO: play demo... got %d\n", item);
}

void cinematicHandler( int item ) {
	CG_Printf("DEBUG: TODO: play cinematic... got %d\n", item);
}

void modHandler( int item ) {
	CG_Printf("DEBUG: TODO: load mod... got %d\n", item);
}

// 0 = yes, 1 = no
void exitHandler( int item ) {
	if ( item == 0 ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
	} else {
		CG_Printf("DEBUG: canceled exiting...\n");
	}
}

// 0 = yes, 1 = no
void defaultsHandler( int item ) {
	if ( item == 0 ) {
		// should put cvar_restart first? sometimes people put cvars in it -- this would apply to q3_ui too
		trap_Cmd_ExecuteText( EXEC_APPEND, "exec default.cfg\n");
		trap_Cmd_ExecuteText( EXEC_APPEND, "cvar_restart\n");
		trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
	} else {
		CG_Printf("DEBUG: canceled reseting to defaults...\n");
	}
}

void newGame( int item ) {
	trap_Cvar_SetValue( "ui_singlePlayerActive", 1 );
	trap_Cvar_SetValue( "g_doWarmup", 0 );
	trap_Cvar_SetValue( "g_gametype", GT_SINGLE_PLAYER );
	trap_Cmd_ExecuteText( EXEC_APPEND, "map q3dm0\n" );
}

void endGame( int item ) {
	trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
}

void restartMap( int item ) {
	trap_Cmd_ExecuteText( EXEC_APPEND, "map_restart 0\n" );
}


/*

	Common cvar range definitions

*/

#define VINT( x, str ) { CVT_INT, #x, str }
#define VFLOAT( x, str ) { CVT_FLOAT, #x, str }
#define VSTRING( x, str ) { CVT_STRING, x, str }
#define VCMD( x, str ) { CVT_CMD, x, str }
#define VEND { CVT_NONE, NULL, NULL }

static cvarValuePair_t cp_bool[] = { VINT( 0, "off" ), VINT( 1, "on" ), VEND };
static cvarValuePair_t cp_boolInvert[] = { VINT( 0, "on" ), VINT( 1, "off" ), VEND };

// 0.0 to 1.0 slider
static cvarRange_t cr_zeroToOne = { 0, 1, 0.05f };

/*

	Menu item definitions

*/

#ifdef MISSIONPACK
menuitem_t mainmenu_items[] =
{
	{ MIF_BIGTEXT|MIF_HEADER, "TEAM ARENA", NULL, M_NONE, 20 },
	{ MIF_BIGTEXT|MIF_CALL, "Quake3", launchQ3, M_NONE, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Rules", NULL, M_RULES, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Credits", NULL, M_CREDITS, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Mods", NULL, M_MODS, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Cinematics", NULL, M_CINEMATICS, 0 },

	{ MIF_BIGTEXT|MIF_HEADER, "PLAY", NULL, M_NONE, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "SinglePlayer", NULL, M_SINGLEPLAYER, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "MultiPlayer", NULL, M_MULTIPLAYER, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "StartServer", NULL, M_START_SERVER, 0 },

	{ MIF_BIGTEXT|MIF_HEADER, "OPTIONS", NULL, M_NONE, 340 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Player", NULL, M_PLAYER, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Setup", NULL, M_SETUP, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Demos", NULL, M_DEMOS, 0 },

	{ MIF_BIGTEXT|MIF_HEADER|MIF_SUBMENU, "QUIT", NULL, M_EXIT, 0 }
};
#else
menuitem_t mainmenu_items[] =
{
	{ MIF_BIGTEXT|MIF_SUBMENU, "Single Player", NULL, M_SINGLEPLAYER, 134 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Multiplayer", NULL, M_MULTIPLAYER, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Setup", NULL, M_SETUP, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Demos", NULL, M_DEMOS, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Cinematics", NULL, M_CINEMATICS, 0 },
	{ MIF_BIGTEXT|MIF_CALL, "Team Arena", launchTeamArena, M_NONE, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Mods", NULL, M_MODS, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Exit", NULL, M_EXIT, 0 }
};
#endif

cvarValuePair_t cp_skill[] = {
	VINT( 1, "I Can Win" ),
	VINT( 2, "Bring It On" ),
	VINT( 3, "Hurt Me Plenty" ),
	VINT( 4, "Hardcore" ),
	VINT( 5, "Nightmare!" ),
	VEND
};

menuitem_t singleplayermenu_items[] =
{
	{ MIF_BIGTEXT|MIF_CALL, "Bot skill:", NULL, M_NONE, 0, "g_spSkill", cp_skill },
	{ MIF_BIGTEXT|MIF_CALL, "New Game", newGame, M_NONE, 0 },
	{ MIF_BIGTEXT|MIF_CALL, "End Game", endGame, M_NONE, 0 },
	//{ MIF_BIGTEXT, "Level Select...", NULL, M_NONE, 0 },
};

menuitem_t setupmenu_items[] =
{
#ifndef MISSIONPACK
	{ MIF_BIGTEXT|MIF_SUBMENU, "Players", NULL, M_PLAYER, 0 }, // Player or Players in q3_ui depending on number of max players
#endif
	{ MIF_BIGTEXT|MIF_SUBMENU, "Controls", NULL, M_CONTROLS, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "System", NULL, M_SYSTEM, 0 },
	{ MIF_BIGTEXT|MIF_SUBMENU, "Game Options", NULL, M_GAME_OPTIONS, 0 },
#ifndef MISSIONPACK
	{ MIF_BIGTEXT|MIF_SUBMENU, "Credits", NULL, M_CREDITS, 0 },
#endif
	{ MIF_BIGTEXT|MIF_SUBMENU, "Defaults", NULL, M_DEFAULTS, 0 }, // TODO: only show if not in-game?
};

menuitem_t demosmenu_items[] =
{
	{ MIF_BIGTEXT|MIF_CALL|MIF_NEXTBUTTON, "Play Demo", demoHandler, M_NONE, 0 },
};

menuitem_t cinematicsmenu_items[] =
{
	{ MIF_BIGTEXT|MIF_CALL|MIF_NEXTBUTTON, "Play Cinematic", cinematicHandler, M_NONE, 0 },
};

menuitem_t modsmenu_items[] =
{
	{ MIF_BIGTEXT|MIF_CALL|MIF_NEXTBUTTON, "Load Mod", modHandler, M_NONE, 0 },
};

menuitem_t exitmenu_items[] =
{
	{ MIF_BIGTEXT|MIF_CALL, "Yes", exitHandler, M_NONE, 0 },
	{ MIF_BIGTEXT, "/", NULL, M_NONE, 0 },
	{ MIF_BIGTEXT|MIF_CALL|MIF_POPMENU, "No", exitHandler, M_NONE, 0 },
};

menuitem_t defaultsmenu_items[] =
{
	{ MIF_BIGTEXT|MIF_CALL, "Yes", defaultsHandler, M_NONE, 0 },
	{ MIF_BIGTEXT, "/", NULL, M_NONE, 0 },
	{ MIF_BIGTEXT|MIF_CALL|MIF_POPMENU, "No", defaultsHandler, M_NONE, 0 },

	{ MIF_BIGTEXT|MIF_HEADER, "WARNING: This will reset *ALL*", NULL, M_NONE, 400 },
	{ MIF_BIGTEXT, "options to their default values.", NULL, M_NONE, 0 },
};


// From Team Arena's ui_main.c
// ZTM: TODO: add anisotropy, multisample, and compressed textures
void graphicsPresetUpdate( int item ) {
	int val;

	(void)item;

	val = trap_Cvar_VariableIntegerValue( "ui_glCustom" );

	//Com_Printf("graphicsPresetUpdate: %d\n", val);

	// ZTM: I'm not really a fan of changing settings that are not displayed in system -> video menu...
	// ZTM: Disabled fullscreen because it takes effect immediately...
	switch (val) {
		case 0:	// very high quality
			//trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_Reset( "r_subdivisions" );
			trap_Cvar_Reset( "r_vertexlight" );
			trap_Cvar_Reset( "r_lodbias" );
			trap_Cvar_Reset( "r_colorbits" );
			trap_Cvar_Reset( "r_depthbits" );
			trap_Cvar_Reset( "r_picmip" );
			trap_Cvar_SetValue( "r_mode", -2 );
			trap_Cvar_Reset( "r_texturebits" );
			trap_Cvar_Reset( "r_fastSky" );
			trap_Cvar_Reset( "r_inGameVideo" );
			trap_Cvar_Reset( "cg_shadows" );
			trap_Cvar_Reset( "cg_brassTime" );
			trap_Cvar_Reset( "r_texturemode" );
		break;
		case 1:	// high quality
			//trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_Reset( "r_subdivisions" );
			trap_Cvar_Reset( "r_vertexlight" );
			trap_Cvar_Reset( "r_lodbias" );
			trap_Cvar_Reset( "r_colorbits" );
			trap_Cvar_Reset( "r_depthbits" );
			trap_Cvar_Reset( "r_picmip" );
			trap_Cvar_SetValue( "r_mode", 6 ); // 4 in TA
			trap_Cvar_Reset( "r_texturebits" );
			trap_Cvar_Reset( "r_fastSky" );
			trap_Cvar_Reset( "r_inGameVideo" );
			trap_Cvar_Reset( "cg_shadows" );
			trap_Cvar_Reset( "cg_brassTime" );
			trap_Cvar_Reset( "r_texturemode" );
		break;
		case 2: // normal 
			//trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_SetValue( "r_subdivisions", 12 );
			trap_Cvar_Reset( "r_vertexlight" );
			trap_Cvar_Reset( "r_lodbias" );
			trap_Cvar_Reset( "r_colorbits" );
			trap_Cvar_Reset( "r_depthbits" );
			trap_Cvar_SetValue( "r_picmip", 1 );
			trap_Cvar_SetValue( "r_mode", 3 );
			trap_Cvar_Reset( "r_texturebits" );
			trap_Cvar_Reset( "r_fastSky" );
			trap_Cvar_Reset( "r_inGameVideo" );
			trap_Cvar_SetValue( "cg_shadows", 0 );
			trap_Cvar_Reset( "cg_brassTime" );
			trap_Cvar_Reset( "r_texturemode" );
		break;
		case 3: // fast
			//trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_SetValue( "r_subdivisions", 8 );
			trap_Cvar_Reset( "r_vertexlight" );
			trap_Cvar_SetValue( "r_lodbias", 1 );
			trap_Cvar_Reset( "r_colorbits" );
			trap_Cvar_Reset( "r_depthbits" );
			trap_Cvar_SetValue( "r_picmip", 1 );
			trap_Cvar_SetValue( "r_mode", 3 );
			trap_Cvar_SetValue( "r_texturebits", 0 );
			trap_Cvar_SetValue( "r_fastSky", 1 );
			trap_Cvar_SetValue( "r_inGameVideo", 0 );
			trap_Cvar_SetValue( "cg_shadows", 0 );
			trap_Cvar_SetValue( "cg_brassTime", 0 );
			trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_NEAREST" );
		break;
		case 4: // fastest
			//trap_Cvar_SetValue( "r_fullScreen", 1 );
			trap_Cvar_SetValue( "r_subdivisions", 20 );
			trap_Cvar_SetValue( "r_vertexlight", 1 );
			trap_Cvar_SetValue( "r_lodbias", 2 );
			trap_Cvar_SetValue( "r_colorbits", 16 );
			trap_Cvar_SetValue( "r_depthbits", 16 );
			trap_Cvar_SetValue( "r_mode", 3 );
			trap_Cvar_SetValue( "r_picmip", 2 );
			trap_Cvar_SetValue( "r_texturebits", 16 );
			trap_Cvar_SetValue( "r_fastSky", 1 );
			trap_Cvar_SetValue( "r_inGameVideo", 0 );
			trap_Cvar_SetValue( "cg_shadows", 0 );
			trap_Cvar_SetValue( "cg_brassTime", 0 );
			trap_Cvar_Set( "r_texturemode", "GL_LINEAR_MIPMAP_NEAREST" );
		break;
		default: // custom
		break;
	}
}

static cvarValuePair_t cp_glCustom[] = { VINT( 0, "Very High" ), VINT( 1, "High" ), VINT( 2, "Medium" ), VINT( 3, "Fast" ), VINT( 4, "Fastest" ), VINT( 5, "Custom" ), VEND };
static cvarValuePair_t cp_multisample[] = { VINT( 0, "off" ), VINT( 2, "2x MSAA" ), VINT( 4, "4x MSAA" ), VEND };
static cvarValuePair_t cp_anisotropic[] = {
	VCMD( "r_ext_texture_filter_anisotropic 0; reset r_ext_max_anisotropy;", "off" ),
	VCMD( "r_ext_texture_filter_anisotropic 1; r_ext_max_anisotropy 2;", "2x" ),
	VCMD( "r_ext_texture_filter_anisotropic 1; r_ext_max_anisotropy 4;", "4x" ),
	VCMD( "r_ext_texture_filter_anisotropic 1; r_ext_max_anisotropy 8;", "8x" ),
	VCMD( "r_ext_texture_filter_anisotropic 1; r_ext_max_anisotropy 16;", "16x" ),
	VEND
};
static cvarValuePair_t cp_geometricDetail[] = {
	VCMD( "r_lodBias 1; r_subdivisions 20;", "Low" ),
	VCMD( "r_lodBias 1; r_subdivisions 12;", "Medium" ),
	VCMD( "r_lodBias 0; r_subdivisions 4;", "High" ),
	VCMD( "r_lodBias -2; r_subdivisions 4;", "Very High" ), // ZTM: New option to never use low LOD models
	VEND
};
static cvarValuePair_t cp_textureQuality[] = { VINT( 0, "Default" ), VINT( 16, "16 bit" ), VINT( 32, "32 bit" ), VEND };
static cvarValuePair_t cp_textureFilter[] = { VSTRING( "GL_LINEAR_MIPMAP_NEAREST", "Bilinear" ), VSTRING( "GL_LINEAR_MIPMAP_LINEAR", "Trilinear" ), VEND };
static cvarValuePair_t cp_lighting[] = { VINT( 0, "Lightmap (High)" ), VINT( 1, "Vertex (Low)" ), VEND };

static cvarRange_t cr_pimip = { 3, 0, 1 };
static cvarRange_t cr_gamma = { 0.5, 2.0, 0.1 };
static cvarRange_t cr_viewsize = { 30, 100, 10 };

static cvarValuePair_t cp_anaglyphMode[] =
{
	VINT( 0, "off" ),
	VINT( 1, "red-cyan" ),
	VINT( 2, "red-blue" ),
	VINT( 3, "red-green" ),
	VINT( 4, "green-magenta" ),
	VINT( 5, "cyan-red" ),
	VINT( 6, "blue-red" ),
	VINT( 7, "green-red" ),
	VINT( 8, "magenta-green" ),
	VEND
};

static cvarValuePair_t cp_soundSystem[] = { VINT( 0, "SDL" ), VINT( 1, "OpenAL" ), VEND };
static cvarValuePair_t cp_sdlSpeed[] = { VINT( 11025, "Low (11k)" ), VINT( 22050, "Medium (22k)" ), VINT( 44100, "High (44.1k)" ), VINT( 0, "Very High (48k)" ), VEND }; // spearmint default 0 is 48k, ioq3 0 is 44.1k
static cvarValuePair_t cp_networkRate[] = { VINT( 2500, "<= 28.8K" ), VINT( 3000, "33.6K" ), VINT( 4000, "56K" ), VINT( 5000, "ISDN" ), VINT( 25000, "LAN/Cable/xDSL" ), VEND };
static cvarValuePair_t cp_lagComp[] = { VINT( 0, "None" ), VINT( 1, "One Server Frame" ), VINT( 2, "Full" ), VEND };

menuitem_t systemmenu_items[] = {
	{ MIF_BIGTEXT|MIF_PANEL, "Graphics", NULL, M_NONE, 0 },
	{ MIF_CALL, "Graphics Settings:", graphicsPresetUpdate, M_NONE, 0, "ui_glCustom", cp_glCustom }, // Custom, Very High, High, etc
	{ 0, "",							NULL, M_NONE, 0, NULL, NULL },
	// ZTM: let's remove this, because generally no one should turn it off
	//{ MIF_CALL, "GL Extensions:",		NULL, M_NONE, 0, "r_allowExtensions", cp_bool },
	// ZTM: NOTE: I don't really like how Aspect Ratio works in q3_ui and don't feel like implementing it right now. Should aspect just be displayed after resolution size?
	//{ MIF_CALL, "Aspect Ratio:",		NULL, M_NONE, 0 }, // 4:3, ...
	{ MIF_CALL, "Resolution:",			NULL, M_NONE, 0, "r_mode", cp_resolution }, // 1024x768 ...
	{ MIF_CALL, "Fullscreen:",			NULL, M_NONE, 0, "r_fullscreen", cp_bool },
	{ MIF_CALL, "Anti-aliasing:",		NULL, M_NONE, 0, "r_ext_multisample", cp_multisample }, // ZTM: new
	{ MIF_CALL, "Lighting:",			NULL, M_NONE, 0, "r_vertexLight", cp_lighting },
	{ MIF_CALL, "Flares:",				NULL, M_NONE, 0, "r_flares", cp_bool },
	// ZTM: FIXME: Geometric Detail always defaults to 'low' when opening menu
	{ MIF_CALL, "Geometric Detail:",	NULL, M_NONE, 0, "r_lodBias", cp_geometricDetail }, // modifies both "r_lodBias" and "r_subdivisions" cvars
	{ MIF_CALL, "Texture Detail:",		NULL, M_NONE, 0, "r_picmip", NULL, &cr_pimip },
	{ MIF_CALL, "Texture Quality:",		NULL, M_NONE, 0, "r_texturebits", cp_textureQuality },
	{ MIF_CALL, "Compress Textures:",	NULL, M_NONE, 0, "r_ext_compressed_textures", cp_bool }, // ZTM: new, from team arena
	{ MIF_CALL, "Texture Filter:",		NULL, M_NONE, 0, "r_textureMode", cp_textureFilter },
	// ZTM: FIXME: Anisotropic Filtering always defaults to 'off' when opening menu
	{ MIF_CALL, "Anisotropic Filtering:", NULL, M_NONE, 0, "r_ext_max_anisotropy", cp_anisotropic }, // ZTM: new. should it be merged with texture filter?

	// missing driver info button

	{ MIF_BIGTEXT|MIF_PANEL, "Display", NULL, M_NONE, 0 },
	{ MIF_CALL, "Brightness:",			NULL, M_NONE, 0, "r_gamma", NULL, &cr_gamma },
	{ MIF_CALL, "Screen Size:",			NULL, M_NONE, 0, "cg_viewsize", NULL, &cr_viewsize },
	{ MIF_CALL, "Anaglyph Mode:",		NULL, M_NONE, 0, "r_anaglyphMode", cp_anaglyphMode },
	{ MIF_CALL, "Grey Scale:",			NULL, M_NONE, 0, "r_greyscale", NULL, &cr_zeroToOne },

	{ MIF_BIGTEXT|MIF_PANEL, "Sound", NULL, M_NONE, 0 },
	{ MIF_CALL, "Effects Volume:",		NULL, M_NONE, 0, "s_volume", NULL, &cr_zeroToOne },
	{ MIF_CALL, "Music Volume:",		NULL, M_NONE, 0, "s_musicVolume", NULL, &cr_zeroToOne },
	{ MIF_CALL, "Sound System:",		NULL, M_NONE, 0, "s_useOpenAL", cp_soundSystem },
	{ MIF_CALL, "SDL Sound Quality:",	NULL, M_NONE, 0, "s_sdlSpeed", cp_sdlSpeed }, // TODO: disable when sound system is not SDL

	{ MIF_BIGTEXT|MIF_PANEL, "Network", NULL, M_NONE, 0 },
	{ MIF_CALL, "Data Rate:",			NULL, M_NONE, 0, "rate", cp_networkRate },
	{ MIF_CALL, "Voice Chat (VoIP):",	NULL, M_NONE, 0, "cl_voip", cp_bool }, // TODO: disable when rate < 25000
	{ MIF_CALL, "Lag Compensation:",	NULL, M_NONE, 0, "cg_antiLag", cp_lagComp },
};


#ifndef MISSIONPACK_HUD
static cvarValuePair_t cp_teamoverlay[] = { VINT( 0, "off" ), VINT( 1, "upper right" ), VINT( 2, "lower right" ), VINT( 3, "lower left" ), VEND };
#endif
static cvarValuePair_t cp_splitvertical[] = { VINT( 0, "horizontal" ), VINT( 1, "vertical" ), VEND };
static cvarValuePair_t cp_atmeffects[] = { VINT( 0, "off" ), VFLOAT( 0.5, "low" ), VINT( 1, "high" ), VEND };
static cvarValuePair_t cp_brassTime[] = { VINT( 0, "off" ), VINT( 1250, "short" ), VINT( 2500, "long" ), VEND };
static cvarValuePair_t cp_drawGun[] = { VINT( 0, "off" ), VINT( 1, "right-handed" ), VINT( 3, "centered" ), VINT( 2, "left-handed" ), VEND };

// ZTM: TODO: remove the MIF_CALL? I think it's there so they are selectable...
menuitem_t gameoptionsmenu_items[] =
{
	//{ MIF_CALL, "Crosshair:",				NULL, M_NONE, 0, "cg_drawCrosshair", NULL }, // ZTM: TODO: draw crosshair shaders
	{ MIF_CALL, "View Bobbing:",			NULL, M_NONE, 0, "cg_viewbob", cp_bool },
	{ MIF_CALL, "Simple Items:",			NULL, M_NONE, 0, "cg_simpleItems", cp_bool },
	{ MIF_CALL, "Marks on Walls:",			NULL, M_NONE, 0, "cg_marks", cp_bool },
	{ MIF_CALL, "Show Floating Scores:",	NULL, M_NONE, 0, "cg_scorePlums", cp_bool }, // Note: From Q3TA
	{ MIF_CALL, "Show Empty Shells:",		NULL, M_NONE, 0, "cg_brassTime", cp_brassTime }, // was Ejecting Brass
	{ MIF_CALL, "Dynamic Lights:",			NULL, M_NONE, 0, "r_dynamiclight", cp_bool },
	{ MIF_CALL, "Identify Target:",			NULL, M_NONE, 0, "cg_drawCrosshairNames", cp_bool },
	{ MIF_CALL, "Draw Gun:",				NULL, M_NONE, 0, "cg_drawGun", cp_drawGun }, // Note: From Q3TA
	{ MIF_CALL, "High Quality Sky:",		NULL, M_NONE, 0, "r_fastsky", cp_boolInvert }, // Note: Q3TA changed this from High Quality Sky to Low Quality Sky (and flip logic)
	{ MIF_CALL, "Sync Every Frame:",		NULL, M_NONE, 0, "r_finish", cp_bool },
	{ MIF_CALL, "Force Player Models:",		NULL, M_NONE, 0, "cg_forcemodel", cp_bool },
#ifdef MISSIONPACK_HUD // TA hud only have on/off
	{ MIF_CALL, "Draw Team Overlay:",		NULL, M_NONE, 0, "cg_drawTeamOverlay", cp_bool },
#else
	{ MIF_CALL, "Draw Team Overlay:",		NULL, M_NONE, 0, "cg_drawTeamOverlay", cp_teamoverlay },
#endif
	{ MIF_CALL, "Automatic Downloading:",	NULL, M_NONE, 0, "cl_allowDownload", cp_bool }, // Note: Q3TA renamed to Auto Download
	{ MIF_CALL, "Show Time:",				NULL, M_NONE, 0, "cg_drawTimer", cp_bool }, // Note: From Q3TA
#ifdef MISSIONPACK // Q3 doesn't have voice chats
	{ MIF_CALL, "Voice Chat (audio):",		NULL, M_NONE, 0, "cg_noVoiceChats", cp_boolInvert }, // was Voices Off, might be confused with VoIP
	{ MIF_CALL, "Voice Chat (text):",		NULL, M_NONE, 0, "cg_noVoiceText", cp_boolInvert }, // was Voice Text Off
	{ MIF_CALL, "Voice Chat (taunts):",		NULL, M_NONE, 0, "cg_noTaunt", cp_boolInvert }, // was Taunts Off
#endif
	{ MIF_CALL, "Team Chats Only:",			NULL, M_NONE, 0, "cg_teamChatsOnly", cp_bool }, // Note: From Q3TA
#ifdef MISSIONPACK // vanilla Q3 doesn't have videos in level shaders
	{ MIF_CALL, "In Game Video:",			NULL, M_NONE, 0, "r_inGameVideo", cp_bool }, // Note: From Q3TA
#endif
	{ MIF_CALL, "Splitscreen Mode:",		NULL, M_NONE, 0, "cg_splitviewVertical", cp_splitvertical },
	{ MIF_CALL, "Snow/Rain:",				NULL, M_NONE, 0, "cg_atmosphericEffects", cp_atmeffects },
};

// ZTM: TODO: Include Team Arena 'additional credits'?
menuitem_t creditsmenu_items[] =
{
	{ MIF_BIGTEXT|MIF_PANEL, "Quake 3", NULL, M_NONE, 0 },
	{ 0, "id Software is:", NULL, M_NONE, 0 },
	{ 0, "", NULL, M_NONE, 0 },
	{ 0, "Programming:", NULL, M_NONE, 0 },
	{ 0, "    John Carmack, Robert A. Duffy, Jim Dose'", NULL, M_NONE, 0 },
	{ 0, "Art:", NULL, M_NONE, 0 },
	{ 0, "    Adrian Carmack, Kevin Cloud, Kenneth Scott,", NULL, M_NONE, 0 },
	{ 0, "    Seneca Menard, Fred Nilsson", NULL, M_NONE, 0 },
	{ 0, "Game Designer:", NULL, M_NONE, 0 },
	{ 0, "    Graeme Devine", NULL, M_NONE, 0 },
	{ 0, "Level Design:", NULL, M_NONE, 0 },
	{ 0, "    Tim Willits, Christian Antkow, Paul Jaquays", NULL, M_NONE, 0 },
	{ 0, "CEO:", NULL, M_NONE, 0 },
	{ 0, "    Todd Hollenshead", NULL, M_NONE, 0 },
	{ 0, "Director of Business Development:", NULL, M_NONE, 0 },
	{ 0, "    Marty Stratton", NULL, M_NONE, 0 },
	{ 0, "Office Manager and id Mom:", NULL, M_NONE, 0 }, // Listed as Biz Assist and id Mom in Q3A
	{ 0, "    Donna Jackson", NULL, M_NONE, 0 },
	{ 0, "Development Assistance:", NULL, M_NONE, 0 },
	{ 0, "    Eric Webb", NULL, M_NONE, 0 },
	{ 0, "Bot AI and Programming Assistance:", NULL, M_NONE, 0 },
	{ 0, "    Jan Paul van Waveren", NULL, M_NONE, 0 },

	// These are all people that have made commits to Subversion, and thus
	//  probably incomplete.
	// (These are in alphabetical order, for the defense of everyone's egos.)
	{ MIF_BIGTEXT|MIF_PANEL, "ioquake3", NULL, M_NONE, 0 },
	{ 0, "ioquake3 contributors", NULL, M_NONE, 0 },
	{ 0, "", NULL, M_NONE, 0 },
	{ 0, "Tim Angus", NULL, M_NONE, 0 },
	{ 0, "James Canete", NULL, M_NONE, 0 },
	{ 0, "Vincent Cojot", NULL, M_NONE, 0 },
	{ 0, "Ryan C. Gordon", NULL, M_NONE, 0 },
	{ 0, "Aaron Gyes", NULL, M_NONE, 0 },
	{ 0, "Zack Middleton", NULL, M_NONE, 0 },
	{ 0, "Ludwig Nussel", NULL, M_NONE, 0 },
	{ 0, "Julian Priestley", NULL, M_NONE, 0 },
	{ 0, "Scirocco Six", NULL, M_NONE, 0 },
	{ 0, "Thilo Schulz", NULL, M_NONE, 0 },
	{ 0, "Zachary J. Slater", NULL, M_NONE, 0 },
	{ 0, "Tony J. White", NULL, M_NONE, 0 },
	{ 0, "...and many, many others!", NULL, M_NONE, 0 }, // keep this one last.

	{ MIF_BIGTEXT|MIF_PANEL, "Spearmint", NULL, M_NONE, 0 },
	{ 0, "Spearmint contributors", NULL, M_NONE, 0 },
	{ 0, "", NULL, M_NONE, 0 },
	{ 0, "Zack Middleton - main developer", NULL, M_NONE, 0 },
	{ 0, "Tobias Kuehnhammer & friends - Bot AI, bug reports", NULL, M_NONE, 0 },
	{ 0, "...and other contributors", NULL, M_NONE, 0 },
};

// Team Arena uses; Replay, Demo, Menu, Next
menuitem_t postgamemenu_items[] =
{
	{ MIF_BIGTEXT|MIF_CALL, "Menu", endGame, M_NONE, 0 },
	{ MIF_BIGTEXT|MIF_CALL|MIF_POPMENU, "Replay", restartMap, M_NONE, 0 },
	{ MIF_BIGTEXT|MIF_CALL|MIF_POPMENU, "Next", restartMap, M_NONE, 0 }, // TODO: add next map logic
};

menuitem_t stubmenu_items[] =
{
	{ 0, "STUB", NULL, M_NONE, 0 },
};


/*

	Menu definitions

*/

// quick menu definitions
#define QMENUDEF( items, flags, header ) { flags, header, items, ARRAY_LEN( items ) }
#define QMENUSTUB( items, flags, header ) { flags, header, stubmenu_items, ARRAY_LEN( stubmenu_items ) }

menudef_t ui_menus[M_NUM_MENUS] = {
	{ 0, NULL, NULL, 0 },										// M_NONE

	QMENUDEF( mainmenu_items, MF_MAINMENU, NULL ),				// M_MAIN
	QMENUDEF( singleplayermenu_items, 0, "Single Player" ),		// M_SINGLEPLAYER
	QMENUSTUB( multiplayermenu_items, 0, "Multiplayer" ),		// M_MULTIPLAYER
	QMENUDEF( setupmenu_items, 0, "Setup" ),					// M_SETUP
#ifdef MISSIONPACK
	QMENUDEF( demosmenu_items, 0, "Team Arena Demos" ),						// M_DEMOS
	QMENUDEF( cinematicsmenu_items, 0, "Team Arena Cinematics" ),			// M_CINEMATICS
	QMENUDEF( modsmenu_items, 0, "Team Arena Mods" ),						// M_MODS
	QMENUDEF( exitmenu_items, MF_DIALOG|MF_NOBACK, "Quit Team Arena?" ),	// M_EXIT
#else
	QMENUDEF( demosmenu_items, 0, "Demos" ),						// M_DEMOS
	QMENUDEF( cinematicsmenu_items, 0, "Cinematics" ),				// M_CINEMATICS
	QMENUDEF( modsmenu_items, 0, "Mods" ),							// M_MODS
	QMENUDEF( exitmenu_items, MF_DIALOG|MF_NOBACK, "Exit game?" ),	// M_EXIT
#endif

	// Setup menus
	QMENUSTUB( playermenu_items, 0, "Player Settings" ),	// M_PLAYER
	QMENUSTUB( controlsmenu_items, 0, "Controls" ),			// M_CONTROLS
	QMENUDEF( systemmenu_items, 0, "System Setup" ),		// M_SYSTEM
	QMENUDEF( gameoptionsmenu_items, 0, "Game Options" ),	// M_GAME_OPTIONS
	QMENUDEF( creditsmenu_items, 0, "Credits" ),			// M_CREDITS
	QMENUDEF( defaultsmenu_items, MF_DIALOG|MF_NOBACK, "Set to defaults?" ),	// M_DEFAULTS

	// Team Arena stubs
	QMENUSTUB( rulesmenu_items, 0, "Rules" ),					// M_RULES
	QMENUSTUB( startservermenu_items, 0, "Start Server" ),		// M_START_SERVER

	// in game menus
	QMENUDEF( postgamemenu_items, MF_POSTGAME|MF_NOESCAPE|MF_NOBACK, NULL ),	// M_POSTGAME
};
