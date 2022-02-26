/*
Aluno: David Fanchic Chatelard
Matrícula: 180138863
Disciplina: Software Básico
Professor: Bruno Luiggi Macchiavello Espinoza
IDE: Visual Studio Code
Sistema Operacional: Windows 10
Compilador: MinGW gcc g++ 9.2.0
*/

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

using namespace std;

// declaração das structs



int main(int argc, char const *argv[]) {
    string file_name(argv[2]);
    ifstream file(file_name);
    string line, no_comments_line, aux;

    if (!file.is_open()){
        cout << "file not found." << endl;
        return 1;
    }

     while (getline(file, line)) {
         cout << "Linha: " << line << endl;
         stringstream ss_line(line);
         getline(ss_line, no_comments_line, ';');
         cout << "Linha sem comentários: " << no_comments_line << endl;
         cout << "Testando o .find(): " << no_comments_line.find('u') << endl;
         cout << "Testando o .substr(): " << no_comments_line.substr(no_comments_line.find('u')) << endl;
         aux = no_comments_line.substr(no_comments_line.find('u'));
         aux.erase(0, 1);
         cout << "Testando o .erase(): " << aux << endl;
     }
    

    
    file.close();

    return 0;
}
