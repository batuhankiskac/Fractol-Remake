#include "julia.h"

int julia(double x0, double y0, double cx, double cy, int max_iter) {
    double x = x0, y = y0;
    int iter = 0;
    while (x*x + y*y <= 4.0 && iter < max_iter) {
        double xt = x*x - y*y + cx;
        y = 2.0*x*y + cy;
        x = xt;
        iter++;
    }
    return iter;
}
