/*
 *  guiTypeSpace.h
 *  GA_Interactive
 *
 *  Created by Chris on 8/27/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once


#include "guiBaseObject.h"
#include "guiColor.h"
#include "simpleColor.h"
#include "guiValue.h"

class guiTypeSpace: public guiBaseObject{
	
public:
	
	//------------------------------------------------
	void setup(string sliderName){
		value.addValue(0,0,0);
		name = sliderName;
	}
	
	//-----------------------------------------------.
	void updateGui(float x, float y, bool firstHit, bool isRelative = false){
		bShowText = false;
	}
	
	void updateBoundingBox(){
		//if(bShowText){
			//we need to update out hitArea because the text will have moved the gui down
			//hitArea.y = boundingBox.y + displayText.getTextHeight() + titleSpacing;
			//boundingBox.height = hitArea.height + displayText.getTextHeight() + titleSpacing;
		//}else{
			//we need to update out hitArea because the text will have moved the gui down
			hitArea.y = boundingBox.y;
		//}
	}
	//-----------------------------------------------.
	void render(){
		ofPushStyle();
		glPushMatrix();
		//guiBaseObject::renderText();
		
		//draw the background
		//ofFill();
		//glColor4fv(bgColor.getColorF());
		//ofRect(hitArea.x, hitArea.y, hitArea.width, hitArea.height);
		
		//draw the foreground
		//glColor4fv(fgColor.getColorF());
		//ofRect(hitArea.x, hitArea.y, hitArea.width * value.getPct(), hitArea.height);
		
		//draw the outline
		ofNoFill();
		glColor4f(1,1,1,.25);//v(outlineColor.getColorF());
		ofLine(hitArea.x, hitArea.y-(hitArea.height*.5), hitArea.x+hitArea.width, hitArea.y-(hitArea.height*.5));
		//ofRect(hitArea.x, hitArea.y, hitArea.width, hitArea.height);
		glPopMatrix();
		ofPopStyle();
	}
	
	
};
