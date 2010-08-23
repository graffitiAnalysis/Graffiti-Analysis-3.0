#include "pocoDirectoryLister.h"

ofxPocoDirectoryLister::ofxPocoDirectoryLister()
{
	extensionToLookFor = "jpg";
}

ofxPocoDirectoryLister::~ofxPocoDirectoryLister()
{

}

void ofxPocoDirectoryLister::getFileNames( vector<string> & files )
{
	refreshFiles();
	for( int i = 0; i < currentFiles.size(); i++)
		files.push_back(currentFiles[i].getFileName());
}


void ofxPocoDirectoryLister::refreshFiles()
{
	std::string dir;

	currentPath.makeAbsolute();

	cout << "Directory is: " << currentPath.toString() << endl;

	currentFiles.clear(); //empty it

	try
	{
		Poco::DirectoryIterator it(currentPath);
		DirectoryIterator end;
		while (it != end)
		{
			Path p(it->path());

			if(p.getExtension() == extensionToLookFor)
			{
				currentFiles.push_back(p);

//				p.std::cout << (it->isDirectory() ? 'd' : '-')
//				<< (it->canRead() ? 'r' : '-')
//				<< (it->canWrite() ? 'w' : '-')
//				<< ' '
//				<< DateTimeFormatter::format(it->getLastModified(),
//											 DateTimeFormat::SORTABLE_FORMAT)
//				<< ' '
//				<< p.getFileName()
//				<< std::endl;
			}

			++it;
		}
	}
	catch (Poco::Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}
}

void ofxPocoDirectoryLister::findSubDirectories( vector<string> & directories)
{

    try
	{
		Poco::DirectoryIterator it(currentPath);
		DirectoryIterator end;
		while (it != end)
		{
			Path p(it->path());

			if( it->isDirectory() )
			{
				directories.push_back(p.getFileName());
				std::cout << "dir: " << p.getFileName() << endl;
			}

			++it;
		}
	}
	catch (Poco::Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}
}

void ofxPocoDirectoryLister::listFiles()
{
//	Path dataPath = Path(ofToDataPath(""));
//	dataPath.makeAbsolute();
//	cout << "The data path is " <<  dataPath.toString() << endl;

	std::cout << "Beginning list... " << std::endl;

	for (std::vector<Path>::const_iterator it = currentFiles.begin(); it != currentFiles.end (); it++)
	{
		std::cout << "Filename: " << it->getFileName() << " full path " << it->toString() << std::endl;
	}

	std::cout << "Ending list... " << std::endl;
}

Path ofxPocoDirectoryLister::getPath(long l)
{
	Path tempPath = Path();

	if(l < getNumberOfFiles())
	{
		tempPath = currentFiles[l];
	}

	return tempPath;
}

bool ofxPocoDirectoryLister::existsDirectory(string currentDir, string subDirectory )
{
	
	setPath(currentDir);
	//currentPath.makeAbsolute();
	
	cout << "Directory is: " << currentPath.toString() << endl;
	
    try
	{
		Poco::DirectoryIterator it(currentPath);
		DirectoryIterator end;
		while (it != end)
		{
			Path p(it->path());
			
			cout << "checking " << p.getFileName() << endl;
			
			if( it->isDirectory() && p.getFileName() == subDirectory)
			{
				return true;
			}
			
			++it;
		}
	}
	catch (Poco::Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}
	
	return false;
}
