CXX := "g++"

default: build

# Build the target
build:
  {{CXX}} -std=c++11 -O3 -march=native -lcurl -o hhhash hhhash.cpp

test:
  ./hhhash https://circl.lu/
