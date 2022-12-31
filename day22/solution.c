#include <ctype.h>
#include <string.h>
#include "..\utilities.h"

#define input_file "input"

#define map_width 150
#define map_height 200

static byte map[map_height][map_width];
static int *path;

void parse_input()
{
    memset(map, ' ', sizeof(map));
    char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input) - 2; ++i) {
        for (int j = 0; j < list_length(input[i]); ++j) {
            map[i][j] = input[i][j];
        }
    }

    char* description = input[list_length(input) - 1];
    for (int i = 0; i < list_length(description); ++i) {
        if (isdigit(description[i])) {
            int val = 0;
            while (isdigit(description[i])) {
                val *= 10;
                val += description[i++] - '0';
            }
            list_push(path, val);
        }
        if (description[i] != '\0')
            list_push(path, description[i] == 'R' ? -3 : -5);
    }

    release_input(input);
}

typedef struct int2 {
    int x;
    int y;
} int2;

static int2 directions[] = {
    {.x = 1, .y = 0},
    {.x = 0, .y = 1},
    {.x = -1, .y = 0},
    {.x = 0, .y = -1}
};
enum { right, down, left, up };

int mod(int x, int n)
{
    return (x % n + n) % n;
}

int part_one()
{
    int r = 0;
    int c = 0;

    while (map[r][c] == ' ') c++;

    /*
            0, -1
    -1, 0     +     1, 0
            0, 1
    */
    int forward = 0;
        
    for (int i = 0; i < list_length(path); ++i) {
        int next = path[i];

        if (next < 0) {
            forward = mod(forward + next, 4);
        }
        else {
            for (int step = 0; step < next; ++step) {
                int dx = (c + directions[forward].x) % map_width;
                int dy = (r + directions[forward].y) % map_height;

                if (forward == right && (dx >= map_width || map[dy][dx] == ' ')) {
                    for (dx = 0; map[dy][dx] == ' '; ++dx);
                }
                else if (forward == left && (dx < 0 || map[dy][dx] == ' ')) {
                    for (dx = map_width -1; map[dy][dx] == ' '; --dx);
                }
                else if (forward == down && (dy >= map_height || map[dy][dx] == ' ')) {
                    for (dy = 0; map[dy][dx] == ' '; ++dy);
                }
                else if (forward == up && (dy < 0 || map[dy][dx] == ' ')) {
                    for (dy = map_height -1; map[dy][dx] == ' '; --dy);
                }

                if (map[dy][dx] == '#') {
                    break;
                }
                r = dy;
                c = dx;
            }
        }
    }

    int answer = 0;
    answer += 1000 * (r + 1);
    answer += 4 * (c + 1);
    answer += forward;
    return answer;
}

typedef struct int3 {
    int x;
    int y;
    int z;
} int3;

#define wrap_table_length 14
static int3 wrap_table[][2] = {
    {{.y = 0, .x = 1, .z = up}, {.y = 3, .x = 0 , .z = right}},
    {{.y = 2, .x = 0, .z = up}, {.y = 1, .x = 1 , .z = right}},
    {{.y = 0, .x = 2, .z = up}, {.y = 3, .x = 0 , .z = up}},

    {{.y = 0, .x = 2, .z = down}, {.y = 1, .x = 1 , .z = left}},
    {{.y = 2, .x = 1, .z = down}, {.y = 3, .x = 0 , .z = left}},
    {{.y = 3, .x = 0, .z = down}, {.y = 0, .x = 2 , .z = down}},

    {{.y = 0, .x = 2, .z = right}, {.y = 2, .x = 1 , .z = left}},
    {{.y = 2, .x = 1, .z = right}, {.y = 0, .x = 2 , .z = left}},
    {{.y = 1, .x = 1, .z = right}, {.y = 0, .x = 2 , .z = up}},
    {{.y = 3, .x = 0, .z = right}, {.y = 2, .x = 1 , .z = up}},

    {{.y = 0, .x = 1, .z = left}, {.y = 2, .x = 0 , .z = right}},
    {{.y = 2, .x = 0, .z = left}, {.y = 0, .x = 1 , .z = right}},
    {{.y = 1, .x = 1, .z = left}, {.y = 2, .x = 0 , .z = down}},
    {{.y = 3, .x = 0, .z = left}, {.y = 0, .x = 1 , .z = down}}
};

