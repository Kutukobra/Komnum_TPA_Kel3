#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_LENGTH 256
#define MAX_DATA_POINTS 100
#define POLYNOMIAL_DEGREE 3  // Derajat polinomial untuk fitting

// Struktur untuk menyimpan data dari CSV
typedef struct {
    int year;
    double internetPercentage;
    double population;
} DataPoint;

// Fungsi untuk membaca data dari file CSV
int readCSV(const char* filename, DataPoint* data) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Tidak dapat membuka file %s\n", filename);
        return 0;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    // Membaca header dan mengabaikannya
    fgets(line, MAX_LINE_LENGTH, file);
    
    // Membaca data
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

// Fungsi untuk melakukan regresi polinomial
void polynomialRegression(DataPoint* data, int count, double* coefficients, int degree, int dataType) {
    // Membuat matrix augmented untuk metode Gauss-Jordan
    double** matrix = (double**)malloc((degree + 1) * sizeof(double*));
    for (int i = 0; i <= degree; i++) {
        matrix[i] = (double*)malloc((degree + 2) * sizeof(double));
    }
    
    // Inisialisasi matrix dengan 0
    for (int i = 0; i <= degree; i++) {
        for (int j = 0; j <= degree + 1; j++) {
            matrix[i][j] = 0.0;
        }
    }
    
    // Mengisi matrix sesuai persamaan normal
    for (int i = 0; i <= degree; i++) {
        for (int j = 0; j <= degree; j++) {
            double sum = 0.0;
            for (int k = 0; k < count; k++) {
                // Normalisasi tahun untuk meningkatkan stabilitas numerik
                double x = (double)(data[k].year - 2000) / 100.0;
                sum += pow(x, i + j);
            }
            matrix[i][j] = sum;
        }
        
        double sum = 0.0;
        for (int k = 0; k < count; k++) {
            // Normalisasi tahun untuk meningkatkan stabilitas numerik
            double x = (double)(data[k].year - 2000) / 100.0;
            double y = (dataType == 0) ? data[k].population : data[k].internetPercentage;
            sum += y * pow(x, i);
        }
        matrix[i][degree + 1] = sum;
    }
    
    // Melakukan eliminasi Gauss-Jordan
    for (int i = 0; i <= degree; i++) {
        // Mencari pivot maksimum
        int maxRow = i;
        double maxVal = fabs(matrix[i][i]);
        for (int j = i + 1; j <= degree; j++) {
            if (fabs(matrix[j][i]) > maxVal) {
                maxVal = fabs(matrix[j][i]);
                maxRow = j;
            }
        }
        
        // Menukar baris jika perlu
        if (maxRow != i) {
            for (int j = 0; j <= degree + 1; j++) {
                double temp = matrix[i][j];
                matrix[i][j] = matrix[maxRow][j];
                matrix[maxRow][j] = temp;
            }
        }
        
        // Eliminasi
        for (int j = 0; j <= degree; j++) {
            if (j != i) {
                double ratio = matrix[j][i] / matrix[i][i];
                for (int k = i; k <= degree + 1; k++) {
                    matrix[j][k] -= ratio * matrix[i][k];
                }
            }
        }
    }
    
    // Mendapatkan koefisien
    for (int i = 0; i <= degree; i++) {
        coefficients[i] = matrix[i][degree + 1] / matrix[i][i];
    }
    
    // Pembersihan memory
    for (int i = 0; i <= degree; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Fungsi untuk menghitung nilai berdasarkan koefisien polinomial
double calculatePolynomial(double* coefficients, int degree, int year) {
    double x = (double)(year - 2000) / 100.0;  // Normalisasi tahun
    double result = 0.0;
    for (int i = 0; i <= degree; i++) {
        result += coefficients[i] * pow(x, i);
    }
    return result;
}

int main() {
    DataPoint data[MAX_DATA_POINTS];
    int count = readCSV("Data Tugas Pemrograman A.csv", data);
    
    if (count == 0) {
        printf("Tidak ada data yang dibaca.\n");
        return 1;
    }
    
    printf("Berhasil membaca %d data.\n", count);
    
    // Koefisien untuk regresi polinomial populasi
    double popCoefficients[POLYNOMIAL_DEGREE + 1];
    double internetCoefficients[POLYNOMIAL_DEGREE + 1];
    
    // Filter data untuk populasi dan persentase internet
    DataPoint popData[MAX_DATA_POINTS];
    DataPoint internetData[MAX_DATA_POINTS];
    int popCount = 0;
    int internetCount = 0;
    
    for (int i = 0; i < count; i++) {
        // Menyalin data populasi
        popData[popCount] = data[i];
        popCount++;
        
        // Menyalin data persentase internet (hanya data yang valid)
        if (data[i].internetPercentage > 0) {
            internetData[internetCount] = data[i];
            internetCount++;
        }
    }
    
    // Melakukan regresi polinomial untuk populasi
    polynomialRegression(popData, popCount, popCoefficients, POLYNOMIAL_DEGREE, 0);
    
    // Melakukan regresi polinomial untuk persentase internet
    polynomialRegression(internetData, internetCount, internetCoefficients, POLYNOMIAL_DEGREE, 1);
    
    // Menampilkan persamaan polinomial untuk populasi
    printf("\nPersamaan polinomial untuk populasi:\n");
    printf("y = ");
    for (int i = POLYNOMIAL_DEGREE; i >= 0; i--) {
        if (i == 0) {
            printf("%.6lf\n", popCoefficients[i]);
        } else {
            printf("%.6lf * x^%d + ", popCoefficients[i], i);
        }
    }
    
    // Menampilkan persamaan polinomial untuk persentase internet
    printf("\nPersamaan polinomial untuk persentase internet:\n");
    printf("y = ");
    for (int i = POLYNOMIAL_DEGREE; i >= 0; i--) {
        if (i == 0) {
            printf("%.6lf\n", internetCoefficients[i]);
        } else {
            printf("%.6lf * x^%d + ", internetCoefficients[i], i);
        }
    }
    
    // Estimasi populasi di tahun 2030
    double estimatedPop2030 = calculatePolynomial(popCoefficients, POLYNOMIAL_DEGREE, 2030);
    printf("\nEstimasi populasi Indonesia di tahun 2030: %.0lf jiwa\n", estimatedPop2030);
    
    // Estimasi persentase internet di tahun 2035
    double estimatedInternetPercentage2035 = calculatePolynomial(internetCoefficients, POLYNOMIAL_DEGREE, 2035);
    
    // Batasi persentase maksimum 100%
    if (estimatedInternetPercentage2035 > 100.0) {
        estimatedInternetPercentage2035 = 100.0;
    }
    
    // Estimasi populasi di tahun 2035
    double estimatedPop2035 = calculatePolynomial(popCoefficients, POLYNOMIAL_DEGREE, 2035);
    
    // Estimasi jumlah pengguna internet di tahun 2035
    double estimatedInternetUsers2035 = estimatedPop2035 * (estimatedInternetPercentage2035 / 100.0);
    
    printf("Estimasi persentase pengguna internet Indonesia di tahun 2035: %.2lf%%\n", estimatedInternetPercentage2035);
    printf("Estimasi populasi Indonesia di tahun 2035: %.0lf jiwa\n", estimatedPop2035);
    printf("Estimasi jumlah pengguna internet di Indonesia di tahun 2035: %.0lf jiwa\n", estimatedInternetUsers2035);
    
    return 0;
}