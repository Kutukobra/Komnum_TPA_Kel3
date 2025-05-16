#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_LENGTH 256
#define MAX_DATA_POINTS 100
#define POLYNOMIAL_DEGREE 3  // Derajat polinomial for fitting

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

// Fungsi regresi polinomial
void polynomialRegression(DataPoint* data, int count, double* coefficients, int degree, int dataType) {
    int i, j, k, maxRow;
    double sum, x, y, ratio, maxVal, temp;
    
    // matrix augmented untuk metode Gauss-Jordan
    double** matrix = (double**)malloc((degree + 1) * sizeof(double*));
    for (i = 0; i <= degree; i++) {
        matrix[i] = (double*)malloc((degree + 2) * sizeof(double));
    }
    
    // Inisialisasi matrix 0
    for (i = 0; i <= degree; i++) {
        for (j = 0; j <= degree + 1; j++) {
            matrix[i][j] = 0.0;
        }
    }
    
    for (i = 0; i <= degree; i++) {
        for (j = 0; j <= degree; j++) {
            sum = 0.0;
            for (k = 0; k < count; k++) {
                // Normalisasi tahun 
                x = (double)(data[k].year - 2000) / 100.0;
                sum += pow(x, i + j);
            }
            matrix[i][j] = sum;
        }
        
        sum = 0.0;
        for (k = 0; k < count; k++) {
            // Normalisasi tahun 
            x = (double)(data[k].year - 2000) / 100.0;
            y = (dataType == 0) ? data[k].population : data[k].internetPercentage;
            sum += y * pow(x, i);
        }
        matrix[i][degree + 1] = sum;
    }
    
    // eliminasi Gauss-Jordan
    for (i = 0; i <= degree; i++) {
        // pivot maksimum
        maxRow = i;
        maxVal = fabs(matrix[i][i]);
        for (j = i + 1; j <= degree; j++) {
            if (fabs(matrix[j][i]) > maxVal) {
                maxVal = fabs(matrix[j][i]);
                maxRow = j;
            }
        }
        
        if (maxRow != i) {
            for (j = 0; j <= degree + 1; j++) {
                temp = matrix[i][j];
                matrix[i][j] = matrix[maxRow][j];
                matrix[maxRow][j] = temp;
            }
        }
        
        // Eliminasi
        for (j = 0; j <= degree; j++) {
            if (j != i) {
                ratio = matrix[j][i] / matrix[i][i];
                for (k = i; k <= degree + 1; k++) {
                    matrix[j][k] -= ratio * matrix[i][k];
                }
            }
        }
    }
    
    // koefisien
    for (i = 0; i <= degree; i++) {
        coefficients[i] = matrix[i][degree + 1] / matrix[i][i];
    }
    
    for (i = 0; i <= degree; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void displayPolynomialEquation(double* coefficients, int degree) {
    int i;
    int firstTerm = 1;
    
    printf("y = ");
    
    for (i = degree; i >= 0; i--) {
        if (fabs(coefficients[i]) > 1e-10) {
            if (!firstTerm && coefficients[i] > 0) {
                printf("+ ");
            } else if (!firstTerm && coefficients[i] < 0) {
                printf("- ");
                // nilai absolut 
                coefficients[i] = fabs(coefficients[i]);
            }
            
            // koefisien dan variabel dengan pangkat
            if (i == 0) {
                printf("%.4f", coefficients[i]);
            } else if (i == 1) {
                printf("%.4fx", coefficients[i]);
            } else {
                printf("%.4fx^%d ", coefficients[i], i);
            }
            
            firstTerm = 0;
        }
    }
    printf("\n");
}

int main() {
    DataPoint data[MAX_DATA_POINTS];
    int count, i, popCount, internetCount;
    
    count = readCSV("Data Tugas Pemrograman A.csv", data);
    
    if (count == 0) {
        printf("Tidak ada data yang dibaca.\n");
        return 1;
    }
    
    printf("Berhasil membaca %d data.\n", count);
    
    // Koefisien untuk regresi polinomial populasi
    double popCoefficients[POLYNOMIAL_DEGREE + 1];
    double internetCoefficients[POLYNOMIAL_DEGREE + 1];
    
    // Filter untuk populasi dan persentase internet
    DataPoint popData[MAX_DATA_POINTS];
    DataPoint internetData[MAX_DATA_POINTS];
    popCount = 0;
    internetCount = 0;
    
    for (i = 0; i < count; i++) {
        // data populasi
        popData[popCount] = data[i];
        popCount++;
        
        if (data[i].internetPercentage > 0) {
            internetData[internetCount] = data[i];
            internetCount++;
        }
    }
    
    // regresi polinomial untuk populasi
    polynomialRegression(popData, popCount, popCoefficients, POLYNOMIAL_DEGREE, 0);
    
    // Melakukan regresi polinomial untuk persentase internet
    polynomialRegression(internetData, internetCount, internetCoefficients, POLYNOMIAL_DEGREE, 1);
    
    // persamaan polinomial
    printf("\na. Persentase pengguna Internet Indonesia:\n");
    printf("   Persamaan: y = ax^3 + bx^2 + cx + d\n");
    printf("   x = (tahun - 2000) / 100\n");
    printf("   ");
    displayPolynomialEquation(internetCoefficients, POLYNOMIAL_DEGREE);
    
    printf("\nb. Pertumbuhan populasi Indonesia:\n");
    printf("   Persamaan: y = ax^3 + bx^2 + cx + d\n");
    printf("   x = (tahun - 2000) / 100\n");
    printf("   ");
    displayPolynomialEquation(popCoefficients, POLYNOMIAL_DEGREE);
    
    return 0;
}