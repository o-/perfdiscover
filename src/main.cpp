#include <lib.h>
#include <gen.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>


using namespace std;

int main(int argc, char** argv) {

  int runs = 2;
  float speed = 1;

  if (argc > 1)
    runs = atoi(argv[1]);
  if (argc > 2)
    speed = atof(argv[2]);

  std::ostringstream oss;

  for (int i = 0; i < runs; ++i)
    run(oss, speed);

  std::ostringstream time;
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  time << tm.tm_year << "-" << tm.tm_mon << "-" << tm.tm_mday << "-"
       << tm.tm_hour << ":" << tm.tm_min << "." << tm.tm_sec;

  std::ofstream f("out/run_" + time.str() + ".csv");
  f << "WS, ES, ORD, ticks\n";
  f << oss.str();
  f.flush();
  f.close();
}
