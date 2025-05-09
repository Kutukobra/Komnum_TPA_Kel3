#include <stdio.h>

#define INPUT_FILE "DataTPA.csv"
#define BUFFER_SIZE 1024
#define DATA_COUNT 60
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

void forward(float x[DATA_COUNT], float y[DATA_COUNT][DATA_COUNT], float a) 
{ 
    int i, j; 
    float h, u, sum, p; 
    for (j = 1; j < DATA_COUNT; j++) { 
        for (i = 0; i < DATA_COUNT - j; i++) { 
            y[i][j] = y[i + 1][j - 1] - y[i][j - 1]; 
        } 
    } 

    p = 1.0; 
    sum = y[0][0]; 
    h = x[1] - x[0]; 
    u = (a - x[0]) / h; 
    for (j = 1; j < DATA_COUNT; j++) { 
        p = p * (u - j + 1) / j; 
        sum = sum + p * y[0][j]; 
    } 

    printf("Forward y at x=%0.0f is %0.5f", a, sum); 
} 

void BackWard(float x[DATA_COUNT], float y[DATA_COUNT][DATA_COUNT], float a) 
{ 
    int i, j; 
    float h, u, sum, p; 
    for (j = 1; j < DATA_COUNT; j++) { 
        for (i = j; i < DATA_COUNT; i++) { 
            y[i][j] = y[i][j - 1] - y[i - 1][j - 1]; 
        } 
    } 

    p = 1.0; 
    sum = y[DATA_COUNT - 1][0]; 
    h = x[1] - x[0]; 
    u = (a - x[DATA_COUNT - 1]) / h; 
    for (j = 1; j < DATA_COUNT; j++) { 
        p = p * (u + j - 1) / j; 
        sum = sum + p * y[DATA_COUNT - 1][j]; 
    } 

    printf("Backward y at x=%0.0f is %0.5f\n", a, sum); 
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

    float tableu_percentage[DATA_COUNT][DATA_COUNT] = {0.0f};
    float tableu_population[DATA_COUNT][DATA_COUNT] = {0.0f};
    float year[DATA_COUNT];

    for (int i = 0; i < DATA_COUNT; i++) {
        tableu_percentage[i][0] = dataArray[i].user_percentage;
        tableu_population[i][0] = dataArray[i].population;
        year[i] = dataArray[i].year;
    }


    int search[SEARCH_COUNT] = {2005, 2006, 2015, 2016};

    for (int i = 0; i < SEARCH_COUNT; i++) {
        printf("-----\n");
        printf("Data for %d\n", search[i]);
        printf("Percentage\n");
        forward(year, tableu_percentage, search[i]);
        putchar('\n');
        BackWard(year, tableu_percentage, search[i]);
        printf("Population\n");
        forward(year, tableu_population, search[i]);
        putchar('\n');
        BackWard(year, tableu_population, search[i]);
        printf("-----\n");
    }
}

