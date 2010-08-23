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
}

GaThreadedLoader::~GaThreadedLoader()
{
	delete [] tags;
 
}

void GaThreadedLoader::setup(string dirPath)
{
	totalLoaded = 0;
	totalToLoad = 0;
	
	getDirectoryInfo( dirPath );
	
	//start();
}

void GaThreadedLoader::start(){
	
	if (isThreadRunning() == false){
        bResponseReady = false;
        startThread(false, false);   // blocking, verbose
    }

}

void GaThreadedLoader::stop(){
    stopThread();
}

void GaThreadedLoader::threadedFunction(){
		
    if( totalLoaded < totalToLoad )
	{
		
		gIO.loadTag( filesToLoad[ totalLoaded ], &tags[ totalLoaded ]);
		tags[ totalLoaded ].tagname = filenames[ totalLoaded ];
		
		smoother.smoothTag(4, &tags[ totalLoaded ]);
		tags[totalLoaded].average();
		tags[totalLoaded].average();
		
		totalLoaded++;
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

	totalToLoad = filesToLoad.size();
	
	tags = new grafTagMulti[totalToLoad];
}
