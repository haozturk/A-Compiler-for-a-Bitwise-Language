#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>
#include "Token.h"
#include <cstring>

using namespace std;

//Global variables

vector<string> input;  //each line in the input file is stored in this vector line by line
vector<Token> tokenVec; //each token is stored in this vector
string tokenType = "";  //type of the focused token
string tokenString = "";  // string value of the focused token
ofstream outputFile;
int printCount = 0;  // to avoid duplicate labels in the assembly code
int lineCount = 0;  // to determine when the program will return to the operating system
int errCount = 0;  // to decide in which line the error occurred
bool  error = false;  //  to decide whether the "result.asm was generated" will show up or not

/*this variable specifies the current token's index, it is incremented by readToken function
*it is set to zero after each line processed
*/
int currentToken = 0;

//Function declarations
vector<string> varToken;
vector<Token> tokenize(string line,vector<Token>& v);  //tokenizes each given line and puts these tokens into a vector
/*
 * This function reads the tokens from the TokenVec vector and initializes tokenType and tokenString variables
 * according to these readed values, then increments the currentToken by 1.
 */
void readToken(string& type,string& str);

string lookAtToken();  // returns the type of the focused token
/*
 * These 5 functions parse the given token list
 * and determines the precedence of the operators
 */
void morefactors();
void factor();
void moreterms();
void term();
void expr();

void assignment(); // assigns an evaluated expression into a variable

/*
 * Firstly this function calls the tokenize function, then if there is a equal sign the token list after a variable
 * it invokes the assignment function. If there is no assignment operation in the given line then just prints
 * the values of the given expressions.
 */
void processLine(string line);




