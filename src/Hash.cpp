


#include "Hash.h"






double Hash::toUnit(uint64_t integer) {
   return (double)integer / (double)0xffffffffffffffff;
}

uint64_t Hash::rotate(uint64_t x, uint64_t b) {
   return (x << b) ^ (x >> (64 - b));
}

uint64_t Hash::pcg(uint64_t a) {
   uint64_t b{ a * 0xff51afd7ed558ccd };
   for (size_t ii = 0; ii < 3; ii++) {
      a = Hash::rotate((a ^ 0xcafebabe) + (b ^ 0xdeadbeef), 23);
      b = Hash::rotate((a ^ 0xcabba6e5) + (b ^ 0xb01dface), 5);
      //a ^= b;
      //b ^= a;
      //a ^= b;
   }
   return a ^ b;
}

double Hash::pcgUnit(uint64_t a) {
   return Hash::toUnit(Hash::pcg(a));
}

glm::i64vec3 Hash::pcg3(uint64_t a) {
   uint64_t b = a, c = a * 0xff51afd7ed558ccd;
   for (size_t ii = 0; ii < 3; ii++) {
      a = Hash::rotate((a ^ 0xcafebabe) + (b ^ 0xdeadbeef) + (c ^ 0x0b5e55ed), 23);
      b = Hash::rotate((a ^ 0xcabba6e5) + (b ^ 0xb01dface) + (c ^ 0x0b5e55ed), 5);
      c = Hash::rotate((a ^ 0xba5eba11) + (b ^ 0x6a5f1e1d) + (c ^ 0xdead50fa), 17);
   }
   // https://www.dcode.fr/words-containing
   return glm::i64vec3{a,b,c};
}

glm::dvec3 Hash::pcgUnit3(uint64_t a) {
   glm::i64vec3 rr{ Hash::pcg3(a) };
   return glm::dvec3{
      Hash::toUnit(rr.x),
      Hash::toUnit(rr.y),
      Hash::toUnit(rr.z),
   };
}

uint64_t Hash::pcg(uint64_t a, uint64_t b) {
   a *= 0xff51afd7ed558ccd;
   b *= 0xc4ceb9fe1a85ec53;
   for (size_t ii = 0; ii < 3; ii++) {
      a = Hash::rotate((a ^ 0xcafebabe) + (b ^ 0xdeadbeef), 23);
      b = Hash::rotate((a ^ 0xcabba6e5) + (b ^ 0xb01dface), 5);
   }
   return a ^ b;
}

uint64_t Hash::pcg(glm::i64vec2 p) {
   return Hash::pcg(p.x, p.y);
}

double Hash::pcgUnit(uint64_t a, uint64_t b) {
   return Hash::toUnit(Hash::pcg(a, b));
}

double Hash::pcgUnit(glm::i64vec2 p) {
   return Hash::toUnit(Hash::pcg(p.x, p.y));
}

uint64_t Hash::pcg(uint64_t a, uint64_t b, uint64_t c) {
   a *= 0xff51afd7ed558ccd;
   b *= 0xc4ceb9fe1a85ec53;
   c *= 0x9e3779b97f4a7c15;
   for (size_t ii = 0; ii < 3; ii++) {
      a = Hash::rotate((a ^ 0xcafebabe) + (b ^ 0xdeadbeef) + (c ^ 0x0b5e55ed), 23);
      b = Hash::rotate((a ^ 0xcabba6e5) + (b ^ 0xb01dface) + (c ^ 0x0b5e55ed), 5);
      c = Hash::rotate((a ^ 0xba5eba11) + (b ^ 0x6a5f1e1d) + (c ^ 0xdead50fa), 17);
   }
   // https://www.dcode.fr/words-containing
   return a ^ b ^ c;
}

uint64_t Hash::pcg(glm::i64vec3 p) {
   return Hash::pcg(p.x, p.y, p.z);
}

double Hash::pcgUnit(uint64_t a, uint64_t b, uint64_t c) {
   return Hash::toUnit(Hash::pcg(a, b, c));
}

double Hash::pcgUnit(glm::i64vec3 p) {
   return Hash::toUnit(Hash::pcg(p.x, p.y, p.z));
}
