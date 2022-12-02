#include "..\utilities.h"

#define input_file "input"

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    char opponent, me;
    opponent = me = 0;
    int score = 0;
    for (int i = 0; i < list_length(input); ++i) {
        sscanf_s(input[i], "%c %c", &opponent, 1, &me, 1);
        opponent -= 'A';
        me -= 'X';

        if (opponent == me)
            score = 3;
        else if ((opponent + 1) % 3 == me)
            score = 6;
        else
            score = 0;
        answer += me + score + 1;
    }

    release_input(input);
    return answer;
}

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    char opponent, result;
    opponent = result = 0;
    for (int i = 0; i < list_length(input); ++i) {
        sscanf_s(input[i], "%c %c", &opponent, 1, &result, 1);
        opponent -= 'A';

        switch (result)
        {
        case 'X': {
            int me = (opponent + 2) % 3 + 1;
            answer += me;
        } break;
        case 'Y': {
            answer += opponent + 3 + 1;
        } break;
        case 'Z': {
            int me = (opponent + 1) % 3 + 1;
            answer += me + 6;
        } break;
        default:
            assert("should not hit :)!");
            break;
        }
    }

    release_input(input);
    return answer;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}