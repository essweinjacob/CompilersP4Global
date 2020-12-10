#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>     // cin & cout
#include <string>       // string type, very useful
#include <map>          // Pretty much dictonaries from Python
#include <vector>       // Arrays but can change in size, very cool
using namespace std;

// Enumorator for tokenID
enum tokenID {idTK, keywordTK, intTK, opTK, EOFTK};

// Token type
typedef struct Token{
    tokenID id;         // Id of token type
    int lineNum;        // Line number the token was found on
    string val;    // Value of token
    string data;
}Token;


class Language{
    protected:
        // Operator and Keyword map
        map<string, string>opMap;
        map<string, string>keywordMap;

        // Types of tokens
        const vector<string>tkNames ={"Identifier", "Keywords","Integer","Operator","End of File"};

        // Reservered words
        const vector<string>keywords = {"start", "stop","iter","void","int","exit","scanf","printf","main","if","then","let","data","func"};

        // Delimiter for comments
        const char COM_DELIM = '#';

        // Operators that can be found (single)
        const vector<char>sOperators = {'=', '<', '>', ':', '+', '-', '*', '/', '%', '.', ',', ';', '(', ')', '{', '}', '[', ']'};
        const vector<string>nsOperators = {"=>", "=<", "=="};

        void initOpMap();
        void initKeywordMap();
        int isOp(char ch);
        int isNsOp(string str);
        int getOp(Token &tk);
        int getKeyword(Token &tk);
    
    public:
        void printTK(Token tk);
    
};

#endif