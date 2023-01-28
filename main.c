#include "main.h"

#include "beehive.h"
#include "glider.h"
#include "grower.h"

// --
// Beehive
// --
// int patternHeight = BEEHIVE_HEIGHT;
// int patternWidth = BEEHIVE_WIDTH;
// uint8_t* pattern = (uint8_t*)beehive;

// --
// Glider
// --
// int patternHeight = GLIDER_HEIGHT;
// int patternWidth = GLIDER_WIDTH;
// uint8_t* pattern = (uint8_t*)glider;

// --
// Grower
// --
int patternHeight = GROWER_HEIGHT;
int patternWidth = GROWER_WIDTH;
bool* pattern = (bool*)grower;

bool *grid;
bool *swap_grid;

const int dx[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
const int dy[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

/**
 * Initializes the grid.
 * @param grid the grid.
 * @param pattern the pattern.
 * @param patternHeight the height of the pattern.
 * @param patternWidth the width of the pattern.
 */
void populate_grid(bool* grid, bool* pattern, int patternHeight, int patternWidth) {
    int row, col;
    for (row = 0; row < patternHeight; row++) {
        for (col = 0; col < patternWidth; col++) {
            // Set the pattern on the middle [row ½, col ½].
            grid[((HEIGHT/2) + row) * WIDTH + ((WIDTH/2) + col)] = pattern[row * patternWidth + col];
        }
    }
}

/**
 * Processes the grid.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
void process_grid(bool *grid, int height, int width) {
    int row, col = 0;

    // Update the grid based on the Game of Life rules
    #pragma omp parallel for private(row, col)
    for (row = 1; row < HEIGHT - 1; row++) {
        for (col = 1; col < WIDTH - 1; col++) {
            swap_grid[row * WIDTH + col] = process_cell(grid[row * WIDTH + col], row, col, grid, HEIGHT, WIDTH);
        }
    }
}

/**
 * Processes the cell.
 * @param status the status of the cell.
 * @param row the row of the cell.
 * @param col the column of the cell.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 * @returns the processed cell's status.
*/
bool process_cell(bool status, int row, int col, bool *grid, int height, int width) {
    // Count the number of neighbours
    int live_neighbours = count_live_neighbors(row, col, grid, HEIGHT, WIDTH);

    // Game of life rules.
    switch (status) {
        case ALIVE:
            if (live_neighbours < 2 || live_neighbours > 3) {
                return DEAD;
            } 
            break;

        case DEAD:
            if (live_neighbours == 3) { 
                return ALIVE;
            } 
            break;
    } 
    return status;
}

/**
 * Counts the number of live neighbors for cell row, col.
 * @param x the targeted cell's row.
 * @param y the targeted cell's column.
 * @param grid the cell's grid.
 * @param height the grid's height.
 * @param width the grid's width.
 * @returns the number of live neighbors.
 */
int count_live_neighbors(int row, int col, bool *grid, int height, int width) {
    int count = 0;
    int neighborX, neighborY;

    // We loop over all 8 neighbors of cell row, col.
    for (int xy = 0; xy < 8; xy++) {
        neighborX = row + dx[xy];
        neighborY = col + dy[xy];

        // Check if the neighbor is within the grid boundaries
        if ((unsigned)neighborX < (unsigned)height && (unsigned)neighborY < (unsigned)width) {
                // Count if alive.
            if (*(grid + neighborX * width + neighborY)) {
                count++;
            }
        }
    }
    
    return count;
}

/**
 * Counts the population.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
int count_population(bool *grid, int height, int width) {
    int row, col;
    int population = 0;
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            if (grid[row * width + col]) {
                population++;
            }
        }
    }
    return population;
}

/**
 * Prints the grid.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
void print_grid(bool *grid, int height, int width) {
    int row, col;
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            printf("%c", grid[row * width + col] ? '*' : '.');
        }
        printf("\n");
    }
}

/**
 * Main Method
 * @param argc the argument count
 * @param argv the argument values.
 * @returns the processes' status.
 */
int main(int argc, char* argv[]) {
    int i; 
    int population;
    double start_time, end_time;

    // Allocate the memory and set the default value to DEAD (0).
    grid = (bool *) calloc(HEIGHT * WIDTH, sizeof(bool));
    swap_grid = (bool *) calloc(HEIGHT * WIDTH, sizeof(bool));
    if (grid == NULL || swap_grid == NULL) {
        fprintf(stderr, "Error: calloc failed \n");
        exit(EXIT_FAILURE);
    }
    // Populate the grid.
    populate_grid(grid, pattern, patternHeight, patternWidth);

    // Perform the Game of Life.
    for (i = 1; i <= ITERATIONS; i++) {
        start_time = omp_get_wtime();
        process_grid(grid, HEIGHT, WIDTH);
        end_time = omp_get_wtime();

        // Copy the values back to grid.
        memcpy(grid, swap_grid, HEIGHT * WIDTH * sizeof(bool));
        
        population = count_population(grid, HEIGHT, WIDTH);
        printf("Generation: %d, population count: %d, obtained in %f seconds\n", i, population, end_time-start_time);
    }

    return(EXIT_SUCCESS);
}

