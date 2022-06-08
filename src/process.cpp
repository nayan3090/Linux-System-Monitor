#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

//Class constructor
Process::Process(int pid)
{
    pid_ = pid;
    user_ = LinuxParser::User(pid);
    cpu_usage_ = LinuxParser::ProcessCpuUtilization(pid);
    ram_ = LinuxParser::Ram(pid);
    uptime_ = LinuxParser::UpTime() - LinuxParser::UpTime(pid);
    command_ = LinuxParser::Command(pid);
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return cpu_usage_; }

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return a.cpu_usage_ < cpu_usage_;
}