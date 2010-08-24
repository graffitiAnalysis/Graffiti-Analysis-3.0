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
#include "grafCurveSmoother.h"

class GaThreadedLoader : public ofxThread{

	public:
		GaThreadedLoader();
		~GaThreadedLoader();
	
		void setup(string dirPath);
		void start();
		void stop();
		
		string getFileName(int index);
		
		bool bResponseReady;
		int totalLoaded, totalToLoad;
		grafTagMulti * tags;
	
	protected:
	
		void getDirectoryInfo(string myTagDirectory);
		
		// threading
		void threadedFunction();
		
	
		
		ofxPocoDirectoryLister	dirLister;				// searches directory for gml files
		string					myTagDirectory;			// directory to load from (changeable in data/settings/directorySettings.xml
		vector<string>			filesToLoad;			// list of all file paths
		vector<string>			filenames;				// list of corresponding file names (sans .gml)
		string					nxtFileToLoad;
		string					nxtFileName;
		
		grafIO gIO;					// gml loader/saver
		grafCurveSmoother smoother;						// adds points to smooth tag

};
