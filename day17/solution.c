#include "..\utilities.h"

#define input_file "input"

typedef struct int2 {
    int x;
    int y;
} int2;

typedef struct rock {
    int height;
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
    int x;

    // simulation
    u64 top;
    u64 bottom;
    u64 tunnel_offset;

    u64 highest_point;

    u64 bottom_y[7];
    byte found_repetition;
    byte bottom_mask;

    u64 jit;
    u64 rit;
} game_data;

static game_data game;

#define tunnel_width 9
#define tunnel_depth 2048
static byte tunnel[tunnel_depth][tunnel_width];

byte read_tunnel_at(u64 x, u64 y)
{
    u64 yd = y - game.tunnel_offset;
    return tunnel[yd % tunnel_depth][x];
}

void write_tunnel_at(byte data, u64 x, u64 y)
{
    u64 yd = y - game.tunnel_offset;
    tunnel[yd % tunnel_depth][x] = data;
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
    u64 xd = game.x;
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
    game.active_rock = &rocks[game.rit % num_elements(rocks)];
    game.top = 3 + game.highest_point + game.active_rock->height;

    for (u64 y = game.highest_point + 1; y <= game.top; ++y) {
        for (int x = 1; x < tunnel_width - 1; ++x) {
            write_tunnel_at('.', x, y);
        }
    }
    game.x = 3;
}

int check_and_move(int2 direction)
{
    int x = game.x + direction.x;
    u64 y = (game.top + direction.y);

    for (int i = 0; i < game.active_rock->parts_count; ++i) {
        u64 yd = y - game.active_rock->parts[i].y;
        int xd = x + game.active_rock->parts[i].x;

         if (xd > 0 && xd < 8 && yd > 0) {
            char tunnel_part = read_tunnel_at(xd, yd);
            // if (tunnel_part != '.' && tunnel_part != ' ')
            if (tunnel_part == '#')
                return 0;
        }
        else
            return 0;
    }
    game.x = x;
    game.top = y;
    return 1;
}

typedef struct observation {
    int topology[7];
    u64 rid;
    u64 jid;
    u64 top;
} observation;


#define max_history 1024
static int history_count;
static observation history[max_history];

int has_observation(observation item)
{
    int num_rocks = num_elements(rocks);
    for (int i = 0; i < history_count; ++i) {
        if (history[i].rid % num_rocks == item.rid % num_rocks &&
            history[i].jid == item.jid) {
            for (int j = 0; j < 7; ++j) {
                if (history[i].topology[j] != item.topology[j])
                    return 0;
            }
            return i;
        }
    }
    return 0;
}

int step_simulation(u64 threshold)
{
    if (game.rit == threshold)
        return 0;

    if (!game.active_rock) {
        spawn_rock();
    }

    do {
        // read jet pattern
        int cmd = jet_pattern[game.jit++ % list_length(jet_pattern)] == '<' ? -1 : 1;
        // jet pattern move & rock falls 1 unit
        check_and_move((int2) { .x = cmd });
    } while (check_and_move((int2) { .y = -1 }));

    // rest at rock
    int xd = game.x;
    u64 yd = game.top;

    for (int i = 0; i < game.active_rock->parts_count; ++i) {
        int x = xd + game.active_rock->parts[i].x;
        write_tunnel_at('#', x, yd - game.active_rock->parts[i].y);

        if (!game.found_repetition) {
            if (!check_bit(game.bottom_mask, x)) {
                set_bit(game.bottom_mask, x);
            }
            game.bottom_y[x - 1] = __max(game.bottom_y[x - 1], yd - game.active_rock->parts[i].y);
        }
    }

    game.highest_point = game.top > game.highest_point ? game.top : game.highest_point;
    game.active_rock = null_ptr;
    game.rit++;

    if (!game.found_repetition && game.bottom_mask == 0xfe) {
        u64 min_y = game.bottom_y[0];
        for (int i = 0; i < 7; ++i) {
            min_y = __min(min_y, game.bottom_y[i]);
        }

        observation current = {
            .top = game.highest_point,
            .jid = game.jit % list_length(jet_pattern),
            .rid = game.rit
        };
        for (int i = 0; i < 7; ++i) {
            current.topology[i] = game.bottom_y[i] - game.bottom;
        }

        int index = 0;
        if (!(index = has_observation(current))) {
            history[history_count++] = current;
        }
        else {
            observation prev = history[index];

            u64 distance = current.rid - prev.rid;
            u64 height = current.top - prev.top;
            u64 left_to_drop = (threshold - current.rid);

            u64 cycles = left_to_drop / distance;
            u64 gain_in_height = cycles * height;
            u64 game_rock_it = game.rit + cycles * distance;

            game.tunnel_offset = gain_in_height;
            game.rit = game_rock_it;

            game.highest_point += gain_in_height;
            game.found_repetition = 1;
        }

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

    // assert(3068 == answer);

    return answer;
}

u64 part_two()
{
    u64 answer = 0;

    while (step_simulation(1000000000000));
    answer = game.highest_point;

    assert(answer == 1585673352422); // 1585673355287);

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