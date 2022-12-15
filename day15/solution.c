#include "..\utilities.h"

#define input_file "input"

typedef struct int2 {
    int x;
    int y;
} int2;

typedef struct sensor_data {
    int2 sensor;
    int2 beacon;
    int distance;
} sensor_data;

static sensor_data* sensors;
static int2 coord_min, coord_max;

int manhattan_distance(int2 a, int2 b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int is_position_invalid(sensor_data *search_space, int2 position)
{
    int valid = 0;
    for (int i = 0; i < list_length(search_space); ++i) {
        if (search_space[i].beacon.x == position.x && search_space[i].beacon.y == position.y)
             return 0;
        if (manhattan_distance(position, search_space[i].sensor) <= search_space[i].distance)
            valid++;
    }
    return valid;
}

void parse_input()
{
    char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input); ++i) {
        int2 sensor, beacon;
        sscanf_s(input[i], "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &sensor.x, &sensor.y, &beacon.x, &beacon.y);
        int distance = manhattan_distance(sensor, beacon);
        list_push(sensors, (sensor_data) { .sensor = sensor, .beacon = beacon, .distance = distance });

        coord_max.x = __max(coord_max.x, sensor.x + distance);
        coord_max.y = __max(coord_max.y, sensor.y + distance);
        coord_min.x = __min(coord_min.x, sensor.x - distance);
        coord_min.y = __min(coord_min.y, sensor.y - distance);
    }
    release_input(input);
}

int part_one()
{
    int count = 0;

    int y = 2000000;
    sensor_data* to_search = null_ptr;

    for (int i = 0; i < list_length(sensors); ++i) {
        int length = abs(sensors[i].sensor.y - y);
        if (length <= sensors[i].distance)
            list_push(to_search, sensors[i]);
    }

    for (int i = coord_min.x; i <= coord_max.x; ++i) {
        count += is_position_invalid(to_search, (int2) { .x = i, .y = y }) != 0;
    }

    list_free(to_search);
    return count;
}

u64 part_two()
{
    int maxval = 4000000;
    for (int i = 0; i < list_length(sensors); ++i) {
        int2 origin = sensors[i].sensor;
        int distance = sensors[i].distance + 1;

        int yd = 0;
        for (int x = origin.x - distance; x <= origin.x + distance; ++x) {
            int y1 = origin.y + yd;
            int y2 = origin.y - yd;

            if (x >= 0 && x <= maxval && y1 >= 0 && y1 <= maxval && y2 >= 0 && y2 <= maxval) {
                if (!is_position_invalid(sensors, (int2) { .x = x, .y = y1 })) {
                    return ((u64)x * 4000000) + y1;
                }
                if (y1 != y2) {
                    if (!is_position_invalid(sensors, (int2) { .x = x, .y = y2 })) {
                        return ((u64)x * 4000000) + y2;
                    }
                }
            }
            yd = x >= origin.x ? --yd : ++yd;
        }
    }
    return 0;
}

void main()
{
    parse_input();
    printf("x min: %d, x max: %d\ny min: %d, y max: %d\n", coord_min.x, coord_max.x, coord_min.y, coord_max.y);

    printf("%d\n", part_one());
    printf("%"PRIu64"\n", part_two());
    list_free(sensors);
}
