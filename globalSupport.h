#ifndef GLOBALSUPPORT_H
#define GLOBALSUPPORT_H
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "node.h"
#include "token.h"
using namespace std;

// Token object for each indiviual token
typedef struct tokenTable{
    Token id;       // Token name i.e x
    Token value;    // Token value i.e 1
}TokenTable;

// Global Semantics extends Language since we use initOpMap() andinitKeyMap() functions
class Semantics: public Language{
    private:
        TokenTable holder;      // Holds values for token before being put on stack
        vector<TokenTable>st;   // Stack for values

        ofstream file;          // File for file stream
        string asmFileName;     // assembly file name

        unsigned int tempVarsCount = 0;     // How many variables while generating code
        unsigned int carryNamesNum = 0;     // Carrying over number of variables
        unsigned int tempNamesNum = 0;      // How many numbers we dealing with boss?

        void checkDec(Token tk, Token value);
        void checkDef(Token tk);
        void toASM(Node *node);
    
    public:
        void verify(Node *node);
        Semantics(string fileName, bool kbInput);
        void translateToASM(Node *node);
};

#endif