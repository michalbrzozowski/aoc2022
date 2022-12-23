#include "..\utilities.h"

#define input_file "input"

typedef byte byte4[4];

enum {
    ore         = 0,
    clay        = 1,
    obsidian    = 2,
    geode       = 3,
    resource_count
};

#define max_bp 32

typedef struct blueprint {
    byte index;
    byte4 robots[resource_count];
} blueprint;

int blueprint_count;
blueprint blueprints[max_bp];

typedef struct state {
    byte4 robots;
    byte4 resources;
} state;

void parse_input()
{
    char* format = "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.";
    char** input = aquire_input(input_file);

    for (int i = 0; i < list_length(input); ++i) {
        int index, or , cr, oro, orc, gro, grob;
        int retval= sscanf_s(input[i], format, &index, &or , &cr, &oro, &orc, &gro, &grob);
        blueprint bp = {
            .index = (byte)index,
            .robots[ore][ore] = (byte) or ,
            .robots[clay][ore] = (byte)cr,
            .robots[obsidian][ore] = (byte)oro,
            .robots[obsidian][clay] = (byte)orc,
            .robots[geode][ore] = (byte)gro,
            .robots[geode][obsidian] = (byte)grob
        };

        blueprints[blueprint_count++] = bp;
    }
    release_input(input);
}

int calculate_buildtime(blueprint bp, byte robot, byte resource, state current)
{
    int cost = bp.robots[robot][resource];
    int resources = current.resources[resource];

    if (resources >= cost) {
        return 1;
    }
    int robots = current.robots[resource];
    int time = (cost - resources + robots - 1) / robots;
    return  1 + time;
}

state calculate_next_state(blueprint bp, state current, int ticks, byte type)
{
    state next = current;
    for (int i = 0; i < 4; ++i) {
        next.resources[i] += ticks * next.robots[i];
        next.resources[i] -= bp.robots[type][i];
    }
    next.robots[type]++;
    return next;
}

static int cnt;

void print_state(state current)
{
    if (cnt++ > 1000) return;
    int r0, r1, r2, r3, r4, r5, r6, r7;
    r0 = current.resources[ore];
    r1 = current.resources[clay];
    r2 = current.resources[obsidian];
    r3 = current.resources[geode];

    r4 = current.robots[ore];
    r5 = current.robots[clay];
    r6 = current.robots[obsidian];
    r7 = current.robots[geode];

    printf("%d, %d, %d, %d, %d, %d, %d, %d\n", r0, r1, r2, r3, r4, r5, r6, r7);
}

int explore(blueprint bp, int time, state current)
{
    int max_yield = current.resources[geode] + time * current.robots[geode];

    if (current.robots[ore] < bp.robots[clay][ore] ||
        current.robots[ore] < bp.robots[obsidian][ore] ||
        current.robots[ore] < bp.robots[geode][ore]) {
        int dt = calculate_buildtime(bp, ore, ore, current);
        if (time > dt) {
            state next = calculate_next_state(bp, current, dt, ore);

            int eval = explore(bp, time - dt, next);
            max_yield = __max(max_yield, eval);
        }
    }

    if (current.robots[clay] < bp.robots[obsidian][clay]) {
        int dt = calculate_buildtime(bp, clay, ore, current);
        if (time > dt) {
            state next = calculate_next_state(bp, current, dt, clay);

            int eval = explore(bp, time - dt, next);
            max_yield = __max(max_yield, eval);
        }
    }

    if (current.robots[clay] && current.robots[obsidian] < bp.robots[geode][obsidian]) {
        int dt1 = calculate_buildtime(bp, obsidian, ore, current);
        int dt2 = calculate_buildtime(bp, obsidian, clay, current);
        int dt = __max(dt1, dt2);

        if (time > dt) {
            state next = calculate_next_state(bp, current, dt, obsidian);

            int eval = explore(bp, time - dt, next);
            max_yield = __max(max_yield, eval);
        }
    }

    if (current.robots[obsidian]) {
        int dt1 = calculate_buildtime(bp, geode, ore, current);
        int dt2 = calculate_buildtime(bp, geode, obsidian, current);
        int dt = __max(dt1, dt2);

        if (time > dt) {
            state next = calculate_next_state(bp, current, dt, geode);

            int eval = explore(bp, time - dt, next);
            max_yield = __max(max_yield, eval);
        }
    }

    return max_yield;
}

int part_one()
{
    parse_input();
    int answer = 0;

    for (int i = 0; i < blueprint_count; ++i) {
        int number = explore(blueprints[i], 24, (state){.robots[ore] = 1 });
        answer += (i + 1) * number;
    }

    return answer;
}

int part_two()
{
    int answer = 1;
    for (int i = 0; i < 3; ++i) {
        int number = explore(blueprints[i], 32, (state){.robots[ore] = 1 });
        answer *= number;
    }
    return answer;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}