#define square_size 50

int3 wrap(int x, int y, int direction)
{
    int3 next = { 0 };
    int ox = x / square_size;
    int oy = y / square_size;

    int wi = 0;
    for (; wi < wrap_table_length; ++wi) {
        if (ox == wrap_table[wi][0].x &&
            oy == wrap_table[wi][0].y &&
            direction == wrap_table[wi][0].z)
            break;
    }
    assert(wi != wrap_table_length);

    if (direction == up && wrap_table[wi][1].z == right) {
        next.x = wrap_table[wi][1].x * square_size;
        next.y = wrap_table[wi][1].y * square_size + (x % square_size);
    }
    else if (direction == up && wrap_table[wi][1].z == up) {
        next.x = wrap_table[wi][1].x * square_size + (x % square_size);
        next.y = map_height - 1;
    }
    else if (direction == down && wrap_table[wi][1].z == left) {
        next.x = wrap_table[wi][1].x * square_size + square_size - 1;
        next.y = wrap_table[wi][1].y * square_size + (x % square_size);
    }
    else if (direction == down && wrap_table[wi][1].z == down) {
        next.x = wrap_table[wi][1].x * square_size + (x % square_size);
        next.y = 0;
    }
    else if (direction == right && wrap_table[wi][1].z == left) {
        next.x = wrap_table[wi][1].x * square_size + square_size - 1;
        next.y = (wrap_table[wi][1].y * square_size + square_size - 1) - (y % square_size);
    }
    else if (direction == right && wrap_table[wi][1].z == up) {
        next.x = wrap_table[wi][1].x * square_size + (y % square_size);
        next.y = wrap_table[wi][1].y * square_size + square_size - 1;
    }
    else if (direction == left && wrap_table[wi][1].z == right) {
        next.x = wrap_table[wi][1].x * square_size;
        next.y = (wrap_table[wi][1].y * square_size + (square_size - 1)) - (y % square_size);
    }
    else if (direction == left && wrap_table[wi][1].z == down) {
        next.x = wrap_table[wi][1].x * square_size + (y % square_size);
        next.y = wrap_table[wi][1].y * square_size;
    }
    next.z = wrap_table[wi][1].z;

    return next;
}

int part_two()
{
    int r = 0;
    int c = 0;

    while (map[r][c] == ' ') c++;

    int forward = 0;
    for (int i = 0; i < list_length(path); ++i) {
        int cmd = path[i];

        if (cmd < 0) {
            forward = mod(forward + cmd, 4);
            continue;
        }
        for (int step = 0; step < cmd; ++step) {
            int dx = (c + directions[forward].x);
            int dy = (r + directions[forward].y);
            int df = forward;

            if (forward == right && (dx >= map_width || map[dy][dx] == ' ')) {
                int3 next = wrap(c, r, forward);
                dx = next.x;
                dy = next.y;
                df = next.z;

            }
            else if (forward == left && (dx < 0 || map[dy][dx] == ' ')) {
                int3 next = wrap(c, r, forward);
                dx = next.x;
                dy = next.y;
                df = next.z;
            }
            else if (forward == down && (dy >= map_height || map[dy][dx] == ' ')) {
                int3 next = wrap(c, r, forward);
                dx = next.x;
                dy = next.y;
                df = next.z;
            }
            else if (forward == up && (dy < 0 || map[dy][dx] == ' ')) {
                int3 next = wrap(c, r, forward);
                dx = next.x;
                dy = next.y;
                df = next.z;
            }

            if (map[dy][dx] == '#') {
                break;
            }

            if (map[dy][dx] == ' ') assert(0);

            r = dy;
            c = dx;
            forward = df;
        }
    }

    int answer = 0;
    answer += 1000 * (r + 1);
    answer += 4 * (c + 1);
    answer += forward;
    return answer;
}

void main()
{
    parse_input();
    printf("%d\n", part_one());
    printf("%d\n", part_two());

    list_free(path);
}