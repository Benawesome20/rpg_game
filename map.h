#ifndef MAP_H
#define MAP_H

#include "hash_table.h"

/**
 * A structure to represent the map. The implementation is private.
 */
struct Map;

// A function pointer type for drawing MapItems.
// All tiles are 11x11 blocks.
// u,v is the top left corner pixel of the block
typedef void (*DrawFunc)(int u, int v);

/**
 * The data for elements in the map. Each item in the map HashTable is a
 * MapItem.
 */
typedef struct {
    /**
     * Indicates the "type" of the MapItem: WALL, DOOR, PLANT, etc. This is
     * useful for determining how to interact with the object when updating the
     * game state.
     */
    int type;
    
    /**
     * A function pointer to the drawing function for this item. Used by draw_game.
     */
    DrawFunc draw;
    
    /**
     * If zero, this item should block character motion.
     */
    int walkable;
    
    /**
     * Arbitrary extra data for the MapItem. Could be useful for keeping up with
     * special information, like where a set of stairs should take the player.
     *
     * Iterpretation of this can depend on the type of the MapItem. For example,
     * a WALL probably doesn't need to use this (it can be NULL), where an NPC
     * might use it to store game state (have I given the player the key yet?).
     */
    void* data;
} MapItem;

typedef struct {
    int tm;
    int tx, ty;
} StairsData;

// MapItem types
// Define more of these!
#define WALL    0
#define PLANT   1
#define NPC     2
#define KEY     3
#define DOOR    4
#define STAIRS  5
#define WIN_ITEM 6

/**
 * Initializes the internal structures for all maps. This does not populate
 * the map with items, but allocates space for them, initializes the hash tables, 
 * and sets the width and height.
 */
void maps_init();

/**
 * Returns a pointer to the active map.
 */
Map* get_active_map();

/**
 * Sets the active map to map m, where m is the index of the map to activate.
 * Returns a pointer to the new active map.
 */
Map* set_active_map(int m);

/**
 * Returns the map m, regardless of whether it is the active map. This function
 * does not change the active map.
 */
Map* get_map(int m);

/**
 * Print the active map to the serial console.
 */
void print_map();

// Access
/**
 * Returns the width of the active map.
 */
int map_width();

/**
 * Returns the heigh of the active map.
 */
int map_height();

/**
 * Returns the total number of cells in the active map.
 */
int map_area();

/**
 * Returns the MapItem immediately above the given location.
 */
MapItem* get_north(int x, int y);

/**
 * Returns the MapItem immediately below the given location.
 */
MapItem* get_south(int x, int y);

/**
 * Returns the MapItem immediately to the right of the given location.
 */
MapItem* get_east(int x, int y);

/**
 * Returns the MapItem immediately to the left of  the given location.
 */
MapItem* get_west(int x, int y);

/**
 * Returns the MapItem at the given location.
 */
MapItem* get_here(int x, int y);

// Directions, for using the modification functions
#define HORIZONTAL  0
#define VERTICAL    1

/**
 * If there is a MapItem at (x,y), remove it from the map.
 */
void map_erase(int x, int y);

/**
 * If there is a MapItem at (x,y), remove it from the map and return its value.
 */
void* map_remove(int x, int y);


/**
 * Add WALL items in a line of length len beginning at (x,y).
 * If dir == HORIZONTAL, the line is in the direction of increasing x.
 * If dir == VERTICAL, the line is in the direction of increasing y.
 *
 * If there are already items in the map that collide with this line, they are
 * erased.
 */
void add_wall(int x, int y, int dir, int len);

/**
 * Add a PLANT item at (x,y). If there is already a MapItem at (x,y), erase it
 * before adding the plant.
 */
void add_plant(int x, int y);

/**
 * Add an NPC item at (x,y) with a certain state. If there is already a MapItem at (x,y), erase it
 * before adding the NPC.
 */
void add_NPC(int x, int y, int* state);

/**
 * Add a key item at (x,y). If there is already a MapItem at (x,y), erase it
 * before adding the key.
 */
void add_key(int x, int y);

/**
 * Add a door at (x,y). If there is already a MapItem at (x,y), erase it
 * before adding the door.
 * It can be open or closed.
 */
void add_door(int x, int y, int open);


/**
 * Add stairs at (x,y) to map map. If there is already a MapItem at (x,y), erase it
 * before adding the stairs.
 */
void add_stairs(int x, int y, int* map);

/**
 * Add a throne at (x,y). If there is already a MapItem at (x,y), erase it
 * before adding the throne.
 */
void add_win_item(int x, int y);

/*
 * Add a maze of walls starting at (x,y) given by the char array.
 */
void add_maze(int x, int y, const char* maze);

/**
 * Removes the walls that make up the maze at (x,y).
 */
void remove_maze(int x, int y, const char* maze);

#endif //MAP_H
