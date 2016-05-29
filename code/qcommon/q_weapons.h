#ifndef Q_WEAPONS_H
#define Q_WEAPONS_H

typedef enum {
	WP_NONE,

	WP_GAUNTLET,
	WP_MACHINEGUN,
	WP_SHOTGUN,
	WP_GRENADE_LAUNCHER,
	WP_ROCKET_LAUNCHER,
	WP_LIGHTNING,
	WP_RAILGUN,
	WP_PLASMAGUN,
	WP_BFG,
	WP_GRAPPLING_HOOK,
	WP_NAILGUN,
	WP_PROX_LAUNCHER,
	WP_CHAINGUN,

	WP_COLT,

	WP_NUM_WEAPONS
} weapon_t;


typedef enum {
	AM_NONE,

	AM_5MM,
	AM_SHELLS,
	AM_GRENADES,
	AM_ROCKETS,
	AM_LIGHTNING,	// !TODO: Come up with something better for this.
	AM_SLUGS,		// !TODO: Come up with something better for this.
	AM_CELLS,
	AM_NAILS,
	AM_MINES_PROX,

	AM_9MM,

	AM_NUM_AMMOS
} ammo_t;

#endif
