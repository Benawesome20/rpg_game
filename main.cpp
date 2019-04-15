// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"
#include "maze.h"

// Functions in this file
MapItem* next_to(int x, int y, int type, int on, int erase);
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
int main ();

// Constants
#define NO_ACTION_LIMIT 200 // Accelerometer sensitivity limit required for movement
// NPC states
#define START 1
#define GO    2
#define FOUND 3
#define END   4
/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key; // if the player is holding the key
    bool omni; // if omnipotent mode is turned on
} Player;

// NPC walk counter, coordinates, and state
static int walk_counter = 0;
static int NPC_x = 24;
static int NPC_y = 22;
static int state = START;

// Looks for a MapItem of the given type next to the x,y
// and returns a pointer to it.
// If on is true, look at the tile at x,y.
// If erase is true, erase the tile it finds.
MapItem* next_to(int x, int y, int type, int on, int erase)
{
    MapItem* up    = get_north(Player.x, Player.y);
    MapItem* left  = get_west(Player.x, Player.y);
    MapItem* right = get_east(Player.x, Player.y);
    MapItem* down  = get_south(Player.x, Player.y);
    MapItem* here  = get_here(Player.x, Player.y);
    MapItem* output = NULL; //if no tile exists, return null

    if(up->type == type)
        output = up;
    else if(left->type == type)
        output = left;
    else if(right->type == type)
        output = right;
    else if(down->type == type)
        output = down;
    else if(on && here->type == type)
        output = here;
    // if erase is on and the tile was found
    if(erase && output) {
        if(up->type == type)
            map_erase(Player.x, Player.y - 1);
        else if(left->type == type)
            map_erase(Player.x - 1, Player.y);
        else if(right->type == type)
            map_erase(Player.x + 1, Player.y);
        else if(down->type == type)
            map_erase(Player.x, Player.y + 1);
        else if(here->type == type)
            map_erase(Player.x, Player.y);
    }

    return output;
}

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possible return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define OMNI_BUTTON 7
int get_action(GameInputs inputs)
{
    // Check for button presses first
    if(!inputs.b1)
        return ACTION_BUTTON;
    else if(!inputs.b2)
        return MENU_BUTTON;
    else if(!inputs.b3)
        return OMNI_BUTTON;
    // If x and y axes are within a certain bound, do not move
    else if(abs(inputs.ax) < NO_ACTION_LIMIT && abs(inputs.ay) < NO_ACTION_LIMIT)
        return NO_ACTION;
    // Otherwise, move in the direction of the greatest axis value
    else if(abs(inputs.ax) > abs(inputs.ay))
        return (inputs.ax > 0) ? GO_RIGHT : GO_LEFT;
    else
        return (inputs.ay > 0) ? GO_UP : GO_DOWN;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 *
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT       0
#define GAME_OVER_WIN   1
#define GAME_OVER_LOSS  2
#define FULL_DRAW       3
int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;

    MapItem* nextTile;

    bool full_draw = false;

    // if the player is actually doing something, increase the npc walk counter
    if(action)
        walk_counter += 1;

    // If the walk counter has reached 5 and we're in the main map, move the NPC in a random direction
    if(walk_counter >= 5 && get_active_map() == get_map(0)) {
        pc.printf("Starting NPC move\r\n");
        // save the old NPC spot
        int NPC_px = NPC_x;
        int NPC_py = NPC_y;
        do {
            int dir = rand() % 5; // move in the 4 directions or stay still
            pc.printf("Attempting to move in: %u\r\n", dir);
            switch(dir) {
                // move up
                case 1:
                    nextTile = get_north(NPC_x, NPC_y);
                    if(!nextTile || nextTile->walkable) // move if nextTile does not exist or is walkable
                        NPC_y -= 1;
                    else
                        continue;
                    break;
                // move right
                case 2:
                    nextTile = get_east(NPC_x, NPC_y);
                    if(!nextTile || nextTile->walkable)
                        NPC_x += 1;
                    else
                        continue;
                    break;
                // move down
                case 3:
                    nextTile = get_south(NPC_x, NPC_y);
                    if(!nextTile || nextTile->walkable)
                        NPC_y += 1;
                    else
                        continue;
                    break;
                // move left
                case 4:
                    nextTile = get_north(NPC_x, NPC_y);
                    if(!nextTile || nextTile->walkable)
                        NPC_y -= 1;
                    else
                        continue;
                    break;
                // stay still
                default:
                    nextTile = get_here(NPC_x, NPC_y);
                    break;
            }
        // loop if nextTile exists and is not walkable.
        }while(nextTile && !nextTile->walkable);
    // Update the NPC's location
    map_remove(NPC_px, NPC_py);
    add_NPC(NPC_x, NPC_y, &state);
    pc.printf("NPC removed and added\r\n");
    // Finally, reset the walk counter and make sure to return a full draw
    walk_counter = 0;
    full_draw = true;
    }

    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case GO_UP:
            pc.printf("Up\r\n");
            nextTile = get_north(Player.x, Player.y);
            if(Player.omni || !nextTile || nextTile->walkable) //if omni is on or the next tile doesn't exist or the next tile is walkable
                Player.y -= 1;
            break;
        case GO_LEFT:
            pc.printf("Left\r\n");
            nextTile = get_west(Player.x, Player.y);
            if(Player.omni || !nextTile || nextTile->walkable)
                Player.x -= 1;
            break;
        case GO_DOWN:
            pc.printf("Down\r\n");
            nextTile = get_south(Player.x, Player.y);
            if(Player.omni || !nextTile || nextTile->walkable)
                Player.y += 1;
            break;
        case GO_RIGHT:
            pc.printf("Right\r\n");
            nextTile = get_east(Player.x, Player.y);
            if(Player.omni || !nextTile || nextTile->walkable)
                Player.x += 1;
            break;
        case ACTION_BUTTON:
            pc.printf("Action button\r\n");
            // If you are standing next to an NPC
            MapItem* npc = next_to(Player.x, Player.y, NPC, false, false);
            if(npc) {
                pc.printf("NPC found\r\n");
                if (npc->data) pc.printf("NPC data: %u\r\n", *((int*)npc->data));

                // set the NPC to say the correct lines if the player has the key
                if(npc->data && Player.has_key == true && *((int*)npc->data) == GO) {
                    state = FOUND;
                    npc->data = &state;
                }

                if(npc->data && *((int*)npc->data) == START) {
                    const char* lines[] = { "Wha... where am  ",
                                            "I? Who are you?  ",
                                            "No, wait... I'm  ",
                                            "supposed to tell ",
                                            "you something... ",
                                            "There's a key    ",
                                            "hidden in those  ",
                                            "shifting ruins   ",
                                            "just south of    ",
                                            "here; take the   ",
                                            "stairs down. It's",
                                            "the only way to  ",
                                            "escape... How do ",
                                            "I know that?     "};
                    long_speech(lines, 14);

                    // set the NPC to say the next lines
                    state = GO;
                    npc->data = &state;
                    return FULL_DRAW;
                }
                else if(npc->data && *((int*)npc->data) == GO) {
                    const char* lines[] = { "You have to get  ",
                                            "that key. I'm    ",
                                            "not allowed to   ",
                                            "leave this map.  "};
                    long_speech(lines, 4);

                    return FULL_DRAW;
                }
                else if(npc->data && *((int*)npc->data) == FOUND) {
                    const char* lines[] = { "Thank god, you   ",
                                            "found it. There's",
                                            "only one lock in ",
                                            "this godforsaken ",
                                            "place, it's just ",
                                            "south of here.   ",
                                            "You know the     ",
                                            "place.           "};
                    long_speech(lines, 8);

                    // set the NPC to say the next lines
                    state = END;
                    npc->data = &state;
                    return FULL_DRAW;
                }
                else if(npc->data && *((int*)npc->data) == END) {
                    const char* lines[] = { "Please, end it.  "};
                    long_speech(lines, 1);
                    return FULL_DRAW;
                }
                else {
                    const char* lines[] = { "YOU SHOULDN'T BE",
                              "HERE.           "};
                    long_speech(lines, 2);
                    return FULL_DRAW;
                }
            }

            // If you are standing on or next to a key, take it and erase it
            if(next_to(Player.x, Player.y, KEY, true, true)) {
                pc.printf("Key found\r\n");

                // if you're in the ruins, swap the mazes
                if(get_active_map() == get_map(1)) {
                    remove_maze(2, 17, maze1);
                    add_maze(2, 17, maze2);
                    pc.printf("Maze shifted\r\n");
                }
                Player.has_key = 1;

                return FULL_DRAW;
            }

            // If you are standing next to a door with a key, open it
            MapItem* door = next_to(Player.x, Player.y, DOOR, false, false);
            if(Player.has_key && (door)) {
                pc.printf("Door opened\r\n");
                door->walkable = true;
                door->draw = draw_door_open;

                return FULL_DRAW;
            }

            // If you are standing on or next to a win item, take it and win the game.
            if(next_to(Player.x, Player.y, WIN_ITEM, true, false)) {
                pc.printf("Win item taken\r\n");

                return GAME_OVER_WIN;
            }

            // If you are standing on or next to stairs, go to their map.
            nextTile = next_to(Player.x, Player.y, STAIRS, true, false);
            if(nextTile) {
                pc.printf("Going down stairs\r\n");
                int map_num = *((int*)nextTile->data);
                set_active_map(map_num);
                if(map_num == 1) {
                    Player.x = 7;
                    Player.y = 28;
                }
                else if(map_num == 0) {
                    Player.x = 22;
                    Player.y = 26;
                }
                else
                    Player.x = Player.y = 25; // just in case
                return FULL_DRAW;
            }
            break;
        case MENU_BUTTON:
            pc.printf("Menu button\r\n");
            break;
        case OMNI_BUTTON:
            pc.printf("Omnipotent Mode activated/deactivated: %d\r\n", !Player.omni);
            Player.omni = !Player.omni; //toggle on/off
            break;
        default:
            break;
    }
    if(full_draw)
        return FULL_DRAW;
    return NO_RESULT;
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // Draw game border first
    if(init) draw_border();
    
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                draw_player(u, v, Player.has_key);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height() && (i != 0 || j != 0)) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars
    draw_upper_status(Player.x, Player.y);
    draw_lower_status(Player.has_key);
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion.
 */
