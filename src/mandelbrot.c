#include "mandelbrot.h"

int mandelbrot(double x0, double y0, int max_iter) {
    double x = 0.0, y = 0.0;
    int iter = 0;
    while (x*x + y*y <= 4.0 && iter < max_iter) {
        double xt = x*x - y*y + x0;
        y = 2.0*x*y + y0;
        x = xt;
        iter++;
    }
    return iter;
}
