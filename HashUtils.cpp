#include "HashUtils.h"
#include <sstream>
#include <functional>

// Define the function here, ensuring it is only defined once in the project
std::string generateHash(const std::string& input) {
    std::hash<std::string> hasher;
    size_t hashValue = hasher(input);
    std::stringstream ss;
    ss << std::hex << hashValue;
    return ss.str();
}
