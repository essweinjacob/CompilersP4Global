#include "globalSupport.h"

// This function initializes operator and keyword maps, which will be used for comparison
Semantics::Semantics(string fileName, bool kbInput){
    // Initialize Operatior Map
    initOpMap();
    // Initialize Keyword map
    initKeywordMap();
    // Add asm file extension

    // If keyboard was used
    if(kbInput){
        fileName = "kb";
    }else{
        // Otherwise file intake
        size_t extIndex = fileName.find(".");
        fileName = fileName.substr(0, extIndex);
    }
    // Assign Assembly file name
    asmFileName.assign(fileName + ".asm");
}

// This function checks if a variable has been delcared yet and stores values for the variable if it hasnt
void Semantics::checkDec(Token tk, Token value){
    int isDec = -1;     // Initial set to -1 as a "false" indicator
    
    // Search through stack to see if variable has been defined
    for(unsigned int i = 0; i < st.size(); i++){
        // If found matching variable set isDec to the index value
        if(st[i].id.val == tk.val){
            isDec = i;
            break;
        }
    }

    // If variable already declared send variable
    if(isDec > -1){
        cout << "SEMANTICS ERROR: Varable: " << tk.val << " on line " << tk.lineNum << " was already declared on line " << st[isDec].id.lineNum << endl;
    }else{
        // Store values in holder and push holder onto stack
        holder.id = tk;
        holder.value = value;
        st.push_back(holder);
    }
}

// This function checks if a given variable is defined or not
void Semantics::checkDef(Token tk){
    // Initial set to -1 as a "false" indicator
    int isDef = -1;

    // Search through stack to see if variable has been declared
    for(unsigned int i = 0; i < st.size(); i++){
        // If variable found in stack set isDef to index found
        if(st[i].id.val == tk.val){
            isDef = i;
            break;
        }
    }

    // If variable hasnt been defined, set error
    if(isDef < 0){
        cout << "SEMANTICS ERROR: Variable: " << tk.val << " on line: " << tk.lineNum << " is not defined or declared on the stack yet" << endl;
        exit(EXIT_FAILURE);
    }
}

// This function verifes nodes follow scoping rules as a whole based off what type of node we are in
void Semantics::verify(Node* node){
    // Skip null nodes
    if(node == NULL){
        return;
    }

    // If a vars node check if a varible is declared since it declares variables
    if(node->name == "<vars>"){
        // Search for identifier tokens that arent EMPTY 
        for(unsigned int i = 0; i < node->tokens.size(); i++){
            if(node->tokens[i].id == idTK && node->tokens[i].val != "EMPTY"){
                checkDec(node->tokens[i], node->tokens[i+1]);
            }
        }
    }else{
        // If in a node that requires a token such as <R>, <assign> and <in>
        vector<string> names = {"<R>", "<assign>", "<in>"};
        for(unsigned int i = 0; i < names.size(); i++){
            if(node->name == names[i]){
                // Same comments as above >.>
                for(unsigned int j = 0; j < node->tokens.size(); j++){
                    if(node->tokens[j].id == idTK){
                        checkDef(node->tokens[j]);
                    }
                }
            }
        }
    }

    // Verify all tokens connected to this token
    verify(node->n1);
    verify(node->n2);
    verify(node->n3);
    verify(node->n4);
}

