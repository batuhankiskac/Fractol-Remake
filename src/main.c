#include <stdio.h>
#include "graphics.h"
#include "color.h"

int main() {
    GraphicsContext ctx;
    // Initial parameters
    ctx.width = 800;
    ctx.height = 600;
    ctx.max_iter = 500;
    ctx.x_min = -2.0;
    ctx.x_max = 1.0;
    ctx.y_min = -1.2;
    ctx.y_max = 1.2;
    ctx.is_julia = 0;
    ctx.palette_size = 256;
    ctx.palette = NULL;         // initialize palette pointer
    ctx.palette_index = 0;      // start with first palette

    if (init_graphics(&ctx, "Mandelbrot & Julia Fractal", ctx.width, ctx.height) != 0) {
        fprintf(stderr, "Failed to initialize graphics\n");
        return 1;
    }
    init_palette(&ctx);
    render_fractal(&ctx);
    handle_events(&ctx, render_fractal);
    cleanup_graphics(&ctx);
    return 0;
}
