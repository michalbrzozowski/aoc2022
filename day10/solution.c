#include "..\utilities.h"

#define input_file "input"

int part_one()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    int ip = 0;
    int regX = 1;
    int cycle = 1;
    int value = 0;
    int is_executing = 0;

    int bp = 20;

    for (int cycle = 1; ip < list_length(input); ++cycle) {
        if (cycle == bp) {
            answer += regX * cycle;
            bp += 40;
        }
        if (!is_executing) {
            switch (input[ip][0])
            {
            case 'a': {
                sscanf_s(input[ip], "addx %d", &value);
                is_executing++;
            } break;
            case 'n' : break;
            default:
                break;
            }
            ip++;
        }
        else {
            regX += value;
            is_executing--;
        }
    }

    release_input(input);
    return answer;
}

static char screen[6][40];

int part_two()
{
    int answer = 0;
    char** input = aquire_input(input_file);

    int ip = 0;
    int regX = 1;
    int cycle = 1;
    int value = 0;
    int is_executing = 0;

    int crc_x = 0;
    int crc_y = 0;

    for (int cycle = 1; ip < list_length(input); ++cycle) {
        crc_x = (cycle - 1) % 40;
        crc_y = (cycle - 1) / 40;

        if (regX - 1 == crc_x || regX == crc_x || regX + 1 == crc_x) {
            screen[crc_y][crc_x] = '#';
        } else {
            screen[crc_y][crc_x] = '.';
        }

        if (!is_executing) {
            switch (input[ip][0])
            {
            case 'a': {
                sscanf_s(input[ip], "addx %d", &value);
                is_executing++;
            } break;
            case 'n' : break;
            default:
                break;
            }
            ip++;
        }
        else {
            regX += value;
            is_executing--;
        }
    }
    release_input(input);

    for (int j = 0; j < 6; ++j) {
        for (int i = 0; i < 40; ++i) {
            printf("%c", screen[j][i]);
        }
        printf("\n");
    }

    return answer;
}

void main()
{
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}