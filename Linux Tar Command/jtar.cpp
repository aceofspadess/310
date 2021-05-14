//Cevontae Miller
//Jtar

#include <fstream>
#include <iostream>
#include "file.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <string>
#include <filesystem>
#include <vector>


bool statIsFile(char* file);
string statProtection(char* file);
int statSize(char* file);
string statDate(char* file);


using namespace std;
using recursive_directory_iterator = filesystem::recursive_directory_iterator; //What i will use to recusively get the file paths of all files and directories in a specific directory

int main( int argc, char *argv[])
{

	string filePath = "";
	string args="";
	string protection = "";
	bool result=true;
	int check=0;
	vector<File> fileList;
	char* holder;

//If not enough arguments for any command are provided throw error
	if(argc < 2)
	{
		cerr << "Error to few arguments provided"<<endl;
		return 0;
	}

//Get cla command
	args = argv[1];

	if(args == "-cf" )
	{
		if(argc < 4)
		{
			cerr <<"Error -cf requires a minimum of 4 arguments to run"<<endl;
			return 0;
		}
	
		
//Loop as many times as files to be tar'd are given

		for(int i=3; i < argc; i++)
		{
//Check if the current file is actually a file or a directory			
			result = statIsFile(argv[i]);
			holder = argv[i];		

//If file is a directory then mark it as such while creating the file object
//Also this make sure that the original directory itself is pushed into the vector <File>

			if( result == false)
			{	
				File x( holder, statProtection(holder).c_str(), to_string(statSize(holder)).c_str(), statDate(holder).c_str());
				x.flagAsDir();
				fileList.push_back(x);

				for(const auto& dirEntry : recursive_directory_iterator(holder)) 
				{

//Creating file objects for each file and directory in the specified directory

					char pathname[81];
					strcpy(pathname, dirEntry.path().c_str());
					File x( pathname, statProtection(pathname).c_str(), to_string(statSize(pathname)).c_str(), statDate(pathname).c_str());

					char name[81];
					strcpy(name, dirEntry.path().c_str());
					
					result = statIsFile( name );

//If the current file is an actual file inside of a directory
//make sure to not mark it as one
					if( result == false)
						x.flagAsDir();

//Push each file into a vector
					fileList.push_back(x);
				}

			}
			else
			{
//If the current file is actually just a file
//create a file object of it and push to vector
				args = holder;
				File y( args.c_str(), statProtection(holder).c_str(), to_string(statSize(holder)).c_str(), statDate(holder).c_str()); 
				fileList.push_back(y);
				
			}

		
		}
		

//Create Tar File
		
		fstream tar(argv[2], ios::out|ios::binary);
		fstream readIn;	
		int s = fileList.size();
		tar.write((char *) &s, sizeof(int));
	
		for(int i=0; i<s; i++)
		{

//Write out number of files to the binary file first
			tar.write((char*) &fileList[i], sizeof(File));

//For each file write that file and its contents to the binary file
			if(fileList[i].isADir() == false)
			{
				readIn.open(fileList[i].getName(), ios::in);
				char readFile[ stoi(fileList[i].getSize()) ];
				readIn.read( readFile, stoi( fileList[i].getSize()) );
				tar.write( readFile, stoi( fileList[i].getSize()) );
				readIn.close();
			}
		}

		tar.close();
	}
	else if(args == "-tf")
	{
		if(argc < 3)
			cerr<< "Error -tf requires a minimum of 3 arguments to run"<<endl;
		
		fstream openTar(argv[2], ios::in|ios::binary);

//If the provided Tar file doesnt exist, throw error
		if(!openTar)
			cerr << "Provided Tar file does not exist " << endl;
		

		int tarSize;
		File nameTar;
		openTar.read( (char *) &tarSize, sizeof(int));

//Read in the number of string paths in Tar and loop that many times
		for(int i=0; i< tarSize; i++)
		{
			openTar.read( (char *) &nameTar, sizeof(File));
			cout << nameTar.getName() << endl;

//Skip over file contents
			if(nameTar.isADir() == false)
				openTar.seekg(stoi(nameTar.getSize()), ios::cur); 		
		}
		openTar.close();	

	}
	else if(args == "-xf")
	{
		if(argc < 3)
			cerr<< "Error -xf requries a minimum of 3 arguments to run"<<endl;

//Open Tar file
		fstream openTarFile(argv[2], ios::in|ios::binary);

		int tarFileSize;
		File nameTarFile;

//Get number of files in Tar		
		openTarFile.read( (char *) &tarFileSize, sizeof(int));
		
		for(int i=0; i< tarFileSize; i++)
		{
			openTarFile.read( (char *) &nameTarFile, sizeof(File));

			if(nameTarFile.isADir() == false)
			{

//Open a file with the name given from the binary file and feel its contents
				fstream	x;
				x.open(nameTarFile.getName().c_str(), ios::out);	
				char fileContent[ stoi(nameTarFile.getSize()) ];
				openTarFile.read( fileContent , sizeof(fileContent) );
				x.write( fileContent, sizeof(fileContent) );
				x.close();
			}
			else
			{

//If directory already not created, create a directory with binary file given name
				fstream alive(nameTarFile.getName().c_str(), ios::in);

				if(!alive)
				{
					system( ("mkdir " + nameTarFile.getName()).c_str()  );
				}
				
			
			}
		}	
	

	}
	else if(args == "--help")
	{
		cout <<endl;

		cout << "	jtar -cf tarfile file1 dir1... : Specifies jtar to make a tar file named tarfile basedon the files or directories following the name of the tarfile."<<endl; 
		cout << "		All files in this assignment will be text files. No files will be symbolic or hard links." <<endl;

		cout<<endl;
		cout<<endl;

		cout <<"	jtar -tf tarfile: Specifies jtar to list the names of all files that have packed into a tarfile."<<endl; 
		cout <<"		It does not recreate anything." <<endl;
	
		cout<<endl;
		cout<<endl;
	
		cout <<"	jtar -xf tarfile: Specifies jtar to read a tar file, and recreate all the files saved in that tarfile."<<endl; 
		cout <<"		This includes making any directories that must exist to hold the files. The files thus created should be exactly like the original files."<<endl; 
		cout <<"		This means that they should have the same access and modification times, and the same protection modes." <<endl;


		cout <<endl;
	}
	else
		cerr << "Error incorrect arguments provided"<<endl;


	return 0;
}

