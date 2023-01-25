#include <stdio.h>
#include <stdlib.h>
#include "/usr/local/opt/libomp/include/omp.h"

#include "main.h"
#include "beehive.h"

extern uint8_t beehive[BEEHIVE_HEIGHT][BEEHIVE_WIDTH];

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
    int x = height / 2;
    int y = width / 2;

    int aliveNeighbors = count_live_neighbors(x, y, grid, height, width);
    
    // Check all central cells of the matrix
    switch (grid[x * width + y]) {
        case 0: // dead
            if (aliveNeighbors == 3) {
                grid[x * width + y] = ALIVE;
            }
            break;

        case 1: // alive
            if (aliveNeighbors != 3) {
                grid[x * width + y] = DEAD;
            }
            break;
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

    populate_grid((uint8_t*) grid, height, width, pattern, patternHeight, patternWidth);
    print_grid((uint8_t*)grid, height, width);

    return 0;
}