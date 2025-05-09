#include <stdio.h>

#define INPUT_FILE "DataTPA.csv"
#define BUFFER_SIZE 1024
#define DATA_COUNT 60
#define SAMPLE_SIZE 3
#define SEARCH_COUNT 4

typedef struct Data {
    float year;
    float user_percentage;
    float population;
} Data;

Data readDataLine(char *line) {
    Data ret;
    sscanf(line, "%f,%f,%f", &(ret.year), &(ret.user_percentage), &(ret.population));
    return ret;
}

float newton_interpolation(float x[SAMPLE_SIZE], float y[SAMPLE_SIZE][SAMPLE_SIZE], float a) 
{ 
    int i, j; 
    float h, u, sum, p; 
    for (j = 1; j < SAMPLE_SIZE; j++) { 
        for (i = 0; i < SAMPLE_SIZE - j; i++) { 
            y[i][j] = y[i + 1][j - 1] - y[i][j - 1]; 
        } 
    } 

    p = 1.0; 
    sum = y[0][0]; 
    h = x[1] - x[0]; 
    u = (a - x[0]) / h; 
    for (j = 1; j < SAMPLE_SIZE; j++) { 
        p = p * (u - j + 1) / j; 
        sum = sum + p * y[0][j]; 
    } 

    return sum;
} 


int main() {
    FILE *finput = fopen(INPUT_FILE, "r");

    char buffer[BUFFER_SIZE];
    fgets(buffer, 1024, finput);

    Data dataArray[DATA_COUNT];
    
    for (int i = 0; i < DATA_COUNT && fgets(buffer, BUFFER_SIZE, finput); i++) {
        Data temp = readDataLine(buffer);
        dataArray[i] = temp;
        printf("%s", buffer);
    }

    float tableu_percentage[SAMPLE_SIZE][SAMPLE_SIZE] = {0.0f};
    float tableu_population[SAMPLE_SIZE][SAMPLE_SIZE] = {0.0f};
    float year[SAMPLE_SIZE];

    int search[SEARCH_COUNT] = {2005, 2006, 2015, 2016};

    for (int j = 0; j < SEARCH_COUNT; j++) {
        int mid_index = -1;
        for (int i = 0; i < DATA_COUNT; i++) {
            if (dataArray[i].year >= search[j]) {
                mid_index = i;
                break;
            }
        }
        if (mid_index == -1) mid_index = DATA_COUNT - 1;  // fallback if search is beyond max year

        int sample_start = mid_index - SAMPLE_SIZE / 2;
        if (sample_start < 0) sample_start = 0;
        if (sample_start + SAMPLE_SIZE > DATA_COUNT) sample_start = DATA_COUNT - SAMPLE_SIZE;

            
        for (int i = 0; i < SAMPLE_SIZE; i++) {
            year[i] = dataArray[i + sample_start].year;
            tableu_percentage[i][0] = dataArray[i + sample_start].user_percentage;
            tableu_population[i][0] = dataArray[i + sample_start].population;
        }

        float percentage = newton_interpolation(year, tableu_percentage, search[j]);
        float population = newton_interpolation(year, tableu_population, search[j]);
        printf("---\n%d\n", search[j]);
        printf("Internet User Percentage\t:\t%f\n", percentage);
        printf("Population\t\t\t:\t%f\n", population);
    }
}

