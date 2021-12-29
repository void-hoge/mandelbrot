#pragma once

// #include <gmp.h>
// #include <gmpxx.h>
#include <complex>
// #include <mpreal.h>
#include "bmp.hpp"

const int precision = 256;

// using hogefloat_t = mpfr::mpreal;
using hogefloat_t = double;
// using hogefloat_t = mpf_class;

using complex_t = std::complex<hogefloat_t>;

int calc_mandelbrot(const int n, complex_t c);

int calc_julia(const int n, complex_t z);

void mandelbrot_AA(const complex_t center, const complex_t range, const unsigned width, const unsigned height, const int num_of_threads);

int make_mandelbrot_thread(const complex_t start, const complex_t resolution, const int num_of_threads, const int offset, const std::vector<pixel> gradation_waypoint, const int height, const int width, grid& data);

grid mandelbrot_bmp_multithread(const complex_t center, const complex_t range, const std::vector<pixel> gradation_waypoint, const unsigned width, const unsigned height, const int num_of_threads);

pixel gradation(const std::vector<pixel>& waypoint, double p);
