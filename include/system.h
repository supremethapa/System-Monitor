#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "linux_parser.h"

using namespace std;

class System {
  public:
    Processor& Cpu();                   // See src/system.cpp
    vector<Process>& Processes();       // See src/system.cpp
    double MemoryUtilization();          // See src/system.cpp
    long UpTime();                      // See src/system.cpp
    int TotalProcesses();               // See src/system.cpp
    int RunningProcesses();             // See src/system.cpp
    string Kernel();                    // See src/system.cpp
    string OperatingSystem();           // See src/system.cpp

  // Define any necessary private members
  private:
    Processor cpu_{};
    vector<Process> processes_{};
    vector<int> pids_;
    std::string kernel_{};
    std::string os_{};
};

#endif