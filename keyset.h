#ifndef KEYSET_H
#define KEYSET_H

#include "keycodes.h"
#include "utils/set.h"

class KeySet : public Set<keycode_t> {
  private:
    static keycode_t initializer();
    static char validator(keycode_t item);
    static char comparator(keycode_t a,keycode_t b);
  public:
    KeySet(int size = 64) : Set(size,initializer,validator,comparator){}
};

#endif