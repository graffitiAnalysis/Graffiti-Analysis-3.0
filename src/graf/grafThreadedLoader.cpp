/*
 *  grafThreadedLoader.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 8/23/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "grafThreadedLoader.h"

GaThreadedLoader::GaThreadedLoader()
{
	totalLoaded = 0;
	totalToLoad = 0;
	bResponseReady = true;
	bSetup	= false;
	bHaveDirs	= false;
	bUseRss = false;
	bDownloadedFiles = false;

	threadMode = 0;
	
	rssUrl = "http://000000book.com/data.rss?keywords=";
}

GaThreadedLoader::~GaThreadedLoader()
{
	delete [] tags;
 
}

void GaThreadedLoader::setup(string dirPath)
{
	filesToLoad.clear();
	filenames.clear();
	totalLoaded = 0;	
	getDirectoryInfo( dirPath );
	if( filesToLoad.size() > 0 ) bHaveDirs = true;
	bSetup = true;
	bUseRss = false;
	threadMode = 0;
	
	ofxXmlSettings xml;
	if( xml.loadFile("appSettings.xml") )
	{
		rssUrl = xml.getValue("rss_url", rssUrl);
	}
	
	if(totalLoaded > 0) delete [] tags;
	totalLoaded = 0;
	//totalToLoad = 0;
}

void GaThreadedLoader::setup(vector<string> nfilesToLoad,vector<string> nfilenames)
{
	filesToLoad.assign(nfilesToLoad.begin(),nfilesToLoad.end());
	filenames.assign(nfilenames.begin(),nfilenames.end());
	totalToLoad = filesToLoad.size();
	tags = new grafTagMulti[totalToLoad];
	bSetup = true;
	if( filesToLoad.size() > 0 ) bHaveDirs = true;
	bUseRss = false;
}

void GaThreadedLoader::setup(string keyword, string directory)
{
	totalLoaded = 0;
	
	myKeyword	= keyword;
	myDirectory = directory;
	threadMode	= 1;
	bSetup		= true;
	bHaveDirs	= false;
	bUseRss		= true;
	bDownloadedFiles = false;
	
	if(totalLoaded > 0) delete [] tags;
	totalLoaded = 0;
}

void GaThreadedLoader::update()
{
	if(bUseRss)
	{
		if( bSetup && !bDownloadedFiles)
		{
			start();
		}else if( bSetup && !bHaveDirs )
		{
			getDirectoryInfo();
			start();
		}else if(totalLoaded < totalToLoad)
			start();
		
	}else{
		if( bSetup && !bHaveDirs)
		{
			;
		}else if( bSetup && bHaveDirs && totalLoaded < totalToLoad)
		{
			start();
		}
	}
	
}

void GaThreadedLoader::getDirectoryInfo()
{
	if( myDirectory.length() > 0 && myKeyword.length() >  0)
	{
		filesToLoad.clear();
		filenames.clear();
		string myPath = myDirectory+"/"+myKeyword+"/";
		getDirectoryInfo(myPath);
		if( filesToLoad.size() > 0 ) bHaveDirs = true;
	}
}

void GaThreadedLoader::start(){
	
	if (isThreadRunning() == false){
        
		bResponseReady = false;
        
		if(threadMode == 0)
		{
			nxtFileToLoad = filesToLoad[ totalLoaded ];
			nxtFileName = filenames[ totalLoaded ];
		}
		
		startThread(false, false);   // blocking, verbose
    }

}

void GaThreadedLoader::stop(){
    stopThread();
}

void GaThreadedLoader::threadedFunction(){
		
    if( threadMode == 0 )
	{
		if( totalLoaded < totalToLoad )
		{
			
			gIO.loadTag( nxtFileToLoad, &tags[ totalLoaded ]);
			tags[ totalLoaded ].tagname = nxtFileName;
			
			cout << "loaded " << nxtFileName << endl;
			
			totalLoaded++;
		}
	}else if( threadMode == 1 )
	{
		loadTagsFromRSS(myKeyword, myDirectory);
		threadMode = 0;
	}
	
	stop();
    bResponseReady = true;
	
}

void GaThreadedLoader::getDirectoryInfo(string myTagDirectory)
{

	cout << "PATH TO TAGS: " << ofToDataPath(myTagDirectory,true) << endl;

	vector<string> dirs;
	dirLister.setPath( ofToDataPath(myTagDirectory,true) );
	dirLister.findSubDirectories(dirs);

	for( int i = 0; i < dirs.size(); i++)
	{
		dirLister.setPath( ofToDataPath(myTagDirectory+dirs[i]+"/",true) );
		dirLister.setExtensionToLookFor("gml");
		int num = dirLister.getNumberOfFiles();
		if( num > 0 )
		{
			filenames.push_back(dirs[i]);
			filesToLoad.push_back(  ofToDataPath(myTagDirectory+dirs[i]+"/"+dirs[i]+".gml") );
		}
	}


	dirs.clear();
	dirLister.setPath( ofToDataPath(myTagDirectory,true) );
	dirLister.setExtensionToLookFor( "gml" );
	dirLister.getFileNames(dirs);

	for( int i = 0; i < dirs.size(); i++)
	{
		filesToLoad.push_back( ofToDataPath(myTagDirectory+dirs[i]) );
	
		string name = dirs[i];
		int endgml = name.find(".gml");
		if( endgml >= 0 )
			name.erase( endgml,name.size() );
	
		filenames.push_back(name);
	}

	int totalBefore = totalToLoad;
	totalToLoad = filesToLoad.size();
	
	/*if( totalBefore > 0 && totalBefore < totalToLoad )
	{
		totalBefore = 0;
		delete [] tags;
	}
	*/
	if( totalToLoad > 0 )
		tags = new grafTagMulti[totalToLoad];
}


