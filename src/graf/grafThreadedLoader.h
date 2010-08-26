/*
 *  grafThreadedLoader.h
 *  GA_Interactive
 *
 *  Created by Chris on 8/23/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxThread.h"
#include "pocoDirectoryLister.h"
#include "grafIO.h"
//#include "grafCurveSmoother.h"

//includes for the rss feed loader hack
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/RegularExpression.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/XML/XMLString.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NamedNodeMap.h"
#include <iostream>
#include <fstream>
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::File;
using Poco::URI;
using Poco::Exception;
using Poco::RegularExpression;
using Poco::URIStreamOpener;
using Poco::Net::HTTPStreamFactory;
using Poco::XML::XMLString;
using Poco::XML::DOMParser;
using Poco::XML::Document;
using Poco::XML::NodeIterator;
using Poco::XML::NodeFilter;
using Poco::XML::Node;
using Poco::XML::NamedNodeMap;

class GaThreadedLoader : public ofxThread{

	public:
		GaThreadedLoader();
		~GaThreadedLoader();
	
		void setup(string dirPath);
		void setup(vector<string> nfilesToLoad,vector<string> nfilenames);
		void setup(string key,string dirPath);
		void start();
		void stop();
		void update();
		
		void getDirectoryInfo();
		string getFileName(int index);
		int getResponse();
		
		bool bResponseReady;
		int totalLoaded, totalToLoad;
		grafTagMulti * tags;
		

	protected:
	
		void getDirectoryInfo(string myTagDirectory);
		
		// threading
		void threadedFunction();
		
		// rss loading
		void loadTagsFromRSS( string thisword, string myTagDirectory );
		void getGuidValues( const std::string& result, std::vector<int>* dest ); //thx v
		bool makeDirectory( string dirPath, bool bRelativeToData = true ); //found on web, from zach
		string myKeyword, myDirectory;
		int threadMode;
		
		// utils
		void splitString( string & str, vector<string> & tokens, string delimiters);
	
		
		ofxPocoDirectoryLister	dirLister;				// searches directory for gml files
		string					myTagDirectory;			// directory to load from (changeable in data/settings/directorySettings.xml
		vector<string>			filesToLoad;			// list of all file paths
		vector<string>			filenames;				// list of corresponding file names (sans .gml)
		string					nxtFileToLoad;
		string					nxtFileName;
		
		grafIO gIO;					// gml loader/saver
		
		bool bSetup;
		bool bHaveDirs;
		bool bUseRss;
		bool bDownloadedFiles;
};
