#include <ctype.h>
#include "..\utilities.h"

#define input_file "input"

#define max_monkeys 4096

typedef struct monkey {
    u32 index;

    u64 value;
    int depdends[3];
    int x;
    byte op;
} monkey;

int monkey_lookups;
u32 monkey_lookup[max_monkeys];

monkey monkeys[max_monkeys];

int lookup_or_add(byte node[4])
{
    u32* val = (u32*)node;

    u32 v = *val;

    int i = 0;
    for (; i < monkey_lookups; ++i) {
        if (monkey_lookup[i] == *val)
            return i;
    }
    if (i < max_monkeys) {
        monkey_lookup[monkey_lookups++] = *val;
        return i;
    }
    return -1;
}

void parse_input()
{
    monkey_lookups = 0;
    char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input); ++i) {
        char *line = input[i];
        monkey m = { 0 };
        m.index = lookup_or_add(line);

        if (isdigit(input[i][6])) {
            m.value = atoi(line + 6);
            assert(m.op == 0);
        }
        else {
            m.depdends[0] = lookup_or_add(line + 6);
            m.op = line[11];
            m.depdends[1] = lookup_or_add(line + 13);
        }
        monkeys[m.index] = m;
    }
    release_input(input);
}

u64 calculate(int mid)
{
    monkey m = monkeys[mid];
    if (m.op == 0)
        return m.value;

    u64 op1 = calculate(m.depdends[0]);
    u64 op2 = calculate(m.depdends[1]);

    u64 result = 0;
    switch (m.op)
    {
    case '+': result = op1 + op2; break;
    case '-': result = op1 - op2; break;
    case '*': result = op1 * op2; break;
    case '/': result = op1 / op2; break;
    case '=': result = op1 == op2; break;

    default:
        break;
    }

    monkeys[mid].op = 0;
    monkeys[mid].value = result;

    return result;
}

u64 part_one()
{
    parse_input();
    u64 answer = calculate(lookup_or_add("root"));
    return answer;
}

int has_dependency(int a, int b)
{
    if (a == b)
        return 1;
    if (monkeys[a].x == b)
        return monkeys[a].depdends[2];
    if (monkeys[a].op == 0)
        return 0;

    monkeys[a].x = b;
    monkeys[a].depdends[2] = has_dependency(monkeys[a].depdends[0], b) ||
        has_dependency(monkeys[a].depdends[1], b);
    return monkeys[a].depdends[2];
}

u64 find_value(int a, int b, u64 val)
{
    if (a == b)
        return val;
    
    int dep0 = monkeys[a].depdends[0];
    int dep1 = monkeys[a].depdends[1];

    u64 result = 0;
    if (has_dependency(dep0, b)) {
        switch (monkeys[a].op)
        {
        case '+': result = find_value(dep0, b, val - calculate(dep1)); break;
        case '-': result = find_value(dep0, b, val + calculate(dep1)); break;
        case '*': result = find_value(dep0, b, val / calculate(dep1)); break;
        case '/': result = find_value(dep0, b, val * calculate(dep1)); break;
        }
    }
    else {
        switch (monkeys[a].op)
        {
        case '+': result = find_value(dep1, b, val - calculate(dep0)); break;
        case '-': result = find_value(dep1, b, calculate(dep0) - val); break;
        case '*': result = find_value(dep1, b, val / calculate(dep0)); break;
        case '/': result = find_value(dep1, b, calculate(dep0) / val); break;
        }
    }
    return result;
}

u64 part_two()
{
    parse_input();

    int rmid = lookup_or_add("root");
    monkeys[rmid].op = '-';

    int hid = lookup_or_add("humn");

    u64 answer = find_value(rmid, hid, 0);
    return answer;
}

void main()
{
    printf("%"PRIu64"\n", part_one());
    printf("%"PRIu64"\n", part_two());
}