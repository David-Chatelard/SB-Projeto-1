#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator> 
#include <ctype.h>
#include <stdlib.h>
#include <tuple>

using namespace std;

int main(int argc, char const *argv[]) {
    string input_file_name(argv[2]);
    string output_file_name(argv[3]);
    ifstream original_file(input_file_name);
    ofstream file_after_pass_0(output_file_name);

    string line;

    if (!file_after_pass_0.is_open()){
        cout << "file not found." << endl;
        return 1;
    }


    while (getline(original_file, line)){
        file_after_pass_0 << line <<endl;
    }

    original_file.close();
    file_after_pass_0.close();


    ifstream new_file(output_file_name);

    while (getline(new_file, line)){
        cout << line <<endl;
    }

    new_file.close();

    return 0;
}
