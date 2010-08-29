#ifndef GRAFIO_H
#define GRAFIO_H

//#include "grafTag.h"
#include "grafTagMulti.h"
#include "ofxXmlSettings.h"

class grafIO
{
    public:
        grafIO();
        virtual ~grafIO();

		// for saving
		void setup(string uId, string client, string vers);
		void addKeyword( string keywd );
		void clearKeywords();
		void constructGML(grafTagMulti * tag );
		
		// for loading
		void loadTag(string name, grafTagMulti * tag);
		void saveTag(string path);
		
		ofxXmlSettings		xml;
		
		string username;
		string uniquID;
		string clientname;
		string version;
		string keywords;
		
    protected:
	
    private:
};

#endif // GRAFIO_H
