#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_LENGTH 256
#define MAX_DATA_POINTS 100
#define POLYNOMIAL_DEGREE 3

typedef struct {
    int year;
    double internetPercentage;
    double population;
} DataPoint;

int readCSV(const char* filename, DataPoint* data) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Tidak dapat membuka file %s\n", filename);
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    fgets(line, MAX_LINE_LENGTH, file); 

    while (fgets(line, MAX_LINE_LENGTH, file) && count < MAX_DATA_POINTS) {
        char* token = strtok(line, ",");
        if (token == NULL) continue;

        data[count].year = atoi(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        data[count].internetPercentage = atof(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        data[count].population = atof(token);

        count++;
    }

    fclose(file);
    return count;
}
// beda terbagi
void calculateDividedDifference(double x[], double y[], int n, double dd[][MAX_DATA_POINTS]) {
    int i, j;
    for (i = 0; i < n; i++) {
        dd[i][0] = y[i];
    }

    for (j = 1; j < n; j++) {
        for (i = 0; i < n - j; i++) {
            dd[i][j] = (dd[i+1][j-1] - dd[i][j-1]) / (x[i+j] - x[i]);
        }
    }
}

//koefisien newton to standard
void newtonToStandard(double x[], double dd[], int n, double coeffs[]) {
    double temp[MAX_DATA_POINTS];
    double new_temp[MAX_DATA_POINTS];
    int i, j;

    for (i = 0; i < n; i++) temp[i] = 0.0;
    temp[0] = 1.0;

    for (i = 0; i < n; i++) coeffs[i] = 0.0;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            coeffs[j] += dd[i] * temp[j];
        }

        for (j = 0; j < n; j++) {
            new_temp[j] = 0.0;
        }

        for (j = 0; j < n; j++) {
            if (j > 0) {
                new_temp[j] += temp[j-1];
            }
            new_temp[j] -= temp[j] * x[i];
        }

        for (j = 0; j < n; j++) {
            temp[j] = new_temp[j];
        }
    }
}

void printPolynomial(double coeffs[], int degree) {
    int i;
    printf("y = ");
    for (i = degree; i >= 0; i--) {
        if (i < degree) {
            if (coeffs[i] >= 0) {
                printf(" + ");
            } else {
                printf(" - ");
                coeffs[i] = -coeffs[i];
            }
        }

        if (i == 0) {
            printf("%.6f", coeffs[i]);
        } else if (i == 1) {
            printf("%.6f * x", coeffs[i]);
        } else {
            printf("%.6f * x^%d", coeffs[i], i);
        }
    }
    printf("\n");
}

int main() {
    DataPoint data[MAX_DATA_POINTS];
    int count = readCSV("Data Tugas Pemrograman A.csv", data);

    if (count == 0) {
        printf("Tidak ada data yang dibaca.\n");
        return 1;
    }

    int selected_points = 4;
    double pop_x[selected_points], pop_y[selected_points];
    double internet_x[selected_points], internet_y[selected_points];
    int i, j;

    // 4 titik terakhir untuk populasi
    for (i = 0; i < selected_points; i++) {
        int idx = count - selected_points + i;
        pop_x[i] = data[idx].year - 2000;
        pop_y[i] = data[idx].population;
    }

    // 4 titik terakhir untuk internet dengan persentase > 0
    int internet_count = 0;
    for (i = count - 1; i >= 0 && internet_count < selected_points; i--) {
        if (data[i].internetPercentage > 0) {
            internet_x[internet_count] = data[i].year - 2000;
            internet_y[internet_count] = data[i].internetPercentage;
            internet_count++;
        }
    }

    double pop_dd[MAX_DATA_POINTS][MAX_DATA_POINTS];
    double pop_coeffs[selected_points];
    double internet_dd[MAX_DATA_POINTS][MAX_DATA_POINTS];
    double internet_coeffs[selected_points];

    calculateDividedDifference(pop_x, pop_y, selected_points, pop_dd);
    newtonToStandard(pop_x, pop_dd[0], selected_points, pop_coeffs);
    printf("Persamaan polinomial untuk populasi:\n");
    printPolynomial(pop_coeffs, POLYNOMIAL_DEGREE);

    calculateDividedDifference(internet_x, internet_y, selected_points, internet_dd);
    newtonToStandard(internet_x, internet_dd[0], selected_points, internet_coeffs);
    printf("\nPersamaan polinomial untuk persentase internet:\n");
    printPolynomial(internet_coeffs, POLYNOMIAL_DEGREE);

    return 0;
}
