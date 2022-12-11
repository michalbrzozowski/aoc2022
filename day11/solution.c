#include "..\utilities.h"

#define input_file "input"

u32 gcd(u32 lhs, u32 rhs)
{
    if (rhs == 0) {
        return lhs;
    }
    return gcd(rhs, lhs % rhs);
}

int find_lcm(int* array, int length)
{
    int result = array[0];

    for (int i = 1; i < length; i++) {
        result = (result * array[i]) / gcd(result, array[i]);
    }

    return result;
}

int compare (const void * a, const void * b)
{
    u64 arg1 = *(const u64*)a;
    u64 arg2 = *(const u64*)b;
 
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

typedef u64 (*operation_fptr)(u64, u64);

typedef struct monkey {
    u64* item_worry_levels;
    operation_fptr operation;
    u64 op_value;
    u32 div_value;
    u64 throw_to[2];
} monkey;

u64 addition_op(u64 term0, u64 term1)
{
    return term0 + term1;
}

u64 multiplication_op(u64 factor0, u64 factor1)
{
    return factor0 * factor1;
}

static monkey* monkeys;
static u64 *monkey_inspection_count;
static u32 lcm;

void parse_input()
{
    u32 *divisors = null_ptr;
    char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input); i += 7) {
        u32 item_id = 0;
        u32 monkey_id = 0;
        monkey current_monkey = { 0 };

        char* starting_items = input[i + 1] + 18;
        sscanf_s(starting_items, "%u", &item_id);
        list_push(current_monkey.item_worry_levels, item_id);

        u32 it = 2;
        while (starting_items + it != list_end(input[i+1]) && sscanf_s(starting_items + it, ", %u", &item_id)) {
            list_push(current_monkey.item_worry_levels, item_id);
            it += 4;
        }

        // operation
        current_monkey.operation = (input[i + 2][23] == '+') ? addition_op : multiplication_op;
        current_monkey.op_value = atoi(input[i + 2] + 25);

        // test
        u32 div = atoi(input[i + 3] + 21);
        list_push(divisors, div);

        current_monkey.div_value = div;

        sscanf_s(input[i + 4], "    If true: throw to monkey %u", &monkey_id);
        current_monkey.throw_to[1] = monkey_id;
        sscanf_s(input[i + 5], "    If false: throw to monkey %u", &monkey_id);
        current_monkey.throw_to[0] = monkey_id;

        list_push(monkeys, current_monkey);
        list_push(monkey_inspection_count, 0);
    }
    lcm = find_lcm(divisors, (int)list_length(divisors));
    list_free(divisors);
    release_input(input);
}

/*
Monkey 0:
  Monkey inspects an item with a worry level of 79.
    Worry level is multiplied by 19 to 1501.
    Monkey gets bored with item. Worry level is divided by 3 to 500.
    Current worry level is not divisible by 23.
    Item with worry level 500 is thrown to monkey 3.
  Monkey inspects an item with a worry level of 98.
    Worry level is multiplied by 19 to 1862.
    Monkey gets bored with item. Worry level is divided by 3 to 620.
    Current worry level is not divisible by 23.
    Item with worry level 620 is thrown to monkey 3.
*/

void simulate_monkey_turn(u32 relief)
{
    for (int i = 0; i < list_length(monkeys); ++i) {
        // monkey inspects and item
        for (int j = 0; j < list_length(monkeys[i].item_worry_levels); ++j) {
            monkey_inspection_count[i]++;
            u64 old_wl = monkeys[i].item_worry_levels[j];
            u64 op_value = monkeys[i].op_value == 0 ? old_wl : monkeys[i].op_value;
            u64 new_wl = monkeys[i].operation(old_wl, op_value);

            // monkey gets bored
            if (relief)
                new_wl /= 3;
            else
                new_wl %= lcm;

            u64 throw_to = monkeys[i].throw_to[(new_wl % monkeys[i].div_value) == 0];
            list_push(monkeys[throw_to].item_worry_levels, new_wl);
        }
        list_clear(monkeys[i].item_worry_levels);
    }
}

void print_monkey_information()
{
    for (int i = 0; i < list_length(monkeys); ++i) {
        printf("monkey (%d) inspected in total (%"PRIu64"), current worry list: ", i, monkey_inspection_count[i]);
        for (int j = 0; j < list_length(monkeys[i].item_worry_levels); ++j) {
            printf("%"PRIu64" ", monkeys[i].item_worry_levels[j]);
        }
        printf("\n");
    }
}

u64 simulate_and_gather_information(u32 turns, u32 relief)
{
    parse_input();

    for (u32 i = 0; i < turns; ++i) {
        simulate_monkey_turn(relief);
    }
    print_monkey_information();

    size_t count = list_length(monkey_inspection_count);
    qsort(monkey_inspection_count, count, sizeof(u64), compare);

    u64 answer = monkey_inspection_count[count - 1] * monkey_inspection_count[count - 2];

    list_free(monkeys);
    list_free(monkey_inspection_count);

    return answer;
}

u64 part_one()
{
    return simulate_and_gather_information(20, 1);
}

u64 part_two()
{
    return simulate_and_gather_information(10000, 0);
}

void main()
{
    printf("%"PRIu64"\n", part_one());
    printf("%"PRIu64"\n", part_two());
}