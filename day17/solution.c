#include "..\utilities.h"

#define input_file "input"

typedef struct int2 {
    int x;
    int y;
} int2;

typedef struct rock {
    int height;
    int offset;
    int parts_count;
    int2 parts[5];
} rock;

rock rocks[] = {
{
    .height = 1,
    .parts_count = 4,
    .parts = {
        {.x = 0, .y = 0},
        {.x = 1, .y = 0},
        {.x = 2, .y = 0},
        {.x = 3, .y = 0}
    }
},
{
    .height = 3,
    .parts_count = 5,
    .parts = {
        {.x = 1, .y = 0},
        {.x = 0, .y = 1},
        {.x = 1, .y = 1},
        {.x = 2, .y = 1},
        {.x = 1, .y = 2}
    }
},
{
    .height = 3,
    .parts_count = 5,
    .parts = {
        {.x = 2, .y = 0},
        {.x = 2, .y = 1},
        {.x = 0, .y = 2},
        {.x = 1, .y = 2},
        {.x = 2, .y = 2}
    }
},
{
    .height = 4,
    .parts_count = 4,
    .parts = {
        {.x = 0, .y = 0},
        {.x = 0, .y = 1},
        {.x = 0, .y = 2},
        {.x = 0, .y = 3}
    }
},
{
    .height = 2,
    .parts_count = 4,
    .parts = {
        {.x = 0, .y = 0},
        {.x = 1, .y = 0},
        {.x = 0, .y = 1},
        {.x = 1, .y = 1}
    }
}
};

static byte* jet_pattern;

typedef struct game_data {
    rock *active_rock;
    u64 rocks_at_rest;

    // simulation
    u64 top;
    u64 bottom;

    u64 highest_point;
    int jet_pattern;

    u64 bottom_y[8];
    byte bottom_mask;

    u64 rock_it;
} game_data;

static game_data game;

#define tunnel_width 9
#define tunnel_depth 2048
static byte tunnel[tunnel_depth][tunnel_width];

byte read_tunnel_at(u64 x, u64 y)
{
    return tunnel[y % tunnel_depth][x];
}

void write_tunnel_at(byte data, u64 x, u64 y)
{
    tunnel[y % tunnel_depth][x] = data;
}

/*
rock type / fall order:

####

 # 
###
.#.

..#
..#
###

#
#
#
#

##
##

chamber is 7 units wide

each rock appears so that its left edge is two units away from the left wall and
its bottom edge is three units above the highest rock in the room (or the floor,
if there isn't one).

after a rock appears, it alternates between being pushed by a jet of hot gas
one unit (in the direction indicated by the next symbol in the jet pattern)
and then falling one unit down.

rock appears
jet pushes rock
rock falls 1 unit down

note:
on each spawn of a new rock. the chamber gets 3 units + the height of the rock
taller.

find how tall tower will get after 2022 rocks have stopped falling
example input should yield 3068

*/

void print_tunnel_part(int top, int bottom)
{
    printf("\n");
    for (int y = top; y >= bottom; --y) {
        for (int x = 0; x < 9; ++x) {
            printf("%c", read_tunnel_at(x, y));
        }
        printf("\n");
    }
}

void print_tunnel()
{
    u64 xd = game.active_rock ? game.active_rock->offset : 0;
    u64 yd = game.top;
    if (game.active_rock) {
        for (int i = 0; i < game.active_rock->parts_count; ++i) {
            write_tunnel_at('@', xd + game.active_rock->parts[i].x, yd - game.active_rock->parts[i].y);
        }
    }

    for (u64 y = game.top; y > game.bottom; --y) {
        for (int x = 0; x < 9; ++x) {
            printf("%c", read_tunnel_at(x, y));
        }
        printf("\n");
    }

    if (game.active_rock) {
        for (int i = 0; i < game.active_rock->parts_count; ++i) {
            // tunnel[yd - game.active_rock->parts[i].y][xd + game.active_rock->parts[i].x] = '.';
            write_tunnel_at('.', xd + game.active_rock->parts[i].x, yd - game.active_rock->parts[i].y);
        }
    }
    printf("\n\n");
}

