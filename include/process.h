#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using namespace std;

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
  public:
    Process(int pid);
    int Pid() const;                               // See src/process.cpp
    string User() const;                           // See src/process.cpp
    string Command() const;                        // See src/process.cpp
    double CpuUtilization() const;                  // See src/process.cpp
    void CpuUtilization(long current_active, long current_total);
    string Ram() const;                            // See src/process.cpp
    long int UpTime() const;                       // See src/process.cpp
    bool operator<(Process const& a) const;        // See src/process.cpp
    bool operator>(Process const& a) const;        // See src/process.cpp

  // Declare any necessary private members
  private:
    int pid_{0};
    double cpu_utilization_{0};
    long last_active_{0};
    long last_total_{0};
};

#endif