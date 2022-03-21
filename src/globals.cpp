#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR
#include <string>
#include<experimental/filesystem>
namespace fse = std::experimental::filesystem;
namespace fs = std::filesystem;
const std::string SEP = std::to_string(fse::path::preferred_separator);
#endif

#ifndef YAML_EXT_
#define YAML_EXT_
const std::string YAML_EXT = ".yml";
#endif
