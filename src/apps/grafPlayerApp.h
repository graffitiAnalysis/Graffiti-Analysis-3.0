#ifndef _GRAF_PLAYER_APP
#define _GRAF_PLAYER_APP


#include "ofMain.h"
#include "grafTagMulti.h"
#include "grafIO.h"
#include "grafPlayer.h"
#include "grafCurveSmoother.h"
#include "grafDrawer.h"
#include "grafVParticleField.h"

#include "pocoDirectoryLister.h"
#include "ofxControlPanel.h"
#include "ofxXmlSettings.h"
#include "ofThreadedImage.h"
#include "ofxFileDialog.h"
#include "ofxFBOTexture.h"

#include "audioAnalyzer.h"
#include "dropParticles.h"
#include "grafArchitecture.h"
#include "perspectiveWarper.h"

#include "grafThreadedLoader.h"

#define TAG_DIRECTORY	"tags/"
#define WARP_DIV	.125

//#define GA_STACK_SCREENS

enum{ PLAY_MODE_LOAD, PLAY_MODE_PLAY };
enum{ GA_RMODE_NORMAL, GA_RMODE_RB};
enum{ GA_SCREENS_NORMAL,GA_SCREENS_STACKED};

class GrafPlayerApp{

	public:

		GrafPlayerApp();
		~GrafPlayerApp();
		
		void setup();
		void update();
		void draw();
		
		//--- draws
		void drawTagNormal();
		void drawStereoEye(float eyeDist);
		void drawControls();
				
		void keyPressed(ofKeyEventArgs & event);
		void keyReleased(ofKeyEventArgs & event);
		void mouseMoved(ofMouseEventArgs & event );
		void mouseDragged(ofMouseEventArgs & event );
		void mousePressed(ofMouseEventArgs & event );
		void mouseReleased(ofMouseEventArgs & event );
	
		void loadScreenSettings();
		
		//---------- audio analysis
		AudioAnalyzer	audio;
		
		bool            bShiftOn;
		bool			bUseAudio;
		bool			bUseArchitecture;
		bool			bUseRedBlue;
		bool			bSetup;

	protected:

		//----- updates
		void updateParticles();
		void updateTransition( int type);
		void updateAudio();
		void updateArchitecture();
		
		//---- tag loading
		void preLoadTags();
		void loadTags();
		void saveTagPositions();
	
		//---- tag playback management
		void resetPlayer(int next);
		void nextTag(int dir=1);
		void clearAll();
	
		//--- control panel, interface etc
		void setupControlPanel();
		void updateControlPanel(bool bUpdateAll = false);
		string getCurrentTagName();
	
			
		
		//---------- FBO + Warping
		ofxFBOTexture		fbo;
		PerspectiveWarper	pWarper;
		
		//---------- FBO Red Blue Version
		ofxFBOTexture		fboLeft;
		ofxFBOTexture		fboRight;
		
		//---------- interactive architecture
		GrafArchitecture		archPhysics;		// physics / architecture effects
		void createWarpedArchitecture();
		vector<polySimple> wPolys;

		//---------- audio particle effects
		DropParticles	drops;
		
		//---------- loading data
		GaThreadedLoader		threadedLoader;
		grafIO					gIO;					// gml loader/saver
		ofxPocoDirectoryLister	dirLister;				// searches directory for gml files
		string					myTagDirectory;			// directory to load from (changeable in data/settings/directorySettings.xml
		int						totalToLoad;			// totalFiles found to load
		vector<string>			filesToLoad;			// list of all file paths
		vector<string>			filenames;				// list of corresponding file names (sans .gml)
	
		//---------- gml tags
		vector<grafTagMulti> tags;						// vector of all loaded tags
		int currentTagID;								// id of the current tag being played 

		//---------- player to animate the tag 
		grafPlayer	myTagPlayer;						// manages playback of tag in time
		
		//---------- smooths curve for nice drawing
		grafCurveSmoother smoother;						// adds points to smooth tag
	
		//---------- drawing
		grafDrawer			drawer;						// draws thick time stroked line
		grafVParticleField	particleDrawer;				// draws and animates particles
		float				rotationY;					// y rotation for current tag
		ofPoint				tagPosVel;					// vel for moving tag around on screen
		float				tagMoveForce;				// force for moving tag around
		int					prevStroke;					// for multi stroke tags, to help particle animation
		float				fogStart,fogEnd;			// fog settings
		float				rotFixTime;					// timer for making rotation move to particular position in a fixed time frame
		ofTrueTypeFont		fontS;						// fonts for drawing info to screen
		ofTrueTypeFont		fontL;
		ofTrueTypeFont		fontSS;
	
			
		//---------- application settings
		int						screenW,screenH;	
		float					lastX,lastY;
		ofxThreadedImageSaver	imgsaver;
		ofImage					imageMask;
		float					dt;					// difference time
		float					waitTimer;			// counter to pause after animation finished
		float					waitTime;			// time to pause
	

