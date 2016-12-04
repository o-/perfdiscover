#include <lib.h>
#include <gen.h>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {
  if (argc > 1 && std::string("-g").compare(argv[1]) == 0) {
    codegen(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
    exit(0);
  }
  run();
  run();
  run();
}
