#include "album.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main()
{
    Album empty;
    char x[50] = "1"; 
    char y[50] = "TestMan"; 
    char z[50] = "Testin";
    Album oneEntry(x,y,z);
    Album copy(oneEntry);

    cout <<"Empty: " << empty <<endl;

    cout <<"oneEntry: " << oneEntry <<endl;

    cout <<"copy: " << copy <<endl;

    bool result = empty < copy;

    cout <<endl;
    cout <<"empty and copy comparison: " << result <<endl;
    cout <<endl;

    result = copy < oneEntry;
    cout <<"copy and oneEntry comparison: " << result <<endl;
    cout <<endl;

    string copyUpc = copy.getUPC();
    cout << "copy upc: " << copyUpc << endl;

    int recordSize = copy.recordSize();
    cout << "copy record size: " <<recordSize <<endl;
    cout << endl;

    fstream file;
    file.open("album.baby", fstream::in);

    file >> empty;

    cout << "Empty now has a record: " << empty << endl;
    file.close();

}