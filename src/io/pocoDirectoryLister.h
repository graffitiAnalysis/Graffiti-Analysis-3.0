#ifndef _OFX_POCO_DIRECTORY_LISTER
#define _OFX_POCO_DIRECTORY_LISTER

#define OF_USING_POCO
#include "ofMain.h"

#include "Poco/DirectoryIterator.h"
#include "Poco/Path.h"

using Poco::DirectoryIterator;
using Poco::Path;


class ofxPocoDirectoryLister
{
	public:
	ofxPocoDirectoryLister(); //constructor
	~ofxPocoDirectoryLister(); //destructor

	bool existsDirectory(string currentDir, string subDirectory );

	void setExtensionToLookFor(string extension)
	{
		extensionToLookFor = extension;
	}
	string getExtensionToLookFor()
	{
		return extensionToLookFor;
	}
	void setPath(Poco::Path _p)
	{
		currentPath = _p;
	}
	Poco::Path getCurrentPath()
	{
		return currentPath;
	}
	long getNumberOfFiles()
	{
		refreshFiles();
		return currentFiles.size();
	}

	void findSubDirectories( vector<string> & directories);
	void getFileNames( vector<string> & files );
	
	void listFiles(); //prints out all the files in the current path, only to be called after refreshing


	Poco::Path getPath(long l);
private:
	void refreshFiles(); //refreshes all the files in the current path
	Poco::Path			currentPath;			//the current path of the directory lister
	std::vector <Poco::Path>	currentFiles; //a vector containing all the files in our current path
	string extensionToLookFor;		//a string representing the extension we are looking for
};

#endif

