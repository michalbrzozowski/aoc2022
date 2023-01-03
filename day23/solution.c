#include <string.h>
#include "..\utilities.h"

#define input_file "input"

typedef struct int2 {
    int x;
    int y;
} int2;

typedef struct byte2 {
    byte x;
    byte y;
} byte2;

static int2 nw;
static int2 se;

#define map_size 256
static byte map[map_size][map_size];
static byte2 moves[map_size][map_size];

typedef struct position {
    union {
        u32 xy;
        struct {
            u16 x;
            u16 y;
        };
    };
} position;

typedef struct elf {
    int2 position;
} elf;

int max_proposed;
position proposed_moves[4096];

void parse_input()
{
    char** input = aquire_input(input_file);
    int w = (int)list_length(input[0]);
    int h = (int)list_length(input);

    int xo = (map_size / 2) - (w / 2);
    int yo = (map_size / 2) - (h / 2);

    nw = (int2){ .x = xo, .y = yo };
    se = (int2){ .x = xo + w, .y = yo + h };

    int elfs = 0;

    printf("[0, 0]\nnw\nv");
    for (int i = 0; i < list_length(input); ++i) {
        printf("\n");
        for (int j = 0; j < list_length(input[i]); ++j) {
            if (input[i][j] == '.') {
                printf(".");
            }
            else {
                printf("#");
                map[yo + i][xo + j] = '#';
                elfs++;
            }
        }
    }
    printf("\n");
    for (int i = 0; i < w - 1; ++i) {
        printf(" ");
    }
    printf("^\n");
    for (int i = 0; i < w - 1; ++i) {
        printf(" ");
    }
    printf("se\n");
    for (int i = 0; i < w - 5; ++i) {
        printf(" ");
    }
    printf("[%d, %d]\n", w, h);
    printf("a total of %d elfs\n", elfs);

    release_input(input);
}

void print_map()
{
    for (int y = 0; y < map_size; ++y) {
        for (int x = 0; x < map_size; ++x) {
            printf("%c", map[y][x] == '#' ? '#':'.');
        }
        printf("\n");
    }
}

enum { north, south, west, east };

static int2 directions[] = {
    {.x = 0, .y = -1},
    {.x = 0, .y = 1},
    {.x = -1, .y = 0},
    {.x = 1, .y = 0}
};

int part_one()
{
    int answer = 0;
    int xs = nw.x;
    int xe = se.x;
    int ys = nw.y;
    int ye = se.y;

    int max_rounds = 10;
    for (int round = 0; round < max_rounds; ++round) {
        memset(moves, 0, sizeof(moves));
        xs = nw.x;
        xe = se.x;
        ys = nw.y;
        ye = se.y;

        for (int y = ys; y <= ye; ++y) {
        for (int x = xs; x <= xe; ++x) {
            if (!map[y][x]) continue;
            // if no adjecent elves - do nothing
            if (!map[y - 1][x - 1] &&
                !map[y - 1][x] &&
                !map[y - 1][x + 1] &&
                !map[y][x - 1] &&
                !map[y][x + 1] &&
                !map[y + 1][x - 1] &&
                !map[y + 1][x] &&
                !map[y + 1][x + 1])
                continue;
            // else check adjecent squares 8 positions
            // and propose a tile move

            // n, s, w, e
            int moved = 0;
            for (int i = 0; i < 4; ++i) {
                switch ((round + i) % 4)
                {
                case 0: {
                    if (!map[y - 1][x - 1] &&
                        !map[y - 1][x] &&
                        !map[y - 1][x + 1]) {
                        moves[y - 1][x].x++;
                        moves[y - 1][x].y = south;

                        nw.y = __min(nw.y, y - 1);
                        moved = 1;
                    }
                } break;
                case 1: {
                    if (!map[y + 1][x - 1] &&
                        !map[y + 1][x] &&
                        !map[y + 1][x + 1]) {
                        moves[y + 1][x].x++;
                        moves[y + 1][x].y = north;

                        se.y = __max(se.y, y + 1);
                        moved = 1;
                    }
                } break;
                case 2: {
                    if (!map[y - 1][x - 1] &&
                        !map[y][x - 1] &&
                        !map[y + 1][x - 1]) {
                        moves[y][x - 1].x++;
                        moves[y][x - 1].y = east;

                        nw.x = __min(nw.x, x - 1);
                        moved = 1;
                    }
                } break;
                case 3: {
                    if (!map[y - 1][x + 1] &&
                        !map[y][x + 1] &&
                        !map[y + 1][x + 1]) {
                        moves[y][x + 1].x++;
                        moves[y][x + 1].y = west;

                        se.x = __max(se.x, x + 1);
                        moved = 1;
                    }
                } break;
                }
                if (moved) break;
            }
        }
        }

        xs = nw.x;
        xe = se.x;
        ys = nw.y;
        ye = se.y;

        // if alone on proposed tile. move otherwise ignore
        for (int y = ys; y <= ye; ++y) {
        for (int x = xs; x <= xe; ++x) {
            if (moves[y][x].x == 1) {
                int direction = moves[y][x].y;

                int dx = x + directions[direction].x;
                int dy = y + directions[direction].y;

                byte previous = map[dy][dx];
                assert(previous);
                map[dy][dx] = 0;
                map[y][x] = previous;
            }
        }
        }

        // print_map();
    }

    xs = nw.x;
    xe = se.x;
    ys = nw.y;
    ye = se.y;
    for (int y = ys; y <= ye; ++y) {
    for (int x = xs; x <= xe; ++x) {
        answer += map[y][x] != '#';
    }
    }
    printf("\nw:%d h:%d\n", abs(xe - xs), abs(ye - ys));

    return answer;
}

