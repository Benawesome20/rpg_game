#include "map.h"

#include "globals.h"
#include "graphics.h"

#define MAP_WIDTH 50
#define MAP_HEIGHT 50
#define NUM_TILES MAP_WIDTH*MAP_HEIGHT

/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map;
static int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    // Map it to the number as if each tile of the map were numbered
    // going across from the top left and then down
    unsigned mapped = Y * MAP_WIDTH + X;
    return mapped;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    unsigned hash_value = key % (NUM_TILES);
    return hash_value;
}

void maps_init()
{
    // Initialize hash table
    map.items = createHashTable(map_hash, NUM_TILES);
    // Set width & height
    map.w = MAP_WIDTH;
    map.h = MAP_HEIGHT;
}

Map* get_active_map()
{
    // There's only one map
    return &map;
}

Map* set_active_map(int m)
{
    active_map = m;
    return &map;
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P', 'N', 'K', 'D', 'S', 'I'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    return map.w;
}

int map_height()
{
    return map.h;
}

int map_area()
{
    return map.w * map.h;
}

MapItem* get_north(int x, int y)
{
    // Check if there is no northern tile
    if(y <= 0)
        return NULL;
    return (MapItem*) getItem(map.items, map_hash(XY_KEY(x, y - 1)));
}

MapItem* get_south(int x, int y)
{
    // Check if there is no southern tile
    if(y >= map.h - 1)
        return NULL;
    return (MapItem*) getItem(map.items, map_hash(XY_KEY(x, y + 1)));
}

MapItem* get_east(int x, int y)
{
    // Check if there is no eastern tile
    if(x >= map.w - 1)
        return NULL;
    return (MapItem*) getItem(map.items, map_hash(XY_KEY(x + 1, y)));
}

MapItem* get_west(int x, int y)
{
    // Check if there is no western tile
    if(x <= 0)
        return NULL;
    return (MapItem*) getItem(map.items, map_hash(XY_KEY(x - 1, y)));
}

MapItem* get_here(int x, int y)
{
    // Check if tile is on map
    if(x > -1 && x < map.w && y > -1 && y < map.h)
        return (MapItem*) getItem(map.items, map_hash(XY_KEY(x, y)));
    else
        return NULL;
}


void map_erase(int x, int y)
{
    deleteItem(map.items, map_hash(XY_KEY(x, y)));
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_plant;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_NPC(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC;
    w1->draw = draw_NPC;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_key(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = KEY;
    w1->draw = draw_key;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_door(int x, int y, int open)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = DOOR;
    w1->draw = (open) ? draw_nothing : draw_door; //change to actual draw functions
    w1->walkable = (open) ? true : false; // if the door is open, you can walk through it
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_win_item(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = WIN_ITEM;
    w1->draw = draw_win_item;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
