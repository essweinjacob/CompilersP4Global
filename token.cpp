#include "token.h"


// This function creates pairs for Operator Map
void Language::initOpMap(){
    opMap.insert(make_pair("=>", "greaterThanEqualTK"));
    opMap.insert(make_pair("=<", "lessThanEqualTK"));
    opMap.insert(make_pair("==", "equalEqualTK"));
    opMap.insert(make_pair("=", "equalTK"));
    opMap.insert(make_pair("<", "lessThanTK"));
    opMap.insert(make_pair(">", "moreThanTK"));
    opMap.insert(make_pair(":", "colonTK"));
    opMap.insert(make_pair("+", "plusTK"));
    opMap.insert(make_pair("-", "minusTK"));
    opMap.insert(make_pair("*", "multiplyTK"));
    opMap.insert(make_pair("/", "divideTK"));
    opMap.insert(make_pair("%", "modTK"));
    opMap.insert(make_pair(".", "periodTK"));
    opMap.insert(make_pair(",", "commaTK"));
    opMap.insert(make_pair(";", "semiColonTK"));
    opMap.insert(make_pair("(", "leftParTK"));
    opMap.insert(make_pair(")", "rightParTK"));
    opMap.insert(make_pair("{", "leftCurlTK"));
    opMap.insert(make_pair("}", "rightCurlTK"));
    opMap.insert(make_pair("[", "leftBracketTK"));
    opMap.insert(make_pair("]", "rightBracketTK"));
}

// This function creates pairs for the keyword map
void Language::initKeywordMap(){
    keywordMap.insert(make_pair("start", "startTK"));
    keywordMap.insert(make_pair("stop", "stopTK"));
    keywordMap.insert(make_pair("iter", "iterTK"));
    keywordMap.insert(make_pair("void", "voidTK"));
    keywordMap.insert(make_pair("int", "intTK"));
    keywordMap.insert(make_pair("exit", "exitTK"));
    keywordMap.insert(make_pair("scanf", "scanfTK"));
    keywordMap.insert(make_pair("printf", "printfTK"));
    keywordMap.insert(make_pair("main", "mainTK"));
    keywordMap.insert(make_pair("if", "ifTK"));
    keywordMap.insert(make_pair("then", "thenTK"));
    keywordMap.insert(make_pair("let", "letTK"));
    keywordMap.insert(make_pair("data", "dataTK"));
    keywordMap.insert(make_pair("func", "funcTK"));
}

// This function prints a token
void Language::printTK(Token tk){
    cout << "Line #" << tk.lineNum << ": " << tkNames[tk.id] << ": " << tk.val << endl;
}

// This function searches for an operator
int Language::isOp(char ch){
    // Cycle through the operators and see if given char is one
    for(unsigned int i = 0; i < sOperators.size(); i++){
        // If is one, return 1
        if(ch == sOperators[i]){
            return 1;
        }
    }
    
    // If no operator is found, return -1
    return -1;
}

// This function searches for an operator that is more then one character
int Language::isNsOp(string str){
    // Cycle through non single char operators and compare them to current string
    for(unsigned int i = 0; i < nsOperators.size(); i++){
        // If strings match, return
        if(str.compare(nsOperators[i]) == 0){
            return 1;
        }
    }

    // If no match is found, return -1
    return -1;
}

// This function searches for any operators
int Language::getOp(Token &tk){
    
    // Search through the single operators
    for(unsigned int i = 0; i < sOperators.size(); i++){
        // Get that operator from the vector
        string opToCompare(1, sOperators[i]);
        // Compare the value gotten to current i operator, if found replace token value with equivalent value found in operator map and return its position
        if(tk.val.compare(opToCompare) == 0){
            tk.val = opMap[tk.val];
            return i;
        }
    }

    // Search through the non single operators
    for(unsigned int i = 0; i < nsOperators.size(); i++){
        // Compare the value gotten to current i operator, if found replace token value with equivalent value found in operator map and return its position
        if(tk.val.compare(nsOperators[i]) == 0){
            tk.val = opMap[tk.val];
            return i;
        }
    }

    // If no matches found return -1 for error
    return -1;
}

// This function searches for keywords
int Language::getKeyword(Token &tk){
    // Search through keywords
    for(unsigned int i = 0; i < keywords.size(); i++){
        // Compare if token value matches a specific keyword
        if(tk.val.compare(keywords[i]) == 0){
            // If it does change token value to keyword in map and return its position
            tk.val = keywordMap[tk.val];
            return i;
        }
    }

    // If no keyword is got, return -1 for error
    return -1;
}