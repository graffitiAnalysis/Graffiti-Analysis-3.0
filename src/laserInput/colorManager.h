#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

#define MAX_COLORS 50

class colorManager : public ofBaseDraws{
	public:
		
		colorManager();
		void loadColorSettings(string filePath);
		void buildColorTable();
		void setCurrentColor(int which);
		int getCurrentColor();
		int getTotalNumber();
		void applyColor();
		int getHexColor();
		
		float * getColor3F();
		unsigned char * getColor3I();
		
		unsigned char getRedI();
		unsigned char getGreenI();
		unsigned char getBlueI();
		
		float getRedF();
		float getGreenF();
		float getBlueF();
		
		void drawCurrentColor(int x, int y, int w, int h);
		void drawColorPanel(int x, int y, int w, int h, int num);
		
	protected:
		
		bool 	loaded;
		int  	numColors;
		int 	current;
		
		float 			fRed, fGreen, fBlue;
		unsigned char	r, g, b;
		int				hexColor;
		int				hex[MAX_COLORS];

		unsigned char   charArr[3];
		float			floatArr[3];
		
		ofxXmlSettings xmlSettings;
};
