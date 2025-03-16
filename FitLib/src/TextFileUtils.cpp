
#include "TextFileUtils.hpp"

namespace TextFileUtils {

    std::vector<std::string> ReadList(std::string filename){
        std::ifstream file(filename);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) { lines.push_back(line); }
        file.close();
        return lines;
    }

}
