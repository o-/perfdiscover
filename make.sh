gcc -std=c++11 -lstdc++ -I. main.cpp 
  ./a.out -g 10 28 8 8 > gen.h 
  echo "WS,EL,T" > res.csv 
  echo 1
  gcc -O1 -std=c++11 -lstdc++ -I. main.cpp 
  ./a.out >> res.csv 
  echo 2
  gcc -O1 -std=c++11 -lstdc++ -I. main.cpp 
  ./a.out >> res.csv 
  echo 3
  gcc -O2 -std=c++11 -lstdc++ -I. main.cpp 
  ./a.out >> res.csv 
  R -f plot.R
