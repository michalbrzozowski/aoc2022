#include <intrin.h>
#include <string.h>

#include "..\utilities.h"

#define input_file "input"

#define max_steps 4096

#if 1

#define valley_width 122
#define valley_height 27

#define blizzard_lcm 600
#define blizzard_width 120
#define blizzard_height 25

#else

#define valley_width 8
#define valley_height 6

#define blizzard_lcm 12
#define blizzard_width 6
#define blizzard_height 4

#endif

enum blizzard_directions {
    north   = 1 << 0, // ^
    south   = 1 << 1, // v
    west    = 1 << 2, // <
    east    = 1 << 3  // >
};

typedef struct int2 {
    int x;
    int y;
} int2;

static int2 directions[] = {
    {.x = 0, .y = -1},
    {.x = 0, .y = 1},
    {.x = -1, .y = 0},
    {.x = 1, .y = 0},
    {.x = 0, .y = 0}
};

typedef struct step {
    char x;
    char y;
    short t;
} step;

step start = { .x = 1, .y = 0 };
step end = { .x = valley_width - 2, .y = valley_height - 1 };

static int blizzard_states;
static byte blizzard_state[blizzard_lcm][blizzard_height][blizzard_width];

void parse_input()
{
 char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input); ++i) {
        for (int j = 0; j < list_length(input[i]); ++j) {
            byte c = input[i][j];
            switch (c)
            {
            case '^': {
                assert(i != 0 || j != 0);
                blizzard_state[0][i - 1][j - 1] = north;
            } break;
            case 'v': {
                assert(i != 0 || j != 0);
                blizzard_state[0][i - 1][j - 1] = south;
            } break;
            case '<': {
                assert(i != 0 || j != 0);
                blizzard_state[0][i - 1][j - 1] = west;
            } break;
            case '>': {
                assert(i != 0 || j != 0);
                blizzard_state[0][i - 1][j - 1] = east;
            } break;
            default: {
            } break;
            }
        }
    }
    blizzard_states++;
    release_input(input);
}

#define max_cb (blizzard_height*blizzard_width)

typedef struct cb {
    u32 head;
    u32 tail;
    u32 count;
    step data[max_cb];
} cb;
cb frontier;

void frontier_push(step next)
{
    assert(frontier.count + 1 <= max_cb);
    frontier.data[frontier.head++] = next;
    frontier.head %= max_cb;
    frontier.count++;
}

step frontier_pop()
{
    assert(frontier.count > 0);
    step id = frontier.data[frontier.tail++];
    frontier.tail %= max_cb;
    frontier.count--;
    return id;
}

void frontier_clear()
{
    frontier.head = frontier.tail = frontier.count = 0;
}

int next_step(int p, int border)
{
    if (p < 0) return border - 1;
    else if (p >= border) return 0;
    else return p;
}

void step_blizzard()
{
    if (blizzard_states == blizzard_lcm) return;
    int current = blizzard_states - 1;
    int next = blizzard_states;
    for (int y = 0; y < blizzard_height; ++y) {
    for (int x = 0; x < blizzard_width; ++x) {
        byte state = blizzard_state[current][y][x];
        if (state == 0) continue;

        if (check_bitmask(state, north)) {
            blizzard_state[next][next_step(y - 1, blizzard_height)][x] |= north;
        }
        if (check_bitmask(state, south)) {
            blizzard_state[next][next_step(y + 1, blizzard_height)][x] |= south;
        }
        if (check_bitmask(state, west)) {
            blizzard_state[next][y][next_step(x - 1, blizzard_width)] |= west;
        }
        if (check_bitmask(state, east)) {
            blizzard_state[next][y][next_step(x + 1, blizzard_width)] |= east;
        }
    }
    }
    blizzard_states++;
}

int check_position(step position)
{
    if ((position.x == start.x && position.y == start.y) ||
        (position.x == end.x && position.y == end.y))
        return 1;

    if (position.x > 0 && position.x < valley_width - 1 &&
        position.y > 0 && position.y < valley_height - 1 &&
        !blizzard_state[position.t % blizzard_lcm][position.y - 1][position.x - 1]) {
        return 1;
    }
    return 0;
}

int navigate(step from, step to)
{
    static short visited[max_steps][valley_height][valley_width] = { 0 };
    memset(visited, 0, sizeof(visited));
    frontier_clear();

    frontier_push(from);
    while (frontier.count > 0) {
        step current = frontier_pop();
        if (current.x == to.x && current.y == to.y)
            return current.t;

        if (visited[current.t][current.y][current.x])
            continue;

        visited[current.t][current.y][current.x]++;

        if (current.t + 1 >= blizzard_states) {
            step_blizzard();
        }

        for (int i = 0; i < num_elements(directions); ++i) {
            step next = {
                .x = current.x + directions[i].x,
                .y = current.y + directions[i].y,
                .t = current.t + 1
            };
            int pos_ok = check_position(next);
            int taken = visited[next.t][next.y][next.x];
            if (pos_ok && !taken) {
                frontier_push(next);
            }
        }
    }
    return 0;
}

int part_one()
{
    return navigate(start, end);
}

int part_two(int time)
{
    int back = navigate((step) { .x = end.x, .y = end.y, .t = time }, start);
    int goal = navigate((step) { .x = start.x, .y = start.y, .t = back }, end);
    return goal;
}

void main()
{
    parse_input();
    int time = part_one();
    printf("%d\n", time);
    printf("%d\n", part_two(time));
}