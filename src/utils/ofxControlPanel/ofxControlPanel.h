#pragma once


#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "guiIncludes.h"

//hehe - there is no xml yet!!!
//but we are setting things up for XML

#define borderWidth  10.f
#define topSpacing   20.f
#define tabWidth	25.f
#define tabHeight	10.f

class xmlAssociation{
    public:
        xmlAssociation(guiBaseObject * objPtr, string xmlNameIn, int numParamsIn){
            guiObj      = objPtr;
            xmlName     = xmlNameIn;
            numParams   = numParamsIn;
        }

        guiBaseObject * guiObj;
        string xmlName;
        int numParams;
};

class ofxControlPanel: public guiBaseObject{

	public:

        ofxControlPanel();
        ~ofxControlPanel();

        void setup(string controlPanelName, float panelX, float panelY, float width, float height, bool bUseMyFont= false);
        void loadFont( string fontName, int fontsize );

        guiTypePanel * addPanel(string panelName, int numColumns, bool locked = false);

        void setWhichPanel(int whichPanel);
        void setWhichPanel(string panelName);
        void setWhichColumn(int column);
	
	
		int getSelectedPanel(){  return selectedPanel; };
		string getSelectedPanelName(){	
			if (panels.size() > 0 && selectedPanel < panels.size()) return panels[selectedPanel]->name;
			else return "";
		};
		void setSelectedPanel( string panelName );

        void setSliderWidth(int width);

        guiTypeToggle * addToggle(string name, string xmlName, bool defaultValue);
        guiTypeMultiToggle * addMultiToggle(string name, string xmlName, int defaultBox, vector <string> boxNames);
        guiTypeSlider * addSlider(string sliderName, string xmlName, float value , float min, float max, bool isInt);
        guiType2DSlider * addSlider2D(string sliderName, string xmlName, float valueX, float valueY, float minX, float maxX, float minY, float maxY, bool isInt);
        guiTypeDrawable * addDrawableRect(string name, ofBaseDraws * drawablePtr, int drawW, int drawH);
        guiTypeCustom * addCustomRect(string name, guiCustomImpl * customPtr, int drawW, int drawH);
        guiTypeLogger * addLogger(string name, simpleLogger * logger, int drawW, int drawH);
        guiTypeFileLister * addFileLister(string name, simpleFileLister * lister, int drawW, int drawH);
		guiTypeTextInput * addTextInput(string name, string xmlName, string value, int maxX, int maxY );

        void setValueB(string xmlName, bool value,  int whichParam = 0);
        void setValueI(string xmlName, int value,  int whichParam = 0);
        void setValueF(string xmlName, float value,  int whichParam = 0);
		void setValueS( string xmlName, string value, int whichParam = 0);
        bool getValueB(string xmlName, int whichParam = 0);
        float getValueF(string xmlName, int whichParam = 0);
        int getValueI(string xmlName, int whichParam = 0);
		string getValueS(string xmlName, int whichParam = 0);
		
		void addChar(char key, int whichParam = 0);
		void deleteLastChar( int whichParam = 0);
	
        void setIncrementSave(string incrmentalFileBaseName);
        void disableIncrementSave();
        void loadSettings(string xmlFile);
        void reloadSettings();
        void saveSettings(string xmlFile);
        void saveSettings();

        void setDraggable(bool bDrag);
        void setMinimized(bool bMinimize);
        void show();
        void hide();

        void toggleView();
        void mousePressed(float x, float y, int button);
        void mouseDragged(float x, float y, int button);
        void mouseReleased();

        void updateBoundingBox();
        void update();
        void draw();
	
		bool isAnyTextBoxActive();
		bool isMouseInPanel(int x, int y);


        ofTrueTypeFont guiTTFFont;

        vector <xmlAssociation> xmlObjects;
        vector <guiBaseObject *> guiObjects;
        vector <guiTypePanel *> panels;
        vector <ofRectangle> panelTabs;

		ofxXmlSettings settings;
		string currentXmlFile;
		string settingsDirectory;

		ofRectangle topBar;
		ofRectangle minimizeButton;
		ofRectangle saveButton;
		ofRectangle restoreButton;

        string incrementSaveName;

        bool hidden;
        bool usingXml;
        bool bUseTTFFont;
		bool minimize;
		bool saveDown;
		bool incrementSave;
		bool restoreDown;
		bool bDraggable;
		bool bMouseInPanel;
		bool bNewPanelSelected;

		int selectedPanel;
        int currentPanel;

        ofPoint prevMouse;

		int sliderWidth;


		ofPoint mouseDownPoint;

		bool dragging;

};


