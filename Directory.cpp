//**********************************************************************************//
//  Given a folder pathway, 1. calculate size of total files including subfolders   //
//                          2. calculate size of EXE and DLL files                  //
//**********************************************************************************//
#include <stdio.h>
#include <algorithm>
#include <sys/stat.h> 
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int fSizeInDir(string dirFold, char ch, string file, int &eSize);
int dirSize(string dirFold, int &dllSize);

int main(int argc,char *argv[])
//Pre: input a folder pathway
//post: print out 1. the size of total files
//                2. the size of EXE and DLL files
{
	int eSize=0;
	string strPath;
	if (argc>1) strPath=argv[1];
	else {
		printf("Enter directory:");
		getline(cin, strPath);
	}
	while (strPath.size()>0 && (strPath[strPath.size()-1]=='\\' || strPath[strPath.size()-1]=='/'))
		strPath.assign(strPath, 0, strPath.size()-1);
    struct stat status;
	if (stat(strPath.c_str(), &status) == 0 && (status.st_mode & S_IFDIR) ) {
		printf("\nTotal size of files:%d bytes", dirSize(strPath, eSize));
		printf("\nSize of Exe/Dll files:%d bytes\n", eSize );
	} else printf("\nDirectory doesn't exist\n");
	printf("\nClick 0 to exit:");
	cin>>eSize;//_getch();
	return eSize;
}//int main(int argc,char *argv[])

int fSizeInDir(string dirFold, char ch, string file, int &eSize)
//pre: given folder path and subfolder/filename
//post: return folder size including subfolders by recursively calculating
//      return size of .exe and .dll to eSize
{
	struct stat mystat;
	int fsize=0;
    if ( file!="." && file!="..") {
	   string fname=dirFold+ch+file;
	   stat( fname.c_str(), &mystat );
	   if ( (mystat.st_mode & S_IFDIR) ) {//Is a Subdirectory
		  fsize +=dirSize(fname, eSize);
	   } else {//Is a file
		  fsize +=mystat.st_size;
		  fname.assign(fname, fname.size()-4, 4);
		  transform(fname.begin(), fname.end(), fname.begin(), ::toupper); 
		  if (fname==".EXE" || fname==".DLL")
			  eSize +=mystat.st_size;
	   }
    }
	return fsize;
}//int fSizeInDir(string dirFold, char ch, string file, int &eSize)

#ifdef _WIN32 || _WIN64
#include <windows.h>
int dirSize(string dirFold, int &dllSize)
//pre: given folder name
//post: return folder size and exe/dll file size to dllSize
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError;
	string findName=dirFold+"\\*";
	int fsize=0;

	hFind = FindFirstFile(findName.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		printf ("Invalid handle. Error %u.\n", GetLastError());
	} else {
		fsize+=fSizeInDir(dirFold, '\\', FindFileData.cFileName, dllSize);
		while (FindNextFile(hFind, &FindFileData) != 0)
			fsize+=fSizeInDir(dirFold, '\\', FindFileData.cFileName, dllSize);
		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES)
			printf ("FindNextFile error. Error %u.\n", dwError);
	}
	return fsize; 
}

#elif defined __linux__
#include <dirent.h>
#include <errno.h>
int dirSize(string dirFold, int &dllSize)
{
	int fsize=0;
	DIR *dp;
	struct dirent *dirp;
	string fname;
	if((dp = opendir(dirFold.c_str())) == NULL) {
		cout << "Error(" << errno << ") opening " << dirFold << endl;
		return fsize;
	}
	while ((dirp = readdir(dp)) != NULL) {
	   fname.assign(dirp->d_name);
	   fsize+=fSizeInDir(dirFold, '/', fname.c_str(), dllSize);
	}
	closedir(dp);
	return fsize;
}
//#elif defined TARGET_OS_MAC 
#else
	#error Unsupported platform
#endif
