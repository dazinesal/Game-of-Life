#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <omp.h>

#define ITERATIONS 5000

#define WIDTH 3000
#define HEIGHT 3000

#define DEAD 0
#define ALIVE 1

void populate_grid(bool *grid, bool* pattern, int patternHeight, int patternWidth);
void process_grid(bool *grid, int height, int width);
bool process_cell(bool status, int row, int col, bool *grid, int height, int width);
int count_live_neighbors(int row, int col, bool *grid, int height, int width);
int count_population(bool *grid, int height, int width);
void print_grid(bool *grid, int height, int width);