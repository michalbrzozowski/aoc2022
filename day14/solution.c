#include <string.h>
#include "..\utilities.h"

#define input_file "input"

typedef struct int2 {
    int x;
    int y;
} int2;

#define map_height 600
#define map_width 2000

static byte map[map_height][map_width];
static int2 sand_marker = { .x = 500, .y = 0 };
static int2 coord_min, coord_max;

int find_first_of(char* str, char it)
{
    for (int i = 0; *str != '\0'; ++i) {
        if (*str++ == it) return i;
    }
    return -1;
}

void add_lines(int2* lines, int count)
{
    for (int i = 0; i+1  < count; ++i) {
        int2 p1 = lines[i];
        int2 p2 = lines[i + 1];

        if (p1.x == p2.x) {
            for (int y = __min(p1.y, p2.y); y <= __max(p1.y, p2.y); ++y) {
                map[y][p1.x] = '#';
            }
        }
        else {
            for (int x = __min(p1.x, p2.x); x <= __max(p1.x, p2.x); ++x) {
                map[p1.y][x] = '#';
            }
        }
        coord_max.x = __max(coord_max.x, p1.x);
        coord_max.y = __max(coord_max.y, p1.y);
        coord_min.x = __min(coord_min.x, p1.x);
    }
    coord_max.x = __max(coord_max.x, lines[count - 1].x);
    coord_max.y = __max(coord_max.y, lines[count - 1].y);
    coord_min.x = __min(coord_min.x, lines[count - 1].x);
}

void parse_map(int x_offset)
{
    char** input = aquire_input(input_file);
    memset(map, '.', sizeof(map));
    coord_min.x = map_width;

    int2 lines[64] = {0};
    for (int i = 0; i < list_length(input); ++i) {
        char* line = input[i];
        int lit = 0;

        sscanf_s(line, "%d,%d", &lines[lit].x, &lines[lit].y);
        lines[lit].x += x_offset;
        lit++;
        int next = find_first_of(line, '>');

        while (next != -1) {
            line += next + 2;
            sscanf_s(line, "%d,%d", &lines[lit].x, &lines[lit].y);
            lines[lit].x += x_offset;
            next = find_first_of(line, '>');
            lit++;
        }
        add_lines(lines, lit);
    }
    // sand marker
    map[sand_marker.y][sand_marker.x] = '+';
    release_input(input);
}

void print_map(int2 start, int2 end)
{
    for (int y = start.y; y <= end.y; ++y) {
        for (int x = start.x; x <= end.x; ++x) {
            printf("%c", map[y][x]);
        }
        printf("\n");
    }
}

int2 update_simulation(int2 grain)
{
    if (map[grain.y + 1][grain.x] == '.') {
        return (int2) { .x = grain.x, .y = grain.y + 1 };
    }
    else if (map[grain.y + 1][grain.x - 1] == '.') {
        return (int2) { .x = grain.x - 1, .y = grain.y + 1 };
    }
    else if (map[grain.y + 1][grain.x + 1] == '.') {
        return (int2) { .x = grain.x + 1, .y = grain.y + 1 };
    }
    return grain;
}

int part_one()
{
    parse_map(0);

    int grains = 0;
    int2 grain = sand_marker;

    while (1) {
        int2 updated = update_simulation(grain);
        if (updated.x == grain.x && updated.y == grain.y) {
            // we are at rest
            map[grain.y][grain.x] = 'o';
            grain = sand_marker;
            grains++;
            // print_map(coord_min, coord_max);
        }
        else if (updated.y > coord_max.y) {
            // we are passed the bottom?
            break;
        }
        else
            grain = updated;
    }
    // print_map(coord_min, coord_max);
    return grains;
}

int part_two()
{
    int x_offset = 500;
    sand_marker.x += x_offset;
    parse_map(x_offset);

    int2 old_max = coord_max;
    int2 old_min = coord_min;

    int2 lines[] = { {.x = 0, .y = coord_max.y + 2 }, {.x = map_width, .y = coord_max.y + 2} };
    add_lines(lines, 2);

    int grains = 0;
    int2 grain = sand_marker;

    old_min.x -= 10;
    old_max.x += 10;
    old_max.y += 2;

    // print_map(old_min, old_max);

    while (1) {
        int2 updated = update_simulation(grain);
        if (updated.x == grain.x && updated.y == grain.y) {
            // we are at rest
            map[grain.y][grain.x] = 'o';
            grain = sand_marker;
            grains++;
            // print_map(old_min, old_max);
            if (updated.y == sand_marker.y && updated.x == sand_marker.x)
                break;
        }
        else
            grain = updated;
    }
    // print_map(old_min, old_max);
    return grains;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}