void init_main_map()
{
    // Create ruins
    Map* map = set_active_map(1);
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    add_wall(1,              17,             HORIZONTAL, 1);
    add_wall(13,             17,             HORIZONTAL, 1);
    add_wall(13,             28,             HORIZONTAL, 1);
    add_wall(1,              28,             HORIZONTAL, 1);
    add_maze(2, 17, maze1);
    static int map1 = 0;
    add_stairs(7, 28, &map1);
    add_key(7,3);
    print_map();

    // "Random" plants
    map = set_active_map(0);
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        // Make sure there are no plants in the building
        if(!(i % map_width() > 16 && i % map_width() < 35 && i / map_width() > 27 && i / map_width() < 40))
            add_plant(i % map_width(), i / map_width());
    }
    pc.printf("plants on main\r\n");

    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    // Player building
    add_wall(23,            23,              HORIZONTAL, 2);
    add_wall(23,            24,              VERTICAL,   4);
    add_wall(26,            23,              HORIZONTAL, 2);
    add_wall(27,            24,              VERTICAL,   4);
    add_wall(24,            27,              HORIZONTAL, 3);
    // Throne building
    add_wall(16,            27,              HORIZONTAL, 7);
    add_wall(28,            27,              HORIZONTAL, 7);
    add_wall(16,            28,              VERTICAL,   12);
    add_wall(35,            27,              VERTICAL,   13);
    add_wall(16,            40,              HORIZONTAL, 20);

    pc.printf("Walls done on main!\r\n");

    add_NPC(24, 22, &state);
    //add_key(24, 20);
    add_door(25, 40, 0);
    add_win_item(25, 33);
    static int map2 = 1;
    add_stairs(22, 26, &map2);
    pc.printf("NPC, key, and door added on main\r\n");

    print_map();
}

/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");

    // Initialize the maps
    maps_init();
    init_main_map();

    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 25;
    Player.has_key = 0;

    GameInputs in;

    // Draw start page
    draw_start_page();

    // Initial drawing
    draw_game(true);

    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();

        // Actually do the game update:
        // 1. Read inputs
        in = read_inputs();
        //pc.printf("X: %d, Y: %d, Z: %d\r\n", in.ax, in.ay, in.az);

        // 2. Determine action (get_action)
        int action = get_action(in);
        // 3. Update game (update_game)
        int result = update_game(action);
        // 3b. Check for game over
        if(result == GAME_OVER_WIN) {
            draw_game_over(1);
            return 1;
        }
        else if(result == GAME_OVER_LOSS) {
            draw_game_over(0);
            return 0;
        }

        // 4. Draw frame (draw_game)
        draw_game(result);

        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
}
