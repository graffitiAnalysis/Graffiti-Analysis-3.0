#include "colorManager.h"

static long ConvertHexStrToLong( char* s )   
{ 
    int hexDigit[] = { 10, 11, 12, 13, 14, 15 };
    long n = 0;
    char c = *s++;

    while( c != '\0' )
    {
        if( c >= '0' && c <= '9' )
            n = n * 16 + (c - '0');
        else
        {
            c = tolower( c );
            n = n * 16 + hexDigit[(c - 'a')];
        }

        c = *s++;
    }

    return n;
}

//----------------------------------------------------
colorManager::colorManager(){
	loaded = false;
	numColors = 0;
	current = 0;
	
	xmlSettings.setVerbose(false);
}

//----------------------------------------------------
void colorManager::loadColorSettings(string filePath){
	filePath = ofToDataPath(filePath);
	
	xmlSettings.loadFile(filePath);
	buildColorTable();	
}

//----------------------------------------------------
void colorManager::buildColorTable(){
	
	string str = "COLOR";
	string hexStr = "";
	for(int i = 0; i < MAX_COLORS; i++){
		hexStr = xmlSettings.getValue(str+ofToString(i)+":HEX", "NONE");
		if(hexStr == "NONE"){
			continue;
		}
		
		hex[numColors] = (int)ConvertHexStrToLong((char *)hexStr.c_str());
		numColors++;
	}
	
	if(numColors > 0)loaded = true;
}

//----------------------------------------------------
void colorManager::setCurrentColor(int which){
	if(which < 0)which = 0;
	if(which >= numColors)which = numColors;
	current = which;
	
	int value = hex[current];
	
	int r = (value >> 16) & 0xff;
	int g = (value >> 8) & 0xff;
	int b = (value >> 0) & 0xff;
	
	charArr[0] = r;
	charArr[1] = g;
	charArr[2] = b;
	
	floatArr[0] = (float)r / 255;
	floatArr[1] = (float)g / 255;
	floatArr[2] = (float)b / 255;
	
}

//----------------------------------------------------
int colorManager::getCurrentColor(){
	return current;
}

//----------------------------------------------------
int colorManager::getHexColor(){
	if(!loaded)return 0xFFFFFF;
	return hex[current];
}

//----------------------------------------------------
int colorManager::getTotalNumber(){
	return numColors;
}

//----------------------------------------------------
void colorManager::applyColor(){
	if(!loaded)return;
	ofSetColor(	hex[current] );
}

//----------------------------------------------------
float * colorManager::getColor3F(){
	return floatArr;
}

//----------------------------------------------------
unsigned char * colorManager::getColor3I(){
	return charArr;
}

//----------------------------------------------------
unsigned char colorManager::getRedI(){
	return charArr[0];
}

//----------------------------------------------------
unsigned char colorManager::getGreenI(){
	return charArr[1];
}		

//----------------------------------------------------
unsigned char colorManager::getBlueI(){
	return charArr[2];
}

//----------------------------------------------------
float colorManager::getRedF(){
	return floatArr[0];
}

//----------------------------------------------------
float colorManager::getGreenF(){
	return floatArr[1];
}

//----------------------------------------------------
float colorManager::getBlueF(){
	return floatArr[2];
}

//----------------------------------------------------
void colorManager::drawCurrentColor(int x, int y, int w, int h){
	applyColor();
	ofFill();
	ofRect(x,y,w,h);

	ofSetColor(0xFFFFFF);
	ofNoFill();
	ofRect(x,y,w,h);
	ofFill();
}

//----------------------------------------------------
void colorManager::drawColorPanel(int x, int y, int w, int h, int num){
				
	if(num < 2 ) num = 2;
	
	float shift = 0;
	float step 	= (float)w / (float)num;
	
	for(int i = 0; i < num; i++){
		int pos = current + i;
		if( pos < 0 || pos >= numColors ) continue;
		
		if(pos == current) ofSetColor(0x777777);
		else ofSetColor(0x00000);
		ofFill();
		ofRect(x + shift,y,step,h);
		
		ofSetColor(	hex[pos] );

		ofFill();
		ofRect( 4 + x + shift, 4 + y,step - 8,h - 8);
										
		shift += step;
	}

}
	