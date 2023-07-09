CXX := "g++"

default: build

# Build the target
build:
  {{CXX}} -std=c++11 -lcurl -o hhhash hhhash.cpp

test:
  ./hhhash https://circl.lu/
