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
//
// g_arenas.c
//

#include "g_local.h"

/*
==================
UpdateTournamentInfo
==================
*/
void UpdateTournamentInfo( void ) {
	int			i;
	gentity_t	*ent;
	int			playerNum;
	int			n, accuracy, perfect,	msglen;
	int			score1, score2;
	qboolean	won;
	char		buf[32];
	char		msg[MAX_STRING_CHARS];

	// find the real player
	ent = NULL;
	for (i = 0; i < level.maxplayers; i++ ) {
		ent = &g_entities[i];
		if ( !ent->inuse ) {
			continue;
		}
		if ( !( ent->r.svFlags & SVF_BOT ) ) {
			break;
		}
	}
	// this should never happen!
	if ( !ent || i == level.maxplayers ) {
		return;
	}
	playerNum = i;

	CalculateRanks();

	if ( level.players[playerNum].sess.sessionTeam == TEAM_SPECTATOR ) {
		Com_sprintf( msg, sizeof(msg), "postgame %i %i 0 0 0 0 0 0 0 0 0 0 0", level.numNonSpectatorPlayers, playerNum );
	}
	else {
		if( ent->player->accuracy_shots ) {
			accuracy = ent->player->accuracy_hits * 100 / ent->player->accuracy_shots;
		}
		else {
			accuracy = 0;
		}

		won = qfalse;
		if (g_gametype.integer >= GT_TEAM) {
			score1 = level.teamScores[TEAM_RED];
			score2 = level.teamScores[TEAM_BLUE];
			if (level.players[playerNum].sess.sessionTeam	== TEAM_RED) {
				won = (level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE]);
			} else {
				won = (level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED]);
			}
		} else {
			if (&level.players[playerNum] == &level.players[ level.sortedPlayers[0] ]) {
				won = qtrue;
				score1 = level.players[ level.sortedPlayers[0] ].ps.persistant[PERS_SCORE];
				score2 = level.players[ level.sortedPlayers[1] ].ps.persistant[PERS_SCORE];
			} else {
				score2 = level.players[ level.sortedPlayers[0] ].ps.persistant[PERS_SCORE];
				score1 = level.players[ level.sortedPlayers[1] ].ps.persistant[PERS_SCORE];
			}
		}
		if (won && ent->player->ps.persistant[PERS_KILLED] == 0) {
			perfect = 1;
		} else {
			perfect = 0;
		}
		Com_sprintf( msg, sizeof(msg), "postgame %i %i %i %i %i %i %i %i %i %i %i %i %i %i",
			level.numNonSpectatorPlayers,
			playerNum,
			accuracy,
			ent->player->ps.persistant[PERS_IMPRESSIVE_COUNT],
			ent->player->ps.persistant[PERS_EXCELLENT_COUNT],
			ent->player->ps.persistant[PERS_DEFEND_COUNT],
			ent->player->ps.persistant[PERS_ASSIST_COUNT],
			ent->player->ps.persistant[PERS_GAUNTLET_FRAG_COUNT],
			ent->player->ps.persistant[PERS_SCORE],
			perfect,
			score1,
			score2,
			level.time,
			ent->player->ps.persistant[PERS_CAPTURES]
			);
	}

	msglen = strlen( msg );
	for( i = 0; i < level.numNonSpectatorPlayers; i++ ) {
		n = level.sortedPlayers[i];
		Com_sprintf( buf, sizeof(buf), " %i %i %i",
			n,
			level.players[n].ps.persistant[PERS_RANK],
			level.players[n].ps.persistant[PERS_SCORE]
			);
		msglen += strlen( buf );
		if( msglen >= sizeof(msg) ) {
			break;
		}
		strcat( msg, buf );
	}
	trap_Cmd_ExecuteText( EXEC_APPEND, msg );
}