// This function will convert our made up code to assembly code
void Semantics::toASM(Node *node){
    // Skip null nodes
    if(node == NULL){
        return;
    }
    
    // <program>-> start <vars> main <block> stop
    if(node->name =="<program>"){
        // Calls vars node
        toASM(node->n1);
        // Calls block node
        toASM(node->n2);

        // Puts "STOP" at the end of the file
        file << "STOP" << endl;
 
        // Put variables we defined at the bottom
        for(unsigned int i = 0; i < st.size(); i++){
            file << st[i].id.val << " " << st[i].value.val << endl;
        }
        // Put variables that are defined at the process at the bottom
        for(unsigned int i = 0; i < tempVarsCount; i++){
            file << "X" << i << " 0" << endl;
        }

        return;
    }

    // <block> -> { <vars> <stats> }
    if(node->name == "<block>"){
        // Goes to vars node
        toASM(node->n1);
        // Goes to stats node
        toASM(node->n2);
        return;
    }

    // <vars> -> empty | let Identifier : Integer <vars>
    if(node->name == "<vars>"){
        // Reads next vars token
        toASM(node->n1);
        return;
    }

    // <expr> -> <N> / <expr> | <N> * <expr> | <N>
    if(node->name == "<expr>"){
        //If the tokens is empty its just <N>
        if(node->tokens.empty()){
            toASM(node->n1);
        }else{
            // Number of current variables
            int numVars = tempVarsCount++;
            //cout << tempVarsCount << endl;

            // Get value of <N>
            toASM(node->n2);
            file << "STORE X" << numVars <<  endl;
            toASM(node->n1);

            if(opMap[node->tokens[0].val] == "divideTK"){
                file << "DIV X" << numVars << endl;
            }else if(opMap[node->tokens[0].val] == "multiplyTK"){
                file << "MULT X" << numVars << endl;
            }
        }
        return;
    }

    // <N> -> <A> + <N> | <A> - <N> | <A>
    if(node->name == "<N>"){
        // Assusmes <A>
        if(node->tokens.empty()){
            toASM(node->n1);
        }else{
            int numVars = tempVarsCount++;
            //cout << tempVarsCount << endl;

            toASM(node->n2);
            file << "STORE X" << numVars << endl;
            toASM(node->n1);

            if(opMap[node->tokens[0].val] == "plusTK"){
                file << "ADD X" << numVars << endl;
            }else if(opMap[node->tokens[0].val] == "minusTK"){
                file << "SUB X" << numVars << endl;
            }
        }
        return;
    }

    // <A> -> %<A> | <R>
    if(node->name == "<A>"){
        // Value of <A> or <R>
        toASM(node->n1);
        // Negate if empty
        if(!node->tokens.empty()){
            file << "MULT -1" << endl;
        }
        return;
    }

    // <R> -> [ <expr> ] | Identifier | Integer
    if(node->name == "<R>"){
        // If expression
        if(node->tokens.empty()){
            toASM(node->n1);
        }else{
            // Load value
            Token tempTK = node->tokens[0];
            file << "LOAD " << tempTK.val << endl;
            //cout << tempTK.val << endl;
        }
        return;
    }

    // <stats> -> <stat> <mStats>
    if(node->name == "<stats>"){
        // <stat>
        toASM(node->n1);
        // <mStats>
        toASM(node->n2);
        return;
    }

    // <mStat> -> empty | <stat> <mStat>
    if(node->name == "<mStat>"){
        // If not empty
        if(node->tokens.empty()){
            // <stat>
            toASM(node->n1);
            // <mStat>
            toASM(node->n2);
        }
        return;
    }

    // <stat> -> <in>. | <out>. | <block> | <if> . | <loop> . | <assign> .
    if(node->name == "<stat>"){
        // <in> or <out> or <block> or <if> or <loop> or <assign>
        toASM(node->n1);
        return;
    }

    // <in> -> scanf [Identifer]
    if(node->name == "<in>"){
        // Read in ID value
        file << "READ " << node->tokens[0].val << endl;
        return;
    }

    // <out> -> printf[<expr>]
    if(node->name == "<out>"){
        // Number of variables sorted
        int numVars = tempVarsCount++;
        //cout << tempVarsCount << endl;
        
        toASM(node->n1);
        // Get value in var
        file << "STORE X" << numVars << endl;
        // Print variable
        file << "WRITE X" << numVars << endl;

        return;
    }

    // <if> -> if[<expr><RO><expr>] then <block>
    if(node->name == "<if>"){
        int numVars = tempVarsCount++;
        //cout << tempVarsCount << endl;
        // Get expr1
        toASM(node->n1);
        file << "STORE X" << numVars << endl;
        // Get expr2
        toASM(node->n3);
        file << "SUB X" << numVars << endl;

        int exitNum = tempNamesNum;
        carryNamesNum = tempNamesNum++;
        // Get RO
        toASM(node->n2);
        // Get block
        toASM(node->n4);
        file << "L" << exitNum << ": NOOP" << endl;

        return;
    }

    // <loop> -> iter[<expr><RO><expr>] <block>
    if(node->name == "<loop>"){
        int numLoop = tempNamesNum++;
        //cout << numLoop << endl;
        int numVars = tempVarsCount++;
        //cout << tempVarsCount << endl;

        //cout << "L" << numLoop << ": NOOP2"<< endl;
        file << "L" << numLoop << ": NOOP" << endl;
        // Get expr 1
        toASM(node->n1);
        file << "STORE X" << numVars << endl;
        // Get expr 2
        toASM(node->n3);
        file << "SUB X" << numVars << endl;

        int exitLoop = tempNamesNum;
        carryNamesNum = tempNamesNum++;
        // Get ro
        toASM(node->n2);
        // get block
        toASM(node->n4);
        file << "BR L" << numLoop << endl;
        file << "L" << exitLoop << ": NOOP" << endl;

        return;
    }

    // <assign> -> Identifer = <expr>
    if(node->name == "<assign>"){
        // Get expr
        toASM(node->n1);
        file << "STORE " << node->tokens[0].val << endl;
        return;
    }

    // <RO> -> =< | => | == | : :
    if(node->name == "<RO>"){
        // Less then equal to
        if(opMap[node->tokens[0].val] == "lessThanEqualTK"){
            file << "BRNEG L" << carryNamesNum << endl;
        // Greater than equal to
        }else if(opMap[node->tokens[0].val] == "greaterThanEqualTK"){
            file << "BRPOS L" << carryNamesNum << endl;
        // Equal too
        }else if(opMap[node->tokens[0].val] == "equalEqualTK"){
            file << "BRNEG L" << carryNamesNum << endl;
            file << "BRPOS L" << carryNamesNum << endl;
            //cout << "equal too" << endl;
        // Not equal to
        }else if(opMap[node->tokens[0].val] == "colonTK"){
            if(node->tokens.size() > 1 && opMap[node->tokens[1].val] == "colonTK"){
                file << "BRZERO L" << carryNamesNum << endl;
                //cout << "Not equal too" << endl;
            }
        }

        return;
    }
}

// This function opens the file to the asm file and begins the process of the translation to asm
void Semantics::translateToASM(Node *node){
    // Open file
    file.open(asmFileName);
    verify(node);
    toASM(node);
    file.close();
}