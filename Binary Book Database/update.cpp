//Cevontae Miller
//Project 1

#include <iostream>
#include <fstream> 
#include <iomanip>
#include <cstdlib>
#include <map>

using namespace std;

typedef char String[25];
struct BookRec
{
    unsigned int isbn;
    String name;
    String author;
    int onhand;
    float price;
    String type;
};

enum TransactionType {Add, Delete, ChangeOnhand, ChangePrice};
struct TransactionRec
{
    TransactionType ToDo;
    BookRec B;
};

void printRecord(BookRec recPrint);

void addRecord (TransactionRec &type, fstream &file, map<unsigned int,long> &map, int &transNumber);

void deleteRecord (TransactionRec &type, fstream &file, map<unsigned int,long> &map,int &transNumber);

void changeOnHand (TransactionRec &type, fstream &file, map<unsigned int,long> &map,int &transNumber);

void changePrice (TransactionRec &type, fstream &file, map<unsigned int,long> &map, int &transNumber);

void createMap (BookRec data, map<unsigned int,long> &map, string masterRead);


int main(int argc, char *argv[])
{
    //master file, transaction file, copy file
    string masterFile = argv[1];
    string transactionFile = argv[2];
    string newMast =argv[3];

    //Copy master file so we have a file to manipulate
    string copy = "cp "+masterFile+" copyMast.out";
    system(copy.c_str());

    TransactionRec transBookData;
    BookRec book;

    //Openning files for binary and regular input and output
    fstream transact (transactionFile, ios::in|ios::out|ios::binary);
    fstream error ("ERRORS", ios::out);
    fstream newMastFile (newMast, ios::out|ios::binary);
    fstream copied("copyMast.out", ios::out|ios::in|ios::binary);

    //This map will hold the isbn's and byte offsets for keeping track
    map<unsigned int, long> dataHolder;

    //Transaction Count
    int transNumber;


    createMap(book, dataHolder, masterFile);

    //Keep pulling add,delete,changeOnhand, and changePrice until the transact instructions are all done
    while(transact.read((char *) &transBookData, sizeof(transBookData) ))
    {       
        if(transBookData.ToDo == Add )
        {
            addRecord(transBookData, copied, dataHolder, transNumber);       
        }
        else if(transBookData.ToDo == Delete)
        {
            deleteRecord(transBookData, copied, dataHolder, transNumber);
        }
        else if(transBookData.ToDo == ChangeOnhand)
        {
            changeOnHand(transBookData, copied, dataHolder, transNumber);
        }
        else if (transBookData.ToDo == ChangePrice)
        {
            changePrice(transBookData, copied, dataHolder, transNumber);
        }

    }

    transact.close();
    error.close();
    //Create an iterator to loop through the map
    map<unsigned int, long>::iterator it;

    //Loop through the map: Write the book data to our new master file, and print the book data to the screen
    for (it = dataHolder.begin(); it != dataHolder.end(); it++)
    {
        copied.seekg( (it->second) - sizeof(book));
        copied.read((char *) &book, sizeof(book));
        newMastFile.write((char *) &book, sizeof(book));
        printRecord(book);
    }
    newMastFile.close();
    copied.close();

    system("rm copyMast.out");
}

//Method to create and fill a map with data from our master file
void createMap (BookRec data, map<unsigned int,long> &map, string masterRead)
{
    long byteoffset;
    fstream masterR (masterRead,ios::in|ios::binary);
    while(masterR.read((char *) &data, sizeof(data) ))
    {
        byteoffset = masterR.tellg();
        map[data.isbn]= byteoffset;
    }
    masterR.close();
}

//Method to add a book record to our map, writing out to our file, and in the case of an error, writing and error message with
//That specific books isbn and transaction number
void addRecord (TransactionRec &type, fstream &file, map<unsigned int,long> &map,int &transNumber)
{
    transNumber += 1;
    fstream errorF("ERRORS", ios::out|ios::app);

    if(map.count(type.B.isbn))
    {
        errorF << "Error in transaction number "<< transNumber << " cannot add—duplicate key " << type.B.isbn << endl;
    }
    else
    {
            file.seekg(0,ios::end);
            file.write((char *) &type.B, sizeof(type.B));
            map[type.B.isbn] = file.tellg();
    }

    errorF.close();
}

//Delete method that will either write out an error or delete a map entry from our isbn map
void deleteRecord (TransactionRec &type, fstream &file, map<unsigned int,long> &map,int &transNumber)
{
    transNumber += 1;
    fstream errorF("ERRORS", ios::out|ios::app);

    if(!map.count(type.B.isbn))
    {
         errorF << "Error in transaction number "<< transNumber << " cannot delete-no such key " << type.B.isbn << endl;
    }
    else
    {
        //Record not added if the isbn is found and record is removed from our map
        map.erase(type.B.isbn);
    }
    errorF.close();
}

//Method that changes the on hand value for our books
//If the transaction action causes the books on hand value to become less than 0, than an error is thrown
// and the value is set to 0
void changeOnHand (TransactionRec &type, fstream &file, map<unsigned int,long> &map, int &transNumber)
{
    fstream errorF("ERRORS", ios::out|ios::app);
    int checkNegative;
    BookRec masterCopy;
    transNumber +=1;

    //If the book isbn exists
    if(map.count(type.B.isbn))
    {
        file.seekg(map[type.B.isbn] - sizeof(BookRec),ios::beg);
        file.read((char *) &masterCopy, sizeof(masterCopy));
        checkNegative = type.B.onhand + masterCopy.onhand;

        //If the transaction action would cause the value to be negative
        if(checkNegative < 0)
        {
            errorF << "Error in transaction number: "<< transNumber << "count = "<< checkNegative <<" for key " << type.B.isbn << endl;
            type.B.onhand =0;
        }
        else
        {
            //Set the value of the books OnHand to the value of the current onhand value - the transaction value
            type.B.onhand = checkNegative;
        }
        file.seekg(0,ios::end);
        file.write((char *) &type.B, sizeof(type.B));
        map[type.B.isbn] = file.tellg();
        
    }
    else if(!map.count(type.B.isbn))
    {
        //If the isbn doesnt currently exist, and error is thrown
         errorF << "Error in transaction number "<< transNumber << "cannot delete-no such key" << type.B.isbn <<endl;
    }

}

//Method to change a books current price
void changePrice (TransactionRec &type, fstream &file, map<unsigned int,long> &map, int &transNumber)
{
    fstream errorF("ERRORS", ios::out|ios::app);
    BookRec masterCopy;
    transNumber +=1;

        //The isbn currently exists
        if(map.count(type.B.isbn))
        {
            //Erase the current version of that isbn pair in the map, and then replace it with a 
            //new pair with and updated book price value
            map.erase(type.B.isbn);
            file.seekg(0,ios::end);
            file.write((char *) &type.B, sizeof(type.B));
            map[masterCopy.isbn]= file.tellg();

        }
        else
        {
            errorF<< "Error in transaction number "<< transNumber << "cannot change price-no such key" << type.B.isbn <<endl;
        }
        

}

//Method to print all values in a BookRec struct
//Thanks Dr Digh
void printRecord( BookRec recPrint )
{
	 cout<<setw(10)<<setfill('0')<<recPrint.isbn
	 <<setw(25)<<setfill(' ')<<recPrint.name
	 <<setw(25)<<recPrint.author
	 <<setw(3)<<recPrint.onhand
	 <<setw(6)<<recPrint.price
	 <<setw(10)<<recPrint.type<<endl;
}