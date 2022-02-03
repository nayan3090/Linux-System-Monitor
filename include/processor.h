#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <linux_parser.h>
#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 std::vector<float> total;
 std::vector<float> idle;
};

#endif