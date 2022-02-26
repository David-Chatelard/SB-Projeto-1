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
#include <tuple>

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
	string label; // label que causou o erro
	string message; // mensagem do erro
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

    // Pega o rotulo
    if (no_comments_line.find(':') != string::npos){ // se tiver rotulo
        getline(ss_no_comments_line, label, ':'); //bota tudo que vem antes de ':' em label
        // ##################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
        // !!!Fazer os corner case para caso tenha vários ':' na mesma linha varios rotulos na mesma linha (por exemplo L1:L2: ADD B)!!!
        // !!!Fazer os corner case para caso tenha quebra de linha para o rótulo!!!
        // !!!Fazer os corner case para caso tenha rotulo com nome invalido(com numero no comeco, caracteres invalidos, palavra reservada)!!!

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

    stringstream ss_no_comments_line(no_comments_line.substr(no_comments_line.find(':')).erase(0, 1)); // pega o texto da linha que vem depois do rotulo e apaga o ':'
    // cout << "linha depois do rotulo >>-- '" << ss_no_comments_line.str() << "'"  << endl;   //TESTE
    }

    // Pega o opcode
    ss_no_comments_line >> opcode;
    // cout << "opcode >>-- '" << opcode << "'"  << endl;   //TESTE
    // ##################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
    // !!!Fazer os corner case para caso tenha opcode invalido!!!
    // !!!Fazer os corner case para caso tenha numero invalido de argumentos dependendo do opcode!!!
    // !!!Fazer os corner case para verificar se tem ',' caso seja COPY!!!


    // Pega arg1, caso não seja STOP
    if (opcode != "STOP") {
        ss_no_comments_line >> arg1;
        // cout << "arg1 >>-- '" << arg1 << "'"  << endl;   //TESTE
    }


    // Pega arg2, caso seja COPY
    if (opcode == "COPY") {
        ss_no_comments_line >> arg2;
        // cout << "virgula arg2 >>-- '" << arg2 << "'"  << endl;   //TESTE
        ss_no_comments_line >> arg2;
        // cout << "arg2 >>-- '" << arg2 << "'"  << endl;   //TESTE
    }


    tokens.push_back(label);
    tokens.push_back(opcode);
    tokens.push_back(arg1);
    tokens.push_back(arg2);
    return tokens;
}

// Adiciona os rotulos na tabela de simbolos e atualiza a tabela de erros caso ocorra algum erro envolvendo rotulos
tuple<vector <Item_symbols_table>, vector <Item_errors_table>> first_pass_labels(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_symbols_table> symbols_table, vector <Item_errors_table> errors_table, int *position_counter, int *line_counter){
    bool found_symbol;
    vector <Item_symbols_table>::iterator iter;
    Item_symbols_table symbol_item;
    Item_errors_table error_item;
    string symbol;

    if (!tokens[0].empty()) { // possui rotulo
        symbol = tokens[0];
        iter = find_if(symbols_table.begin(), symbols_table.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
        found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao

        if (found_symbol) { // se o rotulo ja existe
            // erro de rotulo ja definido, talvez tenha que passar a tabela de erros pra essa funcao
            error_item.label = tokens[0];
            error_item.message = "Erro SINTATICO/SEMANTICO VERIFICAR QUAL EH DEPOIS E ARRUMAR AQUI";
            error_item.line_number = *line_counter;
            errors_table.push_back(error_item);
        }
        else{ //rotulo nao existe
            symbol_item.symbol = symbol;
            symbol_item.address = *position_counter;
            symbols_table.push_back(symbol_item); // adiciona o simbolo na tabela
        }
        

    }


    return {symbols_table, errors_table};
}


int main(int argc, char const *argv[]) {
    int j = 1;

    int position_counter = 0;
    int line_counter = 1;

    string file_name(argv[2]);
    ifstream file(file_name);
    string line, no_comments_line, label;

    vector <string> tokens;
    vector <Item_symbols_table> symbols_table;
    vector <Item_errors_table> errors_table;

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

    // Primeira passagem
     while (getline(file, line)){
        //  Ignora se for uma linha em branco
         if (is_blank_line(line)) {
            line_counter++;
            continue;
         }
        // Passa tudo para uppercase, para não ser case sensitive
        for (int i = 0; i < line.length(); ++i){
	 	    line[i] = toupper(line[i]);
	    }
        // Pega os tokens da linha
         tokens = get_tokens(line);
         cout << "linha " << j << ": --";               //TESTE
         cout << "label: '" << tokens[0] << "' --";     //TESTE
         cout << "opcode: '" << tokens[1] << "' --";    //TESTE
         cout << "arg1: '" << tokens[2] << "' --";      //TESTE
         cout << "arg2: '" << tokens[3] << "'" << endl; //TESTE
         j++;                                           //TESTE

        //  Adiciona os rotulos na tabela de simbolos e atualiza a tabela de erros caso ocorra algum erro envolvendo rotulos
        tie(symbols_table, errors_table) = first_pass_labels(tokens, operations_table, symbols_table, errors_table, &position_counter, &line_counter);
        position_counter += operations_table[tokens[1]].memory_space; //tenho que arrumar para caso seja SPACE ou CONST no final, vou ter que fazer a tabela de dados.

        // #############################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
        // COMENTARIO PARA ME AJUDAR COM O CONST E SPACE NO MEIO DO CODIGO
        // CONST e SPACE pode estar no meio do código, mas vai ter que deslocar para o final(na ordem que aparece no código), para ficar no final no arquivo objeto e o endereço deles na tabela de símbolos vai ser esse endereço no final(não o endereço que seria no meio do código)
        // #############################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
        line_counter++;

     }
    cout << "Tabela de simbolos:------------------------" << endl;
    for (auto symbol_iter : symbols_table) {
        cout << "Simbolo: " << symbol_iter.symbol << " --- ";
        cout << "Endereco: " << symbol_iter.address << endl;
    }
    cout << "Tabela de erros:------------------------" << endl;
    for (auto error_iter : errors_table) {
        cout << "Label: " << error_iter.label << " --- ";
        cout << "Mensagem: " << error_iter.message << " --- ";
        cout << "Linha: " << error_iter.line_number << endl;
    }

    
    file.close();

    return 0;
}
