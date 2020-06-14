#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor(): last_active_{0}, last_idle_{0} {}
  double Utilization();  // See src/processor.cpp

  // Declare any necessary private members
 private:
  long last_active_;
  long last_idle_;
};

#endif