int part_two()
{
    int answer = 0;
    int xs = nw.x;
    int xe = se.x;
    int ys = nw.y;
    int ye = se.y;

    int round = 0;
    int moved = 1;
    while(moved) {
        memset(moves, 0, sizeof(moves));
        moved = 0;
        xs = nw.x;
        xe = se.x;
        ys = nw.y;
        ye = se.y;

        for (int y = ys; y <= ye; ++y) {
        for (int x = xs; x <= xe; ++x) {
            if (!map[y][x]) continue;
            // if no adjecent elves - do nothing
            if (!map[y - 1][x - 1] &&
                !map[y - 1][x] &&
                !map[y - 1][x + 1] &&
                !map[y][x - 1] &&
                !map[y][x + 1] &&
                !map[y + 1][x - 1] &&
                !map[y + 1][x] &&
                !map[y + 1][x + 1])
                continue;
            // else check adjecent squares 8 positions
            // and propose a tile move

            // n, s, w, e
            int moved = 0;
            for (int i = 0; i < 4; ++i) {
                switch ((round + i) % 4)
                {
                case 0: {
                    if (!map[y - 1][x - 1] &&
                        !map[y - 1][x] &&
                        !map[y - 1][x + 1]) {
                        moves[y - 1][x].x++;
                        moves[y - 1][x].y = south;

                        nw.y = __min(nw.y, y - 1);
                        moved = 1;
                    }
                } break;
                case 1: {
                    if (!map[y + 1][x - 1] &&
                        !map[y + 1][x] &&
                        !map[y + 1][x + 1]) {
                        moves[y + 1][x].x++;
                        moves[y + 1][x].y = north;

                        se.y = __max(se.y, y + 1);
                        moved = 1;
                    }
                } break;
                case 2: {
                    if (!map[y - 1][x - 1] &&
                        !map[y][x - 1] &&
                        !map[y + 1][x - 1]) {
                        moves[y][x - 1].x++;
                        moves[y][x - 1].y = east;

                        nw.x = __min(nw.x, x - 1);
                        moved = 1;
                    }
                } break;
                case 3: {
                    if (!map[y - 1][x + 1] &&
                        !map[y][x + 1] &&
                        !map[y + 1][x + 1]) {
                        moves[y][x + 1].x++;
                        moves[y][x + 1].y = west;

                        se.x = __max(se.x, x + 1);
                        moved = 1;
                    }
                } break;
                }
                if (moved) break;
            }
        }
        }

        xs = nw.x;
        xe = se.x;
        ys = nw.y;
        ye = se.y;

        // if alone on proposed tile. move otherwise ignore
        for (int y = ys; y <= ye; ++y) {
        for (int x = xs; x <= xe; ++x) {
            if (moves[y][x].x == 1) {
                int direction = moves[y][x].y;

                int dx = x + directions[direction].x;
                int dy = y + directions[direction].y;

                byte previous = map[dy][dx];
                assert(previous);
                map[dy][dx] = 0;
                map[y][x] = previous;
                moved++;
            }
        }
        }
        // print_map();
        round++;
    }

    return round;
}

void main()
{
    parse_input();
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}