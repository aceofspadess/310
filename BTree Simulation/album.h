#ifndef ALBUM
#define ALBUM

#include <cstring>
#include <string>
#include <fstream>
using namespace std;

const int UpTo = 50;
typedef char String[UpTo];

class Album
{
   public :
       Album (const Album& otherAlbum);
       Album (String upc, String artist,
                String title);
       Album ();
       Album & operator = (const Album& otherAlbum);
       friend bool operator < (const Album& a, const Album& b);
       friend istream & operator >> (istream & stream, Album & C);
       friend ostream & operator << (ostream & stream, Album & C);
       string getUPC ();
       int recordSize();

   private :
       String UPC, Artist, Title;
};
#endif
