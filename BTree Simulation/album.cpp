#include "album.h"

Album::Album ()
{
       strcpy (UPC, "\0");  strcpy (Artist, "\0");
       strcpy (Title, "\0");
}

Album::Album (String upc, String artist,String title)
{
       strcpy (UPC, upc);  strcpy (Artist, artist);
       strcpy (Title, title);
}

Album::Album(const Album& otherAlbum)
{
       strcpy (UPC, otherAlbum.UPC);
       strcpy (Artist, otherAlbum.Artist);
       strcpy (Title, otherAlbum.Title);
}

Album & Album::operator = (const Album& otherAlbum)
{
       strcpy (UPC, otherAlbum.UPC);
       strcpy (Artist, otherAlbum.Artist);
       strcpy (Title, otherAlbum.Title);
       return *this;
}

bool operator < (const Album& a, const Album& b)
{
     return atoi(a.UPC) < atoi(b.UPC); 
}

istream & operator >> (istream & stream, Album & C)
{
   stream.getline(C.UPC, 50, '\n');
   stream.getline(C.Artist,50, '\n');
   stream.getline(C.Title,50, '\n');    

   return stream; //>> C.UPC >> C.Artist >> C.Title;
}

ostream & operator << (ostream & stream, Album & C)
{
    return stream << C.UPC << ", " << C.Artist << ", " << C.Title;
}


string Album::getUPC() 
{
    return UPC;
}

int Album::recordSize()
{
    return (strlen(UPC)+strlen(Artist)+strlen(Title));
}
