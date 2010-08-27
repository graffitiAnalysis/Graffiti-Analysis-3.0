#pragma once

#include "guiBaseObject.h"
#include "guiTypeText.h"
#include "guiColor.h"
#include "simpleColor.h"
#include "guiValue.h"

class guiTypeText : public guiBaseObject{

    public:
	
	//------------------------------------------------
	void setup(string text){
		value.addValue(0,0,0);
		name = text;
	}
	
	//-----------------------------------------------.
	void updateBoundingBox(){
	
		if(bShowText){
			//we need to update out hitArea because the text will have moved the gui down
			hitArea.y = boundingBox.y + displayText.getTextHeight() + titleSpacing;
			boundingBox.height = hitArea.height + displayText.getTextHeight() + titleSpacing;
		}else{
			//we need to update out hitArea because the text will have moved the gui down
			hitArea.y = boundingBox.y;
		}
	}
	
	//-----------------------------------------------.
	void render(){
	
		ofPushStyle();
		glPushMatrix();
		
			guiBaseObject::renderText();
		
		glPopMatrix();
		ofPopStyle();
	}
	
	
};

