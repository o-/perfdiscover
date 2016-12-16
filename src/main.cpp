#include <lib.h>
#include <gen.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>

#include <unistd.h>
#include <limits.h>

char hostname[HOST_NAME_MAX];

using namespace std;

int main(int argc, char** argv) {

  int runs = 2;
  int speed = 20;

  if (argc > 1)
    runs = atoi(argv[1]);
  if (argc > 2)
    speed = atoi(argv[2]);

  std::ostringstream oss;

  for (int i = 0; i < runs; ++i)
    run(oss, speed);

  std::ostringstream fname;
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  gethostname(hostname, HOST_NAME_MAX);
  fname << "run_" << hostname << "_";
  fname << tm.tm_year << "-" << tm.tm_mon << "-" << tm.tm_mday << "-"
        << tm.tm_hour << ":" << tm.tm_min << "." << tm.tm_sec;
  fname << ".csv";

  std::ofstream f("out/" + fname.str());
  f << "WS, ES, ORD, ticks\n";
  f << oss.str();
  f.flush();
  f.close();
}