string  GaThreadedLoader::getFileName(int index)
{
	if( index < totalLoaded ) return filesToLoad[ index ];
	else return "";
}


void GaThreadedLoader::loadTagsFromRSS( string thisword, string myTagDirectory ) {
	try {
		//	std::string thisword = "Rudy"; 
		
		//get RSS
		std::string thisuri = rssUrl;	//"http://000000book.com/data.rss?keywords=";
		thisuri.append(thisword);
		std::cout << thisuri << std::endl;
		URI uri(thisuri);
		std::string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";
		
		HTTPClientSession session(uri.getHost(), uri.getPort());
		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		session.sendRequest(req);
		HTTPResponse res;
		istream& rs = session.receiveResponse(res);
		std::cout << res.getStatus() << " " << res.getReason() << std::endl;
		
		//parse RSS for guid 
		std::string result;
		StreamCopier::copyToString(rs, result);
		std::vector<int> dest;
		getGuidValues( result, &dest );
		
		//create sub directory
		std::string thisdir = myTagDirectory;
		thisdir.append(thisword);
		makeDirectory( thisdir );
		
		//get all guids .gml into subdirectory
		for (int i = 0; i < dest.size(); i++) {
			
			std::ostringstream thisuri;
			thisuri << "http://000000book.com/data/" << dest[i] << ".gml";
			URI uri(thisuri.str());
			std::string path(uri.getPathAndQuery());
			if (path.empty()) path = "/";
			HTTPClientSession session(uri.getHost(), uri.getPort());
			HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
			session.sendRequest(req);
			HTTPResponse res;
			istream& rs = session.receiveResponse(res);
			std::cout << res.getStatus() << " " << res.getReason() << std::endl;
			//std::string result;
			//StreamCopier::copyToString(rs, result);
			
			//save to disk
			ofstream myfile;
			std::ostringstream filename;
			filename << thisdir << "/" << dest[i];
			makeDirectory( filename.str() ); //seems gml player easierly looks for tags/hotword/filename-1/filename-1.gml instead of tags/hotword/*.gml
			filename << "/" << dest[i] << ".gml";
			myfile.open (ofToDataPath(filename.str()).c_str());
			StreamCopier::copyStream(rs, myfile);
			myfile.close();
			
		}
		
		bDownloadedFiles = true;
		//load filenames
		/*filesToLoad.clear();
		filenames.clear();
		
		vector<string> dirs;
		dirLister.setPath( ofToDataPath(myTagDirectory+thisword,true) );
		dirLister.findSubDirectories(dirs);
		
		for( int i = 0; i < dirs.size(); i++)
		{
			dirLister.setPath( ofToDataPath(myTagDirectory+thisword+"/"+dirs[i]+"/",true) );
			dirLister.setExtensionToLookFor("gml");
			int num = dirLister.getNumberOfFiles();
			if( num > 0 )
			{
				filenames.push_back(dirs[i]);
				filesToLoad.push_back(  ofToDataPath(myTagDirectory+thisword+"/"+dirs[i]+"/"+dirs[i]+".gml") );				
			}
		}
		*/
		
	}
	catch (Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
		//exit();
	}
	
	cout << "done loading from rss" << endl;
	
}

void GaThreadedLoader::splitString( string & str, vector<string> & tokens, string delimiters)
{
	
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
	
}


void GaThreadedLoader::getGuidValues( const std::string& result, std::vector<int>* dest ){
	//std::vector<int> intList;
	
	try
	{
		TiXmlDocument doc;
		if( !doc.Parse( result.c_str()) )
		{
			cout << "Error: could not load string" << endl;
		}
		
		TiXmlElement* element;
		TiXmlElement* child;
		TiXmlNode* root;
		
		//get root element 'TodoList', set to hRoot
		root = doc.FirstChildElement();
		if( !root )
		{
			cout << "no valid root!" << endl;
		}
		
		TiXmlElement* channelElem = root->FirstChildElement("channel");
		cout << "channel: " << channelElem->Value() << endl;
		
		element = root->FirstChildElement("channel");
		
		int count = 0;
		for( element=element->FirstChildElement("item"); element; element = element->NextSiblingElement() )
		{
			TiXmlElement* pCur = element->FirstChildElement("guid");
			if(pCur)
			{
				std::cout << "guid: " << pCur->Value() << "   " << pCur->GetText() << std::endl;
				
				int i = -9999;
				std::istringstream val( pCur->GetText() );
				vector<string>tokens;
				string myval = pCur->GetText();
				splitString( myval, tokens,"Tag-");
				if( tokens.size()>0 )
				{
					i = atoi(tokens[0].c_str());
					cout << "adding dest " << i << endl;
					dest->push_back(  i );
				}
			}
		}
	}
	catch( Exception& e )
	{
		std::cerr << e.displayText() << std::endl;
	}
}

bool GaThreadedLoader::makeDirectory(string dirPath, bool bRelativeToData){
	
	if( bRelativeToData ) dirPath = ofToDataPath(dirPath);
	
	File myFile(dirPath);
	bool success = false;
	try{
		success = myFile.createDirectory();
	}catch( Poco::Exception &except ){
		ofLog(OF_LOG_ERROR, "makeDirectory - directory could not be created");
		return false;
	}
	
	if(!success)ofLog(OF_LOG_WARNING, "makeDirectory - directory already exists");
	
	return success;
}


int GaThreadedLoader::getResponse()
{
	if( bResponseReady )
	{
		if(bUseRss && !bDownloadedFiles ) return 0;
		else if( bHaveDirs && totalToLoad > 0 ) return 1;
		else return 0;
	}
	
	return -1;
}

