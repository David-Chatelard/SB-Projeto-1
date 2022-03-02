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
#include<stdio.h>

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
    bool used = false; // indica se o simbolo foi utilizado em algum lugar do codigo
    int line; // linha onde o simbolo foi declarado
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
    // stringstream ss_no_comments_line_aux(no_comments_line);          //TESTE

    // Pega o rotulo
    // TEM QUE FAZER PARA O CASO DE TER A DECLARAÇÃO DO RÓTULO E DEPOIS UMA QUEBRA DE LINHA PARA O RESTO
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
        arg1.erase(remove(arg1.begin(), arg1.end(), ','), arg1.end()); //remove a virgula caso ela esteja colada no arg1
    }


    // Pega arg2, caso seja COPY
    if (opcode == "COPY") {
        // FAZER A VERIFICAÇÃO SE TEM SOMENTE UMA ',' E SOMENTE UM ' ' ENTRE OS ARGUMENTOS
        ss_no_comments_line >> arg2; // pega a virgula, caso tenha varios espacos entre o arg1 e a virgula
        // cout << "virgula arg2 >>-- '" << arg2 << "'"  << endl;   //TESTE
        ss_no_comments_line >> arg2; // pega o argumento em si
        // cout << "arg2 >>-- '" << arg2 << "'"  << endl;   //TESTE
    }


    tokens.push_back(label);
    tokens.push_back(opcode);
    tokens.push_back(arg1);
    tokens.push_back(arg2);
    return tokens;
}


