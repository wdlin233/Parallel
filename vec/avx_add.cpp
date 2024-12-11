/*
    from https://zhuanlan.zhihu.com/p/72953129.
    if I use `-march=native`, it would be better than `-march=skylake-avx512`.
*/
#include <immintrin.h>
#include <iostream>
#include <chrono>
#include <ctime> 

const int N = 8;
const int loop_num = 100000000;
float gemfield_i[8] = {1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8};
float gemfield_m[8] = {2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9};
float gemfield_a[8] = {11.1,12.2,13.3,14.4,15.5,16.6,17.7,18.8};
float gemfield_o[8] = {0};

__m256 gemfield_v_i = _mm256_set_ps(8.8,7.7,6.6,5.5,4.4,3.3,2.2,1.1);
__m256 gemfield_v_m = _mm256_set_ps(9.9,8.8,7.7,6.6,5.5,4.4,3.3,2.2);
__m256 gemfield_v_a = _mm256_set_ps(18.8,17.7,16.6,15.5,14.4,13.3,12.2,11.1);
__m256 gemfield_v_o = _mm256_set_ps(0,0,0,0,0,0,0,0);


void syszuxMulAndAddV() {
    auto start = std::chrono::system_clock::now();
    for(int j=0; j<loop_num; j++){
        gemfield_v_o += _mm256_fmadd_ps(gemfield_v_i, gemfield_v_m, gemfield_v_a);
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "resultV: ";
    // float* f = (float*)&gemfield_v_o;
    for(int i=0; i<N; i++){
        std::cout<<gemfield_v_o[i]<<" ";
    }
    std::cout<< "\nelapsed time: " << elapsed_seconds.count() << "s\n";
}

void syszuxMulAndAdd(){
    auto start = std::chrono::system_clock::now();
    for(int j=0; j<loop_num; j++){
        for(int i=0; i<N; i++){
            gemfield_o[i] += gemfield_i[i] * gemfield_m[i] + gemfield_a[i];
        }
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "result: ";
    for(int i=0; i<8; i++){
        std::cout<<gemfield_o[i]<<" ";
    }
    std::cout<< "\nelapsed time: " << elapsed_seconds.count() << "s\n";
}

int main() {
    syszuxMulAndAdd();
    syszuxMulAndAddV();
    return 0;
}