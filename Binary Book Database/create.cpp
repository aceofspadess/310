// Cevontae Miller
#include <iostream>
#include <fstream> 
#include <iomanip>
#include <cstdlib>

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

void printRecord( BookRec recPrint);

int main(int argc, char *argv[])
{
    BookRec bookData;
    string fileName = argv[1];
    fstream inAndOut (fileName.c_str(), ios::in | ios::out);
    long long checkIsbn;
    long long previousIsbn;
    char hold;
    bool check =false;

    fstream output ("library.out", ios::out|ios::binary); 

    while(inAndOut >> checkIsbn)
    {
        if( checkIsbn < 1)            
        {
            cerr << "This ISBN is illegal, and will be ignored." << "\n";
            check=true;
        }
        else
        {
        	bookData.isbn = checkIsbn;
            inAndOut >> hold;
            inAndOut.getline(bookData.name,25,'|');
            inAndOut.getline(bookData.author,25,'|');
            inAndOut >> bookData.onhand;
            inAndOut >> hold;
            inAndOut >> bookData.price;
            inAndOut >> hold;
            inAndOut.getline(bookData.type,25);

            if(bookData.isbn  <= previousIsbn)
            {
                cerr << "This ISBN has a Out of Sequence error"<< "\n";
                check=true;           
            }        

            if(bookData.onhand < 0)
            {
                cerr << "The OnHand field is less than 0, and the record will be considered illegal." << "\n";
                check=true;
            }

            if (bookData.price < 0)
            {
                cerr << "The Price field is less than 0, and the record will be considered illegal." << "\n";
                check=true;
            }
            if(check == false)
                output.write((char *) &bookData, sizeof(bookData));
            else
                check=false;
        }    
    }
    output.close();
    inAndOut.close();
    fstream binOutput ("library.out", ios::in|ios::binary);

    while(binOutput.read((char *) &bookData, sizeof(bookData) ))
    {
            printRecord(bookData);
    }
    binOutput.close();
}

void printRecord( BookRec recPrint )
{
	 cout<<setw(10)<<setfill('0')<<recPrint.isbn
	 <<setw(25)<<setfill(' ')<<recPrint.name
	 <<setw(25)<<recPrint.author
	 <<setw(3)<<recPrint.onhand
	 <<setw(6)<<recPrint.price
	 <<setw(10)<<recPrint.type<<endl;
}