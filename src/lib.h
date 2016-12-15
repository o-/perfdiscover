#include <cassert>
#include <cstdint>
#include <cstddef>
#include <sstream>
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

template<size_t WS_bytes, size_t ES, size_t PF>
struct Arena {
  typedef Entry<ES, PF> E;
  static const size_t WS = pow(2, WS_bytes);
  static const size_t ELEMS = WS/ES;
  E arena[ELEMS];
  static_assert(sizeof(arena) == WS, "");
  static_assert(ELEMS * sizeof(E) == WS, "");
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

enum Type {
  Linear,
  Reverse,
};

uint64_t rdtsc() {
  uint32_t lo, hi;
  /* We cannot use "=A", since this would use %rax on x86_64 */
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return (uint64_t)hi << 32 | lo;
}

template<size_t WS, size_t ES, Type type, size_t PF=0>
void run(std::ostringstream& out) {
  // setup
  auto arena = new Arena<WS, ES, PF>;
  assert(arena);
  if (type == Linear)
    arena->setupLinear();
  else if (type == Reverse)
    arena->setupReverse();

  for (size_t i = 0; i < 10; ++i)
    onerun(arena);

  unsigned long dur;
  unsigned long rt_dur;

  long unsigned measure_runs = 1600000 * ES / WS / WS;
//  std::cout << measure_runs << "\n";
  {
    auto t1 = Clock::now();
    for (size_t i = 0; i < measure_runs; ++i)
      onerun(arena);
    auto t2 = Clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
  }

  long unsigned target = 5000000000;
  // assert(target > dur);

  long unsigned runs = measure_runs * ((long double)target / (long double)dur);

  {
    auto t1 = Clock::now();
    rdtsc();
    for (size_t i = 0; i < runs; ++i)
      onerun(arena);
    auto t2 = Clock::now();
    rt_dur = rdtsc();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
  }

  double missed_target = ((double)abs((long long)dur-(long long)target)/(double)1000000000L);

  if (missed_target > 1) {
    std::cout << "outlier\n";
    return run<WS,ES,type,PF>(out);
  }

  unsigned long rt_dur_rel = (unsigned long)((long double)rt_dur/(long double)arena->ELEMS/(long double)runs);
  long double dur_rel = ((long double)dur/(long double)arena->ELEMS/(long double)runs);

  std::cout << WS << ", " << ES << ", " << (type==Linear ? "L" : "R") << ", "
            << dur_rel << ", "
            << rt_dur_rel << "\n";
  out       << WS << ", " << ES << ", " << (type==Linear ? "L" : "R") << ", "
            << rt_dur_rel << "\n";

  delete arena;
};
