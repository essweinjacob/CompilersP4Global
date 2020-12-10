#include <iostream>
#include <string>
#include <fstream>
#include "parser.h"
#include "token.h"
#include "scanner.h"
using namespace std;

// This function fills the Parser object with information with the given parameter file
Parser::Parser(string fileName){
    string outFileName = "scannerTemp.dat";     // Output file

    scanner.preScan(fileName, outFileName);     // Pre Scan the file, removing all comments and formatting

    // See if file can be opened
    file.open(outFileName.c_str());
    if(!file.is_open()){
        cout << "[ERROR] " << outFileName << " cannot be opened!" << endl;
        exit(EXIT_FAILURE);
    }

    EmpTK.val = "EMPTY";
    scannerStatus = 0;
    getline(file, inputInfo);
}

// The function parsed through the now filtered file, and returns the root node
Node *Parser::parse(){
    Node *root = NULL;
    nextScan();     // Get next token

    root = program();   // Launch program

    // If not end of file token found send error
    if(tk.id != EOFTK){
        predictTK.assign("eofTK");      // Assign what our predicted token was
        parserError();
        exit(EXIT_FAILURE);
    }else{
        // Otherwise complete
        cout << "[SYSTEM] Parsing complete." << endl;
        file.close();
        // Return root node
        return root;
    }
}

// The function creates new nodes in our tree
Node *Parser::newNode(string prodoName){
    Node *node = new Node;
    node->name = prodoName;
    node->n1 = node->n2 = node->n3 = node->n4 = NULL;
    return node;
}

// This function gets the next token in the file
void Parser::nextScan(){
    // While no errors and still in file
    do{
        if(scannerStatus == -1){
            // Get next bit of information
            if(!getline(file, inputInfo)){
                //Once we've reached the next ws end
                if(file.eof()){
                    scanner.isComMode();
                    scanner.invokeEOF(tk);
                    
                    return;
                }
            }
        }

        scannerStatus = scanner.scan(inputInfo, tk);    // Update status
    }while(scannerStatus == -1);
}

// This function will be called when the parser has found an error, will give descriptive error message and end program
void Parser::parserError(){
    cout << "ERROR parsing at " << scanner.getScanPos() << " expected: " << predictTK << " but got " << tk.val << " inputed instead." << endl;
    file.close();
    exit(EXIT_FAILURE);
}

