/*
Aluno: David Fanchic Chatelard
Matrícula: 180138863
Disciplina: Software Basico
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

// Declaracao das classes
class Code_line {
public:
    string label; // rotulo da linha
    string opcode; // opcode da operacao da linha
    string arg1; // primeiro argumento da linha
    string arg2; // segundo argumento da linha
};

class Item_operations_table {
public:
    string opcode; // opcode da operacao
    int num_args; // numero de argumentos da operacao
    int memory_space; // espaco de memoria que a operacao ocupa
};

class Item_symbols_table {
public:
	string symbol;
	int address = -1; // endereco de memoria onde o simbolo esta, -1 caso não seja definido
};

class Item_errors_table {
public:
	string label; // tipo do erro
	string mensagem; // mensagem do erro
	int line_number; //numero da linha do erro
	
};



auto begin_operations_table(){
	map <string, Item_operations_table> table;
	table["ADD"].opcode = "01", table["ADD"].num_args = 1, table["ADD"].memory_space = 2;
	table["SUB"].opcode = "02", table["SUB"].num_args = 1, table["SUB"].memory_space = 2;
	table["MULT"].opcode = "03", table["MULT"].num_args = 1, table["MULT"].memory_space = 2;
	table["DIV"].opcode = "04", table["DIV"].num_args = 1, table["DIV"].memory_space = 2;
	table["JMP"].opcode = "05", table["JMP"].num_args = 1, table["JMP"].memory_space = 2;
	table["JMPN"].opcode = "06", table["JMPN"].num_args = 1, table["JMPN"].memory_space = 2;
	table["JMPP"].opcode = "07", table["JMPP"].num_args = 1, table["JMPP"].memory_space = 2;
	table["JMPZ"].opcode = "08", table["JMPZ"].num_args = 1, table["JMPZ"].memory_space = 2;
	table["COPY"].opcode = "09", table["COPY"].num_args = 2, table["COPY"].memory_space = 3;
	table["LOAD"].opcode = "10", table["LOAD"].num_args = 1, table["LOAD"].memory_space = 2;
	table["STORE"].opcode = "11", table["STORE"].num_args = 1, table["STORE"].memory_space = 2;
	table["INPUT"].opcode = "12", table["INPUT"].num_args = 1, table["INPUT"].memory_space = 2;
	table["OUTPUT"].opcode = "13", table["OUTPUT"].num_args = 1, table["OUTPUT"].memory_space = 2;
	table["STOP"].opcode = "14", table["STOP"].num_args = 0, table["STOP"].memory_space = 1;
	
	return table;
}

// Verifica se a linha esta em branco
bool is_blank_line(string line) {
    return line.empty();
}

// Separa os tokens da linha
auto get_tokens(string line) {
    vector <string> tokens;
    string no_comments_line, label = "", opcode = "", arg1 = "", arg2 = "";
    string aux; //TESTE

    stringstream ss_line(line); //transforma em stream

    // Remove os comentarios
    getline(ss_line, no_comments_line, ';');
    // ##################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
    // !!!Fazer os corner case para caso tenha varios ';' na linha!!!

    stringstream ss_no_comments_line(no_comments_line); //transforma em stream
    // stringstream ss_no_comments_line_aux(no_comments_line); //TESTE

    // Pega os rotulos
    if (no_comments_line.find(':') != string::npos){ // se tiver rotulo
        getline(ss_no_comments_line, label, ':'); //bota tudo que vem antes de ':' em label
        // ##################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
        // !!!Fazer os corner case para caso tenha vários ':' na linha (por exemplo L1:L2: ADD B)!!!
        // !!!Fazer os corner case para caso tenha quebra de linha para o rótulo!!!

        stringstream ss_label(label); //transforma em stream
        ss_label >> label; //retira os espacos em branco do rotulo

        // cout << "linha antes-- " << no_comments_line << endl;        //TESTE
        // ss_no_comments_line_aux >> aux;                              //TESTE
        // cout << "linha depois de 1 >>-- '" << aux << "'" << endl;    //TESTE
        // ss_no_comments_line_aux >> aux;                              //TESTE
        // cout << "linha depois de 2 >>-- '" << aux << "'"  << endl;   //TESTE
        // ss_no_comments_line_aux >> aux;                              //TESTE
        // cout << "linha depois de 3 >>-- '" << aux << "'" << endl;    //TESTE
        // ss_no_comments_line_aux >> aux;                              //TESTE
        // cout << "linha depois de 4 >>-- '" << aux << "'"  << endl;   //TESTE
        // ss_no_comments_line_aux >> aux;                              //TESTE
    }

    stringstream ss_line_after_label(no_comments_line.substr(no_comments_line.find(':')).erase(0, 1)); // pega o texto da linha que vem depois do rotulo e apaga o ':'
    cout << "linha depois do rotulo >>-- '" << ss_line_after_label.str() << "'"  << endl;   //TESTE

    // Pega os opcodes
    ss_line_after_label >> opcode;
    cout << "opcode >>-- '" << opcode << "'"  << endl;   //TESTE
    // ##################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
    // !!!Fazer os corner case para caso tenha opcode invalido!!!

    tokens.push_back(label);
    return tokens;
}

int main(int argc, char const *argv[]) {
    string file_name(argv[2]);
    ifstream file(file_name);
    string line, no_comments_line, label;

    vector <string> tokens;

    map <string, Item_operations_table> operations_table;

    string aux, aux2; //dos testes do começo, pode apagar depois

    if (!file.is_open()){
        cout << "file not found." << endl;
        return 1;
    }

    operations_table = begin_operations_table(); // inicializa a tabela de operacoes

    //  while (getline(file, line)) {
    //      cout << "Linha: " << line << endl;
    //      stringstream ss_line(line);
    //      getline(ss_line, no_comments_line, ';');
    //      cout << "Linha sem comentarios: " << no_comments_line << endl;
    //      cout << "Testando o .find(): " << no_comments_line.find('u') << endl;
    //      cout << "Testando o .substr(): " << no_comments_line.substr(no_comments_line.find('u')) << endl;
    //      aux = no_comments_line.substr(no_comments_line.find('u'));
    //      aux.erase(0, 1);
    //      cout << "Testando o .erase(): " << aux << endl;
    //      ss_line >> ws >> aux2;
    //      cout << "Testando o aux2: " << aux2 << endl;
    //  }

     while (getline(file, line)){
         if (is_blank_line(line)) //ignora se for uma linha em branco
            continue;
         tokens = get_tokens(line);
         cout << "'" << tokens[0] << "'" << endl;
     }

    
    file.close();

    return 0;
}
