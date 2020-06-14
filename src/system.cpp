#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"
#include <iostream>

using namespace std;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
// [Improved] sort the processes with decreasing cpu utilization
vector<Process>& System::Processes() {
    processes_.clear();
    processes_.shrink_to_fit();
    for (int pid : LinuxParser::Pids()) {
        Process proc = Process(pid);
        proc.CpuUtilization(LinuxParser::ActiveJiffies(pid),
                            LinuxParser::Jiffies());
        processes_.emplace_back(proc);
    }

    sort(processes_.begin(), processes_.end());
    reverse(processes_.begin(), processes_.end());
    
    return processes_;
}

// Return the system's kernel identifier (string)
string System::Kernel() {
    if (kernel_.empty())
       kernel_ = LinuxParser::Kernel();
    return kernel_;
}

// Return the system's memory utilization
double System::MemoryUtilization() {
    return LinuxParser::MemoryUtilization();
}

// Return the operating system name
string System::OperatingSystem() {
    if (os_.empty())
       os_ = LinuxParser::OperatingSystem();
    return os_;
}

// Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long int System::UpTime() {
  return LinuxParser::UpTime();
}