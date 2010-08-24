#include "grafIO.h"


grafIO::grafIO()
{
   	uniquID		= "";
	clientname  = "";
	version		= "";
	
	
	
}

grafIO::~grafIO()
{
    //dtor
}


void grafIO::setup(string uId, string client, string vers)
{
	uniquID		= uId;
	clientname  = client;
	version		= vers;
}

void grafIO::addKeyword( string keywd )
{
	if( keywords.size() >  0 )  keywords += ",";
	if( keywd.size() >  0 )		keywords += keywd;
}

void grafIO::clearKeywords()
{
	keywords.clear();
}

void grafIO::loadTag(string gmlFile, grafTagMulti * tag)
{
	ofxXmlSettings xml;
	xml.loadFile(gmlFile);
	
	printf("loading %s\n", gmlFile.c_str());
	
	
	
	xml.pushTag("GML");
	xml.pushTag("tag");
	
	ofPoint up = ofPoint(0,-1,0);
	
	tag->tagname = xml.getValue("header:client:name", "graf");
	
	if( xml.pushTag("environment", 0) )
	{
		// get position
		float xScale = xml.getValue("screenBounds:x", 1024);
		float yScale = xml.getValue("screenBounds:y", 768);
		
		//tag->drawScale = (xScale>yScale) ? xScale : yScale;
		
		up.x = xml.getValue("up:x", 0);
		up.y = xml.getValue("up:y", -1);
		up.z = xml.getValue("up:z", 0);
		
		tag->position.x = xml.getValue("offset:x", 0.f);
		tag->position.y = xml.getValue("offset:y", 0.f);
		tag->position.z = xml.getValue("offset:z", 1.f);
		
		// check that z not == 0
		if(tag->position.z == 0 ) tag->position.z = 1;
		
		tag->rotation.x = xml.getValue("rotation:x", 0.f);
		tag->rotation.y = xml.getValue("rotation:y", 0.f);
		tag->rotation.z = xml.getValue("rotation:z", 0.f);
		
		tag->rotation_o = tag->rotation;
		tag->position_o = tag->position;
		
		xml.popTag();
	}
	
	if( xml.pushTag("drawing", 0) )
	{
		int num = xml.getNumTags("stroke");
			
		for(int k = 0; k < num; k++)
		{
				
			if(xml.pushTag("stroke", k))
			{
				if(k>0) tag->nextStroke();
					
				int numPt = xml.getNumTags("pt");
					
				for(int j = 0; j < numPt; j++)
				{
						
					ofPoint pt;
					pt.x =  xml.getValue("pt:x", 0.0, j);
					pt.y =  xml.getValue("pt:y", 0.0, j);
					pt.z =  xml.getValue("pt:z", 0.0, j);
					
					// in particular for dustTag
					if( up.x == 1 )
					{
						float px = pt.x;
						pt.x = pt.y;
						pt.y = 1-px;
					}
					
					// time vs t
					float t = -1;
					
					t = xml.getValue("pt:time", -1.0, j);
					if( t < 0 )
						t = xml.getValue("pt:t", 0.0, j);
					
					tag->addNewPoint( pt, -1, -1, t, false);

				}
					
				xml.popTag();
				
			}
				
		}
		xml.popTag();			
	}
	
	xml.popTag();			
	xml.popTag();	
}

void grafIO::constructGML(grafTagMulti * tag)
{
	
	xml.clear();
	
	xml.addTag("GML");
	//xml.addAttribute("GML","spec","0.1b",0);
	
	xml.pushTag("GML");
	//---------------------
	xml.addTag("tag");
	xml.pushTag("tag");
	
	xml.addTag("header");
	xml.pushTag("header");
		xml.addTag("client");
		xml.pushTag("client");
			xml.setValue("name", clientname);
			xml.setValue("version", version);
			xml.setValue("username", "dustTag");
			xml.setValue("keywords", keywords);
			xml.setValue("uniqueKey", uniquID);
		xml.popTag();
	xml.popTag();
	
	xml.addTag("drawing");
	xml.pushTag("drawing");
		// strokes
		for(int s = 0; s < tag->myStrokes.size(); s++)
		{
			xml.addTag("stroke");
			xml.pushTag("stroke",s);
			for( int i = 0; i < tag->myStrokes[s].pts.size(); i++)
			{
				xml.addTag("pt");
				xml.pushTag("pt",i);
				xml.setValue("x",tag->myStrokes[s].pts[i].pos.x);
				xml.setValue("y",tag->myStrokes[s].pts[i].pos.y);
				xml.setValue("z",tag->myStrokes[s].pts[i].pos.z);
				xml.setValue("time",tag->myStrokes[s].pts[i].time);
				xml.popTag();
			}
			xml.popTag();
		}
	xml.popTag();
	xml.popTag();
	//----------------------
	xml.popTag();
	
	//string xmlString;
	//xml.copyXmlToString(xmlString);
	//cout << xmlString << endl;
	

}
