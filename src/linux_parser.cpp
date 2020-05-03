#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stol;
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
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {  
  string key;
  string value;
  string line;
  long memTotal, memFree, buffers, cached;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal") { memTotal = stol(value); }
      if (key == "MemFree") { memFree = stol(value); }
      if (key == "Buffers") { buffers = stol(value); }
      if (key == "Cached") { cached = stol(value); }
      }
  }
  return (float)(memTotal - memFree - buffers - cached)/memTotal; 
} 

// DONE: Read and return the system uptime
long int LinuxParser::UpTime() { 
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies() ; }

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::vector <string> cpuData {};
  long active = 0;
  vector <CPUStates> CPUActiveStates = {kUser_, kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_};
  cpuData = CpuUtilization();
  for (auto state : CPUActiveStates) {
    active+= stol(cpuData[state]);
  }  
  return active; 
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector <string> cpuData {};
  long idle = 0;
  vector <CPUStates> CPUIdleStates = {kIdle_, kIOwait_};
  cpuData = CpuUtilization();
  for (auto state : CPUIdleStates) {
    idle+= stol(cpuData[state]);
  }  
  return idle; 
}
// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string value;
  std::vector <string> data {};
  std::ifstream filestream(kProcDirectory + kStatFilename); 
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line);
    while(linestream >> value) {
      if (value != "cpu") { 
        data.push_back(value);
      }
    }
  }
  return data;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key;
  int value;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return value;
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key;
  int value;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return value;
      }
    }
  }
  return 0;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream;
  filestream.open(kProcDirectory + to_string(pid) + kCmdlineFilename);
  std::getline(filestream, line);
  return line;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string key;
  string value;
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        break; 
        
      }
    }
  }
  return to_string(std::stoi(value)/1000);  // convert from kB to MB
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key;
  string value;
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        break; 
        
      }
    }
  }
  return value;
}  
   
// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string username;
  string uidValue;
  string line;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      linestream >> username >> uidValue;
      if (uidValue == uid) {
        break;
      }
    }
  }
  return username;
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string value;
  string line;
  int counter = 1;
  long uptime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value) {
      if (counter == 22) { uptime = std::stol(value); }
      ++counter;
      }
  }
  return (UpTime() - (float)uptime/((float)sysconf(_SC_CLK_TCK)));
}

// DONE: Read and return the number of active jiffies for a PID
float LinuxParser::CpuUtilization(int pid) {
  string value;
  string line;
  int counter = 1;
  long utime, stime, cutime, cstime, uptime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value) {
      if (counter == 14) { utime = std::stol(value); }
      if (counter == 15) { stime = std::stol(value); }
      if (counter == 16) { cutime = std::stol(value); }
      if (counter == 17) { cstime = std::stol(value); }
      if (counter == 22) { uptime = std::stol(value); }
      ++counter;
    }
  }
  uptime = LinuxParser::UpTime(pid);
  float total_time = ((float)(utime + stime + cutime + cstime)) / ((float)sysconf(_SC_CLK_TCK));
  return (total_time/(float)uptime); 
}