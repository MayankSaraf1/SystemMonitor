#include "processor.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float active = (float) LinuxParser::ActiveJiffies();
    float total = (float) LinuxParser::Jiffies();
    return active/total;
}