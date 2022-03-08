/*
Aluno: David Fanchic Chatelard
Matricula: 180138863
Professor: Bruno Luiggi Macchiavello Espinoza
Disciplina: Software Basico
Compilador: MinGW gcc g++ 9.2.0
Sistema Operacional: Windows 10
IDE: Visual Studio Code
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
using std::cout;
using std::cin;

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
	int address = -1; // endereco de memoria onde o simbolo esta, -1 caso não seja definido, -2 caso seja rotulo de CONST ou SPACE(neste caso sera posto ao final do codigo e o valor -2 vai ser mudado para um endereco mesmo no final)
    bool used = false; // indica se o simbolo foi utilizado em algum lugar do codigo
    int line; // linha onde o simbolo foi declarado
    int value = 0;
    bool is_const = false; // eh definida na funcao first_pass_labels
    bool is_space = false; // eh definida na funcao first_pass_labels
    bool before_stop = true; // eh definida na funcao first_pass_labels
};

class Item_errors_table {
public:
	string label; // label que causou o erro
	string message; // mensagem do erro
	int line_number; //numero da linha do erro
};


// Funcao para inicializar a tabela de operacoes
auto begin_operations_table(){
	map <string, Item_operations_table> table;
	table["ADD"].opcode = "1", table["ADD"].num_args = 1, table["ADD"].memory_space = 2;
	table["SUB"].opcode = "2", table["SUB"].num_args = 1, table["SUB"].memory_space = 2;
	table["MULT"].opcode = "3", table["MULT"].num_args = 1, table["MULT"].memory_space = 2;
	table["DIV"].opcode = "4", table["DIV"].num_args = 1, table["DIV"].memory_space = 2;
	table["JMP"].opcode = "5", table["JMP"].num_args = 1, table["JMP"].memory_space = 2;
	table["JMPN"].opcode = "6", table["JMPN"].num_args = 1, table["JMPN"].memory_space = 2;
	table["JMPP"].opcode = "7", table["JMPP"].num_args = 1, table["JMPP"].memory_space = 2;
	table["JMPZ"].opcode = "8", table["JMPZ"].num_args = 1, table["JMPZ"].memory_space = 2;
	table["COPY"].opcode = "9", table["COPY"].num_args = 2, table["COPY"].memory_space = 3;
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

// Verifica se uma string eh um numero positivo ou negativo
bool is_number(string str) {
    if (str[0] == '-') { // se for um numero negativo
        str.erase(0, 1); // remove o sinal de -
    }
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

// Verifica se o rotulo eh valido de acordo com as regras
bool is_valid_variable(string label, map <string, Item_operations_table> operations_table, bool line_has_const) {
    // Verifica se eh string vazia
    if (label.empty()) {
        return true;
    }
    // Verifica o tamanho do rotulo
    if (label.length() > 99) { // se tiver mais do que 99 caracteres
        return false;
    }
    // Verifica o primeiro digito
    if (!isalpha(label[0]) && (label[0] != '_')) { // se o primeiro caracter nao for letra nem '_'
        if (line_has_const && is_number(label)) { // se tiver CONST na linha e a label for somente um numero
            return true;
        }
        return false;
    }
    // Verifica o resto do rotulo
    for (int i = 1; i < label.length(); i++) {
        if (!isalpha(label[i]) && (label[i] != '_') && !isdigit(label[i])){ // se o caracter nao for letra nem '_' nem numero
            return false;
        }
    }
    // Verifica o rotulo inteiro
    if (operations_table.count(label) || label == "CONST" || label == "SPACE" || label == "EQU" || label == "IF" || label == "MACRO") { // se o rotulo estiver na tabela de operacoes ou for uma diretiva
        return false;
    }
    // Se tiver passado em todos os testes
    return true;
}

// Verifica se a instrucao eh um opcode ou diretiva ou se pelo menos eh uma palavra valida(por exemplo para 2ADD vai retornar false, mas para ADD2 vai retornar true)
// Nao verifica se eh uma instrucao valida, somente verifica se a palavra escrita eh valida
bool is_valid_instruction(string label, map <string, Item_operations_table> operations_table) {
    // Verifica se eh string vazia
    if (label.empty()) {
        return true;
    }
    // Verifica o tamanho do rotulo
    if (label.length() > 99) { // se tiver mais do que 99 caracteres
        return false;
    }
    // Verifica o primeiro digito
    if (!isalpha(label[0]) && (label[0] != '_')) { // se o primeiro caracter nao for letra nem '_'
        return false;
    }
    // Verifica o resto do rotulo
    for (int i = 1; i < label.length(); i++) {
        if (!isalpha(label[i]) && (label[i] != '_') && !isdigit(label[i])){ // se o caracter nao for letra nem '_' nem numero
            return false;
        }
    }
    // Verifica o rotulo inteiro
    if (operations_table.count(label) || label == "CONST" || label == "SPACE" || label == "EQU" || label == "IF" || label == "MACRO") { // se o rotulo estiver na tabela de operacoes ou for uma diretiva
        return true;
    }
    // Se tiver passado em todos os testes
    return true;
}

// Separa os tokens da linha
tuple<vector <string>, bool> get_tokens(string line, char use_mode, ifstream &input_file, int *line_counter) {
    bool special_case = false, has_2_labels = false;
    vector <string> tokens, aux_tokens, aux_tokens2;
    string no_comments_line, label = "", opcode = "", arg1 = "", arg2 = "", aux_line;
    string aux; //TESTE

    // Inicializa o aux_tokens como vazio
    aux_tokens.push_back("");
    aux_tokens2.push_back("");

    stringstream ss_line(line); //transforma em stream

    // Remove os comentarios
    getline(ss_line, no_comments_line, ';');

    stringstream ss_no_comments_line(no_comments_line); //transforma em stream
    // stringstream ss_no_comments_line_aux(no_comments_line);          //TESTE

    // Pega o rotulo
    if (no_comments_line.find(':') != string::npos){ // se tiver rotulo
        getline(ss_no_comments_line, label, ':'); //bota tudo que vem antes de ':' em label
        // ##################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
        // !!!Fazer os corner case para caso tenha vários ':' na mesma linha varios rotulos na mesma linha (por exemplo L1:L2: ADD B)!!!

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

        // Para o caso de declarar o rotulo e quebrar a linha e continuar na proxima linha
        string str(no_comments_line.substr(no_comments_line.find(':')).erase(0, 1));
        str.erase(remove(str.begin(), str.end(), ' '), str.end()); // remove todos os ' '
        if(str.empty()) { // se nao tiver nada depois do ':'
            getline(input_file, aux_line); // pega a proxima linha
            (*line_counter)++;
            //  Ignora se for uma linha em branco
            if (is_blank_line(aux_line) || aux_line == " ") {
                getline(input_file, aux_line); // pega a proxima linha
                (*line_counter)++;
            }
            // Passa tudo para uppercase, para não ser case sensitive
            transform(aux_line.begin(), aux_line.end(),aux_line.begin(), ::toupper);
            // Pega os tokens
            tie(aux_tokens, has_2_labels) = get_tokens(aux_line, use_mode, input_file, line_counter);

            // std::cout << "linha depois do rotulo >>-- '" << ss_no_comments_line.str() << "'"  << endl;   //TESTE
        }
        else if (str.find(':') != string::npos){ // se tiver rotulo depois do primeiro rotulo
            has_2_labels = true;
        }
    }

    if (has_2_labels) { // se tiver 2 rotulos na mesma linha
        string str2(no_comments_line.substr(no_comments_line.find(':')).erase(0, 1)); // pega o texto da linha que vem depois do rotulo e apaga o ':'
        // cout << "linha depois do segundo rotulo >>-- '" << str2 << "'"  << endl;   //TESTE

        tie(aux_tokens2, has_2_labels) = get_tokens(str2, use_mode, input_file, line_counter);
        has_2_labels = true;
    }

    // Pega o opcode
    ss_no_comments_line >> opcode;
    // cout << "opcode >>-- '" << opcode << "'"  << endl;   //TESTE
    // ##################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################################
    // !!!Fazer os corner case para verificar se tem ',' caso seja COPY!!!


    // Pega arg1, caso não seja STOP
    // if (opcode != "STOP") {
    if (!opcode.empty()) {
        ss_no_comments_line >> arg1;
        // cout << "arg1 >>-- '" << arg1 << "'"  << endl;   //TESTE
        if (arg1.find(',') == arg1.length()-1){ // se a virgula estiver na ultima posicao
            arg1.erase(remove(arg1.begin(), arg1.end(), ','), arg1.end()); //remove a virgula caso ela esteja colada no arg1
        }
        else if (arg1.find(',') != string::npos) { // se a ',' nao estiver na ultima posicao do arg1 quer dizer que a instrucao estava sem espaco depois da ',' assim(COPY A,B), entao o arg1 ficou(A,B)
            special_case = true;
            arg2 = arg1.substr(arg1.find(',')).erase(0, 1); // pega tudo a partir da ',' e apaga a ','
            arg1 = arg1.erase(arg1.find(','));
            // cout << "CASO ESPECIAL #################################################" << endl;
            // cout << "arg1 >>-- '" << arg1 << "'"  << endl;   //TESTE
            // cout << "arg2 >>-- '" << arg2 << "'"  << endl;   //TESTE
            // cout << "CASO ESPECIAL #################################################" << endl;
        }
    }


    // Pega arg2, caso seja COPY e nao tenha ocorrido o caso de ser COPY A,B(sem espaco depois da virgula)
    // if (opcode == "COPY" && !special_case) {
    if (!special_case) {
        // FAZER A VERIFICAÇÃO SE TEM SOMENTE UMA ',' E SOMENTE UM ' ' ENTRE OS ARGUMENTOS
        ss_no_comments_line >> arg2; // pega a virgula, caso tenha varios espacos entre o arg1 e a virgula
        // cout << "virgula arg2 >>-- '" << arg2 << "'"  << endl;   //TESTE
        ss_no_comments_line >> arg2; // pega o argumento em si
        // cout << "arg2 >>-- '" << arg2 << "'"  << endl;   //TESTE
        if (arg2.find(',') != string::npos) { // se ainda tiver ',' no arg2 provavelmente a ',' estava colada no arg2 a linha era (COPY A ,B)
            arg2 = arg2.erase(0, 1); // apaga o primeiro caracter
        }
    }

    tokens.push_back(label);
    if (aux_tokens.size() > 1) { // se foi o caso de ter rotulo com quebra de linha depois do ':'
        // cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl; //TESTE
        // cout << aux_tokens.size() << endl;   //TESTE
        // cout << aux_tokens[1] << endl;       //TESTE
        // cout << aux_tokens[2] << endl;       //TESTE
        // cout << aux_tokens[3] << endl;       //TESTE

        tokens.push_back(aux_tokens[1]); // opcode
        tokens.push_back(aux_tokens[2]); // arg1
        tokens.push_back(aux_tokens[3]); // arg2
    }
    else if (has_2_labels) {
        // cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl; //TESTE
        // cout << aux_tokens2.size() << endl;   //TESTE
        // cout << aux_tokens2[0] << endl;       //TESTE
        // cout << aux_tokens2[1] << endl;       //TESTE
        // cout << aux_tokens2[2] << endl;       //TESTE
        // cout << aux_tokens2[3] << endl;       //TESTE

        tokens.push_back(aux_tokens2[1]); // opcode
        tokens.push_back(aux_tokens2[2]); // arg1
        tokens.push_back(aux_tokens2[3]); // arg2
        tokens.push_back(aux_tokens2[0]); // segundo rotulo da linha
    }
    else{
        tokens.push_back(opcode);
        tokens.push_back(arg1);
        tokens.push_back(arg2);
    }
    return {tokens, has_2_labels};
}

// Remove os espacos em branco desnecessarios da linha
string format_line(vector <string> tokens, bool has_2_labels){
    if (has_2_labels) {
        if (tokens[3].empty()){ // nao tem arg2
            return tokens[0] + ':' + tokens[4] + ':' + ' ' + tokens[1] + ' ' + tokens[2];
        }
        // tem arg2
        return tokens[0] + ':' + tokens[4] + ':' + ' ' + tokens[1] + ' ' + tokens[2] + ',' + ' ' + tokens[3];
    }
    else{
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

// Substitui os simbolos dos rotulos por seus valores
tuple<string, vector <Item_symbols_table>> write_label(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_symbols_table> symbols_table, vector <Item_errors_table> errors_table, int *position_counter, int *line_counter, bool has_2_labels) {
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

    return {format_line(tokens, has_2_labels), symbols_table};

    // if (tokens[0].empty()) { // linha sem rotulo
    //     if (tokens[3].empty()){ // nao tem arg2
    //         return {tokens[1] + ' ' + tokens[2], symbols_table};
    //     }
    //     // tem arg2
    //     return {tokens[1] + ' ' + tokens[2] + ',' + ' ' + tokens[3], symbols_table};
    // }
    // else{ //linha com rotulo
    //     if (tokens[3].empty()){ // nao tem arg2
    //         return {tokens[0] + ':' + ' ' + tokens[1] + ' ' + tokens[2], symbols_table};
    //     }
    //     // tem arg2
    //     return {tokens[0] + ':' + ' ' + tokens[1] + ' ' + tokens[2] + ',' + ' ' + tokens[3], symbols_table};
    // }
}

// Adiciona os rotulos na tabela de simbolos e atualiza a tabela de erros caso ocorra erro de rotulo ja definido
tuple<vector <Item_symbols_table>, vector <Item_errors_table>> first_pass_labels(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_symbols_table> symbols_table, vector <Item_errors_table> errors_table, int *position_counter, int *line_counter, bool reached_stop){
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
            error_item.message = "Erro SEMANTICO, declaracao de rotulo repetido";
            error_item.line_number = *line_counter;
            errors_table.push_back(error_item);
        }
        else{ //Rotulo nao existe
            if (tokens[1] == "CONST" || tokens[1] == "SPACE") { //se for o rotulo de uma diretiva, o endeco sera resolvido no fim da primeira passagem, para ficar no final do codigo objeto
                symbol_item.symbol = symbol;
                symbol_item.line = *line_counter;
                if (tokens[1] == "CONST") {
                    symbol_item.is_const = true;
                    if (!tokens[2].empty()){
                        symbol_item.value = stoi(tokens[2]);
                    }
                }
                if (tokens[1] == "SPACE") {
                    symbol_item.is_space = true;
                }
                if (reached_stop) { // eh uma CONST ou SPACE que veio depois do STOP, ja pode botar o endereco certo
                    symbol_item.address = *position_counter;
                    symbol_item.before_stop = false;
                }
                else { // eh uma CONST ou SPACE que veio antes do STOP, vai ter que arrumar o endereco depois
                    symbol_item.address = -2; // o -2 significa que vai ter que resolver depois do STOP na primeira passada
                }
                symbols_table.push_back(symbol_item);
            }
            else { // se for um rotulo sem ser de uma diretiva CONST ou SPACE
                // Adiciona o simbolo na tabela
                symbol_item.symbol = symbol;
                symbol_item.address = *position_counter;
                symbol_item.line = *line_counter;
                symbols_table.push_back(symbol_item);
            }
        }
    }
    return {symbols_table, errors_table};
}

// Atualiza o contador de posicao e a tabela de erros caso ocorra erro de operacao invalida
vector <Item_errors_table> first_pass_instructions(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_errors_table> errors_table, int *position_counter, int *line_counter, bool reached_stop){
    Item_errors_table error_item;
    
    if (operations_table.count(tokens[1])) { // se a operacao estiver na tabela de operacoes
        *position_counter += operations_table.at(tokens[1]).memory_space;
    }
    else if ((tokens[1] == "CONST" || tokens[1] == "SPACE") && reached_stop) { //se for uma diretiva depois do STOP
        (*position_counter)++;
    }
    else if (tokens[1] != "CONST" && tokens[1] != "SPACE") { // nao achou na tabela de operacoes e nao eh diretiva
        // Erro de operacao invalida
        error_item.label = tokens[1];
        error_item.message = "Erro SINTATICO, operacao invalida, token de operacao invalido";
        error_item.line_number = *line_counter;
        errors_table.push_back(error_item);
    }

    return errors_table;
}

// Procura os rotulos na tabela de simbolos e atualiza a tabela de erros caso ocorra erro de declaracao de rotulo ausente
vector <Item_errors_table> second_pass_labels(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_symbols_table> symbols_table, vector <Item_errors_table> errors_table, int *position_counter, int *line_counter) {
    bool found_symbol;
    vector <Item_symbols_table>::iterator iter;
    Item_errors_table error_item;
    string symbol;

    symbol = tokens[2];
    iter = find_if(symbols_table.begin(), symbols_table.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
    found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao
    if (!found_symbol && tokens[2] != "" && is_valid_variable(tokens[2], operations_table, false)) { // se nao encontrou o simbolo e o simbolo nao eh uma string vazia e so possui caracteres validos
        // Erro de declaracao de rotulo ausente
        error_item.label = tokens[2];
        error_item.message = "Erro SEMANTICO, declaracao de rotulo ausente";
        error_item.line_number = *line_counter;
        errors_table.push_back(error_item);
    }

    if (tokens[1] == "COPY") { // se a operacao for COPY tem que pegar os dois operandos
        symbol = tokens[3];
        iter = find_if(symbols_table.begin(), symbols_table.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
        found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao
        if (!found_symbol && tokens[3] != "" && is_valid_variable(tokens[3], operations_table, false)) { // se nao encontrou o simbolo e o simbolo nao era uma string vazia e so possui caracteres validos
            // Erro de declaracao de rotulo ausente
            error_item.label = tokens[3];
            error_item.message = "Erro SEMANTICO, declaracao de rotulo ausente";
            error_item.line_number = *line_counter;
            errors_table.push_back(error_item);
        }
    }

    return errors_table;
}

// Gera o codigo objeto e atualiza a tabela de erros caso ocorra erro de quantidade de operandos invalida
vector <Item_errors_table> second_pass_instructions(vector <string> tokens, map <string, Item_operations_table> operations_table, vector <Item_symbols_table> symbols_table, vector <Item_errors_table> errors_table, ofstream &output_file, int *position_counter, int *line_counter, bool reached_stop) {
    int index;
    int value;
    bool found_symbol;
    vector <Item_symbols_table>::iterator iter;
    Item_errors_table error_item;    
    string symbol, opcode = tokens[1], arg1 = tokens[2], arg2 = tokens[3];

    if (operations_table.count(opcode)) { // se o opcode estiver na tabela de operacoes
        *position_counter += operations_table.at(opcode).memory_space; // atualiza o contador de posicao

        if (opcode == "STOP" && arg1.empty() && arg2.empty()) { // se for STOP e nao tiver nenhum operando
            output_file << operations_table.at(opcode).opcode << ' '; // escreve o opcode de STOP
        }

        else if (opcode == "COPY" && !arg1.empty() && !arg2.empty()) { // se for COPY e tiver 2 operandos
            output_file << operations_table.at(opcode).opcode << ' '; // escreve o opcode de COPY

            // Para o arg1
            symbol = arg1;
            iter = find_if(symbols_table.begin(), symbols_table.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
            found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao
            index = distance(symbols_table.begin(), iter); // indice do rotulo na tabela de simbolos
            if (found_symbol){ // rotulo encontrado
                output_file << symbols_table[index].address << ' '; // escreve o endereco do arg1
            }
            else{ // rotulo nao encontrado
                // TALVEZ PODERIA BOTAR UM XX, JA QUE NAO VAI EXISTIR O ROTULO
                // O ERRO DE ROTULO NAO DEFINIDO JA FOI FEITO NA FUNCAO second_pass_labels
            }

            // Para o arg2
            symbol = arg2;
            iter = find_if(symbols_table.begin(), symbols_table.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
            found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao
            index = distance(symbols_table.begin(), iter); // indice do rotulo na tabela de simbolos
            if (found_symbol){ // rotulo encontrado
                output_file << symbols_table[index].address << ' '; // escreve o endereco do arg2
            }
            else{ // rotulo nao encontrado
                // TALVEZ PODERIA BOTAR UM XX, JA QUE NAO VAI EXISTIR O ROTULO
                // O ERRO DE ROTULO NAO DEFINIDO JA FOI FEITO NA FUNCAO second_pass_labels
            }
        }

        else if (opcode != "STOP" && opcode != "COPY" && !arg1.empty() && arg2.empty()) { // se nao for STOP e nem COPY e tiver 1 operando
            output_file << operations_table.at(opcode).opcode << ' '; // escreve o opcode da operacao

            // Para o arg1
            symbol = arg1;
            iter = find_if(symbols_table.begin(), symbols_table.end(), [&symbol](const Item_symbols_table table_item){return table_item.symbol == symbol;});
            found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao
            index = distance(symbols_table.begin(), iter); // indice do rotulo na tabela de simbolos
            if (found_symbol){ // rotulo encontrado
                output_file << symbols_table[index].address << ' '; // escreve o endereco do arg1
            }
            else{ // rotulo nao encontrado
                // TALVEZ PODERIA BOTAR UM XX, JA QUE NAO VAI EXISTIR O ROTULO
                // O ERRO DE ROTULO NAO DEFINIDO JA FOI FEITO NA FUNCAO second_pass_labels
            }
        }

        else{
            // Erro de instrucao com a quantidade de operando errada
            error_item.label = tokens[1];
            error_item.message = "Erro SINTATICO, instrucao com a quantidade de operando errada";
            error_item.line_number = *line_counter;
            errors_table.push_back(error_item);
        }
    }

    else if (opcode == "CONST" && reached_stop && !arg1.empty() && arg2.empty()) { // se o opcode for CONST e estiver depois do STOP e tiver so 1 operando
        (*position_counter)++; // atualiza o contador de posicao

        // Para o arg1
        value = stoi(arg1);
        iter = find_if(symbols_table.begin(), symbols_table.end(), [&value](const Item_symbols_table table_item){return table_item.value == value;});
        found_symbol = (iter != symbols_table.end()); // indica se o rotulo foi encontrado ou nao
        index = distance(symbols_table.begin(), iter); // indice do rotulo na tabela de simbolos
        if (found_symbol){ // rotulo encontrado
            output_file << symbols_table[index].value << ' '; // escreve o valor do arg1
        }
        else{ // valor nao encontrado, ACHO QUE NUNCA ACONTECE ESSE CASO DE NAO ENCONTRAR O VALOR
            // TALVEZ PODERIA BOTAR UM XX, JA QUE NAO VAI EXISTIR O VALOR
            // O ERRO DE ROTULO NAO DEFINIDO JA FOI FEITO NA FUNCAO second_pass_labels
            }
    }

    else if (opcode == "SPACE" && reached_stop && arg1.empty() && arg2.empty()) { // se o opcode for SPACE e estiver depois do STOP e nao tiver nenhum operando
        (*position_counter)++; // atualiza o contador de posicao
        output_file << "0" << ' '; // escreve o espaco reservado
    }

    else if (opcode == "CONST" && (arg1.empty() || !arg2.empty())){ // se o opcode for CONST e tiver quantidade de operandos errada
        // Erro de instrucao com a quantidade de operando errada
        error_item.label = tokens[1];
        error_item.message = "Erro SINTATICO, instrucao com a quantidade de operando errada";
        error_item.line_number = *line_counter;
        errors_table.push_back(error_item);
    }

    else if (opcode == "SPACE" && (!arg1.empty() || !arg2.empty())){ // se o opcode for SPACE e tiver quantidade de operandos errada
        // Erro de instrucao com a quantidade de operando errada
        error_item.label = tokens[1];
        error_item.message = "Erro SINTATICO, instrucao com a quantidade de operando errada";
        error_item.line_number = *line_counter;
        errors_table.push_back(error_item);
    }

    return errors_table;
}


int main(int argc, char const *argv[]) {
    int j = 1; //TESTE
    int index, macro_line;

    char use_mode = argv[1][1];

    bool found_symbol, if_is_valid=false, line_before_is_if=false, used_macro=false, reached_stop=false, line_has_const=false, has_2_labels=false;

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

    if (use_mode == 'p' || use_mode == 'o') {
        // Passagem EQU e IF
        position_counter = 0;
        line_counter = 1;
        while (getline(file, line)){
            //  Ignora se for uma linha em branco
            if (is_blank_line(line) || line == " ") {
                line_counter++;
                continue;
            }
            // Passa tudo para uppercase, para não ser case sensitive
            transform(line.begin(), line.end(),line.begin(), ::toupper);
            // Pre processar EQU e IF
            tie(tokens, has_2_labels) = get_tokens(line, use_mode, file, &line_counter);

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
                tie(line, symbols_table_equ_if) = write_label(tokens, operations_table, symbols_table_equ_if, errors_table_p, &position_counter, &line_counter, has_2_labels);
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
                if (is_blank_line(line) || line == " ") {
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
                tie(tokens, has_2_labels) = get_tokens(line, use_mode, file, &line_counter);
                tie(line, symbols_table_equ_if) = write_label(tokens, operations_table, symbols_table_equ_if, errors_table_p, &position_counter, &line_counter, has_2_labels);
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
            if (is_blank_line(line) || line == " ") {
                line_counter++;
                continue;
            }
            // Passa tudo para uppercase, para não ser case sensitive
            transform(line.begin(), line.end(),line.begin(), ::toupper);

            // Pre processar MACRO
            tie(tokens, has_2_labels) = get_tokens(line, use_mode, ifile_macro, &line_counter);
            line = format_line(tokens, has_2_labels);
            if (tokens[1] == "MACRO"){    // se a linha for definicao de uma MACRO
                macro_name = tokens[0];
                macro_line = line_counter;
                while(true) { // ler as linhas seguintes ate achar ENDMACRO, quando achar ENDMACRO sai do while
                    line_counter++;
                    getline(ifile_macro, line);

                    if (ifile_macro.eof()) { // chegou no final do arquivo e nao encontrou ENDMACRO
                        // Adiciona o erro para falta de ENDMACRO
                        error_item.label = macro_name;
                        error_item.message = "Erro SEMANTICO, falta de ENDMACRO";
                        error_item.line_number = line_counter;
                        errors_table_m.push_back(error_item);
                        break;
                    }

                    //  Ignora se for uma linha em branco
                    if (is_blank_line(line) || line == " ") {
                        continue;
                    }
                    // Passa tudo para uppercase, para não ser case sensitive
                    transform(line.begin(), line.end(),line.begin(), ::toupper);
                    tie(tokens, has_2_labels) = get_tokens(line, use_mode, ifile_macro, &line_counter);
                    line = format_line(tokens, has_2_labels);

                    if (line.find("ENDMACRO") != string::npos) { // se for a linha que tiver ENDMACRO
                        break; // termina o while
                    }

                    macro_lines.push_back(line); // adiciona a linha no vetor da MACRO
                }
            }

            else if (line.find("ENDMACRO") != string::npos) { // se for a linha que tiver ENDMACRO
                line_counter++;
                continue; // vai para a proxima linha
            }

            else if (tokens[1] == macro_name && !tokens[1].empty()) {   // se a linha tiver o rotulo da MACRO
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
            line_has_const = false;
            //  Ignora se for uma linha em branco
            if (is_blank_line(line) || line == " ") {
                line_counter++;
                continue;
            }
            // Passa tudo para uppercase, para não ser case sensitive
            transform(line.begin(), line.end(),line.begin(), ::toupper);
            // Pega os tokens da linha
            tie(tokens, has_2_labels) = get_tokens(line, use_mode, ifile_pre_processed_file, &line_counter);
            // cout << "linha " << line_counter << ": --";    //TESTE
            // cout << "label: '" << tokens[0] << "' --";     //TESTE
            // cout << "opcode: '" << tokens[1] << "' --";    //TESTE
            // cout << "arg1: '" << tokens[2] << "' --";      //TESTE
            // cout << "arg2: '" << tokens[3] << "' --"; //TESTE
            // cout << "size: '" << tokens.size() << "'" << endl; //TESTE
            // if (tokens.size() > 4){
            //     cout << "label extra: '" << tokens[4] << "'" << endl; //TESTE
            // }

            // Gera os erros de token invalido
            if (tokens[1] == "CONST") {
                line_has_const = true;
            }
            // Gera os erros de token invalido
            for (int k = 0; k < tokens.size(); k++) {
                if (k != 1) { // se nao for o opcode, ou seja, se for rotulo ou operando
                    if (!is_valid_variable(tokens[k], operations_table, line_has_const)) { // se o token nao for valido
                        // Erro token invalido
                        error_item.label = tokens[k];
                        error_item.message = "Erro LEXICO, token invalido";
                        error_item.line_number = line_counter;
                        errors_table_o.push_back(error_item);
                    }
                }
                else { // se for opcode
                    if (!is_valid_instruction(tokens[k], operations_table)) { // se o token nao for valido
                        // Erro token invalido
                        error_item.label = tokens[k];
                        error_item.message = "Erro LEXICO, token invalido";
                        error_item.line_number = line_counter;
                        errors_table_o.push_back(error_item);
                    }
                }
            }

            // Gera os erros de dois rotulos na mesma linha
            if (has_2_labels) {
                // Erro de dois rotulos na mesma linha
                error_item.label = tokens[4];
                error_item.message = "Erro SINTATICO, dois rotulos na mesma linha";
                error_item.line_number = line_counter;
                errors_table_o.push_back(error_item);
            }

            //  Adiciona os rotulos na tabela de simbolos e atualiza a tabela de erros caso ocorra erro de rotulo ja definido
            tie(symbols_table, errors_table_o) = first_pass_labels(tokens, operations_table, symbols_table, errors_table_o, &position_counter, &line_counter, reached_stop);

            // Atualiza o contador de posicao e a tabela de erros caso erro de operacao invalida
            errors_table_o = first_pass_instructions(tokens, operations_table, errors_table_o, &position_counter, &line_counter, reached_stop);
            
            if (tokens[1] == "STOP") { // se o opcode da linha for STOP
                reached_stop = true; // variavel para utilizar na funcao first_pass_labels, para definir o endereco dos CONST e SPACE que vierem depois do STOP
                for (auto &symbol_iter : symbols_table) {
                    if (symbol_iter.address == -2) { // se for CONST ou SPACE que veio antes do STOP
                        symbol_iter.address = position_counter; // bota um endereco depois do STOP
                        position_counter++;
                    }
                }
            }

            line_counter++;
        }

        // Segunda passagem
        position_counter = 0;
        line_counter = 1;
        reached_stop = false;
        ifile_pre_processed_file.clear();
        ifile_pre_processed_file.seekg(0);
        while (getline(ifile_pre_processed_file, line)) {
            //  Ignora se for uma linha em branco
            if (is_blank_line(line) || line == " ") {
                line_counter++;
                continue;
            }
            // Passa tudo para uppercase, para não ser case sensitive
            transform(line.begin(), line.end(),line.begin(), ::toupper);
            // Pega os tokens da linha
            tie(tokens, has_2_labels) = get_tokens(line, use_mode, ifile_pre_processed_file, &line_counter);

            // Procura os rotulos na tabela de simbolos e atualiza a tabela de erros caso ocorra erro de declaracao de rotulo ausente
            errors_table_o = second_pass_labels(tokens, operations_table, symbols_table, errors_table_o, &position_counter, &line_counter);

            // Gera o codigo objeto e atualiza a tabela de erros caso ocorra erro de quantidade de operandos invalida
            errors_table_o = second_pass_instructions(tokens, operations_table, symbols_table, errors_table_o, output_file, &position_counter, &line_counter, reached_stop);

            // Escre no codigo objeto os CONST e SPACE que vieram antes do STOP
            if (tokens[1] == "STOP") { // se o opcode da linha for STOP
                reached_stop = true; // variavel para utilizar na funcao second_pass_instructions

                // Escrevendo no codigo objeto os CONST e SPACE que vieram antes do STOP
                for (auto symbol_iter : symbols_table) {
                    if (symbol_iter.before_stop && (symbol_iter.is_const || symbol_iter.is_space)) { // se veio antes do STOP e eh CONST ou SPACE
                        if (symbol_iter.is_const) { // se for CONST
                            output_file << symbol_iter.value << ' '; // escreve o valor do CONST no codigo objeto
                        }
                        else if (symbol_iter.is_space) { // se for SPACE
                            output_file << "0" << ' '; // escreve o espaco reservado
                        }
                        position_counter++;
                    }
                }

            }          

            line_counter++;
        }
        ifile_pre_processed_file.close();
    }
    // cout << "Tabela de simbolos EQU IF:------------------------" << endl;
    // for (auto symbol_iter : symbols_table_equ_if) {
    //     cout << "Simbolo: " << symbol_iter.symbol << " --- ";
    //     cout << "Linha: " << symbol_iter.line << " --- ";
    //     cout << "Endereco: " << symbol_iter.address << endl;
    // }
    // cout << "Tabela de simbolos:------------------------" << endl;
    // for (auto symbol_iter : symbols_table) {
    //     cout << "Simbolo: " << symbol_iter.symbol << " --- ";
    //     cout << "Linha: " << symbol_iter.line << " --- ";
    //     cout << "Valor: " << symbol_iter.value << " --- ";
    //     cout << "Used: " << symbol_iter.used << " --- ";
    //     cout << "Const: " << symbol_iter.is_const << " --- ";
    //     cout << "Space: " << symbol_iter.is_space << " --- ";
    //     cout << "Before STOP: " << symbol_iter.before_stop << " --- ";
    //     cout << "Endereco: " << symbol_iter.address << endl;
    // }
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
            cout << "Label: " << error_iter.label << "\t --- ";
            cout << "Mensagem: " << error_iter.message << " --- ";
            cout << "Linha do arquivo pre-processado: " << error_iter.line_number << endl;
        }
    }

    output_file.close();

    // remove("EQU_IF.asm");
    // remove("pre_processed.asm");

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
