//
//  main.cpp
//  rbtree
//
//  Created by Joseph on 10/03/14.
//  Copyright (c) 2014 J A Mark. All rights reserved.
//

#include <iostream>

#include "list.h"
#include "rbtree.h"

int main(int argc, const char * argv[])
{
    
    RBTree<int, int> rbt = RBTree<int, int>();
    rbt = rbt.insert(1, 10);
    rbt = rbt.insert(2, 20);
    rbt = rbt.insert(1, 100);
    rbt = rbt.insert(1, 1000);
    rbt.printTree();
    rbt = rbt.remove(1, 100);
    rbt.printTree();
    
    return 0;
}

