#include <stdio.h>
#include <stdlib.h>
// for MacOSx users, 
// #include "/usr/local/opt/libomp/include/omp.h"
// for Windows users,
// #include <omp.h>

#include "main.h"
#define N_THREADS 1

#include "beehive.h"
extern uint8_t beehive[BEEHIVE_HEIGHT][BEEHIVE_WIDTH];

#include "glider.h"
extern uint8_t glider[GLIDER_HEIGHT][GLIDER_WIDTH];

#include "grower.h"
extern uint8_t grower[GROWER_HEIGHT][GROWER_WIDTH];

/**
 * Initializes the grid.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 * @param pattern the pattern.
 * @param patternHeight the height of the pattern.
 * @param patternWidth the width of the pattern.
 */
void populate_grid(uint8_t* grid, int height, int width, uint8_t* pattern, int patternHeight, int patternWidth) {
    for (int i = 0; i < patternHeight; i++) {
        for (int j = 0; j < patternWidth; j++) {
            // grid[i][j] == grid[i*width+j] = value;
            grid[((height/2)+i) * width + ((width / 2)+j)] = pattern[i * patternWidth + j];
        }
    }
}

/**
 * Processes the grid.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
void update_grid(uint8_t* grid, int height, int width) {
    int generations = 10;

    for (int g = 0; g < generations; g++ ) {
        #pragma omp parallel for num_threads(N_THREADS)
        for (int i = 1; i <= height; i++) {
            for (int j = 1; j <= width; j++) {
                int aliveNeighbors = count_live_neighbors(i, j, grid, height, width);
                
                // Check all central cells of the matrix
                switch (grid[i * width + j]) {
                    case 0: // dead
                        if (aliveNeighbors == 3) {
                            grid[i * width + j] = ALIVE;
                        }
                        break;

                    case 1: // alive
                        if (aliveNeighbors < 2 || aliveNeighbors > 3) {
                            grid[i * width + j] = DEAD;
                        }
                        break;
                }           
            }
        }
    }
}

/**
 * Counts the number of live neighbors for cell i, j.
 * @param x the targeted cell's row.
 * @param y the targeted cell's column.
 * @param grid the cell's grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 * @returns the number of live neighbors.
 */
int count_live_neighbors(int x, int y, uint8_t* grid, int height, int width) {
    int count = 0;
    
    // Get all cells from -1 to 1 both X and Y direction.
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }

            int neighborX = x + i;
            int neighborY = y + j;

            if (
                neighborX < 0 || neighborX > height || 
                neighborY < 0 || neighborY > width
            ) {
                continue;
            }

            if (grid[neighborX * height + neighborY] == ALIVE) {
                count++;
            }
        }
    }

    return count;
}

/**
 * Prints the grid.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
void print_grid(uint8_t* grid, int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%d ", grid[i * width + j]);
        }
        printf("\n");
    }
}

/**
 * Main Method
 */
int main() {
    int height = 3000;
    int width = 3000;
    uint8_t grid[3000][3000] = {0};

    // --
    // Beehive
    // --
    int patternHeight = BEEHIVE_HEIGHT;
    int patternWidth = BEEHIVE_WIDTH;
    uint8_t* pattern = (uint8_t*)beehive;

    // --
    // Glider
    // --
    // int patternHeight = GLIDER_HEIGHT;
    // int patternWidth = GLIDER_WIDTH;
    // uint8_t* pattern = (uint8_t*)glider;

    // --
    // Grower
    // --
    // int patternHeight = GROWER_HEIGHT;
    // int patternWidth = GROWER_WIDTH;
    // uint8_t* pattern = (uint8_t*)grower;

    populate_grid((uint8_t*) grid, height, width, pattern, patternHeight, patternWidth);
    update_grid((uint8_t*)grid, height, width);
    print_grid((uint8_t*)grid, height, width);

    return 0;
}