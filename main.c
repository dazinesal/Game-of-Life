#include <mpi.h>
#include "main.h"

// #include "beehive.h"
// extern uint8_t beehive[BEEHIVE_HEIGHT][BEEHIVE_WIDTH];

// #include "glider.h"
// extern uint8_t glider[GLIDER_HEIGHT][GLIDER_WIDTH];

#include "grower.h"
extern uint8_t grower[GROWER_HEIGHT][GROWER_WIDTH];

/**
 * Initializes the grid.
 * @param grid the grid.
 * @param pattern the pattern.
 * @param patternHeight the height of the pattern.
 * @param patternWidth the width of the pattern.
 */
void populate_grid(bool* grid, bool* pattern, int patternHeight, int patternWidth) {
    for (int row = 0; row < patternHeight; row++) {
        for (int col = 0; col < patternWidth; col++) {
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
    // Update the grid based on the Game of Life rules
    #pragma omp parallel for collapse(2)
    {
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                register int live_neighbours = count_live_neighbors(row, col, grid, height, width);

                int idx = row * width + col;
                register bool current_cell = grid[idx];
                switch (current_cell) {
                    case ALIVE:
                        if (live_neighbours < 2 || live_neighbours > 3) {
                            grid[idx] = DEAD;
                        } 
                        break;

                    case DEAD:
                        if (live_neighbours == 3) { 
                            grid[idx] = ALIVE;
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
 * @returns the number of live neighbors.
 */
int count_live_neighbors(int row, int col, bool *grid, int height, int width) {
    int count = 0;
    
    // Get all cells from -1 to 1 both X and Y direction.
    #pragma omp parallel for reduction(+:count)
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            // Skip the current cell.
            if (x == 0 && y == 0) {
                continue;
            }

            int neighborX = row + x;
            int neighborY = col + y;

            // Check if the neighbor is within the grid boundaries
            if (
                neighborX < 0 || neighborX >= height || 
                neighborY < 0 || neighborY >= width
            ) {
                continue;
            }

            if (grid[neighborX * width + neighborY] == ALIVE) {
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
void print_grid(bool *grid, int height, int width) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            printf("%c", grid[row * width + col] ? '*' : '.');
        }
        printf("\n");
    }
}

/**
 * Counts the population.
 * @param grid the grid.
 * @param height the height of the grid.
 * @param width the width of the grid.
 */
int count_population(bool *grid, int height, int width) {
    int population = 0;
    #pragma omp parallel for reduction(+:population)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (grid[i * width + j]) {
                population++;
            }
        }
    }
    return population;
}

/**
 * Main Method
 */
int main(int argc, char* argv[]) {
    bool *grid;
    bool *swap_grid;
    int i;
    double start_time, end_time;
    int population;

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
    
    // Populate the grid.
    grid = (bool *) malloc(HEIGHT * WIDTH * sizeof(bool));
    populate_grid(grid, pattern, patternHeight, patternWidth);

    // Perform the Game of Life.
    for(int i = 1; i <= ITERATIONS; i++) 
    { 
        start_time = MPI_Wtime();
        process_grid(grid, HEIGHT, WIDTH);
        end_time = MPI_Wtime();
        
        population = count_population((bool*)grid, HEIGHT, WIDTH);
        printf("Generation: %d, population count: %d, obtained in %f seconds\n", i, population, end_time-start_time);
    }

    return 0;
}