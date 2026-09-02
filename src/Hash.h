#pragma once




#include <cstdint>

#include <glm/glm.hpp>

class Hash {
private:
   static uint64_t rotate(uint64_t x, uint64_t b);
public:
   static double toUnit(uint64_t integer);
   static uint64_t pcg(uint64_t a);
   static double pcgUnit(uint64_t a);
   static glm::i64vec3 pcg3(uint64_t a);
   static glm::dvec3 pcgUnit3(uint64_t a);
   static uint64_t pcg(uint64_t a, uint64_t b);
   static uint64_t pcg(glm::i64vec2 p);
   static double pcgUnit(uint64_t a, uint64_t b);
   static double pcgUnit(glm::i64vec2 p);
   static uint64_t pcg(uint64_t a, uint64_t b, uint64_t c);
   static uint64_t pcg(glm::i64vec3 p);
   static double pcgUnit(uint64_t a, uint64_t b, uint64_t c);
   static double pcgUnit(glm::i64vec3 p);
};

