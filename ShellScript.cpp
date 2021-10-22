#include <iostream>
#include <vector>
#include <dirent.h>
#include <random>

using namespace std;

std::vector<std::string> getDirectoryContents(const std::string& path) {
    DIR *dir;
    struct dirent *ent;
    std::vector<std::string> directoryContents;
    if ((dir = opendir(path.c_str())) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
          std::string s(ent->d_name);
          if(s != "." && s != "..") {
              directoryContents.push_back(s);
          }
      }
    // Cleanup
    closedir (dir);
    } else {
        std::cerr << "Could not open directory\n" + path + "\n";
        exit(1);
    }
    return directoryContents;
}

int main(){
    
    std::vector<std::string> artFiles = getDirectoryContents("/Users/kylesmith/Development/C++_Code_Projects/TerminalImages/art/");
    const int range_from  = 0;
    const int range_to    = artFiles.size()-1;
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(range_from, range_to);
    int result = distr(generator);
    std::string cmd("cat ~/Development/C++_Code_Projects/TerminalImages/art/" + artFiles[result]);
    system(cmd.c_str());

    return 0;
}
