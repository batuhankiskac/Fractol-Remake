#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <X11/Xlib.h>
#include <pthread.h>

typedef struct {
    Display *display;       // X11 display connection
    int screen;             // Default screen
    Window window;          // Main window
    GC gc;                  // Graphics context
    XImage *image;          // Off-screen image buffer
    int width, height;      // Dimensions
    int max_iter;           // Max iterations for fractal
    double x_min, x_max;    // Viewport bounds (real)
    double y_min, y_max;    // Viewport bounds (imaginary)
    double julia_cx, julia_cy; // Julia set constant (only for Julia rendering)
    int is_julia;           // Flag: 0=Mandelbrot, 1=Julia
    unsigned long *palette; // Color palette
    int palette_size;       // Palette length
    int palette_index;      // Current palette selection index
    pthread_mutex_t lock;   // Protect image buffer
} GraphicsContext;

// Initialize X11 window and graphics context
int init_graphics(GraphicsContext *ctx, const char *title, int width, int height);
// Clean up resources
void cleanup_graphics(GraphicsContext *ctx);
// Plot a pixel in the image buffer
void draw_pixel(GraphicsContext *ctx, int x, int y, unsigned long color);
// Refresh the window with the current image
void update_image(GraphicsContext *ctx);
// Main event loop: handle input and trigger rendering
void handle_events(GraphicsContext *ctx, void (*render_cb)(GraphicsContext *));
// Worker thread to render a portion of the fractal
void *render_thread(void *arg);
// Render the entire fractal using multithreading
void render_fractal(GraphicsContext *ctx);

#endif // GRAPHICS_H
