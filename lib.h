#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <chrono>
#include <math.h>

typedef std::chrono::high_resolution_clock Clock;


template<size_t ES, size_t PF>
struct Entry {
  uint8_t p1[PF];
  union U {
    struct Entry* next;
    uint8_t p2[ES];
  } u;
};

template<size_t WS, size_t ES, size_t PF>
struct Arena {
  typedef Entry<ES, PF> E;
  static const size_t ELEMS = WS/ES;
  E arena[ELEMS];
  static_assert(sizeof(arena) == WS, "");
  static_assert(sizeof(E) == ES, "");
  static_assert(sizeof(E) >= sizeof(E*), "");

  void setupLinear() {
    for (size_t i = 0; i < ELEMS-1; ++i) {
      arena[i].u.next = &arena[i+1];
    }
    arena[ELEMS-1].u.next = &arena[0];
  }

  void setupReverse() {
    for (size_t i = 1; i < ELEMS; ++i) {
      arena[i].u.next = &arena[i-1];
    }
    arena[0].u.next = &arena[ELEMS-1];
  }
};

template<size_t WS, size_t ES, size_t PF>
__attribute__((always_inline)) inline static void onerun(Arena<WS, ES, PF>* arena) {
  auto first = &arena->arena[0];
  auto cur = first;
  do {
    cur = cur->u.next;
  } while (cur != first);
}

template<size_t WS, size_t ES=8, size_t PF=0>
double run() {
  // setup
  size_t s = sizeof(Arena<WS, ES, PF>);
  Arena<WS, ES, PF>* arena = (Arena<WS, ES, PF>*)malloc(s);
  assert(arena);
  //arena->setupLinear();
  arena->setupReverse();

  unsigned long runs = pow(10, 19)/(pow(log2(WS),12));

  onerun(arena);

  auto t1 = Clock::now();
  for (size_t i = 0; i < runs; ++i)
    onerun(arena);

  auto t2 = Clock::now();

  unsigned long dur = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();

  double res = (double)dur / (double)runs / (double)arena->ELEMS;
  delete arena;
  return res;
};

static void codegen(int WORKINGSET_START, int WORKINGSET_END, int ELEMENT_SIZE_START, int ELEMENT_SIZE_END) {
  std::cout << "static void run() {\n";
  for (size_t es = ELEMENT_SIZE_START; es <= ELEMENT_SIZE_END; es *= 2) {
    for (size_t wsb = WORKINGSET_START; wsb <= WORKINGSET_END; ++wsb) {
      size_t ws = 1 << wsb;
      std::cout << "std::cout << "
                << wsb << " << \",\" << "
                << es << " << \",\" "
                << "<< run<" << ws << ", " << es << ">()"
                << "<< \"\\n\";\n";
    }
  }
  std::cout << "}\n";
}
