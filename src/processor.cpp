#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
// Δ active time units / Δ total time units
double Processor::Utilization() {
  long current_active = LinuxParser::ActiveJiffies();
  long current_idle = LinuxParser::IdleJiffies();
  long delta_active = current_active - last_active_;
  long delta_idle = current_idle - last_idle_;
  last_active_ = current_active;
  last_idle_ = current_idle;
  // if (delta_total == 0)
    // return 0;
  return static_cast<double>(delta_active) / delta_idle;
}