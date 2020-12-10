#ifndef NODE_H
#define NODE_H
#include <string>
#include <vector>
#include "token.h"
using namespace std;

typedef struct Node{
    int level;      // The level this node is at
    string name;   // Name of this node
    vector<Token> tokens;       // Type of token of this node
    struct Node *n1, *n2, *n3, *n4;     // Dynamic nodes to fill
}Node;

#endif