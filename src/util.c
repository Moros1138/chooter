#include "chooter.h"

void set_thing_size(Thing *T) {
  int w = 0, h = 0;

  SDL_QueryTexture(T->tex, NULL, NULL, &w, &h);

  T->r.w = w;
  T->r.h = h;
}
