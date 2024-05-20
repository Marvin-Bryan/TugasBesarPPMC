#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define POP_SIZE 100
#define NUM_GENERATIONS 1000
#define MUTATION_RATE 0.1
#define MAX_CITIES 15
#define MIN_CITIES 6
#define MAX_CITY_NAME_LENGTH 50
#define EARTH_RADIUS 6371.0 // Radius Bumi dalam km
#define PI 3.14159265358979323846 // Radius Bumi dalam km

typedef struct {
    char name[MAX_CITY_NAME_LENGTH];
    double latitude;
    double longitude;
} City;

City cities[MAX_CITIES];
double distance_matrix[MAX_CITIES][MAX_CITIES];
int num_cities = 0;
int starting_city = 0;

double to_radians(double degree) {
    return degree * PI / 180.0;
}

double haversine_distance(City c1, City c2) {
    double lat1 = to_radians(c1.latitude);
    double lon1 = to_radians(c1.longitude);
    double lat2 = to_radians(c2.latitude);
    double lon2 = to_radians(c2.longitude);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS * c;
}

void calculate_distance_matrix() {
    for (int i = 0; i < num_cities; i++) {
        for (int j = 0; j < num_cities; j++) {
            if (i != j) {
                distance_matrix[i][j] = haversine_distance(cities[i], cities[j]);
            } else {
                distance_matrix[i][j] = 0.0;
            }
        }
    }
}

double fitness_function(int *route) {
    double total_distance = 0.0;
    for (int i = 0; i < num_cities - 1; i++) {
        int from = route[i];
        int to = route[i + 1];
        total_distance += distance_matrix[from][to];
    }
    total_distance += distance_matrix[route[num_cities - 1]][route[0]]; // Return to starting point
    return total_distance;
}

void initialize_population(int population[POP_SIZE][MAX_CITIES]) {
    for (int i = 0; i < POP_SIZE; i++) {
        for (int j = 0; j < num_cities; j++) {
            population[i][j] = j;
        }
        for (int j = 1; j < num_cities; j++) { // Start shuffling from index 1 to keep the starting city fixed
            int idx1 = 1 + rand() % (num_cities - 1);
            int idx2 = 1 + rand() % (num_cities - 1);
            int temp = population[i][idx1];
            population[i][idx1] = population[i][idx2];
            population[i][idx2] = temp;
        }
    }
}

void mutate(int *individual) {
    if (((double) rand() / RAND_MAX) < MUTATION_RATE) {
        int idx1 = 1 + rand() % (num_cities - 1);
        int idx2 = 1 + rand() % (num_cities - 1);
        int temp = individual[idx1];
        individual[idx1] = individual[idx2];
        individual[idx2] = temp;
    }
}

void crossover(int *parent1, int *parent2, int *child) {
    int start = 1 + rand() % (num_cities - 1);
    int end = start + rand() % (num_cities - start);

    // Initialize the child with -1
    for (int i = 0; i < num_cities; i++) {
        child[i] = -1;
    }

    // Copy the segment from parent1 to child
    for (int i = start; i <= end; i++) {
        child[i] = parent1[i];
    }

    // Fill the remaining positions with elements from parent2
    int currentIndex = (end + 1) % num_cities;
    for (int i = 0; i < num_cities; i++) {
        int candidate = parent2[i];
        // Check if candidate is already in child
        int isAlreadyIncluded = 0;
        for (int j = 0; j < num_cities; j++) {
            if (child[j] == candidate) {
                isAlreadyIncluded = 1;
                break;
            }
        }
        // If candidate is not in child, add it
        if (!isAlreadyIncluded) {
            child[currentIndex] = candidate;
            currentIndex = (currentIndex + 1) % num_cities;
        }
    }

    // Ensure starting city is at the beginning
    for (int i = 0; i < num_cities; i++) {
        if (child[i] == starting_city) {
            int temp = child[0];
            child[0] = child[i];
            child[i] = temp;
            break;
        }
    }
}

void genetic_algorithm() {
    int population[POP_SIZE][MAX_CITIES];
    double fitness[POP_SIZE];
    clock_t start_time, end_time;

    start_time = clock();
    initialize_population(population);

    for (int generation = 0; generation < NUM_GENERATIONS; generation++) {
        for (int i = 0; i < POP_SIZE; i++) {
            fitness[i] = fitness_function(population[i]);
        }

        int new_population[POP_SIZE][MAX_CITIES];
        for (int i = 0; i < POP_SIZE; i++) {
            int parent1_idx = rand() % POP_SIZE;
            int parent2_idx = rand() % POP_SIZE;
            while (parent2_idx == parent1_idx) {
                parent2_idx = rand() % POP_SIZE;
            }

            crossover(population[parent1_idx], population[parent2_idx], new_population[i]);
            mutate(new_population[i]);
        }

        // Replace old population with new population
        for (int i = 0; i < POP_SIZE; i++) {
            for (int j = 0; j < num_cities; j++) {
                population[i][j] = new_population[i][j];
            }
        }
    }

    int best_individual[MAX_CITIES];
    double best_fitness = fitness_function(population[0]);
    for (int i = 0; i < num_cities; i++) {
        best_individual[i] = population[0][i];
    }

    for (int i = 1; i < POP_SIZE; i++) {
        double current_fitness = fitness_function(population[i]);
        if (current_fitness < best_fitness) {
            best_fitness = current_fitness;
            for (int j = 0; j < num_cities; j++) {
                best_individual[j] = population[i][j];
            }
        }
    }
    end_time = clock();

    printf("Best route found:\n");
    for (int i = 0; i < num_cities; i++) {
        int city_index = best_individual[i];
        printf("%s -> ", cities[city_index].name);
    }
    printf("%s\n", cities[best_individual[0]].name); // Return to start
    printf("Best route distance: %f km\n", best_fitness);
    printf("Time elapsed: %f s\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);
}

int main() {
    srand(time(NULL));

    FILE *file = fopen("coordinates.txt", "r");
    if (!file) {
        perror("Unable to open file");
        return 1;
    }

    printf("Enter starting point: ");
    char start_city[MAX_CITY_NAME_LENGTH];
    fgets(start_city, MAX_CITY_NAME_LENGTH, stdin);
    start_city[strcspn(start_city, "\n")] = '\0'; // Remove newline character

    int found_start_city = 0;

    while (fscanf(file, "%[^,],%lf,%lf\n", cities[num_cities].name, &cities[num_cities].latitude, &cities[num_cities].longitude) == 3) {
        if (strcmp(cities[num_cities].name, start_city) == 0) {
            starting_city = num_cities;
            found_start_city = 1;
        }
        num_cities++;
        if (num_cities >= MAX_CITIES) break;
    }
    fclose(file);

    if (!found_start_city) {
        printf("Starting city not found in the list of cities.\n");
        return 1;
    }

    if (num_cities < MIN_CITIES) {
        printf("Not enough cities. Minimum number of cities is %d.\n", MIN_CITIES);
        return 1;
    }

    calculate_distance_matrix();
    genetic_algorithm();

    return 0;
}
