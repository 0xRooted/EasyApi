#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <fstream>
#include <vector>

namespace utils
{
    namespace file
    {
        bool exists(std::string filename);
        bool flineExists(std::string filename, std::string line);
        void append(std::string filename, std::string data);
        void clear(std::string filename);
        std::string read(std::string filename);
        std::vector<std::string> readLines(std::string filename);
        void write(std::string filename, std::string data);
        void writeLines(std::string filename, std::vector<std::string> data);
        void remove(std::string filename);
        std::string getExtension(std::string filename);
        std::string getBasename(std::string filename);
        std::string getDirname(std::string filename);
    }
}

#endif