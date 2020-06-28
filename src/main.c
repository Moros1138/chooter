#include <stdlib.h>

#include "chooter.h"

static State state;
static Arena arena;

static void clean(void) { deinit_sdl(&state); }

static void cap_fps(Sint32 *ticks, float *remainder) {
    Sint32 delay = *remainder + 16, frame_time = SDL_GetTicks() - *ticks;

    *remainder -= (int)*remainder;
    delay -= frame_time;

    if (delay < 1) {
        delay = 1;
    }

    SDL_Delay(delay);

    *remainder += 0.667;
    *ticks = SDL_GetTicks();
}

int main(int ac, char *av[]) {
    if (ac != 1) {
        fprintf(stderr,
                "I do not have any command-line arguments... just run me like "
                "this: \"%s\"\n",
                av[0]);
        return EXIT_FAILURE;
    }

    atexit(clean);

    if (sodium_init() == -1) {
        fputs("could not initialize libsodium... bye :(\n", stderr);
        return EXIT_FAILURE;
    }

    if (!init_sdl(&state) || !arena_init(&state, &arena)) {
        return EXIT_FAILURE;
    }

    Sint32 ticks = SDL_GetTicks();
    float remainder = 0;

    while (state.running) {
        handle_input(&state);
        state.dg.logic(&state, &arena);

        render_clear(&state);
        state.dg.render(&state, &arena);
        render_display(&state);

        cap_fps(&ticks, &remainder);
    }

    return EXIT_SUCCESS;
}
