#ifndef TEXTFILEUTILS_H
#define TEXTFILEUTILS_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

/**
 * Namespace containing utility functions to read/write to text files
*/
namespace TextFileUtils {

    /**
     * Read a list of strings
     * @param filename name of the file to be read
    */
    std::vector<std::string> ReadList(std::string filename);

}

#endif //  TextFileUtils_H
