#include <string.h>
#include "..\utilities.h"

#define input_file "input"

typedef struct directory_node {
    u64 size;
    int parent;
} directory_node;

static directory_node nodes[256];
static u64 total_count;
static u64 total_size;

void parse_directories()
{
    char** input = aquire_input(input_file);

    int it = 0;
    int next = 0;
    u64 current = 0;

    nodes[it].parent = -1;

    for (int i = 1; i < list_length(input); ++i) {
        char* line = input[i];
        if (line[0] == '$') {
            current = 0;
            if (line[2] == 'c') {
                if (line[5] == '.') {
                    int p = nodes[it].parent;
                    nodes[p].size += nodes[it].size;
                    it = p;
                } else {
                    nodes[++next].parent = it;
                    it = next;
                }
            }
        } else {
            while (i < list_length(input) && input[i][0] != '$') {
                line = input[i++];
                int size = 0;
                if (line[0] != 'd') {
                    sscanf_s(line, "%d", &size);
                    current += size;
                }
            }
            --i;
            nodes[it].size += current;
            total_size += current;
        }
    }
    total_count = next;
    release_input(input);
}

u64 part_one()
{
    u64 answer = 0;

    for (int i = 0; i < total_count; ++i) {
        if (nodes[i].size <= 100000)
            answer += nodes[i].size;
    }

    return answer;
}

u64 part_two()
{
    u64 answer = 0;

    answer = 70000000;
    for (int i = 0; i < total_count; ++i) {
        if (total_size - nodes[i].size <= 40000000)
            answer = __min(answer, nodes[i].size);
    }

    return answer;
}

void main()
{
    parse_directories();

    printf("%"PRIu64"\n", part_one());
    printf("%"PRIu64"\n", part_two());
}