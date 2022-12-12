#include <string.h>
#include <ctype.h>

#include "..\utilities.h"

#if 0

#define input_file "sample"

#define map_height 5
#define map_width 8

#else

#define input_file "input"

#define map_height 41
#define map_width 162

#endif

#define cb_maxsize 256

typedef struct position {
    u32 x;
    u32 y;
} position;

typedef struct position_cb {
    u32 head;
    u32 tail;
    u32 count;
    position data[cb_maxsize];
} position_cb;

static byte heightmap[map_height][map_width];
static byte pathmap[map_height][map_width];
static int distancemap[map_height][map_width];
static position came_from[map_height][map_width];

static position start;
static position end;

static position_cb frontier;

void frontier_push(position p)
{
    assert(frontier.count + 1 <= cb_maxsize);
    frontier.data[frontier.head++] = p;
    frontier.head %= cb_maxsize;
    frontier.count++;
}

position frontier_pop()
{
    assert(frontier.count > 0);
    position p = frontier.data[frontier.tail++];
    frontier.tail %= cb_maxsize;
    frontier.count--;
    return p;
}

static int x_offsets[] = { 1, -1, 0, 0 };
static int y_offsets[] = { 0, 0, 1, -1 };
static position* start_positions = null_ptr;


void parse_map()
{
    char** input = aquire_input(input_file);

    assert(list_length(input) == map_height);
    for (int i = 0; i < list_length(input); ++i) {
        assert(list_length(input[i]) == map_width);
        for (int j = 0; j < list_length(input[i]); ++j) {
            heightmap[i][j] = input[i][j];
            if (heightmap[i][j] == 'a')
                list_push(start_positions, (position) { .x = j, .y = i });
            if (heightmap[i][j] == 'S')
                start = (position){ .x = j, .y = i };
            if (heightmap[i][j] == 'E')
                end = (position){ .x = j, .y = i };
        }
    }

    memset(distancemap, -1, sizeof(distancemap));
    memset(pathmap, '.', sizeof(pathmap));
    memset(came_from, -1, sizeof(came_from));
    release_input(input);
}

void print_maps()
{
    FILE* file = null_ptr;
    assert(fopen_s(&file, "distancemap", "w+") == 0);

    for (int i = 0; i < map_height; ++i) {
        for (int j = 0; j < map_width; ++j) {
            int value = distancemap[i][j];
            if (value == -1)
                fprintf(file, "----- ");
            else
                fprintf(file, "%.5d ", value);
        }
        fprintf(file, "\n");
    }
    fclose(file);

    file = null_ptr;
    assert(fopen_s(&file, "pathmap", "w+") == 0);

    pathmap[start.y][start.x] = 'S';
    pathmap[end.y][end.x] = 'E';

    position current = end;

    while (!(current.x == start.x && current.y == start.y)) {
        position from = came_from[current.y][current.x];

        int dx = current.x - from.x;
        int dy = current.y - from.y;

        char m = (dx == 1) ? '>' : (dx == -1) ? '<' : (dy == 1) ? 'v' : '^';

        pathmap[from.y][from.x] = m;
        current = from;
    }

    for (int i = 0; i < map_height; ++i) {
        for (int j = 0; j < map_width; ++j) {
            byte value = pathmap[i][j];
            fprintf(file, "%c", value);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int part_one()
{
    parse_map();

    heightmap[start.y][start.x] = 'a';
    heightmap[end.y][end.x] = 'z';

    distancemap[start.y][start.x] = 0;

    frontier_push(start);

    while (frontier.count > 0) {
        position current = frontier_pop();

        for (int i = 0; i < 4; ++i) {
            int nx = current.x + x_offsets[i];
            int ny = current.y + y_offsets[i];

            if (nx < 0 || nx >= map_width || ny < 0 || ny >= map_height)
                continue;

            byte ch = heightmap[current.y][current.x];
            byte nh = heightmap[ny][nx];

            if (distancemap[ny][nx] == -1 && nh - 1 <= ch) {
                frontier_push((position) { .x = nx, .y = ny });
                came_from[ny][nx] = current;
                distancemap[ny][nx] = distancemap[current.y][current.x] + 1;
            }
        }
    }

    // print_maps();

    int answer = distancemap[end.y][end.x];
    return answer;
}

int part_two(int distance)
{
    int total_positions = list_length(start_positions);
    for (int survey = 0; survey < total_positions; ++survey) {
        memset(distancemap, -1, sizeof(distancemap));

        distancemap[start_positions[survey].y][start_positions[survey].x] = 0;
        frontier_push(start_positions[survey]);

        while (frontier.count > 0) {
            position current = frontier_pop();

            for (int i = 0; i < 4; ++i) {
                int nx = current.x + x_offsets[i];
                int ny = current.y + y_offsets[i];

                if (nx < 0 || nx >= map_width || ny < 0 || ny >= map_height)
                    continue;

                byte ch = heightmap[current.y][current.x];
                byte nh = heightmap[ny][nx];

                if (distancemap[ny][nx] == -1 && nh - 1 <= ch) {
                    frontier_push((position) { .x = nx, .y = ny });
                    distancemap[ny][nx] = distancemap[current.y][current.x] + 1;
                }
            }
        }
        if (distancemap[end.y][end.x] != -1)
            distance = __min(distance, distancemap[end.y][end.x]);
    }

    int answer = distance;
    return answer;
}

void main()
{
    int distance = part_one();
    printf("%d\n", distance);
    printf("%d\n", part_two(distance));
}