//Return if file is actually a file or a directory
bool statIsFile(char* file)
{
        struct stat buf;
        struct utimbuf timebuf;
        char *token;

        lstat (file, &buf);

        if (S_ISREG(buf.st_mode))
                return true;
                
	else
		return false;


}
//Return Pmode of file
string statProtection(char* file)
{
        struct stat buf;
        struct utimbuf timebuf;
        char *token;
	string prot = "";

        lstat (file, &buf);

        if (S_ISREG(buf.st_mode))
   	{
		prot.append(to_string( (buf.st_mode & S_IRWXU) >> 6));		
		prot.append(to_string( (buf.st_mode & S_IRWXG) >> 3));
		prot.append(to_string( (buf.st_mode & S_IRWXO) ));
		return prot;
	}
        else
                return prot;
        
}
//Return size of file
int statSize(char* file)
{
	struct stat buf;
	struct utimbuf timebuf;
	char *token;

	lstat (file, &buf);
	return buf.st_size;
}
//Return timestamp of file
string statDate(char* file)
{
        struct stat buf;
        struct utimbuf timebuf;
        char *token;
	string time = "";

        lstat (file, &buf);
        if (S_ISREG(buf.st_mode))
        {				  
		char stamp[16];
		strftime(stamp, 16, "%Y%m%d%H%M.%S", localtime(&buf.st_ctime));
		time = stamp;
		return time;
        }
        else
        	return time;
}