		//---------- controls
		ofxControlPanel		panel;
		bool				bShowPanel;			// toggle panel on/off
		bool				bRotating;			// toggle auto rotation on/off
		bool				bShowName;			// toggle filename displa
		bool				bShowTime;			// toggle time display
		bool				bUseFog;			// toggle fog
		bool				bUseMask;			// toggle edge masking
		bool				bTakeScreenShot;	
		bool				bUseGravity;

		
		int					mode;				// current play mode PLAY_MODE_LOAD for loading, PLAY_MODE_PLAY for normal playback
	
		// files opening for audio
		ofxFileDialog dialog;
		vector <string> resultString;
	
		// path to files
		string pathToSettings;
		
		
		//------------------------ alternate draw modes
		int modeRender;			// GA_RMODE_NORMAL, GA_RMODE_RB (red-blue stereo)
		int modeDualScreen;		// GA_SCREENS_NORMAL (1 screen), GA_SCREENS_STACKED (1 on top other), GA_SCREENS_VJ (mirrored fbo on 2nd w/o panels)
		bool bDualScreenLaptop; // if true assumes a 3 screen dualhead2go setup so all is rendered at a screenW offset
		
};


/*---------------------- Guide to controls

App Settings (1)
---------------------------
Use Audio			- toggle audio effects on/off
Use Architecture	- toggle interactive architecture effects on/off
Play / Pause		- toggle play of animation of tag and rotation (other effects continue)
Fullscreen			- toggle fullscreen
Rotate				- toggle continuous rotation on y-axis
Rotation Speed		- adjusts speed of above rotation
Display filename	- toggle display of name from gml file
Display time		- toggle display of playback time
Save Tag Position/Rotation - saves any changes to position and rotaiton of tag to the current tag gml. Does not alter point data at alll

Draw Settings (2)
---------------------------
Line Alpha			- alpha value of thick line
Outline Width		- stroke weight of outline
LineScale			- adjust thickness of line
Particle Size		- adjust size of particles (note: changes with audio effects)
Particle Damping	- adjust speed of particles
Number Particles	- adjust number of particle sets used (?disabled)
Use gravity			- toggle gravity effect for transition (does not apply with audio/arch effects)
Use edge mask		- fades out edges of projection area
Use  fog			- toggle fog effects

Audio Settings (3)
---------------------------
Outward amp force	- adjust forces that push particles outward from their resting position
Particle size force	- adjust forces that change the size of particles w/audio
Wave deform force	- adjust forces that deform the line w/fft vals
Bounce force		- adjust forces that make tag bounce with levels
Change wait time	- adjust time to wait before changing to next tag
Drop p threshold	- adjust threshold of amount above average levels that creates a big particle that falls

(other toggles turn these effects on/off)

Architecture Drawing (4)
---------------------------
In this part architectural elements can be sketched by mouse click. 
When finished you must hit "done" to make them physics objects.

left - click : add a new point (also selects closest drawing if > 1 )
right - drag : move all points (might be a bug that only works > 2 points)
left - shift click/drag: select and drag any point individually
TAB : select next point on drawn structure
RETURN: start new drawing/structure
DELETE: remove last point

new strucutre	- start new drawing/structure
done			- converts drawings to objects that can interact with particles
save			- saves drawing to xml file
load			- loads last saved drawings
clear			- clears everything

 Architecture Settings (5)
 ---------------------------
 show architecture		- toggle drawn architecture elements ( only those converted to physics objects)
 show image				- toggle test image on/off
 mass					- adjust mass of particles
 bounce					- adjust bounce forces of particles
 friction				- adjust friction forces of particles
 gravity				- adjust world gravity
 
 FBO Warper (6)
 ---------------------------
 Load Warping			- loads saved warper
 Save Warping			- saves current warper setup
 Reset Warping			- resets to screen width/height
 Toggle Preview			- toggles pic-in-pic style interface
 
 
 Other controls:
 
 (note that these apply when not in architecture drawing panel)
 x: toggle control panel
 p: pause/play animation
 s: save screenshot
 m: toggle cursor
 f: toggle fullscreen
 R: resets tag to default position/rotation
 arrows (r/l) : advance to next or previous tag
 
 left-mouse: position tag
 right-mouse: rotate tag
 left-shift mouse: zoom tag
 
 F3: setup for recording video
 F4: toggle video recording
 
 (video saved in data>output>video)
 
Projects:
---------------------------
all data, tags and settings are loaded from the current "user" folder that can be changed in 
bin > data > projects > user.xml

default is "default" :)

to make a new project, copy and paste default folder, rename it, and change name to match in user.xml


Screen dimensions can be changed in screenSettings.xml, found in settings of current project folder
data > projects > default > settings > screenSettings.xml
 
-------------------------*/

#endif
