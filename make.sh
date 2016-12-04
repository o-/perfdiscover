g++ -std=c++11 -I. main.cpp 
  ./a.out -g 10 28 8 8 > gen.h 
  echo "WS,EL,T" > res.csv 
  echo 1
  g++ -O2 -std=c++11 -I. main.cpp 
  ./a.out >> res.csv 
  echo 2
  g++ -O2 -std=c++11 -I. main.cpp 
  ./a.out >> res.csv 
  echo 3
  g++ -O2 -std=c++11 -I. main.cpp 
   ./a.out >> res.csv 
  R -f plot.R
