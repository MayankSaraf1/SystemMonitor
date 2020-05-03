#include <string>
#include "format.h"

#define MIN_IN_HR (60)
#define SEC_IN_MIN (60)
#define SEC_IN_HR (MIN_IN_HR * SEC_IN_MIN)

using std::string;

// DONE: Complete this helper function - 
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long hours, minutes;
    hours = seconds/SEC_IN_HR;
    seconds = seconds%SEC_IN_HR;
    minutes = seconds/SEC_IN_MIN;
    string s = std::to_string((seconds%SEC_IN_MIN));
    return (std::to_string(hours) + ":" + std::to_string(minutes) + ":" + s.insert(0, 2 - s.length(), '0'));
}