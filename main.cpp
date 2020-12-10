#include <iostream>         // cin & cout
#include <string>           // String type for convience
#include <fstream>          // Operation on files
#include "parser.h"
#include "printTree.h"
#include "scanner.h"
#include "globalSupport.h"
using namespace std;

int main(int argc, char *argv[]){
    // Name of file we will be working with
    string fileName;
    bool kbInput = false;

    // What do we do? Based on amount of command line arguments given
    switch(argc){
        // We will read from keyboard if no file given
        case 1:{
            kbInput = true;
            cout  << "Reading from keyboard" << endl;
            string keyboardInput;
            ofstream tempFile;

            // Open a file to store keyboard input
            fileName = "tempFile.dat";
            tempFile.open(fileName.c_str());

            // User will input lines until user enters ctrl - d
            cout << "Enter token(s) then return end of file (ctl-d) when finished: ";
            while(getline(cin, keyboardInput)){
                tempFile << keyboardInput << endl;
            }
            cout << endl << "Finished inputting tokens" << endl;

            tempFile.close();   // Close file
            break;
        }
        
        // We are given a file extentsion or no extension
        case 2:{
            cout << "Now reading from a file" << endl;

            // Get file name
            fileName = argv[1];

            // Find out if theres an extension to this file but seeing if theres a .
            size_t extFinder = fileName.find(".");

            // If theres not an extenstion give it one
            if(extFinder == string::npos){
                fileName = fileName.append(".fs");
            }
            break;
        }

        default:{
            // If too many arguments given
        cout << "ERROR too many command line arguments given" << endl;
        }
    }

        // Run parser
        Parser parser(fileName);
        Semantics semantics(fileName, kbInput);

        // Build Tree
        Node *root = parser.parse();

        // Verify semantics and translates of parse tree
        semantics.translateToASM(root);


        // Print Tree   DISABLED FOR P3
        //cout << "Printing tree now." << endl;
        //printPreorder(root);

        return 1;
}