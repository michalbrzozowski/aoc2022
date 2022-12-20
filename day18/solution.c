#include "..\utilities.h"

#define input_file "input"

enum { air, lava, water };

typedef struct int3 {
    int x;
    int y;
    int z;
} int3;

int3 offsets[] = {
    {.x = 0, .y = 0, .z = 1},
    {.x = 0, .y = 0, .z = -1},
    {.x = 1, .y = 0, .z = 0},
    {.x = -1, .y = 0, .z = 0},
    {.x = 0, .y = 1, .z = 0},
    {.x = 0, .y = -1, .z = 0},
};

#define grid_size 23
#define position_offsets 6
static byte grid[grid_size][grid_size][grid_size];

int out_of_bounds(int n)
{
    return n < 0 || n >= grid_size;
}

int out_of_bounds3(int3 position)
{
    return out_of_bounds(position.x) || out_of_bounds(position.y) || out_of_bounds(position.z);
}

void parse_input()
{
    char** input = aquire_input(input_file); 
    for (int i = 0; i < list_length(input); ++i) {
        int x, y, z;
        sscanf_s(input[i], "%d,%d,%d", &x, &y, &z);
        grid[x][y][z] = lava;
    }
    release_input(input);
}

int part_one()
{
    int answer = 0;
    parse_input();

    for (int x = 0; x < grid_size; ++x) {
    for (int y = 0; y < grid_size; ++y) {
    for (int z = 0; z < grid_size; ++z) {
        if (grid[x][y][z]) {
        for (int o = 0; o < position_offsets; ++o) {
            int3 dp = {
                .x = x + offsets[o].x,
                .y = y + offsets[o].y,
                .z = z + offsets[o].z
            };
            int is_oob = out_of_bounds3(dp);
            int is_air = grid[dp.x][dp.y][dp.z] == air;
            if (is_oob || is_air)
            {
                answer++;
            }
        }
        }
    }
    }
    }
    return answer;
}

#define cb_maxsize 1024
typedef struct exploration_cb {
    int head;
    int tail;
    int count;
    int max_count;
    int3 to_explore[cb_maxsize];
} exploration_cb;

exploration_cb frontier;

void frontier_push(int3 p)
{
    assert(frontier.count + 1 <= cb_maxsize);
    frontier.to_explore[frontier.head++] = p;
    frontier.head %= cb_maxsize;
    frontier.count++;
    frontier.max_count = __max(frontier.max_count, frontier.count);
}

int3 frontier_pop()
{
    assert(frontier.count > 0);
    int3 p = frontier.to_explore[frontier.tail++];
    frontier.tail %= cb_maxsize;
    frontier.count--;
    return p;
}

void flood()
{
    frontier_push((int3){ .x = 0, .y = 0, .z = 0 });
    grid[0][0][0] = water;

    while (frontier.count > 0) {
        int3 current = frontier_pop();
        int x = current.x;
        int y = current.y;
        int z = current.z;

        for (int o = 0; o < position_offsets; ++o) {
            int3 next = {
                .x = x + offsets[o].x,
                .y = y + offsets[o].y,
                .z = z + offsets[o].z
            };

            if (!out_of_bounds3(next) &&
                !grid[next.x][next.y][next.z]) {
                grid[next.x][next.y][next.z] = 2;
                frontier_push(next);
            }
        }
    }
}

int part_two()
{
    int answer = 0;
    flood();

    for (int x = 0; x < grid_size; ++x) {
    for (int y = 0; y < grid_size; ++y) {
    for (int z = 0; z < grid_size; ++z) {
        if (grid[x][y][z]) {
        for (int o = 0; o < position_offsets; ++o) {
            int3 dp = {
                .x = x + offsets[o].x,
                .y = y + offsets[o].y,
                .z = z + offsets[o].z
            };
            int is_oob = out_of_bounds3(dp);
            int is_air = grid[dp.x][dp.y][dp.z] == air;
            if (!is_oob && is_air)
            {
                answer++;
            }
        }
        }
    }
    }
    }
    return answer;
}

void main()
{
    int surface_area = part_one();
    printf("%d\n", surface_area);
    printf("%d\n", surface_area - part_two());
}