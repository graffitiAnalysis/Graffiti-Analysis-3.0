/*
 *  grafArchitecture.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 6/5/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "grafArchitecture.h"

GrafArchitecture::GrafArchitecture()
{
bSetup = false;
}

GrafArchitecture::~GrafArchitecture()
{
	
}

void GrafArchitecture::clear()
{
}

void GrafArchitecture::reset()
{
	bMakingParticles	= false;
	bMadeAll			= false;
	bPauseKill			= false;
	totalParticlesMade	= 0;

	for(int i=0; i<boxes.size(); i++)
		boxes[i].destroyShape();
	
	boxes.clear();
}


void GrafArchitecture::setup(int sW, int sH)
{
	screenW = sW;
	screenH = sH;
	floorH = sH;
	box2d.init();
	box2d.setGravity(0,6);//6);
	//box2d.createFloor();
	box2d.checkBounds(true);
	box2d.setFPS(30.0);
	
	bDrawingActive	= false;
	toolType		= TOOL_TYPE_POLY;
	pGroup.setup();
	pGroup.setStrokeColor(255, 255, 255, 255);
	
	bShowArchitecture = false;
	bMakingParticles = false;
	bMadeAll = false;
	bPauseKill = false;
	
	
	bUseTestImage	= true;
	
	offSet.set(0,0);
	offSetPre.set(0,0);
	scale = 1;
	// load save architecture
	
	boxBounce	= .53;
	boxFriction = .31;
	boxMass		= 1;
	
	bSetup = true;
}

void GrafArchitecture::turnOnParticleBoxes(particleSystem * PS)
{
	reset();
	
	bMakingParticles = true;
	
	for( int i = PS->numParticles-1; i >= 0; i--)
		PS->bDrawOn[i] = true;
		
	// create random sort
	keys.clear();
	vector<int> world;
	int magnitude = PS->numParticles-1;
	
 	for(int i=0;i<PS->numParticles;i++){
		world.push_back(i);
		keys.push_back(0);
	}
	
	for(int i=0;i<PS->numParticles;i++){
		int pos = int(ofRandom(0,magnitude));
		keys[i] = world[pos];
		world[pos] = world[magnitude];
		magnitude--;
	}
	
}

void GrafArchitecture::createParticleSet(particleSystem * PS)
{

	
	int countThisSet = 0;
	int numThisTime = ofRandom(PS_AT_A_TIME*.25,PS_AT_A_TIME);
	
	totalParticlesMade = 0;
	
	for( int i = PS->numParticles-1; i >= 0; i--)
	{
		int me = keys[i];
		
		// if this particles is not turned drawing off, ok to make it
		// if off add to total off
		if(PS->col[me][3] == 0 ) PS->bDrawOn[me] = false;
		
		if( PS->bDrawOn[me] && countThisSet < numThisTime && boxes.size() < PS_TOTAL_ALIVE)
		{
			float size = PS->sizes[me];
			GrafArchBox rect;
			rect.setPhysics(boxMass, boxBounce, boxFriction);
			rect.setup(box2d.getWorld(), (screenW/2)+offSetPre.x+( scale*PS->pos[me][0]+scale*offSet.x), (screenH/2)+offSetPre.y+(scale*PS->pos[me][1]+scale*offSet.y), scale*size, scale*size);
			rect.setAlpha(PS->col[me][3]);
			rect.setRandomLifespan(2,10);
			boxes.push_back(rect);
			
			PS->bDrawOn[me] = false;
			PS->col[me][3] = 0;
			
			countThisSet++;
			
		}else if( !PS->bDrawOn[me] ){
			totalParticlesMade++;
		}
		
	}
	
	if( totalParticlesMade >=  PS->numParticles ) 
	{	
		bMadeAll = true;
		//bPauseKill = true;
	}
	
	
	
	
}

void GrafArchitecture::update(float dt)
{
	box2d.update();
	
	//if(bDrawingActive)	pGroup.reEnableLast();
	//else				pGroup.disableAll(true);
	
	
	int sH = floorH;
	//sH=768*2;//ofGetHeight();
	//screenH = sH;
	
	for(int i=boxes.size()-1; i>=0; i--)
	{
		boxes[i].updateAlive(dt);
		
		if(boxes[i].getTimeAlive() > boxes[i].lifeSpanTime && !bPauseKill)
		{
		boxes[i].fadeAlpha(dt);
		}
		
		
		if( boxes[i].getPosition().y > sH || boxes[i].alpha <= 0)
		{
			boxes[i].destroyShape();
			boxes.erase(boxes.begin()+i);
		}
	}
	
	//cout << "total boxes: " << boxes.size() << endl;
}

void GrafArchitecture::draw()
{
	for(int i=0; i<boxes.size(); i++)
	{
		boxes[i].draw();
	}
	
	if(bShowArchitecture)
	{
		for(int i=0; i<drawLines.size(); i++)
		{
			drawLines[i].draw();
		}
	}
	
	
}

void GrafArchitecture::drawTool()
{
	
	ofNoFill();
	ofSetColor(255,255,255,255);
	pGroup.draw();
	
}

void GrafArchitecture::drawTestImage()
{
	float scaleImage = (float)ofGetHeight() /  (float)archImage.height;
	archImage.draw(0,0,scaleImage*archImage.width,scaleImage*archImage.height);
}

//--------------------------------------------------------------
void GrafArchitecture::createArchitectureFromPolys(vector<polySimple>polys)
{
	for( int i = 0; i < drawLines.size(); i++)
		drawLines[i].destroyShape();
	
	drawLines.clear();
	
	for( int i = 0; i < polys.size(); i++)
	{
		//drawLines
		ofxBox2dLine lineStrip;
		lineStrip.setWorld(box2d.getWorld());
		lineStrip.clear();
		for(int j=0; j<polys[i].pts.size(); j++)
		{
			lineStrip.addPoint(polys[i].pts[j].x, polys[i].pts[j].y);
		}
		lineStrip.createShape();
		//lineStrip.setPhysics(0, 0, 1);

		drawLines.push_back(lineStrip);
		drawLines[i].setPhysics(0, 0, .2);
		drawLines[i].edgeDef.density = 0.0f;
		drawLines[i].edgeDef.restitution = 0.0f;
		cout << "line friction " << drawLines[i].friction << endl;
	}
	
}

void GrafArchitecture::saveToXML(string filename)
{
	ofxXmlSettings xml;
	
	xml.addTag("grafArchitecture");
	xml.pushTag("grafArchitecture");
		for( int i = 0; i < pGroup.polys.size(); i++)
		{
			xml.addTag("structure");
			xml.setAttribute("structure","type","line",i);
			xml.setAttribute("structure","bClosed",pGroup.polys[i]->bClosed,i);
			xml.pushTag("structure", i);
				for( int j = 0; j < pGroup.polys[i]->pts.size(); j++)
				{
					xml.addTag("pt");
					xml.setAttribute("pt","x", pGroup.polys[i]->pts[j].x, j);
					xml.setAttribute("pt","y", pGroup.polys[i]->pts[j].y, j);
				}
			xml.popTag();
		}
	xml.popTag();
	
	xml.saveFile(filename);//"settings/architecture.xml");
}

void GrafArchitecture::loadFromXML(string filename)
{
	ofxXmlSettings xml;
	xml.loadFile(filename);//"settings/architecture.xml");
	
	xml.pushTag("grafArchitecture");
	
	int totalPolys = xml.getNumTags("structure");
	
	if(totalPolys > 0 ) pGroup.clear();
	
	for( int i = 0; i < totalPolys; i++)
	{
		pGroup.addPoly();
		
		//xml.getAttribute("structure","type",i);
		//xml.getAttribute("structure","bClosed",i);
		xml.pushTag("structure", i);
		
		int totalPts = xml.getNumTags("pt");
		cout << "num pts " << totalPts << endl;
		
		for( int j = 0; j < totalPts; j++)
		{
			
			float xpt = xml.getAttribute("pt","x", 0,j);
			float ypt = xml.getAttribute("pt","y", 0,j);
			
			pGroup.addPoint( ofPoint(xpt,ypt), false );
		}
		xml.popTag();
	}
	xml.popTag();
	
}


void GrafArchitecture::setPhysicsParams(float mass, float bounce, float friction)
{
	boxBounce = bounce;
	boxMass = mass;
	boxFriction = friction;
	
	for( int i = 0; i < boxes.size(); i++)
	{
	boxes[i].setPhysics(boxMass,boxBounce,boxFriction);
	}
}

	
