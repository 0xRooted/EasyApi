#include "utils.hpp"

bool utils::file::exists(std::string filename)
{
    return std::ifstream(filename).good();
}

bool utils::file::flineExists(std::string filename, std::string line)
{
    std::ifstream file(filename);
    std::string currentLine;
    while (getline(file, currentLine)) {
        if (currentLine.find(line) != std::string::npos)
            return true;
    }
    return false;
}

void utils::file::append(std::string filename, std::string data)
{
    std::ofstream file(filename, std::ios::app);
    file << data << std::endl;
    file.close();
}

void utils::file::clear(std::string filename)
{
    std::ofstream file(filename);
    file << "";
    file.close();
}

std::string utils::file::read(std::string filename)
{
    std::ifstream file(filename);
    std::string data;
    std::string currentLine;
    while (getline(file, currentLine))
        data += currentLine;
    
    return data;
}

std::vector<std::string> utils::file::readLines(std::string filename)
{
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string currentLine;
    while (getline(file, currentLine))
        lines.push_back(currentLine);
    
    return lines;
}

void utils::file::write(std::string filename, std::string data)
{
    std::ofstream file(filename);
    file << data;
    file.close();
}

void utils::file::writeLines(std::string filename, std::vector<std::string> data)
{
    std::ofstream file(filename);
    for (std::string line : data)
        file << line << std::endl;
    file.close();
}

void utils::file::remove(std::string filename)
{
    std::remove(filename.c_str());
}

std::string utils::file::getExtension(std::string filename)
{
    return filename.substr(filename.find_last_of(".") + 1);
}

std::string utils::file::getBasename(std::string filename)
{
    return filename.substr(0, filename.find_last_of("."));
}

std::string utils::file::getDirname(std::string filename)
{
    return filename.substr(0, filename.find_last_of("/"));
}