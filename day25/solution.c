#include "..\utilities.h"

#define input_file "input"

i64 from_snafu(char* number)
{
    i64 val = 0;
    for (int i = 0; i < list_length(number); ++i) {
		val *= 5; 
        switch (number[i])
        {
        case '-': val += -1; break;
        case '=': val += -2; break;
        case '0': val += 0; break;
        case '1': val += 1; break;
        case '2': val += 2; break;
        }
    }
    return val;
}

char* to_snafu(i64 number)
{
    char* snafu_digits[] = { '=', '-', '0', '1', '2' };
    char* snafu = null_ptr;
    
    while (number) {
        i64 val = ((number + 2) % 5) - 2;
        list_printf(snafu, "%c", snafu_digits[val + 2]);
        number -= val;
        number /= 5;
    }

    char* h = snafu;
    char* t = list_end(snafu) - 1;
    for (; h < t; h++, t--) {
        char hc = *h, tc = *t;
        *t = hc; *h = tc;
    }

    return snafu;
}

i64 part_one()
{
    i64 answer = 0;
    char** input = aquire_input(input_file);
    for (int i = 0; i < list_length(input); ++i) {
        answer += from_snafu(input[i]);
    }
    release_input(input);
    printf("%s\n", to_snafu(answer));

    return answer;
}

void main()
{
    printf("%"PRId64"\n", part_one());
}