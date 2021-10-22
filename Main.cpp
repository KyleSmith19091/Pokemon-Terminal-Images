#include <iostream>
#include <opencv/4.3.0_5/include/opencv4/opencv2/core.hpp>
#include <opencv/4.3.0_5/include/opencv4/opencv2/imgcodecs.hpp>
#include <opencv/4.3.0_5/include/opencv4/opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>
#include <string>
#include <fstream>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h> // for STDOUT_FILENO
#include <dirent.h>

using namespace std;

enum COLOR{
    BLUE,
    GREEN,
    RED,
    ALPHA
}; 

struct RGB {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    bool operator==(const RGB& pixel2) {
        return this->red == pixel2.red && this->green == pixel2.green && this->blue == pixel2.blue;
    }
};

struct RGBA {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int alpha;
    bool operator==(const RGBA& pixel2) {
        return this->red == pixel2.red && this->green == pixel2.green && this->blue == pixel2.blue && this->alpha == pixel2.alpha;
    }
};

std::string getColorEscape(const RGB& pixel) {
    return "\033[38;2;" + to_string(pixel.red) + ";" + to_string(pixel.green) + ";" + to_string(pixel.blue) + "m";
}

std::string getColorEscape(const RGBA& pixel) {
    return "\033[38;2;" + to_string(pixel.red) + ";" + to_string(pixel.green) + ";" + to_string(pixel.blue) + "m";
}

cv::Mat readImage(const std::string& path) {
    return  cv::imread(path, cv::IMREAD_UNCHANGED);
}

constexpr const unsigned int getColorValue(const cv::Mat& frame, const unsigned int& x, const unsigned int& y, const COLOR& c) {
    return frame.data[frame.channels()*(frame.cols*y + x) + c];
} 

const RGBA getPixelValue(const cv::Mat& frame, const unsigned int& x, const unsigned int& y) {
    RGBA pixel;
    pixel.red = getColorValue(frame, x, y, COLOR::RED);
    pixel.green = getColorValue(frame, x, y, COLOR::GREEN);
    pixel.blue = getColorValue(frame, x, y, COLOR::BLUE);
    pixel.alpha = getColorValue(frame, x, y, COLOR::ALPHA);
    return pixel;
}

void findTopLeft(const cv::Mat& frame, unsigned int& x,unsigned int& y) {
    for(int i = 0; i < frame.rows; i++){
        cv::Mat temp = frame.row(i);
        for(int j = 0; j < temp.cols; j++){
            int w = (int)temp.data[temp.channels() * (temp.cols*i + j) + 3];
            if(w == 255) {
                x = j;
                y = i;
                return;
            }
        }
    }
}

void createUnicodeArt(std::string imageName) {
    // Read image from path
    cv::Mat img = readImage("./images/" + imageName);
    std::string outPath = "./art/" + imageName + ".txt";
    std::cout << outPath << "\n";
    ofstream outFile(outPath);

    // Window size stuff ?
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

    // Creating a string matrix
    std::string string_matrix[img.rows][img.cols];
    std::string art = "";

    // Resizing image
    cv::resize(img,img,cv::Size(),1/(1.3),1/(1.3),cv::InterpolationFlags::INTER_AREA);

    // Populate string matrix
    if(img.cols * 2 <= size.ws_col) {
      // Populate string matrix with block glyph
      for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
          if (getColorValue(img, j, i, COLOR::ALPHA) == 255) {
            string_matrix[i][j] = "██";
          } else {
            string_matrix[i][j] = "  ";
          }
        }
      }

    } else {
        // Populate string matrix with block glyph
      for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
          if (getColorValue(img, j, i, COLOR::ALPHA) == 255) {
            string_matrix[i][j] = "█";
          } else {
            string_matrix[i][j] = " ";
          }
        }
      }
    }

    // Apply color to string matrix using image
    std::string oldColor = "";
    std::string newColor = "";
    std::string escapeColor = "";
    for(int i = 0; i < img.rows; i++){
        art += "\n";
        oldColor.clear();
        for(int j = 0; j < img.cols; j++){
            const RGBA pixel = getPixelValue(img, j, i);
            newColor = getColorEscape(pixel);
            if(newColor == oldColor) {
                escapeColor = "";
            } else {
                escapeColor = newColor;
                oldColor = newColor;
            }
            art += escapeColor + string_matrix[i][j];
        }	
    }

    // Reset
    art += "\033[0m\n";

    std::cout << art;
    outFile << art;
}

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
        throw "Could not open directory";
    }
    return directoryContents;
}

int main(int argc, const char* argv[]){

    std::vector<std::string> artFiles = getDirectoryContents("./images/");

    for(const std::string& s : artFiles) {
        createUnicodeArt(s);    
    }

    return 0;
}
