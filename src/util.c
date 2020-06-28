#include <math.h>
#include <string.h>

#include "chooter.h"

#ifdef max
#undef max
#endif

#define max(a, b) (((a) > (b)) ? (a) : (b))

void set_thing_size(Thing *T) {
    int w = 0, h = 0;

    SDL_QueryTexture(T->tex, NULL, NULL, &w, &h);

    T->r.w = w;
    T->r.h = h;
}

const char *get_res_path(void) {
    // This will hold the base resource path: chooter/res/.
    // It has static lifetime so only one call to SDL_GetBasePath() is needed.
    static char *base_res;

    if (base_res == NULL) {
        char *base_path = SDL_GetBasePath();

        if (base_path == NULL) {
            fprintf(stderr, "could not get resource path: %s\n",
                    SDL_GetError());
            return "";
        }

        base_res = base_path;
        SDL_free(base_path);

        // Replace the last "bin" with "res" to get the the resource path.
        char *s = strstr(base_res, "bin");
        const size_t pos = s - base_res;

        base_res[pos] = 'r';
        base_res[pos + 1] = 'e';
        base_res[pos + 2] = 's';
    }

    return base_res;
}

bool are_colliding(Thing *a, Thing *b) {
    SDL_Rect a_r = {a->r.x, a->r.y, a->r.w, a->r.h},
             b_r = {b->r.x, b->r.y, b->r.w, b->r.h};

    return SDL_HasIntersection(&a_r, &b_r);
}

void find_slope(float x1, float y1, float x2, float y2, float *dx, float *dy) {
    float steps = max(fabsf(x1 - x2), fabsf(y1 - y2));

    if (steps == 0) {
        *dx = 0;
        *dy = 0;
    } else {
        *dx = x1 - x2 / steps;
        *dy = y1 - y2 / steps;
    }
}
