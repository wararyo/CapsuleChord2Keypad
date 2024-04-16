#include "keyset.h"

keycode_t KeySet::initializer() {
  // KeyCodeに0は存在しないため、無効な値として0を用いることができる
  return 0;
}
char KeySet::validator(keycode_t item){
  return ((item & 0b01111111) != 0) ? 1 : 0;
}
char KeySet::comparator(keycode_t a,keycode_t b) {
  return (a == b) ? 1 : 0;
}
