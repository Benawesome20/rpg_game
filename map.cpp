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
static Map ruins;
static int active_map;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    // Map it to the number as if each tile of the map were numbered
    // going across from the top left and then down
    unsigned mapped = Y * map_width() + X;
    return mapped;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    unsigned hash_value = key % (map_width());
    return hash_value;
}

void maps_init()
{
    // Initialize hash table
    map.items = createHashTable(map_hash, MAP_HEIGHT);
    ruins.items = createHashTable(map_hash, 30);
    // Set width & height
    map.w = MAP_WIDTH;
    map.h = MAP_HEIGHT;
    ruins.w = 15;
    ruins.h = 30;
}

Map* get_active_map()
{
    if(active_map == 0)
        return &map;
    else if(active_map == 1)
        return &ruins;
    else
        return &map; //default to map
}

Map* set_active_map(int m)
{
    active_map = m;
    if(active_map == 0)
        return &map;
    else if(active_map == 1)
        return &ruins;
    else
        return NULL;
}

Map* get_map(int m)
{
    if(m == 0)
        return &map;
    else if(m == 1)
        return &ruins;
    else
        return &map; //default to map
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
    return get_active_map()->w;
}

int map_height()
{
    return get_active_map()->h;
}

int map_area()
{
    return get_active_map()->w * get_active_map()->h;
}

MapItem* get_north(int x, int y)
{
    // Check if there is no northern tile
    if(y <= 0)
        return NULL;
    return (MapItem*) getItem(get_active_map()->items, XY_KEY(x, y - 1));
}

MapItem* get_south(int x, int y)
{
    // Check if there is no southern tile
    if(y >= get_active_map()->h - 1)
        return NULL;
    return (MapItem*) getItem(get_active_map()->items, XY_KEY(x, y + 1));
}

MapItem* get_east(int x, int y)
{
    // Check if there is no eastern tile
    if(x >= get_active_map()->w - 1)
        return NULL;
    return (MapItem*) getItem(get_active_map()->items, XY_KEY(x + 1, y));
}

MapItem* get_west(int x, int y)
{
    // Check if there is no western tile
    if(x <= 0)
        return NULL;
    return (MapItem*) getItem(get_active_map()->items, XY_KEY(x - 1, y));
}

MapItem* get_here(int x, int y)
{
    // Check if tile is on map
    if(x > -1 && x < get_active_map()->w && y > -1 && y < get_active_map()->h)
        return (MapItem*) getItem(get_active_map()->items, XY_KEY(x, y));
    else
        return NULL;
}


void map_erase(int x, int y)
{
    deleteItem(get_active_map()->items, XY_KEY(x, y));
}

void* map_remove(int x, int y)
{
    return removeItem(get_active_map()->items, XY_KEY(x,y));
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

void add_NPC(int x, int y, int* state)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC;
    w1->draw = draw_NPC;
    w1->walkable = false;
    w1->data = state;
    pc.printf("NPC created with data %u\r\n", *((int*)w1->data));
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
    w1->draw = (open) ? draw_door_open : draw_door_closed;
    w1->walkable = (open) ? true : false; // if the door is open, you can walk through it
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_stairs(int x, int y, int* map)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = STAIRS;
    w1->draw = draw_stairs;
    w1->walkable = true;
    w1->data = map; //data points to the map the stairs lead to
    pc.printf("NPC created with data %u\r\n", *((int*)w1->data));
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

void add_maze(int x, int y, const char* maze)
{
    for(int i = 0; i < 11*11; i++)
    {
        if(maze[i] == 'w')
            add_wall(x + i % 11, y + i / 11, HORIZONTAL, 1);
    }
}

void remove_maze(int x, int y, const char* maze)
{
    for(int i = 0; i < 11*11; i++)
    {
        if(maze[i] == 'w')
            map_erase(x + i % 11, y + i / 11);
    }
}
