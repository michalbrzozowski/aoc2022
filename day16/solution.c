#include <string.h>
#include <intrin.h>

#include "..\utilities.h"

#define max_nodes 64
#define input_file "input"

typedef struct graph {
    int adjecency_list[max_nodes][max_nodes];
    int alc[max_nodes];
    int flow_rate[max_nodes];
    int node_lookup[max_nodes];
    int nodes;
    int distances[max_nodes][max_nodes];
} graph;

static graph pipe_graph;

int lookup_or_add(byte node[2])
{
    u16* val = (u16*)node;
    int i = 0;
    for (; i < pipe_graph.nodes; ++i) {
        if (pipe_graph.node_lookup[i] == *val)
            return i;
    }
    if (i < max_nodes) {
        pipe_graph.node_lookup[pipe_graph.nodes++] = *val;
        return i;
    }
    return -1;
}

int find_first_of(char* str, char it)
{
    for (int i = 0; *str != '\0'; ++i) {
        if (*str++ == it) return i;
    }
    return -1;
}

int add_node(char* node, int flow_rate)
{
    int id = lookup_or_add(node);
    assert(id != -1);
    assert(flow_rate <= UINT8_MAX);

    pipe_graph.flow_rate[id] = (byte)flow_rate;
    return id;
}

void add_edge(int id, char* node)
{
    int nid = lookup_or_add(node);
    assert(nid != -1);
    pipe_graph.adjecency_list[id][pipe_graph.alc[id]++] = nid;
}

void print_graph()
{
    for (int i = 0; i < pipe_graph.nodes; ++i) {
        int id = pipe_graph.node_lookup[i];
        byte name[3] = {0};
        name[0] = id & 0xFF;
        name[1] = (id >> 8) & 0xFF;
        printf("valve %s with flow rate = %.2d has edge(s): ", name, pipe_graph.flow_rate[i]);
        for (int j = 0; j < pipe_graph.alc[i]; ++j) {
            int eid = pipe_graph.adjecency_list[i][j];
            int nid = pipe_graph.node_lookup[eid];

            name[0] = nid & 0xFF;
            name[1] = (nid >> 8) & 0xFF;
            printf("%s ", name);
        }
        printf("\n");
    }
}

void parse_input()
{
    char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input); ++i) {
        char valve[3] = { 0 };
        int flow_rate = 0;
        sscanf_s(input[i], "Valve %s has flow rate=%d", valve, 3, &flow_rate);

        int nid = add_node(valve, flow_rate);
        char* line = input[i];
        int next = find_first_of(line, ',');

        while (next != -1) {
            valve[0] = line[next - 2];
            valve[1] = line[next - 1];
            add_edge(nid, valve);
            line += next + 1;
            next = find_first_of(line, ',');
        }
        int end = (int)list_length(input[i]);

        valve[0] = input[i][end - 2];
        valve[1] = input[i][end - 1];
        add_edge(nid, valve);
    }
    // print_graph();
    release_input(input);
    memset(pipe_graph.distances, -1, sizeof(pipe_graph.distances));
}

typedef struct index_cb {
    u32 head;
    u32 tail;
    u32 count;
    u32 data[max_nodes];
} index_cb;

index_cb frontier;

void frontier_push(u32 id)
{
    assert(frontier.count + 1 <= max_nodes);
    frontier.data[frontier.head++] = id;
    frontier.head %= max_nodes;
    frontier.count++;
}

u32 frontier_pop()
{
    assert(frontier.count > 0);
    u32 id = frontier.data[frontier.tail++];
    frontier.tail %= max_nodes;
    frontier.count--;
    return id;
}

void calculate_distances(int from)
{
    u32 visited[max_nodes] = { 0 };
    visited[from] = 1;
    frontier_push(from);

    pipe_graph.distances[from][from] = 0;

    while (frontier.count > 0) {
        u32 current = frontier_pop();
        for (int i = 0; i < pipe_graph.alc[current]; ++i) {
            u32 next = pipe_graph.adjecency_list[current][i];
            if (!visited[next]++) {
                pipe_graph.distances[from][next] = pipe_graph.distances[from][current] + 1;
                frontier_push(next);
            }
        }
    }
}

int find_max_path(int current, int time, u64 valve_list, int elephant)
{
    clear_bit(valve_list, current);

    if (time <= 1) return 0;

    int pressure = 0;
    if (elephant >= 0) {
        pressure = find_max_path(elephant , 26, valve_list, -1);
    }

    for (u64 mask = valve_list; mask != 0; mask &= mask - 1) {
        u32 id = 0;
        _BitScanForward64(&id, mask);

        if (pipe_graph.distances[current][id] < time) {
            int time_left = time - pipe_graph.distances[current][id] - 1;
            pressure = __max(pressure, time_left * pipe_graph.flow_rate[id] + find_max_path(id, time_left, valve_list, elephant));
        }
    }
    return pressure;
}

int part_one(u64 valve_list)
{
    int answer = find_max_path(lookup_or_add("AA"), 30, valve_list, -1);
    return answer;
}

int part_two(u64 valve_list)
{
    int start = lookup_or_add("AA");
    int answer = find_max_path(start, 26, valve_list, start);

    return answer;
}

void main()
{
    parse_input();

    int count = 0;
    u64 valve_list = 0;
    for (int i = 0; i < pipe_graph.nodes; ++i) {
        calculate_distances(i);
        if (pipe_graph.flow_rate[i]) {
            set_bit(valve_list, i);
            count++;
        }
    }
    assert(__popcnt64(valve_list) == count);

    printf("%d\n", part_one(valve_list));
    printf("%d\n", part_two(valve_list));
}