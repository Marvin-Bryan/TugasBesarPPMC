#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>

#define PI 3.14159265358979323846
#define MAX_KOTA 100
#define MAX_SEMUT 10
#define ALPHA 1.0
#define BETA 2.0
#define RHO 0.5 // Laju penguapan feromon
#define Q 100.0 // Faktor deposit feromon

typedef struct {
    char nama[100];
    double lat;
    double lon;
} Kota;

double haversine(double lat1, double lon1, double lat2, double lon2) {
    // Menghitung jarak antara dua titik berdasarkan latitude dan longitude
    double dLat = (lat2 - lat1) * PI / 180.0;
    double dLon = (lon2 - lon1) * PI / 180.0;

    // Konversi nilai latitude ke radian
    lat1 = (lat1) * PI / 180.0;
    lat2 = (lat2) * PI / 180.0;

    // Rumus haversine
    double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double rad = 6371; // radius bumi dalam kilometer
    double c = 2 * asin(sqrt(a));
    return rad * c; // Mengembalikan jarak dalam kilometer
}

// Fungsi untuk menghitung total panjang tur
double panjangTur(int *tur, Kota *kota, int numKota) {
    double panjang = 0.0;
    for (int i = 0; i < numKota - 1; i++) {
        panjang += haversine(kota[tur[i]].lat, kota[tur[i]].lon, kota[tur[i+1]].lat, kota[tur[i+1]].lon);
    }
    panjang += haversine(kota[tur[numKota-1]].lat, kota[tur[numKota-1]].lon, kota[tur[0]].lat, kota[tur[0]].lon); // Kembali ke kota awal
    return panjang;
}

void ACO(Kota *kota, int numKota, int numSemut, int numIterasi, double feromon[][MAX_KOTA]) {
    int semut[numSemut][MAX_KOTA]; // Tur setiap semut
    double panjangs[numSemut]; // Panjang tur setiap semut
    int turTerbaik[MAX_KOTA]; // Tur terbaik yang ditemukan
    double panjangTerbaik = DBL_MAX; // Panjang tur terbaik
    double penguapan = 1.0 - RHO; // Faktor penguapan feromon

    // Inisialisasi tingkat feromon
    for (int i = 0; i < numKota; i++) {
        for (int j = 0; j < numKota; j++) {
            feromon[i][j] = 0.1; // Inisialisasi feromon dengan nilai awal kecil
        }
    }

    // Loop utama
    for (int iter = 0; iter < numIterasi; iter++) { // Iterasi selama jumlah iterasi yang ditentukan
        // Pergerakan setiap semut ke kota yang dikunjungi
        for (int k = 0; k < numSemut; k++) { // Untuk setiap semut
            int dikunjungi[numKota]; // Array untuk melacak kota yang sudah dikunjungi oleh semut
            memset(dikunjungi, 0, sizeof(dikunjungi)); // Inisialisasi array dikunjungi dengan 0
            // Set kota pertama
            semut[k][0] = 0; // Mulai dari kota pertama
            dikunjungi[0] = 1; // Tandai kota pertama sebagai dikunjungi
            // Pilih kota awal secara acak jika lebih dari satu kota
            if (numKota > 1) {
                int indeksAcak = rand() % (numKota - 1) + 1; // Kecuali kota pertama
                semut[k][1] = indeksAcak; // Kota kedua dipilih secara acak
                dikunjungi[indeksAcak] = 1; // Tandai kota kedua sebagai dikunjungi
            }
            // Selesaikan tur
            for (int langkah = 2; langkah < numKota; langkah++) { // Untuk setiap langkah dalam tur
                int kotaSekarang = semut[k][langkah-1]; // Kota saat ini
                double total = 0.0;
                for (int kotaSelanjutnya = 0; kotaSelanjutnya < numKota; kotaSelanjutnya++) {
                    if (!dikunjungi[kotaSelanjutnya]) { // Jika kota belum dikunjungi
                        // Hitung probabilitas berdasarkan tingkat feromon dan jarak
                        total += pow(feromon[kotaSekarang][kotaSelanjutnya], ALPHA) *
                                 pow(1.0 / haversine(kota[kotaSekarang].lat, kota[kotaSekarang].lon, kota[kotaSelanjutnya].lat, kota[kotaSelanjutnya].lon), BETA);
                    }
                }
                double r = ((double) rand() / RAND_MAX) * total; // Pilih nilai acak antara 0 dan total probabilitas
                double sum = 0.0;
                int kotaSelanjutnya = 0;
                while (kotaSelanjutnya < numKota) {
                    if (!dikunjungi[kotaSelanjutnya]) { // Jika kota belum dikunjungi
                        // Hitung probabilitas kumulatif
                        sum += pow(feromon[kotaSekarang][kotaSelanjutnya], ALPHA) *
                               pow(1.0 / haversine(kota[kotaSekarang].lat, kota[kotaSekarang].lon, kota[kotaSelanjutnya].lat, kota[kotaSelanjutnya].lon), BETA);
                        if (sum >= r) break; // Pilih kota jika sum lebih besar atau sama dengan nilai acak
                    }
                    kotaSelanjutnya++;
                }
                semut[k][langkah] = kotaSelanjutnya; // Set kota berikutnya dalam tur semut
                dikunjungi[kotaSelanjutnya] = 1; // Tandai kota sebagai dikunjungi
            }
            // Hitung panjang tur
            panjangs[k] = panjangTur(semut[k], kota, numKota); // Hitung panjang tur yang ditempuh semut
            // Perbarui tingkat feromon
            for (int i = 0; i < numKota - 1; i++) {
                // Perbarui feromon berdasarkan panjang tur yang ditemukan
                feromon[semut[k][i]][semut[k][i+1]] = penguapan * feromon[semut[k][i]][semut[k][i+1]] + Q / panjangs[k];
                feromon[semut[k][i+1]][semut[k][i]] = penguapan * feromon[semut[k][i+1]][semut[k][i]] + Q / panjangs[k];
            }
            feromon[semut[k][numKota-1]][semut[k][0]] = penguapan * feromon[semut[k][numKota-1]][semut[k][0]] + Q / panjangs[k];
            feromon[semut[k][0]][semut[k][numKota-1]] = penguapan * feromon[semut[k][0]][semut[k][numKota-1]] + Q / panjangs[k];
            // Perbarui tur terbaik
            if (panjangs[k] < panjangTerbaik) { // Jika panjang tur semut lebih baik dari yang terbaik
                panjangTerbaik = panjangs[k]; // Perbarui panjang tur terbaik
                memcpy(turTerbaik, semut[k], sizeof(turTerbaik)); // Salin tur semut ke tur terbaik
            }
        }
    }
    // Output hasil
    printf("Panjang Minimal: %.2f\n", panjangTerbaik);
    printf("Best Route Found:\n");
    for (int i = 0; i < numKota; i++)
        printf("%s -> ", kota[turTerbaik[i]].nama); // Tampilkan tur terbaik
    printf("%s\n", kota[turTerbaik[0]].nama); // Kembali ke kota awal
}



