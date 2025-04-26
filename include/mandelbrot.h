#ifndef MANDELBROT_H
#define MANDELBROT_H

// Compute Mandelbrot iteration count for point (x0,y0)
// Returns number of iterations before escape (or max_iter)
int mandelbrot(double x0, double y0, int max_iter);

#endif // MANDELBROT_H
