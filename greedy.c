#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define r 6371 //jari" bumi dalam km
#define MAX_CITIES 100 //jumlah maksimum kota yg dpt diproses
#define INF 1e9 //definisi untuk jarak minimum
#define M_PI 3.14159265358979323846

typedef struct{
    char name[100];
    double latitude;
    double longitude;
}City;

City cities[MAX_CITIES]; //array untuk data kota"
int numCities = 0; //jumlah kota yang sudah dibaca


//fungsi untuk menghitung jarak antara dua kota dengan titik koordinat dengan rumus Haversine
double haversineDistance(double lat1, double lon1, double lat2, double lon2){
    lat1 = lat1 * (M_PI / 180); //konversi koordinat latitude dan longitude ke radian
    lon1 = lon1 * (M_PI / 180);
    lat2 = lat2 * (M_PI / 180);
    lon2 = lon2 * (M_PI / 180);

    double dLat = lat2 - lat1; //perbedaan lintang dan bujur antara kedua titik
    double dLon = lon2 - lon1;

    double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2); //rumus Haversine
    double c = 2 * asin(sqrt(a)); //rumus Haversine

    return r * c; //menghitung jarak dalam km

}


//fungsi untuk membaca data kota dari file CSV
void readCSV(const char* filename){
    FILE* file =fopen(filename, "r");

    if(!file){
        perror("File tidak dapat dibuka");
        exit(EXIT_FAILURE);
    }

    char line[256]; //array untuk menyimpan baris yang dibaca dari file
    while(fgets(line,sizeof(line), file)){
        sscanf(line, "%[^,], %lf, %lf", cities[numCities].name, &cities[numCities].latitude, &cities[numCities].longitude);
        numCities++;
    }

    fclose(file);
}


//fungsi untuk mencari indeks kota berdasarkan nama
int findCityIndex(const char* name){
    for(int i = 0; i < numCities; i++){
        if(strcmp(cities[i].name, name) == 0){
            return i;
        }
    }

    return -1;
}


//fungsi untuk menghitung jarak total rute yang ditemukan
double calculateTotalDistance(int path[], int n){
    double totalDistance = 0;
    for(int i = 0; i < n - 1; i++){
        totalDistance += haversineDistance(cities[path[i]].latitude, cities[path[i]].longitude, cities[path[i + 1]].latitude, cities[path[i + 1]].longitude);
    }

    //untuk jarak dari kota terakhir ke kota awal
    totalDistance += haversineDistance(cities[path[n - 1]].latitude, cities[path[n - 1]].longitude, cities[path[0]].latitude, cities[path[0]].longitude);

    return totalDistance;
}


//fungsi untuk print rute perjalanan
void printRoute(int path[], int n){
    for(int i = 0; i < n; i++){
        printf("%s -> ", cities[path[i]].name);
    }

    printf("%s\n", cities[path[0]].name);
}


//fungsi untuk mencari rute terpendek dengan algoritma Greedy
void shortestPathGreedy(int startIndex){
    clock_t start = clock(); //waktu dimulainya eksekusi

    int visited[MAX_CITIES] = {0}; //array untuk menandai kota yang sudah dikunjungi
    int path[MAX_CITIES]; //array untuk menyimpan rute yang ditemukan
    double minDistance = INF; //inisialisasi jarak minimum dengan nilai yang sudah di define
    int shortestPath[MAX_CITIES]; //array untuk menyimpan rute terpendek

    //inisialisasi array path
    for(int i = 0; i < numCities; i++){
        path[i] = -1;
    }

    path[0] = startIndex; //dimulai dari kota awal
    visited[startIndex] = 1;

    //pencarian rute dengan algoritma Greedy
    for(int step = 1; step < numCities; step++){
        double minDist = INF;
        int nextCity = -1;
        for(int j = 0; j < numCities; j++){
            if(!visited[j]){
                double dist = haversineDistance(cities[path[step - 1]].latitude, cities[path[step - 1]].longitude, cities[path[j]].latitude, cities[path[j]].longitude);
                if(dist < minDist){
                    minDist = dist;
                    nextCity = j;
                }
            }
        }

        path[step] = nextCity;
        visited[nextCity] = 1;
    }

    //jarak total rute yang ditemukan
    double totalDist = calculateTotalDistance(path, numCities);
    if(totalDist , minDistance){
        minDistance = totalDist;
        memcpy(shortestPath, path, sizeof(path));
    }

    //menampilkan rute terpendek beserta jaraknya
    printf("Best route found:\n");
    printRoute(shortestPath, numCities);
    printf("Best route distancek: %.2lf km\n", minDistance);

    clock_t end = clock(); //waktu berakhirnya eksekusi

    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time elapsed: %.10lf seconds\n", timeSpent);
}


int main()
{
    char filename[100];
    printf("Enter list of cities file name: ");
    scanf("%s", filename);

    strcat(filename, ".csv"); //menambahkan .csv pada nama file yang dimasukkan

    readCSV(filename);

    char startingCity[100];
    printf("Enter starting point: ");
    scanf("%s", startingCity);

    int startIndex = findCityIndex(startingCity); //mencari index kota awal
    if(startIndex == -1){
        fprintf(stderr, "Kota tidak ditemukan\n", startingCity);
        return EXIT_FAILURE;
    }

    shortestPathGreedy(startIndex);

    return 0;
}

