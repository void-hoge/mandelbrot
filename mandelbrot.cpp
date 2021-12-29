#include "mandelbrot.hpp"
#include "bmp.hpp"
#include <limits>
#include <vector>
#include <cstdlib>
#include <future>
#include <chrono>
// #include <mpfr.h>
// #include <mpreal.h>

inline int calc_mandelbrot(const int n, complex_t c){
	complex_t z(0.0f, 0.0f);
	for (size_t i = 0; i < n; i++){
		z = complex_t(z.real()*z.real()-z.imag()*z.imag(), z.real()*z.imag()*2);
		z += c;
		if (z.real()*z.real()+z.imag()*z.imag() > 4.0f) {
			return i;
		}
	}
	return n;
}

int calc_julia(const int n, complex_t z){
	complex_t c(-0.21f, -0.65f);
	for (size_t i = 0; i < n; i++){
		// z = std::pow(z, 2)+c;
		// z = z*z+c;
		if (abs(z) == std::numeric_limits<hogefloat_t>::infinity()){
			return i;
		}
	}
	return n;
}

void mandelbrot_AA(const complex_t center, const complex_t range, const unsigned width, const unsigned height) {
	const complex_t resolution(range.real()/width, range.imag()/height);
	const complex_t cmpl(center.real()-range.real()/2, center.imag()-range.imag()/2);
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			complex_t hoge(cmpl.real()+j*resolution.real(), cmpl.imag()+i*resolution.imag());
			std::cout << (char)(calc_mandelbrot('~'-' ', hoge)+' ');
		}
		std::cout << std::endl;
	}
}

pixel gradation(const std::vector<pixel>& waypoint, const double p) {
	for (int i = 0; i < waypoint.size()-1; i++) {
		if (((double)(i+1)/(waypoint.size()-1)) > p) {
			double q = p - (double)i/(waypoint.size()-1);
			q /= (double)1/(waypoint.size()-1);
			pixel a;
			a.r = waypoint[i].r + (waypoint[i+1].r-waypoint[i].r)*q;
			a.g = waypoint[i].g + (waypoint[i+1].g-waypoint[i].g)*q;
			a.b = waypoint[i].b + (waypoint[i+1].b-waypoint[i].b)*q;
			return a;
		}
	}
	return *(waypoint.end()-1);
}

inline pixel hsv2rgb(int h, double s, double v) {
	double h_ = (double)(h%60)/60;
	uint8_t a = 255*v;
 	uint8_t b = 255*v*(1-s);
	uint8_t c = 255*v*(1-s*h_);
	uint8_t d = 255*v*(1-s*(1-h_));
	if (s == 0) {
		return pixel(a,a,a);
	}else if (0 <= h && h < 60) {
		return pixel(a,d,b);
	}else if (60 <= h && h < 120) {
		return pixel(c,a,b);
	}else if (120 <= h && h < 180) {
		return pixel(b,a,d);
	}else if (180 <= h && h < 240) {
		return pixel(b,c,a);
	}else if (240 <= h && h < 300) {
		return pixel(d,b,a);
	}else /*if (300 <= h && h < 360)*/ {
		return pixel(a,b,c);
	}
}

int make_mandelbrot_thread(const complex_t start_all, const complex_t resolution, const int num_of_threads, const int offset, const std::vector<pixel> gradation_waypoint, const int height, const int width, grid& data) {
	const complex_t start = complex_t(start_all.real()+resolution.real()*offset, start_all.imag());
	for (int i = 0; i < width/num_of_threads; i++) {
		std::cerr << i << '\n';
		for (int j = 0; j < height; j++) {
			const complex_t point(start.real()+resolution.real()*num_of_threads*i, start.imag()+resolution.imag()*j);
			const int num = calc_mandelbrot(10000000, point);
			// data[i][j] = gradation(gradation_waypoint, (double)num/0xFF);
			if (num == 10000000) {
				data[i][j] = pixel(0,0,0);
			}else {
				data[i][j] = hsv2rgb((num*2)%360, 1.0f, 1.0f);
			}
		}
	}
	return 0;
}

grid mandelbrot_bmp_multithread(const complex_t center, const complex_t range, const std::vector<pixel> gradation_waypoint, const unsigned width, const unsigned height, const int num_of_threads) {
	try {
		if ((width % num_of_threads) != 0) {
			throw std::runtime_error("num_of_threads(%d) must be a divisor of width(%d).\n");
		}
	}
	catch (std::exception &e){
		std::cerr << "Error: " << e.what() << '\n';
		std::abort();
	}

	const complex_t resolution(range.real()/width, range.imag()/height);
	const complex_t start(center.real()-range.real()/2, center.imag()-range.imag()/2);

	std::vector<grid> thread_data;
	thread_data.resize(num_of_threads);
	for (auto &a: thread_data) {
		a.resize(width/num_of_threads);
		for (auto &b: a) {
			b.resize(height);
		}
	}
	std::vector<std::future<int>> v;
	for (int i = 0; i < num_of_threads; i++) {
		v.push_back(std::async(std::launch::async, [=, &thread_data]{
			return make_mandelbrot_thread(start, resolution, num_of_threads, i, gradation_waypoint, height, width, std::ref(thread_data[i]));
		}));
	}
	for (int i = 0; i < v.size(); i++) {
		v[i].get();
	}

	grid data;
	data.resize(height);
	for (int i = 0; i < data.size(); i++) {
		data[i].resize(width);
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			data[i][j] = thread_data[j%num_of_threads][j/num_of_threads][i];
		}
	}

	return data;
}
