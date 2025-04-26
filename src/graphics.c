#include "graphics.h"
#include "mandelbrot.h"
#include "julia.h"
#include "color.h"
#include <X11/keysym.h>
#include <string.h>  // for memmove
#include <X11/Xutil.h> // for XDestroyImage
#include <stdlib.h>
#include <unistd.h>

typedef struct { GraphicsContext *ctx; int y_start, y_end; } RenderArgs;

int init_graphics(GraphicsContext *ctx, const char *title, int width, int height) {
    ctx->display = XOpenDisplay(NULL);
    if (!ctx->display) return -1;
    ctx->screen = DefaultScreen(ctx->display);
    ctx->width = width; ctx->height = height;
    ctx->window = XCreateSimpleWindow(ctx->display, RootWindow(ctx->display, ctx->screen),
        0, 0, width, height, 1, BlackPixel(ctx->display, ctx->screen), WhitePixel(ctx->display, ctx->screen));
    XStoreName(ctx->display, ctx->window, title);
    XSelectInput(ctx->display, ctx->window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    XMapWindow(ctx->display, ctx->window);
    ctx->gc = XCreateGC(ctx->display, ctx->window, 0, NULL);
    // create image buffer
    ctx->image = XCreateImage(ctx->display, DefaultVisual(ctx->display, ctx->screen),
        DefaultDepth(ctx->display, ctx->screen), ZPixmap, 0,
        malloc(width * height * 4), width, height, 32, width * 4); // set bytes_per_line
    pthread_mutex_init(&ctx->lock, NULL);
    return 0;
}

void cleanup_graphics(GraphicsContext *ctx) {
    free(ctx->image->data);
    XDestroyImage(ctx->image);
    free(ctx->palette);
    pthread_mutex_destroy(&ctx->lock);
    XFreeGC(ctx->display, ctx->gc);
    XDestroyWindow(ctx->display, ctx->window);
    XCloseDisplay(ctx->display);
}

void draw_pixel(GraphicsContext *ctx, int x, int y, unsigned long color) {
    pthread_mutex_lock(&ctx->lock);
    XPutPixel(ctx->image, x, y, color);
    pthread_mutex_unlock(&ctx->lock);
}

void update_image(GraphicsContext *ctx) {
    XPutImage(ctx->display, ctx->window, ctx->gc, ctx->image, 0,0,0,0, ctx->width, ctx->height);
    XFlush(ctx->display);
}

void *render_thread(void *arg) {
    RenderArgs *ra = (RenderArgs*)arg;
    GraphicsContext *c = ra->ctx;
    for (int y = ra->y_start; y < ra->y_end; y++) {
        for (int x = 0; x < c->width; x++) {
            double real = c->x_min + x * (c->x_max - c->x_min) / c->width;
            double imag = c->y_min + y * (c->y_max - c->y_min) / c->height;
            int iter = c->is_julia ? julia(real, imag, c->julia_cx, c->julia_cy, c->max_iter)
                                    : mandelbrot(real, imag, c->max_iter);
            unsigned long col = get_color(c, iter);
            draw_pixel(c, x, y, col);
        }
    }
    return NULL;
}

void render_fractal(GraphicsContext *ctx) {
    int n = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[n]; RenderArgs args[n];
    int slice = ctx->height / n;
    for (int i = 0; i < n; i++) {
        args[i].ctx = ctx;
        args[i].y_start = i * slice;
        args[i].y_end = (i == n-1) ? ctx->height : args[i].y_start + slice;
        pthread_create(&threads[i], NULL, render_thread, &args[i]);
    }
    for (int i = 0; i < n; i++) pthread_join(threads[i], NULL);
    update_image(ctx);
}

void handle_events(GraphicsContext *ctx, void (*render_cb)(GraphicsContext *)) {
    XEvent ev;
    while (1) {
        XNextEvent(ctx->display, &ev);
        if (ev.type == Expose) update_image(ctx);
        else if (ev.type == KeyPress) {
            KeySym ks = XLookupKeysym(&ev.xkey, 0);
            if (ks == XK_q) break;
            // iteration controls via main and keypad
            else if (ks == XK_plus || ks == XK_equal || ks == XK_KP_Add) { ctx->max_iter += 50; render_cb(ctx); }
            else if (ks == XK_minus || ks == XK_KP_Subtract) { ctx->max_iter = ctx->max_iter > 50 ? ctx->max_iter - 50 : ctx->max_iter; render_cb(ctx); }
            // cycle color palettes with space
            else if (ks == XK_space) {
                ctx->palette_index = (ctx->palette_index + 1) % 3;
                init_palette(ctx);
                render_cb(ctx);
            }
            // pan viewport with arrow keys, scale by 10% of current view size
            else if (ks == XK_Left || ks == XK_Right || ks == XK_Up || ks == XK_Down) {
                double dx = (ctx->x_max - ctx->x_min) * 0.1;
                double dy = (ctx->y_max - ctx->y_min) * 0.1;
                if (ks == XK_Left) { ctx->x_min -= dx; ctx->x_max -= dx; }
                else if (ks == XK_Right) { ctx->x_min += dx; ctx->x_max += dx; }
                else if (ks == XK_Up) { ctx->y_min -= dy; ctx->y_max -= dy; }
                else if (ks == XK_Down) { ctx->y_min += dy; ctx->y_max += dy; }
                render_cb(ctx);
            }
            // existing palette cycle (c)
            else if (ks == XK_c) {
                unsigned long last = ctx->palette[ctx->palette_size-1];
                memmove(&ctx->palette[1], &ctx->palette[0], (ctx->palette_size-1)*sizeof(unsigned long));
                ctx->palette[0] = last;
                update_image(ctx);
            }
        } else if (ev.type == ButtonPress) {
            int x = ev.xbutton.x, y = ev.xbutton.y;
            if (ev.xbutton.button == Button1) {
                if (!ctx->is_julia) {
                    double real = ctx->x_min + x * (ctx->x_max - ctx->x_min) / ctx->width;
                    double imag = ctx->y_min + y * (ctx->y_max - ctx->y_min) / ctx->height;
                    ctx->julia_cx = real; ctx->julia_cy = imag;
                    ctx->is_julia = 1;
                } else ctx->is_julia = 0;
                render_cb(ctx);
            } else if (ev.xbutton.button == Button4 || ev.xbutton.button == Button5) {
                // get accurate mouse coords, in case pointer moved before event
                Window root, child;
                int root_x, root_y, win_x, win_y;
                unsigned int mask;
                XQueryPointer(ctx->display, ctx->window, &root, &child,
                              &root_x, &root_y, &win_x, &win_y, &mask);
                double factor = (ev.xbutton.button == Button4) ? 0.9 : 1.1;
                // world bounds
                double x0 = ctx->x_min, x1 = ctx->x_max;
                double y0 = ctx->y_min, y1 = ctx->y_max;
                // compute world coord under mouse
                double cx = x0 + win_x * (x1 - x0) / ctx->width;
                double cy = y0 + win_y * (y1 - y0) / ctx->height;
                // scale around cursor: adjust bounds
                ctx->x_min = cx - (cx - x0) * factor;
                ctx->x_max = cx + (x1 - cx) * factor;
                ctx->y_min = cy - (cy - y0) * factor;
                ctx->y_max = cy + (y1 - cy) * factor;
                render_cb(ctx);
            }
        }
    }
}
