#include "scanner.h"

// Initializes Operator map and keyword map
Scanner::Scanner(){
    initOpMap();
    initKeywordMap();
}

// This function 
int Scanner::getCat(char ch){
    // Determine what the what catergory the given character is

    // is it an alphabetical character?
    if(isalpha(ch)){
        return 0;
    }
    // Is it a digit?
    else if(isdigit(ch)){
        return 1;
    }
    // Is it white space?
    else if(isspace(ch)){
        return 2;
    }
    // Is it an operator?
    else if(isOp(ch) != -1){
        return 4;
    }
    // Well idk what it is you fool, throw error
    else{
        return 5;
    }
}

// This function returns error messages
void Scanner::getError(int state, char ch){
    cout << "ERROR found at line #"  << lineNum << ":" << currentScannerPtr << " [" << ch << "]: ";
    // Is int error?
    if(state == ERROR_INT){
        cout << "integer tokens can only be integers/digits" << endl;
        cout << "ERROR code: " << ERROR_INT << endl;
    }
    // it unknown error?
    else if(state == ERROR_UNK){
        cout << "spooky unknown tokens are illegal here sir. License and ID please" << endl;
        cout << "ERROR code: " << ERROR_UNK << endl;
    }
}

// This function checks for comment delimiter start and end so that 'normal' operation can start and stop
char Scanner::checkCom(char ch){
    // If the given character is a comment delimiter
    if(ch == COM_DELIM){
        isComment = !isComment;

        if(isComment){
            ostringstream temp;
            temp << lineNum << ":" << currentScannerPtr;
            lastComPos = temp.str();
        }
        commentLine = true;

        return SCAN_DELIM;
    }

    if(isComment){
        return SCAN_DELIM;
    }

    else{
        return ch;
    }
}    

// This function will throw a warning message at the user if a comment tag isnt ever closed
void Scanner::isComMode(){
    if(isComment){
        cout << "WARNING at " << lastComPos << ". Comment tag isnt ever closed" << endl;
    }
}

// This function gets the current position of the scanner
string Scanner::getScanPos(){
    ostringstream temp;
    temp << "(" << lineNum << ":" << currentScannerPtr << ")";
    return temp.str();
}

// This function invokes the EOF value and gives it to token
void Scanner::invokeEOF(Token &tk){
    tk.lineNum = (lineNum > 1) ? lineNum - 1 : lineNum;
    tk.id = EOFTK;
    tk.val = "EOF";
}

// This function pre scans the file given and removes comments and other formatting and saves it into a new file
void Scanner::preScan(string fileName, string outFileName){
    //cout << fileName << outFileName << endl;
    ifstream inFile(fileName.c_str());
    ofstream outFile(outFileName.c_str());
    Token token;

    if(inFile.is_open()){
        string input;
        while(getline(inFile, input)){
            string scanInput;
            while(scan(input, token) == 0){
                scanInput.append(token.data);
                //cout << scanInput << endl;
                
                if(currentScannerPtr < input.length()){
                    scanInput.append(" ");
                }
            }

            outFile << scanInput << endl;
        }
    }else{
        cout << "[ERROR] Cannot open file" << endl;
        exit(EXIT_FAILURE);
    }

    inFile.close();
    outFile.close();
    currentScannerPtr = 0;
    lineNum = 1;
}

// This is the 'main' working function of the scanner
int Scanner::scan(string &input, Token &tk){
    isComment = false;
    commentLine = false;
    tk.lineNum = lineNum;   // Set current line number to current token line number

    // Initialize variables that need to be tracked during scan
    int curState = 0;       // Current FSA State
    int nextState = 0;      // Current Row of FSA Table
    int nextCat = 0;        // Current column of FSA Table
    string readVal;         // Current reading value of token
    char nextChar;          // What is current token from input?

    // While scanner pointer is less than or equal to string its being compared to
    while(currentScannerPtr <= input.length()){
        // While less then length of current input feed next character
        if(currentScannerPtr < input.length()){
            nextChar = checkCom(input.at(currentScannerPtr));
            if(commentLine){
                break;
            }
        }
        // Once its at last position it means its time for scanner delimiter
        else{
            nextChar = SCAN_DELIM;
        }

        // Look at the FSA Table and reassign next state value of row
        nextCat = getCat(nextChar);
        nextState = TABLE_FSA[curState][nextCat];

        // Make sure no errors have been thrown (if error has been thrown then nextState will be negative)
        if(nextState < 0){
            // If theres an error, get the error message
            getError(nextState, nextChar);
            exit(EXIT_FAILURE);
        }
        // Check if we are at final state
        else if(nextState > FINAL_STATE){
            tk.val = readVal;

            // Determine what final state we are in
            switch(nextState){
                // Identifier
                case STATE_ID_FIN:
                    // See if this is a keyword
                    if(getKeyword(tk) != -1){
                        tk.id = keywordTK;
                        tk.val.assign(readVal);
                        tk.data.assign(readVal);
                    }
                    // If not
                    else{
                        tk.id = idTK;
                        tk.val.assign(readVal);
                        tk.data.assign("idTK " + readVal);
                    }
                    break;
                
                // Integer
                case STATE_INT_FIN:
                    tk.id = intTK;
                    tk.val.assign(readVal);
                    tk.data.assign(readVal);
                    break;
                
                // Operator
                case STATE_OP_FIN:
                    tk.id = opTK;

                    // See if is a nonsingle operator
                    if(currentScannerPtr < input.length()){
                        string nsOperator = readVal + input.at(currentScannerPtr);
                        if(isNsOp(nsOperator) != -1){
                            readVal = nsOperator;
                            currentScannerPtr++;
                        }
                    }

                    tk.val.assign(readVal);
                    tk.data.assign(readVal);
                    getOp(tk);
                    tk.val.assign(readVal);
                    tk.data.assign(readVal);
                    break;
            }

            // If we are in a comment then we want to increment scanner ptr
            if(isComment){
                currentScannerPtr++;
            }
            return 0;
        }

        // Update the scanner and state information
        currentScannerPtr++;
        curState = nextState;

        // If theres no whitespace, change the read value of the tk
        if(!isspace(nextChar)){
            readVal.push_back(nextChar);
        }
    }

    // Once the scanner has reached the end, kill the scanner
    currentScannerPtr = 0;
    lineNum++;
    return -1;
}