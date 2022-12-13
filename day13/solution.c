#include <ctype.h>
#include "..\utilities.h"

#define input_file "input"

typedef enum {
    is_number,
    is_list
} item_type;

typedef struct list_item {
    item_type type;
    union {
        struct list_item* list;
        int number;
    };
} list_item;

list_item* parse_list(char* input, int *count)
{
    int it = count != null_ptr ? *count : 0;
    list_item* list = null_ptr;
    while (input[it] != '\0') {
        switch (input[it])
        {
            case '[': {
                it++;
                list_item new_list = { .type = is_list, .list = parse_list(input, &it)};
                list_push(list, new_list);
            } break;
            case ']': {
                it++;
                if (count)
                    *count = it;
                return list;
            }
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                int val = 0;
                while (isdigit(input[it])) {
                    val *= 10;
                    val += input[it++] - '0';
                }
                list_item new_number = { .type = is_number, .number = val };
                list_push(list, new_number);
            } break;
            case ',': {
                it++;
            } break;
        default:
            break;
        }
    }
    return list;
}

void free_list(list_item* list)
{
    for (int i = 0; i < list_length(list); ++i) {
        if (list[i].type == is_list) {
            free_list(list[i].list);
            list_free(list[i].list);
        }
    }
}

void print_list(list_item* list)
{
    size_t length = list_length(list);

    printf("[");
    for (int i = 0; i < length; ++i) {
        if (list[i].type == is_number) {
            printf("%d", list[i].number);
        }
        else
            print_list(list[i].list);
        if (i + 1 < length) printf(",");
    }
    printf("]");
}

int compare(list_item lhs, list_item rhs)
{
    if (lhs.type == is_number && rhs.type == is_number) {
        return lhs.number < rhs.number ? 1 : lhs.number > rhs.number ? -1 : 0;
    }

    list_item* list_to_free = null_ptr;

    if (lhs.type == is_number) {
        int val = lhs.number;
        lhs.type = is_list;
        lhs.list = null_ptr;
        list_push(lhs.list, (list_item) { .type = is_number, .number = val });
        list_to_free = lhs.list;
    }
    else if (rhs.type == is_number) {
        int val = rhs.number;
        rhs.type = is_list;
        rhs.list = null_ptr;
        list_push(rhs.list, (list_item) { .type = is_number, .number = val });
        list_to_free = rhs.list;
    }

    int llength = (int)list_length(lhs.list);
    int rlength = (int)list_length(rhs.list);

    int length = __min(llength, rlength);
    for (int i = 0; i < length; ++i) {
        int cmp = compare(lhs.list[i], rhs.list[i]);
        if (list_to_free) list_free(list_to_free);
        if (cmp != 0) return cmp;
    }
    if (list_to_free) list_free(list_to_free);

    return llength < rlength ? 1 : llength > rlength ? -1 : 0;
}

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    int count = 1;
    for (int i = 0; i < list_length(input); i += 3) {
        list_item* lhs = parse_list(input[i], null_ptr);
        list_item* rhs = parse_list(input[i + 1], null_ptr);

#if 0
        print_list(lhs[0].list);
        printf("\n");
        print_list(rhs[0].list);
        printf("\n\n");
#endif

        if (compare(lhs[0], rhs[0]) == 1)
            answer += count;
        count++;

        free_list(lhs);
        list_free(lhs);
        free_list(rhs);
        list_free(rhs);
    }

    release_input(input);
    return answer;
}

int qsort_compare(const void* a, const void* b)
{
    list_item lhs = *(const list_item*)a;
    list_item rhs = *(const list_item*)b;
    return compare(lhs.list[0], rhs.list[0]);
}

int part_two()
{
    char** input = aquire_input(input_file);

    list_item* packets = null_ptr;

    for (int i = 0; i < list_length(input); i += 3) {
        list_push(packets, (list_item) { .type = is_list, .list = parse_list(input[i], null_ptr) });
        list_push(packets, (list_item) { .type = is_list, .list = parse_list(input[i + 1], null_ptr) });
    }

    release_input(input);

    list_item* divider_packets[2] = { 
        parse_list("[[2]]", null_ptr),
        parse_list("[[6]]", null_ptr)
    };

    list_push(packets, (list_item) { .type = is_list, .list = divider_packets[0] });
    list_push(packets, (list_item) { .type = is_list, .list = divider_packets[1] });

    int count = (int)list_length(packets);
    qsort(packets, count, sizeof(list_item), qsort_compare);

    int divider_index[2] = { 0 };
    for (int i = 0; i < count; ++i) {
        if (packets[i].list == divider_packets[0])
            divider_index[0] = count - i;
        else if (packets[i].list == divider_packets[1])
            divider_index[1] = count - i;
    }

    for (int i = 0; i < count; ++i) {
        free_list(packets);
        list_free(packets);
    }

    return divider_index[0] * divider_index[1];
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}