// This is the Program BNF
Node *Parser::program(){

    // Create program node
    Node *node = newNode("<program>");
    //cout << "<program>" << endl;

    // If file has a start token
    if((tk.id == keywordTK) && (keywordMap[tk.val] == "startTK")){
        nextScan(); // Get next token
        node->n1 = vars();  // Launch vars

        // If token is keyword main
        if((tk.id == keywordTK) && (keywordMap[tk.val] == "mainTK")){
            nextScan(); // Get next token
            node->n2 = block();     // Launch block

            // If token is stop token
            if((tk.id == keywordTK) && (keywordMap[tk.val] == "stopTK")){
                nextScan();    // Get next token
                
                return node;   // Return node
            }else{
                // If no stop token send error
                predictTK.assign("stopTK");
                parserError();
            }
        }else{
            // If no main token, send error
            predictTK.assign("mainTK");
            parserError();
        }
    }else{
        // If no start token, send error
        predictTK.assign("startTK");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// This the vars BNF function I really dont want to have to comment this out as its redundent, its the same shit as above basically
Node *Parser::vars(){
    Node *node = newNode("<vars>");
    //cout << "<vars>" << endl;

    if((tk.id == keywordTK) && (keywordMap[tk.val] == "letTK")){
        nextScan();

        if(tk.id == idTK){
            nextScan();
            node->tokens.push_back(tk);     // Push this token into our thing
            nextScan();

            if((tk.id == opTK) && (opMap[tk.val] == "colonTK")){
                nextScan();

                if(tk.id == intTK){
                    node->tokens.push_back(tk);
                    nextScan();

                    node->n1 = vars();
                    return node;
                }else{
                    predictTK.assign("Integer");
                    parserError();
                }
            }else{
                predictTK.assign(":");
                parserError();
            }
        }else{
            predictTK.assign("Identifer");
            parserError();
        }
    }else{
        node->tokens.push_back(EmpTK);
        return node;
    }

    exit(EXIT_FAILURE);
}

// Block BNF Function
Node *Parser::block(){
    Node *node = newNode("<block>");
    //cout << "<block>" << endl;
    if((tk.id == opTK) && (opMap[tk.val] == "leftCurlTK")){
        nextScan();

        node->n1 = vars();
        node->n2 = stats();

        if((tk.id == opTK) && (opMap[tk.val] == "rightCurlTK")){
            nextScan();
            return node;
        }else{
            predictTK.assign("}");
            parserError();
        }
    }else{
        predictTK.assign("{");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// expr BNF function
Node *Parser::expr(){
    Node *node = newNode("<expr>");
    //cout << "<expr>" << endl;

    node->n1 = N();

    if(tk.id == opTK){
        if(opMap[tk.val] == "divideTK"){
            node->tokens.push_back(tk);
            nextScan();
            node->n2 = expr();
        }else if(opMap[tk.val] == "multiplyTK"){
            node->tokens.push_back(tk);
            nextScan();

            node->n2 = expr();
        }
    }

    return node;
}

// N BNF function
Node *Parser::N(){
    Node *node = newNode("<N>");
    //cout << "<N>" << endl;

    node->n1 = A();

    if(tk.id == opTK){
        if(opMap[tk.val] == "plusTK"){
            node->tokens.push_back(tk);
            nextScan();

            node->n2 = N();
        }else if(opMap[tk.val] == "minusTK"){
            node->tokens.push_back(tk);
            nextScan();

            node->n2 = N();
        }
    }

    return node;
}

// A BNF function
Node *Parser::A(){
    Node *node = newNode("<A>");
    //cout << "<A>" << endl;
    if((tk.id == opTK) && (opMap[tk.val] == "modTK")){
        node->tokens.push_back(tk);;
        nextScan();

        node->n1 = A();
        return node;
    }

    node->n1 = R();
    return node;
}

// R BNF function
Node *Parser::R(){
    Node *node = newNode("<R>");
    //cout << "<R>" << endl;

    if((tk.id == opTK) && (opMap[tk.val] == "leftBracketTK")){
        nextScan();
        node->n1 = expr();
        if((tk.id == opTK) && (opMap[tk.val] == "rightBracketTK")){
            nextScan();
            return node;
        }else{
            predictTK.assign("]");
            parserError();
        }
    }else if(tk.id == idTK){
        nextScan();
        node->tokens.push_back(tk);
        nextScan();
        return node;
    }else if(tk.id == intTK){
        //cout << tk.val << endl;
        node->tokens.push_back(tk);
        nextScan();
        //cout << tk.val << endl;
        return node;
    }else{
        predictTK.assign("[ or Identifier or Integer");
        parserError();
    }
    exit(EXIT_FAILURE);
}

// stats BNF Function
Node *Parser::stats(){
    Node *node = newNode("<stats>");
    //cout << "<stats>" << endl;

    node->n1 = stat();
    node->n2 = mStat();

    return node;
}

// mStat BNF function
Node *Parser::mStat(){
    Node *node = newNode("<mStat>");
    //cout << "<mStats>" << endl;

    if(((tk.id == keywordTK) && ((keywordMap[tk.val] == "scanfTK") || (keywordMap[tk.val] == "printfTK") || (keywordMap[tk.val] == "ifTK") || (keywordMap[tk.val] == "iterTK")))
        || ((tk.id == opTK) && (opMap[tk.val] == "leftCurlTK")) 
        || tk.id == idTK){
            node->n1 = stat();
            node->n2 = mStat();

            return node;
    }else{
        node->tokens.push_back(EmpTK);
        return node;
    }

}

// stat BNF function
Node *Parser::stat(){
    Node *node = newNode("<stat>");
    //cout << "<stat>" << endl;

    if(tk.id == keywordTK){
        if(keywordMap[tk.val] == "scanfTK"){

            node->n1 = in();

            if((tk.id == opTK) && (opMap[tk.val] == "periodTK")){
                nextScan();
                return node;
            }else{
                predictTK.assign(".");
                parserError();
            }
        }else if(keywordMap[tk.val] == "printfTK"){
            //nextScan();

            node->n1 = out();

            if((tk.id == opTK) && (opMap[tk.val] == "periodTK")){
                nextScan();
                return node;
            }else{
                predictTK.assign(".");
                parserError();
            }
        }else if(keywordMap[tk.val] == "ifTK"){

            node->n1 = iff();
            
            if((tk.id == opTK) && (opMap[tk.val] == "periodTK")){
                nextScan();
                return node;
            }else{
                predictTK.assign(".");
                parserError();
            }
        }else if(keywordMap[tk.val] == "iterTK"){

            node->n1 = loop();
            
            if((tk.id == opTK) && (opMap[tk.val] == "periodTK")){
                nextScan();
                return node;
            }else{
                predictTK.assign(".");
                parserError();
            }
        }else{
            predictTK.assign("printf or scanf or if or iter or {");
            parserError();
        }
    }else if(tk.id == opTK){
        if(opMap[tk.val] == "leftCurlTK"){
            node->n1 = block();

            return node;
        }else{
            predictTK.assign("printf or scanf or if or iter or {");
            parserError();
        }
    }else if(tk.id == idTK){
        node->n1 = assign();

        return node;
    }else{
        nextScan();
        predictTK.assign("printf or scanf or if or iter or { or identifier");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// in BNF function
Node *Parser::in(){
    Node *node = newNode("<in>");
    //cout << "<in>" << endl;

    if((tk.id == keywordTK) && (keywordMap[tk.val] == "scanfTK")){
        nextScan();
        if((tk.id == opTK) && (opMap[tk.val] == "leftBracketTK")){
            nextScan();
            if(tk.id == idTK){
                nextScan();
                node->tokens.push_back(tk);
                nextScan();

                if((tk.id == opTK) && (opMap[tk.val] == "rightBracketTK")){
                    nextScan();
                    return node;
                }else{
                    predictTK.assign("]");
                    parserError();
                }
            }else{
                nextScan();
                predictTK.assign("Identifier");
                parserError();
            }
        }else{
            nextScan();
            predictTK.assign("[");
            parserError();
        }
    }else{
        nextScan();
        predictTK.assign("scanf");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// out BNF function
Node *Parser::out(){
    Node *node = newNode("<out>");
    //cout << "<out>" << endl;

    if((tk.id == keywordTK) && (keywordMap[tk.val] == "printfTK")){
        nextScan();
        if((tk.id == opTK) && (opMap[tk.val] == "leftBracketTK")){
            nextScan();
            node->n1 = expr();

            if((tk.id == opTK) && (opMap[tk.val] == "rightBracketTK")){
                nextScan();
                return node;
            }else{
                predictTK.assign("]");
                 parserError();
            }
        }else{
            predictTK.assign("[");
            parserError();
        }
    }else{
        predictTK.assign("scanf");
        parserError();
    }
    
    exit(EXIT_FAILURE);
}

// if BNF function
Node *Parser::iff(){
    Node *node = newNode("<if>");
    //cout << "<if>" << endl;

    if((tk.id == keywordTK) && (keywordMap[tk.val] == "ifTK")){
        nextScan();

        if((tk.id == opTK) && (opMap[tk.val] == "leftBracketTK")){
            nextScan();

            node->n1 = expr();
            node->n2 = RO();
            node->n3 = expr();

            if((tk.id == opTK) && (opMap[tk.val] == "rightBracketTK")){
                nextScan();

                if((tk.id == keywordTK) && (keywordMap[tk.val] == "thenTK")){
                    nextScan();

                    node->n4 = block();

                     return node;
                }else{
                    predictTK.assign("then");
                    parserError();
                }
            }else{
                predictTK.assign("]");
                parserError();
            }
        }else{
            predictTK.assign("[");
            parserError();
        }
    }else{
        predictTK.assign("if");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// loop BNF function
Node *Parser::loop(){
    Node *node = newNode("<loop>");
    //cout << "<loop>" << endl;

    if((tk.id == keywordTK) && (keywordMap[tk.val] == "iterTK")){
        nextScan();

        if((tk.id == opTK) && (opMap[tk.val] == "leftBracketTK")){
            nextScan();

            node->n1 = expr();
            node->n2 = RO();
            node->n3 = expr();

            if((tk.id == opTK) && (opMap[tk.val] == "rightBracketTK")){
                nextScan();
                node->n4 = block();
                return node;
            }else{
                predictTK.assign("]");
                parserError();
            }
        }else{
            predictTK.assign("[");
            parserError();
        }
    }else{
        predictTK.assign("iter");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// assign BNF function
Node *Parser::assign(){
    Node *node = newNode("<assign>");
    //cout << "<assign>" << endl;

    if(tk.id == idTK){
        nextScan();
        node->tokens.push_back(tk);
        nextScan();

        if((tk.id == opTK) && (opMap[tk.val] == "equalTK")){
            nextScan();

            node->n1 = expr();

            if((tk.id == opTK) && (opMap[tk.val] == "periodTK")){
                nextScan();
                return node;
            }else{
                predictTK.assign(".");
                parserError();
            }
            return node;
        }else{
            predictTK.assign("=");
            parserError();
        }
    }else{
        predictTK.assign("Identifier");
        parserError();
    }

    exit(EXIT_FAILURE);
}

// RO BNF function
Node *Parser::RO(){
    Node *node = newNode("<RO>");
    //cout << "<R0>" << endl;

    if(tk.id == opTK){
        if(opMap[tk.val] == "lessThanEqualTK"){
            node->tokens.push_back(tk);
            nextScan();
            
            return node;
        }else if(opMap[tk.val] == "greaterThanEqualTK"){
            node->tokens.push_back(tk);
            nextScan();

            return node;
        }else if(opMap[tk.val] == "equalEqualTK"){
            node->tokens.push_back(tk);
            nextScan();

            return node;
        }else if(opMap[tk.val] == "colonTK"){
            node->tokens.push_back(tk);
            nextScan();

            if(opMap[tk.val] == "colonTK"){
                node->tokens.push_back(tk);
                nextScan();

                return node;
            }else{
                predictTK.assign(":");
                parserError();
            }
        }else{
            predictTK.assign("=< or => or == or : :");
            parserError();
        }
    }else{
        predictTK.assign("=< or => or == or : :");
        parserError();
    }

    exit(EXIT_FAILURE);
}