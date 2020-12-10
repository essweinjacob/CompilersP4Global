#ifndef SCANNER_H
#define SCANNER_H
#include <iostream>     // cin & cout
#include <string>       // string > char arrays
#include <cstring>      // cool c string things
#include <cctype>       // isalpha/isdigit/etc
#include <cstdlib>      
#include <sstream>      // String stream classes
#include <fstream>      // Files
#include "token.h"
#include<stdio.h>
using namespace std;

class Scanner: public Language{
    private:
        unsigned int currentScannerPtr = 0; //Keeps track of scanner finished last input, initially set to 0
        bool isComment = false;             // Is the scanner currently in a comment?
        bool commentLine = false;
        string lastComPos = "";             // Log when the last time we were in a comment
        int lineNum = 1;

        enum{
            ERROR_UNK = -2,         // Error Unkown State     
            ERROR_INT = -1,         // Error Integer State
            INIT_STATE = 0,         // Initial State
            OP_STATE = 1,           // Operator State
            ID_STATE = 2,           // Identifier State
            INT_STATE = 3,          // Integeter State
            FINAL_STATE = 1000,     // Final State
            STATE_ID_FIN = 1001,    // Indetifier Final State
            STATE_INT_FIN = 1003,   // Int final state
            STATE_OP_FIN = 1004,    // Op Final State
            STATE_EOF_FIN = 1005    // End of File Final State
        };

        // FSA Table
        const int TABLE_FSA[4][6] = {
            {ID_STATE, INT_STATE, INIT_STATE, STATE_EOF_FIN, OP_STATE, ERROR_UNK}, 
            {STATE_OP_FIN, STATE_OP_FIN, STATE_OP_FIN, STATE_OP_FIN, STATE_OP_FIN, STATE_OP_FIN},
            {ID_STATE, ID_STATE, STATE_ID_FIN, STATE_ID_FIN, STATE_ID_FIN, ERROR_UNK}, 
            {ERROR_INT, INT_STATE, STATE_INT_FIN, STATE_INT_FIN, STATE_INT_FIN, ERROR_UNK}
        };

        // Delimiter for scanner
        const char SCAN_DELIM = ' ';

        int getCat(char ch);
        void getError(int state, char ch);
        char checkCom(char ch);
    
    public:
        Scanner();
        void preScan(string fileName, string outFileName);
        int scan(string &input, Token &tk);
        string getScanPos();
        void isComMode();
        void invokeEOF(Token &tk);
};

#endif