// Adiciona os rotulos dos EQU na tabela de simbolos da passagem 0 e atualiza a tabela de erros caso ocorra algum erro envolvendo rotulos
tuple<vector <Item_symbols_table>, vector <Item_errors_table>> zero_pass_labels_equ(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_symbols_table> symbols_table, vector <Item_errors_table> errors_table, int *position_counter, int *line_counter){
    bool found_symbol;
    vector <Item_symbols_table>::iterator iter;
    Item_symbols_table symbol_item;
    Item_errors_table error_item;
    string symbol;

    // Procura rotulo
    if (!tokens[0].empty()) { // possui rotulo
        symbol = tokens[0];
        iter = find_if(symbols_table.begin(), symbols_table.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
        found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao

        if (found_symbol) { // Se o rotulo ja existe
            // Erro de rotulo ja definido
            error_item.label = tokens[0];
            error_item.message = "Erro SEMANTICO, rotulo repetido";
            error_item.line_number = *line_counter;
            errors_table.push_back(error_item);
        }
        else{ //Rotulo nao existe
            // Adiciona o simbolo na tabela
            symbol_item.symbol = symbol;
            symbol_item.address = stoi(tokens[2]);
            symbol_item.line = *line_counter;
            symbols_table.push_back(symbol_item);
        }
    }
    return {symbols_table, errors_table};
}


tuple<string, vector <Item_symbols_table>> write_label(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_symbols_table> symbols_table, vector <Item_errors_table> errors_table, int *position_counter, int *line_counter) {
    int i;

    if (!tokens[2].empty()) { // possui arg1
        for (i = 0; i < symbols_table.size(); i++) { // percorre a tabela de simbolos
            if (symbols_table[i].symbol == tokens[2]) { // se o simbolo for igual ao rotulo
                tokens[2] = to_string(symbols_table[i].address); // troca o rotulo pelo valor na tabela de simbolos
                symbols_table[i].used = true; // indica que o rotulo foi usado
            }
        }
    }

    if (!tokens[3].empty()) { // possui arg2
        for (i = 0; i < symbols_table.size(); i++) { // percorre a tabela de simbolos
            if (symbols_table[i].symbol == tokens[3]) { // se o simbolo for igual ao rotulo
                tokens[3] = to_string(symbols_table[i].address); // troca o rotulo pelo valor na tabela de simbolos
                symbols_table[i].used = true; // indica que o rotulo foi usado
            }
        }
    }

    if (tokens[0].empty()) { // linha sem rotulo
        if (tokens[3].empty()){ // nao tem arg2
            return {tokens[1] + ' ' + tokens[2], symbols_table};
        }
        // tem arg2
        return {tokens[1] + ' ' + tokens[2] + ',' + ' ' + tokens[3], symbols_table};
    }
    else{ //linha com rotulo
        if (tokens[3].empty()){ // nao tem arg2
            return {tokens[0] + ':' + ' ' + tokens[1] + ' ' + tokens[2], symbols_table};
        }
        // tem arg2
        return {tokens[0] + ':' + ' ' + tokens[1] + ' ' + tokens[2] + ',' + ' ' + tokens[3], symbols_table};
    }
}

string format_line(vector <string> tokens){
    if (tokens[0].empty()) { // linha sem rotulo
        if (tokens[3].empty()){ // nao tem arg2
            return tokens[1] + ' ' + tokens[2];
        }
        // tem arg2
        return tokens[1] + ' ' + tokens[2] + ',' + ' ' + tokens[3];
    }
    else{ //linha com rotulo
        if (tokens[3].empty()){ // nao tem arg2
            return tokens[0] + ':' + ' ' + tokens[1] + ' ' + tokens[2];
        }
        // tem arg2
        return tokens[0] + ':' + ' ' + tokens[1] + ' ' + tokens[2] + ',' + ' ' + tokens[3];
    }
}

// Adiciona os rotulos na tabela de simbolos e atualiza a tabela de erros caso ocorra algum erro envolvendo rotulos
tuple<vector <Item_symbols_table>, vector <Item_errors_table>> first_pass_labels(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_symbols_table> symbols_table, vector <Item_errors_table> errors_table, int *position_counter, int *line_counter){
    bool found_symbol;
    vector <Item_symbols_table>::iterator iter;
    Item_symbols_table symbol_item;
    Item_errors_table error_item;
    string symbol;

    // Procura rotulo
    if (!tokens[0].empty()) { // possui rotulo
        symbol = tokens[0];
        iter = find_if(symbols_table.begin(), symbols_table.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
        found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao

        if (found_symbol) { // Se o rotulo ja existe
            // Erro de rotulo ja definido
            error_item.label = tokens[0];
            error_item.message = "Erro SEMANTICO, rotulo repetido";
            error_item.line_number = *line_counter;
            errors_table.push_back(error_item);
        }
        else{ //Rotulo nao existe
            // Adiciona o simbolo na tabela
            symbol_item.symbol = symbol;
            symbol_item.address = *position_counter;
            symbol_item.line = *line_counter;
            symbols_table.push_back(symbol_item);
        }
    }
    return {symbols_table, errors_table};
}

int main(int argc, char const *argv[]) {
    int j = 1; //TESTE
    int index, macro_line;

    char use_mode = argv[1][1];

    bool found_symbol, if_is_valid = false, line_before_is_if = false, used_macro = false;

    int position_counter = 0;
    int line_counter = 1;

    string input_file_name(argv[2]); // nome do codigo fonte a ser lido
    ifstream file(input_file_name);  // leitura do arquivo fonte

    string equ_if_file_name("EQU_IF.asm");
    ofstream ofile_equ_if(equ_if_file_name); // escrita do codigo intermediario a ser gerado depois da passada EQU e IF

    string macro_file_name; // string que sera o nome do arquivo a ser lido no processamento da MACRO

    string pre_processed_file_name("pre_processed.asm");
    ofstream ofile_pre_processed_file(pre_processed_file_name); // escrita do codigo pre processado a ser gerado depois das passadas de EQU, IF e MACRO

    string output_file_name(argv[3]);
    ofstream output_file(output_file_name); // escrita do codigo objeto final

    string line; // linha lida do arquivo
    string macro_name(""); // nome da MACRO
    // string no_comments_line, label;         //TESTE

    vector <string> tokens;
    vector <string> macro_lines; // vetor com as linhas da MACRO
    vector <Item_symbols_table> symbols_table_equ_if; // tabela de simbolos utilizada para EQU e IF
    vector <Item_symbols_table> symbols_table; // tabela de simbolos para as duas passagens
    vector <Item_errors_table> errors_table_p; // tabela de erro para o -p (EQU e IF)
    vector <Item_errors_table> errors_table_m; // tabela de erro para o -m (MACRO)
    vector <Item_errors_table> errors_table_o; // tabela de erro para o -o (Duas passagens)
    vector <Item_symbols_table>::iterator iter;

    Item_errors_table error_item;

    map <string, Item_operations_table> operations_table;

    string aux, aux2;   //TESTE
    string symbol;

    if (!file.is_open()){
        cout << "file not found." << endl;
        return 1;
    }

    if (!ofile_equ_if.is_open()){
        cout << "couldn't open file." << endl;
        return 1;
    }

    if (!ofile_pre_processed_file.is_open()){
        cout << "couldn't open file." << endl;
        return 1;
    }

    if (!output_file.is_open()){
        cout << "couldn't open file." << endl;
        return 1;
    }

    operations_table = begin_operations_table(); // inicializa a tabela de operacoes

    //  while (getline(file, line)) {                                                                           //TESTE
    //      cout << "Linha: " << line << endl;                                                                  //TESTE
    //      stringstream ss_line(line);                                                                         //TESTE
    //      getline(ss_line, no_comments_line, ';');                                                            //TESTE
    //      cout << "Linha sem comentarios: " << no_comments_line << endl;                                      //TESTE
    //      cout << "Testando o .find(): " << no_comments_line.find('u') << endl;                               //TESTE
    //      cout << "Testando o .substr(): " << no_comments_line.substr(no_comments_line.find('u')) << endl;    //TESTE
    //      aux = no_comments_line.substr(no_comments_line.find('u'));                                          //TESTE
    //      aux.erase(0, 1);                                                                                    //TESTE
    //      cout << "Testando o .erase(): " << aux << endl;                                                     //TESTE
    //      ss_line >> ws >> aux2;                                                                              //TESTE
    //      cout << "Testando o aux2: " << aux2 << endl;                                                        //TESTE
    //  }                                                                                                       //TESTE

    if (use_mode == 'p' || use_mode == 'o') {
        // Passagem EQU e IF
        position_counter = 0;
        line_counter = 1;
        while (getline(file, line)){
            //  Ignora se for uma linha em branco
            if (is_blank_line(line)) {
                line_counter++;
                continue;
            }
            // Passa tudo para uppercase, para não ser case sensitive
            transform(line.begin(), line.end(),line.begin(), ::toupper);
            // Pre processar EQU e IF
            tokens = get_tokens(line);

            // cout << "linha " << line_counter << ": --";     //TESTE
            // cout << "label: '" << tokens[0] << "' --";      //TESTE
            // cout << "opcode: '" << tokens[1] << "' --";     //TESTE
            // cout << "arg1: '" << tokens[2] << "' --";       //TESTE
            // cout << "arg2: '" << tokens[3] << "'" << endl;  //TESTE

            if (tokens[1] == "EQU") { // se a linha tiver EQU
                // Gera uma tabela de simbolos com os simbolos dos EQU
                tie(symbols_table_equ_if, errors_table_p) = zero_pass_labels_equ(tokens, operations_table, symbols_table_equ_if, errors_table_p, &position_counter, &line_counter);
            }
            else{ // se a linha nao tiver EQU
                // Substitui os simbolos dos EQU por seus valores
                tie(line, symbols_table_equ_if) = write_label(tokens, operations_table, symbols_table_equ_if, errors_table_p, &position_counter, &line_counter);
                // ofile_equ_if << line << endl; // escreve no arquivo objeto sem os simbolos do EQU, so com os valores BOTEI LA EM BAIXO
            }

            if (tokens[1] == "IF") { // se a linha tiver IF
                symbol = tokens[2];
                iter = find_if(symbols_table_equ_if.begin(), symbols_table_equ_if.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
                found_symbol = (iter != symbols_table_equ_if.end()); // indica se o rotulo foi encontrado ou nao
                index = distance(symbols_table_equ_if.begin(), iter); // indice do rotulo na tabela de simbolos do equ if
                if (found_symbol){ //rotulo encontrado
                    if (symbols_table_equ_if[index].address != 0) {
                        if_is_valid = true;
                    }
                    else{
                        if_is_valid = false;
                    }
                }
                else{ //rotulo nao encontrado
                    // Erro, usando IF com rotulo nao declarado por um EQU
                    error_item.label = tokens[2];
                    error_item.message = "Erro SEMANTICO, IF com rotulo nao declarado por um EQU";
                    error_item.line_number = line_counter;
                    errors_table_p.push_back(error_item);
                }
                line_before_is_if = true;
                getline(file, line); // Pega a linha seguinte ao IF
                line_counter++;
                if (is_blank_line(line)) {
                    line_counter++;
                    continue;
                }
                // Passa tudo para uppercase, para não ser case sensitive
                transform(line.begin(), line.end(),line.begin(), ::toupper);      
            }
            else{ // se a linha nao tiver IF
                line_before_is_if = false;
            }
            if (!if_is_valid && line_before_is_if) { // O IF anterior a essa linha nao foi valido, entao a linha nao sera usada no codigo
                line_counter++;
                continue;
            }
            if (if_is_valid && line_before_is_if && tokens[1] != "EQU") { // O IF anterior a essa linha foi valido, entao a linha sera usada no codigo
                tokens = get_tokens(line);
                tie(line, symbols_table_equ_if) = write_label(tokens, operations_table, symbols_table_equ_if, errors_table_p, &position_counter, &line_counter);
                ofile_equ_if << line << endl; // escreve no arquivo objeto sem os simbolos do EQU, so com os valores
            }
            if (!line_before_is_if && tokens[1] != "EQU") { // Nao tinha IF antes dessa linha
                ofile_equ_if << line << endl; // escreve no arquivo objeto sem os simbolos do EQU, so com os valores
            }
            // ofile_equ_if << line << endl; // escreve no arquivo objeto sem os simbolos do EQU, so com os valores

            line_counter++;
        }
        // Adiciona os erros para rotulo de EQU nao utilizado
        for (auto symbol_iter : symbols_table_equ_if) {
            if (!symbol_iter.used) {
                error_item.label = symbol_iter.symbol;
                error_item.message = "Erro SEMANTICO, EQU com rotulo nao utilizado";
                error_item.line_number = symbol_iter.line;
                errors_table_p.push_back(error_item);
            }
        }
    }

    file.close();
    ofile_equ_if.close();

    if (use_mode == 'o') {        
        macro_file_name = equ_if_file_name; // leitura do codigo com EQU e IF ja processadas
    }
    else if (use_mode == 'm') {
        macro_file_name = input_file_name; // leitura do codigo original
    }

    if (use_mode == 'm' || use_mode == 'o'){
        ifstream ifile_macro(macro_file_name);
        if (!ifile_macro.is_open()){
            cout << "file not found." << endl;
            return 1;
        }

        // Passagem MACRO
        position_counter = 0;
        line_counter = 1;
        while(getline(ifile_macro, line)) {
            //  Ignora se for uma linha em branco
            if (is_blank_line(line)) {
                line_counter++;
                continue;
            }
            // Passa tudo para uppercase, para não ser case sensitive
            transform(line.begin(), line.end(),line.begin(), ::toupper);

            // Pre processar MACRO
            tokens = get_tokens(line);
            line = format_line(tokens);
            if (tokens[1] == "MACRO"){    // se a linha for definicao de uma MACRO
                macro_name = tokens[0];
                macro_line = line_counter;
                while(true) { // ler as linhas seguintes ate achar ENDMACRO, quando achar ENDMACRO sai do while
                    line_counter++;
                    getline(ifile_macro, line);

                    //  Ignora se for uma linha em branco
                    if (is_blank_line(line)) {
                        continue;
                    }
                    // Passa tudo para uppercase, para não ser case sensitive
                    transform(line.begin(), line.end(),line.begin(), ::toupper);
                    tokens = get_tokens(line);
                    line = format_line(tokens);

                    if (line.find("ENDMACRO") != string::npos) { // se for a linha que tiver ENDMACRO
                        break; // termina o while
                    }
                    if (ifile_macro.eof()) { // chegou no final do arquivo e nao encontrou ENDMACRO
                        // Adiciona o erro para falta de ENDMACRO
                        error_item.label = macro_name;
                        error_item.message = "Erro SEMANTICO, falta de ENDMACRO";
                        error_item.line_number = line_counter;
                        errors_table_m.push_back(error_item);
                    }

                    macro_lines.push_back(line); // adiciona a linha no vetor da MACRO
                }
            }

            else if (line.find("ENDMACRO") != string::npos) { // se for a linha que tiver ENDMACRO
                line_counter++;
                continue; // vai para a proxima linha
            }

            else if (tokens[1] == macro_name) {   // se a linha tiver o rotulo da MACRO
                used_macro = true;
                for (int i = 0; i < macro_lines.size(); i++) {
                    ofile_pre_processed_file << macro_lines[i] << endl; // adiciona as linhas da MACRO no arquivo pre processado
                }
            }

            else { // se nao for a definicao nem a chamada de uma MACRO
                ofile_pre_processed_file << line << endl; // adiciona a linha no arquivo pre processado
            }

            line_counter++;
        }
        // Adiciona o erro MACRO nao utilizada
        if (!used_macro && !macro_name.empty()) { // se a MACRO nao for usada e tiver sido definida
            error_item.label = macro_name;
            error_item.message = "Erro SEMANTICO, MACRO nao utilizada";
            error_item.line_number = macro_line;
            errors_table_m.push_back(error_item);
        }
    
        ifile_macro.close();
    }
    ofile_pre_processed_file.close();

    if (use_mode == 'o') {
        ifstream ifile_pre_processed_file(pre_processed_file_name); // leitura do codigo com EQU, IF e MACRO ja processadas

        if (!ifile_pre_processed_file.is_open()){
            cout << "file not found." << endl;
            return 1;
        }

        // Primeira passagem
        position_counter = 0;
        line_counter = 1;
        while (getline(ifile_pre_processed_file, line)){
            //  Ignora se for uma linha em branco
            if (is_blank_line(line)) {
                line_counter++;
                continue;
            }
            // Passa tudo para uppercase, para não ser case sensitive
            transform(line.begin(), line.end(),line.begin(), ::toupper);
            // Pega os tokens da linha
            tokens = get_tokens(line);
            cout << "linha " << line_counter << ": --";    //TESTE
            cout << "label: '" << tokens[0] << "' --";     //TESTE
            cout << "opcode: '" << tokens[1] << "' --";    //TESTE
            cout << "arg1: '" << tokens[2] << "' --";      //TESTE
            cout << "arg2: '" << tokens[3] << "'" << endl; //TESTE

            //  Adiciona os rotulos na tabela de simbolos e atualiza a tabela de erros caso ocorra algum erro envolvendo rotulos
            tie(symbols_table, errors_table_o) = first_pass_labels(tokens, operations_table, symbols_table, errors_table_o, &position_counter, &line_counter);
            position_counter += operations_table[tokens[1]].memory_space; //tenho que arrumar para caso seja SPACE ou CONST no final, vou ter que fazer a tabela de dados.

            // #############################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
            // COMENTARIO PARA ME AJUDAR COM O CONST E SPACE NO MEIO DO CODIGO
            // Talvez usar uma flag end_program, que será true quando tiver passado pelo STOP, da pra setar o valor na função get_tokens()
            // CONST e SPACE pode estar no meio do código, mas vai ter que deslocar para o final(na ordem que aparece no código), para ficar no final no arquivo objeto e o endereço deles na tabela de símbolos vai ser esse endereço no final(não o endereço que seria no meio do código)
            // #############################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
            
            line_counter++;

        }
        ifile_pre_processed_file.close();
    }
    cout << "Tabela de simbolos:------------------------" << endl;
    for (auto symbol_iter : symbols_table) {
        cout << "Simbolo: " << symbol_iter.symbol << " --- ";
        cout << "Endereco: " << symbol_iter.address << endl;
    }
    if (use_mode == 'p') {
        cout << "Tabela de erros -p:------------------------" << endl;
        for (auto error_iter : errors_table_p) {
            cout << "Label: " << error_iter.label << " --- ";
            cout << "Mensagem: " << error_iter.message << " --- ";
            cout << "Linha do arquivo original: " << error_iter.line_number << endl;
        }
    }
    if (use_mode == 'm') {
        cout << "Tabela de erros -m:------------------------" << endl;
        for (auto error_iter : errors_table_m) {
            cout << "Label: " << error_iter.label << " --- ";
            cout << "Mensagem: " << error_iter.message << " --- ";
            cout << "Linha do arquivo original: " << error_iter.line_number << endl;
        }
    }
    if (use_mode == 'o') {
        cout << "Tabela de erros -o:------------------------" << endl;
        for (auto error_iter : errors_table_o) {
            cout << "Label: " << error_iter.label << " --- ";
            cout << "Mensagem: " << error_iter.message << " --- ";
            cout << "Linha do arquivo pre-processado: " << error_iter.line_number << endl;
        }
    }

    output_file.close();

    // if(remove("EQU_IF.asm") != 0 ){
    //     cout << "Error deleting file" << endl;
    // }
    // else{
    //     cout << "File successfully deleted" << endl;
    // }

    // if(remove("pre_processed.asm") != 0 ){
    //     cout << "Error deleting file" << endl;
    // }
    // else{
    //     cout << "File successfully deleted" << endl;
    // }

    return 0;
}
