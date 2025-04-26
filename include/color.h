#ifndef COLOR_H
#define COLOR_H

#include "graphics.h"  // For GraphicsContext

// Initialize color palette based on palette_size in ctx
void init_palette(GraphicsContext *ctx);
// Map iteration count to a color from the palette
unsigned long get_color(GraphicsContext *ctx, int iter);

#endif // COLOR_H
