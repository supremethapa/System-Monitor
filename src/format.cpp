#include <string>
#include <iomanip>

#include "format.h"

using std::string;

// helper function converting secs to hh:mm:ss
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int ss = seconds % 60;
    int mm = (seconds/60) % 60;
    int hh = (seconds/3600) % 24;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hh << ":"
        << std::setw(2) << std::setfill('0') << mm << ":"
        << std::setw(2) << std::setfill('0') << ss;
    return oss.str();
}