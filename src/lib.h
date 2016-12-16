#include <cassert>
#include <cstdint>
#include <cstddef>
#include <sstream>
#include <iostream>
#include <chrono>
#include <math.h>

#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <malloc.h>

typedef std::chrono::high_resolution_clock Clock;


template<size_t ES>
struct Entry {
  union U {
    struct Entry* next;
    uint8_t p2[ES];
  } u;
};

template<size_t WS_bytes, size_t ES>
struct Arena {
  typedef Entry<ES> E;
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

  void setupRandom() {
    std::srand (123);
    std::vector<int> vec;

    for (size_t i = 0; i < ELEMS; ++i) vec.push_back(i);
    std::random_shuffle(vec.begin(), vec.end());

    int first = vec[0];
    int pos = first;
    for (size_t i = 1; i < ELEMS; ++i) {
      arena[pos].u.next = &arena[vec[i]];
      pos = vec[i];
    }
    arena[pos].u.next = &arena[first];
  }
};

template<size_t WS, size_t ES>
__attribute__((always_inline)) inline static void onerun(Arena<WS, ES>* arena) {
  auto first = &arena->arena[0];
  auto cur = first;
  // size_t count = 0;
  do {
    // ++count;
    cur = cur->u.next;
    __builtin_expect(cur != first, true);
  } while (cur != first);
  // assert(count == arena->ELEMS);
}

enum Type {
  Linear,
  Reverse,
  Random,
};

uint64_t rdtsc() {
  uint32_t lo, hi;
  /* We cannot use "=A", since this would use %rax on x86_64 */
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return (uint64_t)hi << 32 | lo;
}

template<size_t WS, size_t ES, Type type>
void run(std::ostringstream& out, float speed) {
  // setup

  size_t s = sizeof(Arena<WS, ES>);
  Arena<WS, ES>* arena = (Arena<WS, ES>*)memalign(s, s);
  assert(arena);

  if (type == Linear)
    arena->setupLinear();
  else if (type == Reverse)
    arena->setupReverse();
  else if (type == Random)
    arena->setupRandom();

  unsigned long dur;
  unsigned long rt_dur;

  std::string t;
  switch(type) {
    case Linear:  t = "Linear";  break;
    case Reverse: t = "Reverse"; break;
    case Random:  t = "Random";  break;
  };

  std::cout << "Running : " << WS << ", " << ES << ", " << t << "\n";
  std::cout << "measure\n";

  // Warmup
  onerun(arena);

  // Measure how long one run takes
  dur = 0;
  static long unsigned target_measure_runs = 3;
  long unsigned measure_runs = 0;
  {
    while (dur < 100000) {
      auto t1 = Clock::now();
      for (size_t i = 0; i < target_measure_runs; ++i) {
        onerun(arena);
      }
      measure_runs += target_measure_runs;
      auto t2 = Clock::now();
      dur += std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
    }
  }

  // Target experiment time in ns
  long unsigned target = 1000000000;
  target *= speed;
  long unsigned runs = measure_runs * ((long double)target / (long double)dur);

  // A lower limit of runs, even if it takes longer than the target
  // experimentation time
  if (runs < 5) {
    runs = 5;
    std::cout << "Adjusting target: measure_runs "
              << measure_runs << " " << dur << "\n";
    target = runs * (long double)dur/(long double)measure_runs;
  }

  unsigned long rt_dur_rel;
  long double dur_rel;

  while (true) {
    std::cout << "running " << runs << "x ...\n";
    {
      auto t1 = Clock::now();
      rdtsc();
      for (size_t i = 0; i < runs; ++i)
        onerun(arena);
      rt_dur = rdtsc();
      auto t2 = Clock::now();
      dur = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
    }

    double missed_target = (long double)(dur + abs((long long)dur-(long long)target)) /
                           (long double)dur;

    rt_dur_rel = (unsigned long)((long double)rt_dur/(long double)arena->ELEMS/(long double)runs);
    dur_rel = ((long double)dur/(long double)arena->ELEMS/(long double)runs);

    std::cout << "ran for " << (long double)dur / 1000000000L << "s ("
              << (int)(100.0*missed_target)-100 << "% off target), "
              << dur_rel << "ns per El (" << rt_dur_rel << ")\n";

    // If we are way off target its probably an outlier and we discard it
    if (missed_target > 2.0) {
      runs = runs * ((long double)target / (long double)dur);
      std::cout << "outlier, adjusting runs to " << runs << "x\n";
    } else {
      break;
    }
  }
  std::cout << "========================================\n";

  out       << WS << ", " << ES << ", " << t << ", "
            << rt_dur_rel << "\n";

  free(arena);
};
