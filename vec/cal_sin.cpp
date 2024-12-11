/*
  from https://zhuanlan.zhihu.com/p/8129089606.
*/
#include <chrono>
#include <cstdio>
#include <immintrin.h> // Intel 内联函数头文件，包含 SSE/AVX/AVX2 等 SIMD 指令集的内联函数
#include <omp.h> // OpenMP 头文件，用于支持多线程并行编程， #pragma omp parallel 和 #pragma omp for 指令需要该头文件

void sinx_scalar(int N, int terms, float *x, float *y) {
  for (int i = 0; i < N; i++) {
    float value = x[i];
    float numer = x[i] * x[i] * x[i];
    int denom = 6; // 3!
    int sign = -1;
    for (int j = 1; j <= terms; j++) {
      value += sign * numer / denom;
      numer *= x[i] * x[i];
      denom *= (2 * j + 2) * (2 * j + 3);
      sign *= -1;
    }
    y[i] = value;
  }
}

void sinx_avx(int N, int terms, float *x, float *y) {
  int i;
  for (i = 0; i < N - 7; i += 8) {
    // 从内存加载 8 个 float 到 256 位向量寄存器
    __m256 x_vec = _mm256_loadu_ps(&x[i]);
    // 初始化 value 向量为 x_vec
    __m256 value = x_vec;
    // 计算 x^3 向量
    __m256 x_n = _mm256_mul_ps(_mm256_mul_ps(x_vec, x_vec), x_vec);
    // 创建一个所有元素都是 6.0f 的向量
    __m256 denom = _mm256_set1_ps(6.0f);
    // 创建一个所有元素都是 -1.0f 的向量
    __m256 sign = _mm256_set1_ps(-1.0f);

    for (int j = 1; j <= terms; j++) {
      // 计算当前项：sign * numer / denom
      __m256 term = _mm256_div_ps(_mm256_mul_ps(sign, x_n), denom);
      // 将当前项加到 value 上
      value = _mm256_add_ps(value, term);
      // 更新 numer：numer *= x^2
      x_n = _mm256_mul_ps(x_n, _mm256_mul_ps(x_vec, x_vec));
      // 更新 denom：denom *= (2j+2) * (2j+3)
      denom = _mm256_mul_ps(denom, _mm256_set1_ps((2 * j + 2) * (2 * j + 3)));
      // 翻转 sign 的符号
      sign = _mm256_xor_ps(sign, _mm256_set1_ps(-0.0f));
    }

    // 将计算结果存回内存
    _mm256_storeu_ps(&y[i], value);
  }

  // 处理剩余的元素
  for (; i < N; i++) {
    float value = x[i];
    float numer = x[i] * x[i] * x[i];
    int denom = 6; // 3!
    int sign = -1;
    for (int j = 1; j <= terms; j++) {
      value += sign * numer / denom;
      numer *= x[i] * x[i];
      denom *= (2 * j + 2) * (2 * j + 3);
      sign *= -1;
    }
    y[i] = value;
  }
}

void sinx_avx_parallel(int N, int terms, float *x, float *y) {
#pragma omp parallel // 创建一个并行区域，默认创建与 CPU 核心数相同的线程
  {
#pragma omp for // 并行化 for 循环：不同的线程会同时处理不同的 i 值区间
    // 例如有 4 个线程时的处理方式：
    // 线程 0 处理：i = 0,  32, 64, ...
    // 线程 1 处理：i = 8,  40, 72, ...
    // 线程 2 处理：i = 16, 48, 80, ...
    // 线程 3 处理：i = 24, 56, 88, ...
    for (int i = 0; i < N - 7; i += 8) {
      // 每个线程内部使用 AVX 并行处理 8 个数据
      // 实现了两级并行：
      // 1 级：OpenMP 多线程并行（不同线程处理不同的 i 区间）
      // 2 级：AVX SIMD 并行（每个线程内同时处理 8 个数）
      __m256 x_vec = _mm256_loadu_ps(&x[i]); // 加载 8 个连续的 float 数
      __m256 value = x_vec;
      __m256 x_n = _mm256_mul_ps(_mm256_mul_ps(x_vec, x_vec), x_vec);
      __m256 denom = _mm256_set1_ps(6.0f);
      __m256 sign = _mm256_set1_ps(-1.0f);

      for (int j = 1; j <= terms; j++) {
        __m256 term = _mm256_div_ps(_mm256_mul_ps(sign, x_n), denom);
        value = _mm256_add_ps(value, term);
        x_n = _mm256_mul_ps(x_n, _mm256_mul_ps(x_vec, x_vec));
        denom = _mm256_mul_ps(denom, _mm256_set1_ps((2 * j + 2) * (2 * j + 3)));
        sign = _mm256_xor_ps(sign, _mm256_set1_ps(-0.0f));
      }

      _mm256_storeu_ps(&y[i], value); // 存储 8 个结果
    }

// 处理剩余元素（不能被 8 整除的部分）
#pragma omp for // 剩余元素也使用多线程并行处理
    for (int i = N - (N % 8); i < N; i++) {
      // 例如 N=1027 时，最后 3 个元素 (1024,1025,1026) 会被不同线程并行处理
      float value = x[i];
      float numer = x[i] * x[i] * x[i];
      int denom = 6;
      int sign = -1;
      for (int j = 1; j <= terms; j++) {
        value += sign * numer / denom;
        numer *= x[i] * x[i];
        denom *= (2 * j + 2) * (2 * j + 3);
        sign *= -1;
      }
      y[i] = value;
    }
  }
}

int main() {
  const int N = 1000000;
  const int terms = 10;
  float *x = new float[N];
  float *y_scalar = new float[N];
  float *y_avx = new float[N];

  // 初始化输入数组
  for (int i = 0; i < N; i++) {
    x[i] = static_cast<float>(i) / N; // C++类型转换
  }

  // 测量标量版本的性能
  auto start = std::chrono::high_resolution_clock::now();
  sinx_scalar(N, terms, x, y_scalar);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration_scalar =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  // 测量 AVX 版本的性能
  start = std::chrono::high_resolution_clock::now();
  sinx_avx(N, terms, x, y_avx);
  end = std::chrono::high_resolution_clock::now();
  auto duration_avx =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  // 测量并行 AVX 版本的性能
  float *y_avx_parallel = new float[N];
  start = std::chrono::high_resolution_clock::now();
  sinx_avx_parallel(N, terms, x, y_avx_parallel);
  end = std::chrono::high_resolution_clock::now();
  auto duration_avx_parallel =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  // 打印结果
  printf("标量版本耗时：%ld 微秒\n", duration_scalar.count());
  printf("AVX 版本耗时：%ld 微秒\n", duration_avx.count());
  printf("并行 AVX 版本耗时：%ld 微秒\n", duration_avx_parallel.count());
  printf("加速比：%.2f\n",
         static_cast<float>(duration_scalar.count()) / duration_avx.count());
  printf("并行 AVX 加速比：%.2f\n",
         static_cast<float>(duration_scalar.count()) /
             duration_avx_parallel.count());

  // 验证结果
  for (int i = 0; i < N; i++) {
    if (std::abs(y_scalar[i] - y_avx[i]) > 1e-5) {
      printf("结果不匹配在索引 %d: 标量 = %f, AVX = %f\n", i, y_scalar[i],
             y_avx[i]);
      break;
    }
  }

  // 清理内存
  delete[] x;
  delete[] y_scalar;
  delete[] y_avx;

  return 0;
}