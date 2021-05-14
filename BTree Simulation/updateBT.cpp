//updateBT file

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include "btree.h"
#include "album.h"


using namespace std;

int main(int argc, char* argv[])
{
    if(argc < 3 || argc > 3)
    {
        cout << "Tree File name and Transaction File name required." <<endl;
        return 0;
    }

    string treeFile = argv[1];
    fstream transFile;
    transFile.open(argv[2], fstream::in);
    string action;
    BTree B;
    keyType dummy;


	while(transFile >> action)
	{
        //cout <<action <<endl;
		if(action == "S" || action == "s")
        {
            transFile >> action;
            if( B.search(action) == false)
                 cout << "      Result of search for \" " << action << " \": Not Found" << endl;
            else
                 cout << "      Result of search for \" " << action << " \": Found" << endl;

        }
        else if(action == "I" || action == "i")
        {
            getline(transFile,action);
            transFile >> dummy;
            cout << "      Attempting to insert: " << dummy <<endl;
            B.insert(dummy);
            cout << "      Insert successful" <<endl;
        }
    }
    B.totalio();
    transFile.close();
}