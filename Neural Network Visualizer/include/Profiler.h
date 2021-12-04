#ifndef PROFILER_H
#define PROFILER_H
#include <string>
#include <iostream>
#include <chrono>
#include <stack>


class Profiler {
public:
    void start(const std::string& name);
    void end(std::ostream& out);
    void endStart(std::ostream& out, const std::string& name);
private:
    std::stack<std::string> names;
    std::stack<std::chrono::system_clock::time_point> start_times;
};


#endif //PROFILERH