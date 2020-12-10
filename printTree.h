#ifndef PRINTTREE_H
#define PRINTTREE_H
#include <iostream>     // Standard
#include <string>       // Strings
#include <vector>       // Vectors
#include "node.h"
using namespace std;    // Standard

// This function prints tree in preorder
void printPreorder(Node *node, int depth = 0, string dent = "", bool last = true);

#endif