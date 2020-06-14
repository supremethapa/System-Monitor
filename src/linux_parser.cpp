#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
// #include <filesystem> gcc > 8 needed

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  // std::filesystem::path = kProcDirectory.c_str();
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
// [Improved] float -> double for higher precision
double LinuxParser::MemoryUtilization() { 
  string line, key, value;
  double mem_total = 0;
  double mem_available = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal")
          mem_total = std::stof(value);
        if (key == "MemAvailable")
          mem_available = std::stof(value);
      }
    }
    return (mem_total - mem_available) / mem_total;
  }
  return 0.0;
}

// Read and return the system uptime
// [Corrected] get the right system uptime
long LinuxParser::UpTime() { 
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream stream(line);
      if (stream >> value)
        return std::stol(value);
  }
  return 0;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return UpTime() * sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for a PID
// Reference: utime - user mode jiffies / stime - kernel mode jiffies
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
// https://stackoverflow.com/questions/1420426/how-to-calculate-the-cpu-usage-of-a-process-by-pid-in-linux-from-c
// Guess: cutime and sutime not needed <- as zero hard-coded
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string key;
  string value;
  long utime;
  long stime;
  long cutime;
  long sutime;
  long starttime;
  utime = stime = cutime = sutime = starttime = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      short count = 0;
      while (linestream >> value) {  
        count++;
        if (count == 14) 
          utime = std::stol(value);
        if (count == 15)
          stime = std::stol(value);
        if (count == 16)
          cutime = std::stol(value);
        if (count == 17)
          sutime = std::stol(value);
        if (count == 22)
          starttime = std::stol(value);
      }
    }
    // long seconds = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
    // return 100 * (utime + stime + cutime + sutime) / sysconf(_SC_CLK_TCK) / seconds;
    return utime + stime + cutime + sutime;
  }
  return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> proc_stat = CpuUtilization();
  long kuser = std::stol(proc_stat.at(CPUStates::kUser_));
  long knice = std::stol(proc_stat.at(CPUStates::kNice_));
  long ksystem = std::stol(proc_stat.at(CPUStates::kSystem_));
  long kirq = std::stol(proc_stat.at(CPUStates::kIRQ_));
  long ksoftirq = std::stol(proc_stat.at(CPUStates::kSoftIRQ_));
  long ksteal = std::stol(proc_stat.at(CPUStates::kSteal_));
  long kguest = std::stol(proc_stat.at(CPUStates::kGuest_));
  long kguestnice = std::stol(proc_stat.at(CPUStates::kGuestNice_));
  return kuser + knice + ksystem + kirq + ksoftirq + ksteal + kguest + kguestnice;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> proc_stat = LinuxParser::CpuUtilization();
  long kidle = std::stol(proc_stat.at(LinuxParser::CPUStates::kIdle_));
  long kiowait = std::stol(proc_stat.at(LinuxParser::CPUStates::kIOwait_));
  return kidle + kiowait;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_stat;
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {  
        if (value == "cpu") {
          while (linestream >> value) 
            cpu_stat.emplace_back(value);
          return cpu_stat;
        }
      }
    }
  }
  return {};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {  
        if (key == "processes") 
          return std::stoi(value);
      }
    }
  }
  return 0; 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running")
          return std::stoi(value);
      }
    }
  }
  return 0; 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {  
        if (key == "VmSize") 
          return to_string(std::stol(value) / 1024);
      }
    }    
  }
  return string(); 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {  
        if (key == "Uid") 
          return value;
      }
    }    
  }
  return string(); 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string user;
  string x;
  string uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid) {  
        if (uid == Uid(pid)) 
          return user;
      }
    }    
  }
  return string(); 
}

// Read and return the uptime of a process
// [Corrected] starttime -> utime in /proc/[pid]/stat
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      short count = 0;
      while (linestream >> value) {  
        count++;
        if (count == 22)
          return UpTime() - std::stol(value) / sysconf(_SC_CLK_TCK);
      }
    }    
  }
  return 0; 
}
