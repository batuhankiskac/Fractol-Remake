#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <stdlib.h>
#include "color.h"

void init_palette(GraphicsContext *ctx) {
    int n = ctx->palette_size;
    // free existing palette if any
    if (ctx->palette) free(ctx->palette);
    ctx->palette = malloc(n * sizeof(unsigned long));
    // generate palette based on palette_index
    if (ctx->palette_index == 0) { // Rainbow swirl
        for (int i = 0; i < n; i++) {
            double t = (double)i / n;
            unsigned char r = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t)));
            unsigned char g = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t + 2.094)));
            unsigned char b = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t + 4.188)));
            ctx->palette[i] = (r << 16) | (g << 8) | b;
        }
    } else if (ctx->palette_index == 1) { // Grayscale
        for (int i = 0; i < n; i++) {
            unsigned char v = (unsigned char)(255 * i / (n - 1));
            ctx->palette[i] = (v << 16) | (v << 8) | v;
        }
    } else if (ctx->palette_index == 2) { // Sunset
        for (int i = 0; i < n; i++) {
            double t = (double)i / n;
            unsigned char r = (unsigned char)(255 * sin(M_PI * t));
            unsigned char g = (unsigned char)(255 * sin(M_PI * t * 0.5));
            unsigned char b = (unsigned char)(255 * (1.0 - sin(M_PI * t)));
            ctx->palette[i] = (r << 16) | (g << 8) | b;
        }
    } else if (ctx->palette_index == 3) { // Oceanic
        for (int i = 0; i < n; i++) {
            double t = (double)i / n;
            unsigned char r = (unsigned char)(255 * (0.3 * sin(2 * M_PI * t) + 0.5));
            unsigned char g = (unsigned char)(255 * (0.3 * sin(2 * M_PI * t + 2.094) + 0.5));
            unsigned char b = (unsigned char)(255 * (0.3 * sin(2 * M_PI * t + 4.188) + 0.5));
            ctx->palette[i] = (r << 16) | (g << 8) | b;
        }
    } else { // Cyberpunk
        for (int i = 0; i < n; i++) {
            double t = (double)i / n;
            unsigned char r = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t + M_PI)));
            unsigned char g = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t + 2.094 + M_PI)));
            unsigned char b = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t + 4.188 + M_PI)));
            ctx->palette[i] = (r << 16) | (g << 8) | b;
        }
    }
}

unsigned long get_color(GraphicsContext *ctx, int iter) {
    if (iter >= ctx->max_iter) return 0; // black for points inside set
    return ctx->palette[iter % ctx->palette_size];
}