void spawn_rock()
{
    // pick next rock to be spawned and set it to active rock
    int num_rocks = num_elements(rocks);
    int it = game.rock_it++ % num_rocks;
    game.active_rock = &rocks[it];

    game.top = 3 + game.highest_point + game.active_rock->height;
    for (u64 y = game.highest_point + 1; y <= game.top; ++y) {
        write_tunnel_at('|', 0, y);
        for (int x = 1; x < tunnel_width - 1; ++x) {
            write_tunnel_at('.', x, y);
        }
        write_tunnel_at('|', tunnel_width - 1, y);
    }

    game.active_rock->offset = 3;
    // its left edge is two units away from the left wall and its bottom edge is 
    // three units above the highest rock in the room (or the floor, if there isn't one).
}

int check_and_move(int2 direction)
{
    int x = game.active_rock->offset + direction.x;
    u64 y = game.top + direction.y;

    for (int i = 0; i < game.active_rock->parts_count; ++i) {
        u64 yd = y - game.active_rock->parts[i].y;
        int xd = x + game.active_rock->parts[i].x;
        char tunnel_part = read_tunnel_at(xd, yd);
        if (tunnel_part != '.')
            return 0;
    }
    game.active_rock->offset = x;
    game.top = y;
    return 1;
}

typedef struct history {
    int topology[7];
    int rock_id;
    int stream_id;
} history;

static history history_buffer;

int step_simulation(u64 threshold)
{
    if (game.rocks_at_rest == threshold)
        return 0;

    if (!game.active_rock) {
        spawn_rock();
    }

    // read jet pattern
    int total_patterns = (int)list_length(jet_pattern);
    int pid = game.jet_pattern++ % total_patterns;
    int cmd = jet_pattern[pid] == '<' ? -1 : 1;

    // jet pattern move
    check_and_move((int2) { .x = cmd });

    // rock falls 1 unit
    if (!check_and_move((int2) { .y = -1 })) {
        int xd = game.active_rock->offset;
        u64 yd = game.top;
        for (int i = 0; i < game.active_rock->parts_count; ++i) {
            int x = xd + game.active_rock->parts[i].x;
            write_tunnel_at('#', x, yd - game.active_rock->parts[i].y);

            if (!check_bit(game.bottom_mask, x)) {
                set_bit(game.bottom_mask, x);
            }
            game.bottom_y[x] = __max(game.bottom_y[x], yd - game.active_rock->parts[i].y);
        }
        game.highest_point = game.top > game.highest_point ? game.top : game.highest_point;
        game.active_rock = null_ptr;
        game.rocks_at_rest++;
    }
    if (game.bottom_mask == 0xfe) {
        u64 min_y = game.bottom_y[1];
        for (int i = 2; i < 8; ++i) {
            min_y = __min(min_y, game.bottom_y[i]);
        }

        // printf("diff %"PRIu64" rock: %"PRIu64", windidx %d\n", min_y - game.bottom, game.rock_it % 5, game.jet_pattern % (int)list_length(jet_pattern));

        game.bottom = min_y;
        game.bottom_mask = 0;
    }
    return 1;
}

void parse_input()
{
    FILE* file = null_ptr;
    assert(fopen_s(&file, input_file, "rb") == 0);

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    int to_read, read;
    read = to_read = 0;
    char line[4096] = { 0 };

    do {
        to_read = min(4095, file_size - read);
        assert(fread(line, 1, to_read, file) == to_read);
        line[to_read] = '\0';
        list_printf(jet_pattern, line);
        read += to_read;
    } while (read < file_size);

    assert(list_length(jet_pattern) == read);

    fclose(file);
}

u64 part_one()
{
    u64 answer = 0;

    while (step_simulation(2022));
    answer = game.highest_point;

    return answer;
}

u64 part_two()
{
    u64 answer = 0;

    while (step_simulation(1000000000000));
    answer = game.highest_point;

    return answer;
}

void main()
{
#if 0
    for (int x = 1; x < 8; ++x) {
        tunnel[0][x] = '-';
    }
    tunnel[0][0] = '+';
    tunnel[0][8] = '+';

    for (int y = 1; y < tunnel_depth; ++y) {
        tunnel[y][0] = '|';
        for (int x = 1; x < 8; ++x) {
            tunnel[y][x] = '.';
        }
        tunnel[y][8] = '|';
    }
#endif
    parse_input();
    // printf("%"PRIu64"\n", part_one());
    printf("%"PRIu64"\n", part_two());
}