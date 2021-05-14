//CreateBT file

#include <fstream>
#include <iostream>
#include <string>
#include "btree.h"

using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 3 || argc > 3)
    {
        cout << "Sequential File name and B-Tree File name required." <<endl;
        return 0;
    }

    char * sequentialFile = argv[1];
    char * indexFile = argv[2];

    fstream readMaster(argv[1], ios::in);
    BTree B;
    B.reset(indexFile);
    keyType dummy;

    while(readMaster >> dummy)
    {
        cout<<"Inserting " <<dummy << "         Current Height: " << B.getHeight() << endl;
        B.insert(dummy);
    }    

    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    B.printTree();
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    cout<< "Current Height: " << B.getHeight() << endl;

}