// Program Utama
int main() {
    Kota kota[MAX_KOTA];
    char namaFile[100];
    char kotaAwal[100];
    char buf[100];
    int i = 0;
    
    clock_t mulai, akhir;
    double waktu_penggunaan_cpu;

    printf("Masukkan nama File yang ingin dibuka: ");
    scanf("%s", namaFile);
    printf("Masukkan nama Kota untuk memulai perjalanan: ");
    scanf("%s", kotaAwal);

    mulai = clock();

    FILE* ptr = fopen(namaFile, "r");
    if (ptr == NULL) {
        printf("Tidak ada file tersebut.\n");
        return 0;
    }

    // Baca dan cetak setiap baris file CSV
    while (fgets(buf, sizeof(buf), ptr) != NULL) {
        char* token = strtok(buf, ",");
        strcpy(kota[i].nama, token);
        
        token = strtok(NULL, ",");
        if (token != NULL)
            kota[i].lat = atof(token);
        
        token = strtok(NULL, ",");
        if (token != NULL)
            kota[i].lon = atof(token);
        
        //printf("%s %lf %lf\n", kota[i].nama, kota[i].lat, kota[i].lon);
        i++;
    }
    fclose(ptr);
    
    int numKota = i;

    // Temukan indeks kota awal
    int indeksAwal = -1;
    for (int j = 0; j < numKota; j++) {
        if (strcmp(kota[j].nama, kotaAwal) == 0) {
            indeksAwal = j;
            break;
        }
    }

    if (indeksAwal == -1) {
        printf("Kota awal tidak ditemukan dalam data.\n");
        return 0;
    }

    // Tukar kota awal ke posisi pertama
    Kota temp = kota[indeksAwal];
    kota[indeksAwal] = kota[0];
    kota[0] = temp;

    double feromon[MAX_KOTA][MAX_KOTA];

    // Panggil fungsi ACO untuk mencari jalur terpendek
    ACO(kota, numKota, MAX_SEMUT, 100, feromon);

    akhir = clock();
    
    waktu_penggunaan_cpu = ((double) (akhir - mulai)) / CLOCKS_PER_SEC;
    printf("Waktu yang dibutuhkan untuk menjalankan program: %f detik.\n", waktu_penggunaan_cpu);

    return 0;
}
