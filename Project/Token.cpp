//
// Created by tony on 28.10.2017.
//

#include "Token.h"
#include <string>

using namespace std;

Token::Token(string tokentype, string tokenString) {
    this->tokenType = tokentype;
    this->tokenString = tokenString;
}
