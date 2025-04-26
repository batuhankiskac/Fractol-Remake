#include <math.h>
#include <stdlib.h>
#include "color.h"

void init_palette(GraphicsContext *ctx) {
    int n = ctx->palette_size;
    // free existing palette if any
    if (ctx->palette) free(ctx->palette);
    ctx->palette = malloc(n * sizeof(unsigned long));
    // generate palette based on palette_index
    if (ctx->palette_index == 0) {
        for (int i = 0; i < n; i++) {
            double t = (double)i / n;
            unsigned char r = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t)));
            unsigned char g = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t + 2.094)));
            unsigned char b = (unsigned char)(255 * (0.5 + 0.5 * sin(6.28318 * t + 4.188)));
            ctx->palette[i] = (r << 16) | (g << 8) | b;
        }
    } else if (ctx->palette_index == 1) {
        for (int i = 0; i < n; i++) {
            unsigned char v = (unsigned char)(255 * i / (n - 1));
            ctx->palette[i] = (v << 16) | (v << 8) | v; // grayscale
        }
    } else {
        // fallback to sine palette
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