int main(int argc, char* argv[]) {

    ifstream inFile;
    inFile.open(argv[1]);

    if(!inFile.is_open()){
        exit(EXIT_FAILURE);
    }

    string line;
    while(getline(inFile,line)){
        input.push_back(line);
    }


    outputFile.open("result.asm");

    outputFile << "code segment" << endl;

    for(int i = 0; i< input.size();i++){
        lineCount++;
        processLine(input[i]);

        currentToken = 0;
        tokenVec.clear();
    }



    for(int i=0; i<varToken.size(); i++){
        if(i==0){
            outputFile << varToken[i] << " dw 0" << endl;
        }
        else {
            int count = 0;
            for (int j = 0 ;j < i; j++) {
                if (varToken[i] == varToken[j]) {
                    count++;
                }
            }
            if(count == 0){
                outputFile << varToken[i] << " dw 0" << endl;
            }

        }

    }


    outputFile << "code ends" << endl;

    outputFile.close();
    if(error == false){
    cout << "result.asm was generated." << endl;
    }

    return 0;
}
vector<Token> tokenize(string line,vector<Token>& v){

    /*
     * this piece of code omits the empty string characters
     */
    char* pot;
    pot = strtok((char *)line.c_str(), " ");
    string str1;
    while(pot!= NULL){
        str1 += pot;
        pot = strtok(NULL, " ");
    }

    /*
     * In this loop given string line is classified according to the grammer of the language.
     * Each created token is put the TokenVec vector
     */
    for(int i=0;i<str1.length();i++){
        if(str1[i] == '$'){
            int j = i+1;
            while((str1[j] >= 65 && str1[j] <= 90) || (str1[j] >= 97 && str1[j] <= 122) || (str1[j] >= 48 && str1[j] <= 57)){
                j+=1;
                if(!(str1[j] >= 65 && str1[j] <= 90) && !(str1[j] >= 97 && str1[j] <= 122) && !(str1[j] >= 48 && str1[j] <= 57)){
                    break;
                }
            }
            v.push_back(Token("var", str1.substr(i,j-i)));
            i = j-1;


        }
        else if((str1[i] >= 65 && str1[i] <= 70) || (str1[i] >= 97 && str1[i] <= 102) || (str1[i] >= 48 && str1[i] <= 57)){
            int j = i+1;
            while((str1[j] >= 65 && str1[j] <= 70) || (str1[j] >= 97 && str1[j] <= 102) || (str1[j] >= 48 && str1[j] <= 57)){
                j+=1;
                if(!(str1[j] >= 65 && str1[j] <= 70) && !(str1[j] >= 97 && str1[j] <= 102) && !(str1[j] >= 48 && str1[j] <= 57)){
                    break;
                }
            }
            v.push_back(Token("num", str1.substr(i,j-i)));
            i = j-1;

        }
        else if(str1[i]=='='){
            v.push_back(Token("eqSign","="));
        }
        else if(str1[i]=='('){
            v.push_back(Token("openP","("));
        }
        else if(str1[i]==')'){
            v.push_back(Token("closeP",")"));
        }
        else if(str1[i]==','){
            v.push_back(Token("comma", "," ));
        }
        else if(str1[i]=='|'){
            v.push_back(Token("orSign", "|" ));
        }
        else if(str1[i]=='&'){
            v.push_back(Token("andSign", "&" ));
        }
        else if(str1[i] == 'x' && str1[i+1] == 'o' && str1[i+2] == 'r' ){
            v.push_back(Token("xOr", "xor"));
        }
        else if(str1[i] == 'n' && str1[i+1] == 'o' && str1[i+2] == 't' ){
            v.push_back(Token("nOt", "not"));
        }
        else if(str1[i] == 'l' && str1[i+1] == 's'){
            v.push_back(Token("leftSh", "ls"));
        }
        else if(str1[i] == 'r' && str1[i+1] == 's'){
            v.push_back(Token("rightSh", "rs"));
        }
        else if(str1[i] == 'l' && str1[i+1] == 'r'){
            v.push_back(Token("leftRot", "lr"));
        }
        else if(str1[i] == 'r' && str1[i+1] == 'r'){
            v.push_back(Token("rightRot", "rr"));
        }
    }
    /*
     * this line adds an extra token at the end of the token vector in order to
     * avoid std::out of range errors
     */
    v.push_back(Token("end",""));
    return v;
}
void readToken(string& type,string& str){

    type = tokenVec[currentToken].tokenType;
    str = tokenVec[currentToken].tokenString;
    currentToken++;

}
string lookAtToken(){

    return tokenVec.at(currentToken).tokenType;

}
void processLine(string line){
    errCount++;
    tokenize(line,tokenVec);
    if(tokenVec[0].tokenType=="var" && tokenVec[1].tokenType=="eqSign"){
        varToken.push_back(tokenVec[0].tokenString);
        assignment();
    }
    else if(tokenVec[0].tokenType=="var" && tokenVec[1].tokenType=="end"){
        varToken.push_back(tokenVec[0].tokenString);
        string varString = tokenVec[0].tokenString;
        readToken(tokenType,tokenString);
        if(lookAtToken()=="end"){
            outputFile << "push offset "<< tokenString << endl;
            outputFile << "pop bx" << endl;
            outputFile << "mov ax,[bx]" << endl;
            outputFile << "myprint"<< printCount << ":" << endl;
            outputFile << "mov cx,0" << endl;
            outputFile << "mov si,10h" << endl;
            outputFile << "xor dx,dx" << endl;
            outputFile << "nonzero"<< printCount << ":" << endl;
            outputFile << "div si" << endl;

            outputFile << "cmp dx,10d" << endl;
            outputFile << "jb less"<< printCount << endl;
            outputFile << "jae more"<< printCount << endl;

            outputFile << "more"<< printCount << ":" << endl;
            outputFile << "add dx, 87d" << endl;
            outputFile << "jmp goon"<< printCount << endl;

            outputFile << "less"<< printCount << ":" << endl;
            outputFile << "add dx, 48d" << endl;
            outputFile << "jmp goon"<< printCount <<  endl;

            outputFile << "goon" << printCount << ":" <<endl;

            outputFile << "push dx" << endl;
            outputFile << "inc cx" << endl;
            outputFile << "xor dx,dx" << endl;
            outputFile << "cmp ax,0h" << endl;
            outputFile << "jne nonzero"<< printCount << endl;
            outputFile << "writeloop"<< printCount << ":" << endl;
            outputFile << "pop dx" << endl;
            outputFile << "mov ah,02h" << endl;
            outputFile << "int 21h" << endl;
            outputFile << "dec cx" << endl;
            outputFile << "jnz writeloop"<< printCount  << endl;

            outputFile << "mov dx, 10" << endl;
            outputFile << "mov ah, 02h" << endl;
            outputFile << "int 21h" << endl;

            if(lineCount==input.size()){

                outputFile << "mov ax, 4c00h" << endl;
                outputFile << "int 21h" << endl;

            }
            printCount++;
        }



    }


}
void assignment(){

    readToken(tokenType,tokenString);
    outputFile << "push offset " << tokenString << endl;
    readToken(tokenType,tokenString);
    expr();
    if(lookAtToken() != "end"){
        cout << "error-line-" << errCount << endl;
        error = true;
    }
    outputFile << "pop ax" << endl;
    outputFile << "pop bx" << endl;
    outputFile << "mov [bx], ax" << endl;

}
void expr(){
    term();
    moreterms();
}
void term(){
    factor();
    morefactors();
}
void moreterms(){
    if(lookAtToken()=="orSign"){
        readToken(tokenType,tokenString);
        term();
        outputFile << "pop ax" << endl;
        outputFile << "pop bx" << endl;
        outputFile << "or ax,bx" << endl;
        outputFile << "push ax" << endl;
        moreterms();
    }
}
void morefactors(){
    if(lookAtToken()=="andSign"){
        readToken(tokenType,tokenString);
        factor();
        outputFile << "pop ax" << endl;
        outputFile << "pop bx" << endl;
        outputFile << "and ax,bx" << endl;
        outputFile << "push ax" << endl;
    }
}
void factor(){
    if(lookAtToken()=="openP"){
        readToken(tokenType,tokenString);
        expr();
        if(lookAtToken()=="closeP"){
            readToken(tokenType,tokenString);
        }
        else{
            outputFile << "syntax error" << endl;
        }
    }
    else if(lookAtToken()=="xOr"){
        readToken(tokenType,tokenString);
        if(lookAtToken()=="openP"){
            readToken(tokenType,tokenString);
            expr();
            if(lookAtToken()=="comma"){
                readToken(tokenType,tokenString);
                expr();
                if(lookAtToken()=="closeP"){
                    readToken(tokenType,tokenString);
                    outputFile << "pop ax" << endl;
                    outputFile << "pop bx" << endl;
                    outputFile << "xor ax,bx" << endl;
                    outputFile << "push ax" << endl;
                }
                else{
                    outputFile << "syntax error" << endl;
                }
            }
            else{
                outputFile << "syntax error" << endl;
            }
        }
        else{
            outputFile << "syntax error" << endl;
        }
    }
        else if(lookAtToken()=="leftSh"){
        readToken(tokenType,tokenString);
        if(lookAtToken()=="openP"){
            readToken(tokenType,tokenString);
            expr();
            if(lookAtToken()=="comma"){
                readToken(tokenType,tokenString);
                expr();
                if(lookAtToken()=="closeP"){
                    readToken(tokenType,tokenString);
                    outputFile << "pop cx" << endl;
                    outputFile << "pop bx" << endl;
                    outputFile << "shl bx,cl" << endl;
                    outputFile << "push bx" << endl;
                }
                else{
                    outputFile << "syntax error" << endl;
                }
            }
            else{
                outputFile << "syntax error" << endl;
            }
        }
        else{
            outputFile << "syntax error" << endl;
        }
    }
    else if(lookAtToken()=="rightSh"){
        readToken(tokenType,tokenString);
        if(lookAtToken()=="openP"){
            readToken(tokenType,tokenString);
            expr();
            if(lookAtToken()=="comma"){
                readToken(tokenType,tokenString);
                expr();
                if(lookAtToken()=="closeP"){
                    readToken(tokenType,tokenString);
                    outputFile << "pop cx" << endl;
                    outputFile << "pop bx" << endl;
                    outputFile << "shr bx,cl" << endl;
                    outputFile << "push bx" << endl;
                }
                else{
                    outputFile << "syntax error" << endl;
                }
            }
            else{
                outputFile << "syntax error" << endl;
            }
        }
        else{
            outputFile << "syntax error" << endl;
        }
    }
    else if(lookAtToken()=="leftRot"){
        readToken(tokenType,tokenString);
        if(lookAtToken()=="openP"){
            readToken(tokenType,tokenString);
            expr();
            if(lookAtToken()=="comma"){
                readToken(tokenType,tokenString);
                expr();
                if(lookAtToken()=="closeP"){
                    readToken(tokenType,tokenString);
                    outputFile << "pop cx" << endl;
                    outputFile << "pop bx" << endl;
                    outputFile << "rol bx,cl" << endl;
                    outputFile << "push bx" << endl;
                }
                else{
                    outputFile << "syntax error" << endl;
                }
            }
            else{
                outputFile << "syntax error" << endl;
            }
        }
        else{
            outputFile << "syntax error" << endl;
        }
    }
    else if(lookAtToken()=="rightRot"){
        readToken(tokenType,tokenString);
        if(lookAtToken()=="openP"){
            readToken(tokenType,tokenString);
            expr();
            if(lookAtToken()=="comma"){
                readToken(tokenType,tokenString);
                expr();
                if(lookAtToken()=="closeP"){
                    readToken(tokenType,tokenString);
                    outputFile << "pop cx" << endl;
                    outputFile << "pop bx" << endl;
                    outputFile << "ror bx,cl" << endl;
                    outputFile << "push bx" << endl;
                }
                else{
                    outputFile << "syntax error" << endl;
                }
            }
            else{
                outputFile << "syntax error" << endl;
            }
        }
        else{
            outputFile << "syntax error" << endl;
        }
    }
    else if(lookAtToken()=="nOt"){
        readToken(tokenType,tokenString);
        if(lookAtToken()=="openP"){
            readToken(tokenType,tokenString);
            expr();
            if(lookAtToken()=="closeP"){
                readToken(tokenType,tokenString);
                outputFile << "pop ax" << endl;
                outputFile << "not ax" << endl;
                outputFile << "push ax" << endl;
            }
            else{
                outputFile << "syntax error" << endl;
            }
        }
        else{
            outputFile << "syntax error" << endl;
        }
    }
    else if(lookAtToken()=="var"){
        readToken(tokenType,tokenString);
        varToken.push_back(tokenString);
        outputFile << "push offset " <<  tokenString << endl;
        outputFile << "pop bx" << endl;
        outputFile << "mov ax,[bx]" << endl;
        outputFile << "push ax" << endl;
    }
    else if(lookAtToken()=="num"){
        readToken(tokenType,tokenString);
        outputFile << "push " << "0" << tokenString <<"h" <<endl;
    }
    else{

    }


}



