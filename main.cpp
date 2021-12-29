#include "mandelbrot.hpp"
#include "bmp.hpp"
#include <cstdlib>
#include <thread>
#include <iomanip>

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		std::cerr << "Enter the file name after the executable file." << '\n';
		std::abort();
	}
	// diverge
	std::vector<pixel> gradation_waypoint;
	gradation_waypoint.push_back(pixel(0x00,0x00,0x00));
	gradation_waypoint.push_back(pixel(0x00,0x00,0x80));
	gradation_waypoint.push_back(pixel(0x33,0xCC,0xCC));
	gradation_waypoint.push_back(pixel(0xFF,0xCC,0x00));
	gradation_waypoint.push_back(pixel(0xFF,0xFF,0xFF));
	// gradation_waypoint.push_back(pixel(0x00,0x00,0x00));
	// converge

	// mpfr::mpreal::set_default_prec(precision);
	// mpf_set_default_prec(precision);
	bmp_write file(argv[1]);
	const unsigned width = 2560;
	const unsigned height = 1600;
	const std::string re_str = "0.36024044343761436323612524444954530";
	const std::string im_str = "-0.64131306106480317486037501517930206";
	// mpf_class re(re_str);
	// mpf_class im(im_str);
	auto re = std::stod(re_str);
	auto im = std::stod(im_str);
	complex_t center(re, im);
	std::cout << std::setprecision(100) << center << '\n';
	// complex_t center(0,0);
	const std::string zoom = "0.000000000001";
	// hogefloat_t width_range(zoom);
	hogefloat_t width_range = std::stod(zoom);
	// hogefloat_t width_range = 4.0f;
	complex_t range(width_range, width_range*((hogefloat_t)height/width));
	grid data = mandelbrot_bmp_multithread(center, range, gradation_waypoint, width, height, std::thread::hardware_concurrency());
	file.set_data(data);
	file.write();
	return 0;
}
