#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include "processing.hpp"
using namespace std;

int main(int argc,char* argv[]) {
    if(argc < 4 || argc > 5){
        cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
        << "WIDTH and HEIGHT must be less than or equal to original" << endl;
        return 1;
    }
    string fig_input=argv[1];
    string fig_output=argv[2];
    ifstream fin(fig_input);
    ofstream fout(fig_output);
    if(!fin.is_open()){
        cout << "Error opening file: " << fig_input << endl;
        return 1;
    }
    Image image_origin;
    Image* img = &image_origin;
    Image_init(img,fin);
    
    
    int newWidth=stoi(argv[3]);
    int newHeight=Image_height(img);
    if(argc==5){newHeight=stoi(argv[4]);}
    if(newWidth < 0 || newWidth > Image_width(img) ||
        newHeight < 0 || newHeight > Image_height(img)){
        cout << "Usage: resize.exe IN_FILENAME OUT_FILENAME WIDTH [HEIGHT]\n"
        << "WIDTH and HEIGHT must be less than or equal to original" << endl;
        return 1;
    }

    
    seam_carve(img,newWidth,newHeight);
    Image_print(img, fout);
}