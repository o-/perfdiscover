#include <lib.h>
#include <gen.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>


using namespace std;

int main(int argc, char** argv) {
  std::ostringstream oss;
  run(oss);
  run(oss);

  std::ostringstream time;
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  time << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");

  std::ofstream f("out/run_" + time.str() + ".csv");
  f << "WS, ES, ORD, ticks\n";
  f << oss.str();
  f.flush();
  f.close();
}
