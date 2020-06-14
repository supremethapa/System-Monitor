#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using namespace std;

Process::Process(int pid) {
    pid_ = pid;
    cpu_utilization_ = CpuUtilization();
}

// Return this process's ID
int Process::Pid() const {
    return pid_;
}

// Return this process's CPU utilization
double Process::CpuUtilization() const {
    return cpu_utilization_;
}

void Process::CpuUtilization(long current_active, long current_total) {
    long delta_active = current_active - last_active_;
    long delta_total = current_total - last_total_;
    last_active_ = current_active;
    last_total_ = current_total;
    //if (delta_active == 0) std::cout << "WARNING FOR active!!!";
    //if (delta_total == 0) std::cout << "WARNING FOR TOTAL!!!";
    cpu_utilization_ = static_cast<double>(delta_active) / delta_total;
}

// Return the command that generated this process
// [Improved] get the first 36 chars when longer and append "..."
string Process::Command() const {
    string cmd = LinuxParser::Command(pid_);
    if (cmd.length() > 36)
        return cmd.substr(0,36).append("...");
    return cmd;
}

// Return this process's memory utilization
string Process::Ram() const {
    return LinuxParser::Ram(pid_);
}

// Return the user (name) that generated this process
string Process::User() const {
    return LinuxParser::User(pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() const {
    return LinuxParser::UpTime(pid_);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    // return true;
    return (CpuUtilization() < a.CpuUtilization());
}

// Overload the "larger than" comparison operator for Process objects
bool Process::operator>(Process const& a) const {
    // return true;
    return (CpuUtilization() > a.CpuUtilization());
}