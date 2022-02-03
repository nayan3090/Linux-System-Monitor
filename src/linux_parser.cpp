#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <unordered_map>

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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float percent;
  string line;
  string key, value;
  vector<string> v_value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:" || key == "MemFree:" || key == "Buffers:" || key == "Cached:" || key == "Shmem:" || key == "SReclaimable:")
      { 
        v_value.push_back(value);
      }
    }
    float total_used_memory = std::stoi(v_value[0]) - std::stoi(v_value[1]);
    float buffer = std::stoi(v_value[2]);
    float cached = std::stoi(v_value[3]) + std::stoi(v_value[5]) - std::stoi(v_value[4]);
    float non_cached_buffer = total_used_memory - (buffer + cached);
    percent = (non_cached_buffer + buffer + cached) / std::stoi(v_value[0]); 
  }
  return percent;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return std::stol(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long jiffies = ActiveJiffies() + IdleJiffies();
  return jiffies;
}

// TODO: Read and return the CPU utilization of a process
// REMOVE: [[maybe_unused]] once you define the function
float LinuxParser::ProcessCpuUtilization(int pid) {
  long sys_uptime = UpTime();
  long proc_uptime = UpTime(pid);
  float total_time = 0;
  string line;
  string value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  int counter = 0;
  while (std::getline(stream, line))
  {
    std::istringstream linestream(line);
    while (linestream >> value)
    {
      counter++;
      if (counter == 14 || counter == 15 || counter == 16 || counter == 17) 
      {
        total_time += std::stof(value);
      }
    }
  }
  float total_elapsed_time_seconds = sys_uptime - proc_uptime;
  float cpu_usage = (total_time / sysconf(_SC_CLK_TCK)) / total_elapsed_time_seconds;
  return cpu_usage;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<long> agg_cpu_jiffies = CpuUtilization();
  long active_jiffies = agg_cpu_jiffies[0] + agg_cpu_jiffies[1] + agg_cpu_jiffies[2] + agg_cpu_jiffies[5] + agg_cpu_jiffies[6] + agg_cpu_jiffies[7];
  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<long> agg_cpu_jiffies = CpuUtilization();
  long idle_jiffies = agg_cpu_jiffies[3] + agg_cpu_jiffies[4];
  return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() { 
  string line;
  string cpu, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  vector<long> agg_cpu_jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    agg_cpu_jiffies = {std::stol(user), std::stol(nice), std::stol(system), std::stol(idle), std::stol(iowait), std::stol(irq), std::stol(softirq), std::stol(steal), std::stol(guest), std::stol(guest_nice)};
  }
  return agg_cpu_jiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") { return std::stoi(value); }
    }
  }
  return std::stoi(value);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") { return std::stoi(value); }
    }
  }
  return std::stoi(value);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open())
  {
    std::getline(stream, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  int value_to_int;
  string key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") { 
        value_to_int = std::stoi(value) / 1000;
        value = to_string(value_to_int);
        return value; }
    }
  }
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open())
  {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") { return value; }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::unordered_map <string,string> user_dict;
  string uid = Uid(pid);
  string line;
  string key, x, value;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open())
  {
    while (std::getline(stream,line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> value >> x >> key;
      if (user_dict.find(key) == user_dict.end())
      {
        user_dict[key] = value;
      }
    }
  }
  return user_dict[uid];
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  string v;
  long value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  int counter = 0;
  while (std::getline(stream, line))
  {
    std::istringstream linestream(line);
    while (linestream >> v)
    {
      counter++;
      if (counter == 22) 
      {
        value = std::stol(v);
        return value / sysconf(_SC_CLK_TCK);
      }
    }
  }
  return value;
}
