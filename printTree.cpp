#include "printTree.h"

// This function prints our node tree in Preorder
void printPreorder(Node *node, int depth, string indent, bool last){
    // If the node is empty end
    if(node == NULL){
        return;
    }

    // Indent sign
    string indentSign = "";     

    // Change indent sign based on if we are in the last node or not
    if(depth > 0){
        indentSign = (last) ? "|_" : "|-";      // Nifty Tenary operator
    }else{
        indentSign = "";
    }
    // Prints indent info
    cout << indent << indentSign << node->name << " ";

    // Gets token values
    for(unsigned int i = 0; i < node->tokens.size(); i++){
        cout << node->tokens[i].val;

        // If more then one token
        if(((i + 1) != node->tokens.size()) && (node->tokens[i].id != opTK)){
            cout << ",";
        }
        cout << " ";
    }
    cout << endl;

    // Change indent based on last
    indent += (last) ? "  " : "|  ";        // Hey look another nifty tenary operator, very cool

    // Get information from dynamic nodes
    vector<Node*>nodes;
    if(node->n1 != NULL){
        nodes.push_back(node->n1);
    }
    if(node->n2 != NULL){
        nodes.push_back(node->n2);
    }
    if(node->n3 != NULL){
        nodes.push_back(node->n3);
    }
    if(node->n4 != NULL){
        nodes.push_back(node->n4);
    }

    // Print rest of node
    for(unsigned int i = 0; i < nodes.size(); i++){
        printPreorder(nodes[i], depth + 1, indent, i == nodes.size() - 1);
    }
}