#include "..\utilities.h"

#define grid_size 99

static u32 grid[grid_size][grid_size];
#define input_file "input"

void fill_grid()
{
    char** input = aquire_input(input_file);
    assert(grid_size == list_length(input));


    for (int i = 0; i < list_length(input); ++i) {
        for (int j = 0; j < list_length(input[i]); ++j) {
            grid[i][j] = input[i][j] - '0';
        }
    }

    release_input(input);
}

int raycast(int x, int y, int *score)
{
    u32 tree = grid[y][x];
    int found = 0;

    int scores[4] = { 0 };

    for (int xf = x + 1; xf < grid_size; ++xf) {
        scores[0]++;
        if (grid[y][xf] >= tree) {
            found++;
            break;
        }
    }
    for (int xb = x - 1; xb >= 0; --xb) {
        scores[1]++;
        if (grid[y][xb] >= tree) {
            found++;
            break;
        }
    }
    for (int yf = y + 1; yf < grid_size; ++yf) {
        scores[2]++;
        if (grid[yf][x] >= tree) {
            found++;
            break;
        }
    }
    for (int yb = y - 1; yb >= 0; --yb) {
        scores[3]++;
        if (grid[yb][x] >= tree) {
            found++;
            break;
        }
    }

    if (score != null_ptr)
        *score = scores[0] * scores[1] * scores[2] * scores[3];

    return found == 4;
}

int part_one()
{
    int answer = 0;
    for (int i = 1; i < grid_size - 1; ++i) {
        for (int j = 1; j < grid_size - 1; ++j) {
            answer += raycast(j, i, null_ptr);
        }
    }
    return (grid_size * grid_size) - answer;
}

int part_two()
{
    int score = 0;
    int answer = 0;
    for (int i = 1; i < grid_size - 1; ++i) {
        for (int j = 1; j < grid_size - 1; ++j) {
            raycast(j, i, &score);
            answer = __max(answer, score);
        }
    }
    return answer;
}

void main()
{
    fill_grid();
    printf("%d\n", part_one());
    printf("%d\n", part_two());
}