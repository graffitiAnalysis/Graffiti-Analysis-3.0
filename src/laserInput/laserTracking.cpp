#include "laserTracking.h"

//---------------------------
static int pnpoly (int npol, float *xp, float *yp, float x, float y){
   int i, j, c = 0;
   for (i = 0, j = npol-1; i < npol; j = i++) {
     if ((((yp[i]<=y) && (y < yp[j])) ||
          ((yp[j]<=y) && (y < yp[i]))) &&
           (x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
       c = !c;
   }
   return c;
 }

//---------------------------
laserTracking::laserTracking(){

	bCameraSetup 	= false;
	bVideoSetup  	= false;
	newStroke		= true;
	shouldClear		= false;
	bStartedInClearZone = false;
	
	width			= 320.0f;
	height			= 480.0f;
	
	noLaserCounter 	= 0;
	distDifference  = 0.0;
	smoothPos.x 	= 0;
	smoothPos.y 	= 0;
	smoothVel.x 	= 0;
	smoothVel.y 	= 0;
	distX			= 0;
	distY			= 0;
	oldX			= 0;
	oldY			= 0;
	clearThresh		= 6;
	pre				= NULL;
	bSetup			= false;
	bCameraSetup	= false;
	bVideoSetup		= false;
}

//lower threshold means more senstive clear zone
//---------------------------
void laserTracking::setClearThreshold(float clearSens){
	clearThresh = clearSens;
}

//
//---------------------------
void laserTracking::setClearZone(vector <ofPoint> pts){
	for(int k = 0; k < clearZone.points.size(); k++){
		clearZone.points[k].x = pts[k].x;
		clearZone.points[k].y = pts[k].y;
	}
}

//
//---------------------------
void laserTracking::setUseClearZone(bool useClearZone){
	clearZone.setActive(useClearZone);
}

//---------------------------
bool laserTracking::isClearZoneHit(){
	if( shouldClear ){
		shouldClear = false;
		return true;
	}
	return false;
}
		
//changing cameras or switching from/to the camera mode
//requires the app to be restarted - mabe we can change this?
//---------------------------		
void laserTracking::setupCamera(int deviceNumber, int width, int height){
	clearData();
	
	VG.setDeviceID(deviceNumber);
	VG.initGrabber(width, height);
	W				= VG.width;
	H				= VG.height;		
		
	bCameraSetup	= true;

	noLaserCounter  = 0;
	distDifference  = 0.0;
	setupCV("settings/cameraQuad.xml");	
	
	bSetup = true;
}

//same as above - from/to using test movies
//requires a restart
//---------------------------		
void laserTracking::setupVideo(string videoPath){
	clearData();

	VP.loadMovie(videoPath);
	VP.play(); 
	VP.setUseTexture(true);
	W				= VP.width;
	H				= VP.height;
	
	bVideoSetup		= true;
	
	noLaserCounter	= 0;
	distDifference  = 0.0;
	setupCV("settings/cameraQuad.xml");
	
	bSetup = true;
}

//good for adjusting the color balance, brightness etc
//---------------------------		
void laserTracking::openCameraSettings(){
	VG.videoSettings();			
}

//---------------------------
void laserTracking::clearData(){
	
	if( pre != NULL ){
		delete [] pre;
		pre = NULL;
		VideoFrame.clear();
		WarpedFrame.clear();
		PresenceFrame.clear();
		hsvFrame.clear();	
	}
	
	bSetup = false;
	bCameraSetup	= false;
	bVideoSetup		= false;	
	
	VG.close();
}

//do all our openCV allocation
//and loads our quad settings from xml
//---------------------------		
void laserTracking::setupCV(string filePath){
				
	//our openCV inits
	VideoFrame.allocate(W, H);
	WarpedFrame.allocate(W, H);
	PresenceFrame.allocate(W, H);
	hsvFrame.allocate(W, H);
				
	//we do all our tracking at 320 240 - regardless 
	//of camera size - larger cameras get scaled down to
	//these dimensions - otherwise 'shit would be slow'
	pre	= new unsigned char[W * H * 3];
	
	//this is so we can select a sub region of the 
	//camera image and warp it to full 320 by 240
	warpSrc[0].x = 0;
	warpSrc[0].y = 0;
	
	warpSrc[1].x = 1.0;
	warpSrc[1].y = 0;
	
	warpSrc[2].x = 1.0;
	warpSrc[2].y = 1.0;

	warpSrc[3].x = 0;
	warpSrc[3].y = 1.0;
	
	//Now we update out quad with the default
	//values and if then if the file is found
	//we overwrite with values from xml file
	QUAD.setup("QUAD_SRC_");
	QUAD.setQuadPoints(warpSrc);
	QUAD.readFromFile(filePath);
	
	//warp dst will always be to 320 by 240
	//unless you want shit slow!
	warpDst[0].x = 0;
	warpDst[0].y = 0;
	
	warpDst[1].x = W;
	warpDst[1].y = 0;
	
	warpDst[2].x = W;
	warpDst[2].y = H;

	warpDst[3].x = 0;
	warpDst[3].y = H;	
}

//the heart of the beast - where we process 
//the incoming frame and look for a laser
//---------------------------		
void laserTracking::processFrame(float hue, float hueThresh, float sat, float value, int minSize, int deadCount, float jumpDist, bool slowButAccurateQuad){
	if( !bSetup ){
		printf("we are not setup! aborting!\n");
		return;
	}
	
	int t1, t2, t3, t4, t5;
	
	accurateQuad = slowButAccurateQuad;
	
	trackerPanel.setHsv(hue, hueThresh, sat, value);
		
	///////////////////////////////////////////////////////////
	// Part 1 - get the video data
	///////////////////////////////////////////////////////////
	
	//pointer to our incoming video pixels			
	unsigned char * pixCam;
	
	//either grab pixels from video or grab from camera
	if(bVideoSetup){
		VP.idleMovie();	
		pixCam 	= VP.getPixels();
	}else{
		VG.grabFrame();
		pixCam 	= VG.getPixels();		
	}
	
	///////////////////////////////////////////////////////////
	// Part 2 - warp the video based on our quad
	///////////////////////////////////////////////////////////
	
	//add to openCV and warp to our dst image
	VideoFrame.setFromPixels(pixCam, W, H);
	
	if(accurateQuad){
		WarpedFrame.warpIntoMe(VideoFrame, QUAD.getScaledQuadPoints(W,H), warpDst);
	}
	
	
	//WarpedFrame.wao hsv and search for matching pixels
	////////////////////////////////////////////////////////////
				
	//Get pixels and convert to hue sat and value;
	hsvFrame = VideoFrame;
	hsvFrame.convertRgbToHsv();
	
	//okay time to look for our laser!
	//based on hue sat and val
	int totalPixels = W*H*3;
	
	unsigned char * pix = hsvFrame.getPixels();
				
	float h 		= hue 		* 255;
	float ht		= hueThresh * 255; 
	float s 		= sat 		* 255;
	float v 		= value 	* 255;
	
	//here we figure out what is the max allowed hue 
	//and the minimum allowed hue. because hue is 
	//continious we have to make sure we handle what happens 
	//if hueMax goes over 255
	//or hueMin goes less than 0
					
	float hueMax	= h + ht*0.5;
	float hueMin    = h - ht*0.5;
		
	int k = 0;
	int x = 0;
	int y = 0;

	//our clear zone stuff
	//back to being able to use any area of the camera
	//as the clear zone - even if it is outide of the quad
	
//	int   clearCount 	= 0;
//	bool  clearActive 	= clearZone.getActive();
//	float clearXMin 	= clearZone.points[0].x * (float)W;
//	float clearYMin 	= clearZone.points[0].y * (float)H;
//	float clearXMax 	= clearZone.points[1].x * (float)W;
//	float clearYMax 	= clearZone.points[2].y * (float)H;
//	
	
	if(accurateQuad){

//		
//		int i, stride;
//		for(int y = clearYMin; y < clearYMax; y++){
//			
//			stride = y*W*3;
//			for(int x = clearXMin; x < clearXMax; x++){
//
//				i = stride + x*3;
//				
//				if( pix[i+1] >= s  && pix[i+2] >= v){
//
//					 //we do this to check the cases when the
//					 //hue min could have wrapped
//					 //or the hue max could have wrapped
//					 //also if saturation is zero
//					 //then hue doesn't matter hence (s == 0)         
//					  float pixHue = pix[i];
//
//					 if( (s == 0) || (pixHue >= hueMin && pixHue <= hueMax) ||
//					    (pixHue -255 >= hueMin && pixHue -255 <= hueMax) ||
//					    (pixHue +255 >= hueMin && pixHue +255 <= hueMax) ){
//					   
//					   
////						if( clearActive && x > clearXMin && x < clearXMax){				   
////							if( y > clearYMin && y < clearYMax){
////								clearCount++;
////							}
////						}
//					}
//				}
//			}
//		}
		
							
		//Get pixels and convert to hue sat and value;
		hsvFrame = WarpedFrame;
		hsvFrame.convertRgbToHsv();
		pix = hsvFrame.getPixels();
		
		k = 0;
						
		for(int i = 0; i < totalPixels; i+=3){
		 
		  float pixHue = pix[i];
		           
		  if( pix[i+1] >= s  && pix[i+2] >= v){
		 
		     //we do this to check the cases when the
		     //hue min could have wrapped
		     //or the hue max could have wrapped
		     //also if saturation is zero
		     //then hue doesn't matter hence (s == 0)               
		     if( (s == 0) || (pixHue >= hueMin && pixHue <= hueMax) ||
				(pixHue -255 >= hueMin && pixHue -255 <= hueMax) ||
				(pixHue +255 >= hueMin && pixHue +255 <= hueMax) ){
		       
		        //we have a white pixel
		        pre[k] = 255;
		     }
		     else pre[k] = 0;
		  }else pre[k] = 0;
		           
		  k++;
		} 	
		
		
	}else{
	
		ofxPoint2f * pts =  QUAD.getScaledQuadPoints(W, H);
	
		float ptsx[4] = {pts[0].x, pts[1].x, pts[2].x, pts[3].x};
		float ptsy[4] = {pts[0].y, pts[1].y, pts[2].y, pts[3].y};
		
		k = 0;
		x = 0;
		y = 0;
	
		for(int i = 0; i < totalPixels; i+=3){

			if( x >= W){
				x = 0;
				y++;
			}
			
			if( pix[i+1] >= s  && pix[i+2] >= v){

			 //we do this to check the cases when the
			 //hue min could have wrapped
			 //or the hue max could have wrapped
			 //also if saturation is zero
			 //then hue doesn't matter hence (s == 0)         
			  float pixHue = pix[i];

			 if( (s == 0) || (pixHue >= hueMin && pixHue <= hueMax) ||
			    (pixHue -255 >= hueMin && pixHue -255 <= hueMax) ||
			    (pixHue +255 >= hueMin && pixHue +255 <= hueMax) ){
			   
			   
//			   if( clearActive && x > clearXMin && x < clearXMax){				   
//			   		if( y > clearYMin && y < clearYMax){
//			   			clearCount++;
//			   		}
//			   }
			   
			   //if we are not in the polygon
				//don't draw
									
				if( !pnpoly(4, ptsx, ptsy, x, y) ){
					pre[k] = 0;
					x++;
					k++;
					continue;
				}
								   
			    //we have a white pixel
			    pre[k] = 255;
			 }
			 else pre[k] = 0;
			}else pre[k] = 0;
			       
			x++;
			k++;
		}
		
	} 			
	
//	if(clearActive && clearCount >= clearThresh){
//		shouldClear = true;
//	}
	
	
	///////////////////////////////////////////////////////////
	// Part 4 - find the largest blob of possible candidates 
	////////////////////////////////////////////////////////////
					
	//lets find blobs!
	PresenceFrame.setFromPixels(pre, W, H);
	
	//max blob size - our laser shouldn't be bigger
	//than 20 pixels by 20 pixels right?
	//so lets make our max blob rpIntoMe(VideoFrame, QUAD.getScaledQuadPoints(W,H), warpDst);


	///////////////////////////////////////////////////////////
	// Part 3 - convert tsize 400

	int maxSize = 999999999;		
	Contour.findContours(PresenceFrame, minSize, maxSize, 50, false);  
	

	///////////////////////////////////////////////////////////
	// Part 5 - finally calculate our laser coordinates
	////////////////////////////////////////////////////////////
								
	//okay so we found some blobs that matched our criteria
	//we are only really interested in the largest one
	if(Contour.nBlobs > 0){
		
		//get the center pos of the largest blob in 0 - 1 range
		float tmpX		= Contour.blobs[0].centroid.x/(float)W;
		float tmpY		= Contour.blobs[0].centroid.y/(float)H;
		
		ofxPoint2f dst[4];
		
		dst[0].x = 0;
		dst[0].y = 0;

		dst[1].x = 1;
		dst[1].y = 0;

		dst[2].x = 1;
		dst[2].y = 1;
		
		dst[3].x = 0;
		dst[3].y = 1;

		if(!accurateQuad){
		
			ofxPoint2f *  src = QUAD.getQuadPoints();

			CW.calculateMatrix(src, dst);
			ofxPoint2f out = CW.transform(tmpX, tmpY);
			
			tmpX = out.x;
			tmpY = out.y;
			
		}
					
		//calculate the horizontal and vertical distance 
		//between the last point
		oldX			= laserX;
		oldY			= laserY;
		
		distX			= tmpX - laserX;
		distY			= tmpY - laserY;
		
		if(distX == 0 && distY == 0)newPos = false;
		else newPos = true;

		distDifference  = sqrt( pow(distX, 2) + pow(distY, 2) );
		
		//now update our laser position with this new position
		laserX 			= tmpX;
		laserY			= tmpY;
		
		// we need to store if a new stroke has occured
		// this is so we can no when to start a new line 
		if(!newStroke) newStroke = (noLaserCounter >= deadCount) ||  (distDifference >= jumpDist);
		noLaserCounter 	= 0;
		
	}else{
		//we are waiting for a laser!
		newPos = false;
		noLaserCounter++;
			
		distX			= 0;
		distY			= 0;			
	}
	
	
	///////////////////////////////////////////////////////////
	// Part 6 - calculate related variables, smoothing etc
	////////////////////////////////////////////////////////////

	if(newStroke){
	
		oldX		= laserX;
		oldY		= laserY;
		smoothPos.x = laserX;
		smoothPos.y = laserY;
		distX		= 0;
		distY		= 0;
		smoothVel.x = 0;
		smoothVel.y = 0;
		
	}
	
	if( newStroke && newPos ){
		if( clearZone.isHit(laserX, laserY) ){
			bStartedInClearZone = true;
		}else{
			bStartedInClearZone = false;
		}
	}
		
	if( bStartedInClearZone ){
		newPos = false;
	}
	
} 

//if you want to warp the coords to another quad
//-----------------------------------------------------------------------
void laserTracking::getWarpedCoordinates(ofxPoint2f * dst, float *warpedX, float *warpedY){
		
	ofxPoint2f srcTmp[4];
		
		srcTmp[0].x = 0;
		srcTmp[0].y = 0;

		srcTmp[1].x = 1;
		srcTmp[1].y = 0;

		srcTmp[2].x = 1;
		srcTmp[2].y = 1;
		
		srcTmp[3].x = 0;
		srcTmp[3].y = 1;
		

	CW.calculateMatrix(srcTmp, dst);
	ofxPoint2f out = CW.transform(laserX, laserY);
	
	*warpedX = out.x;
	*warpedY = out.y;
}


//---------------------------
bool laserTracking::newData(){		
	return newPos;
}

//---------------------------
void laserTracking::clearNewStroke(){
	newStroke = false;
}

//---------------------------
bool laserTracking::isStrokeNew(){
	return newStroke;
}


//this formats the x and y coordinates
//to a string that can be sent to other apps
//over tcp and udp we will see both regular
//and transformed coordinates
//---------------------------		
string laserTracking::getLaserPointString(){
	
	if( newData() ){
		sendStr = ofToString(laserX);
		sendStr += "!"+ ofToString(laserY);
		//printf("sending %s \n", sendStr.c_str());
	}
	
	return sendStr;
}

//---------------------------				
void laserTracking::drawPanels(float x, float y){
	drawPanels(x, y, 600, 150);
}

//---------------------------				
void laserTracking::drawPanels(float x, float y, float w, float h){
	if( !bSetup ){
		return;
	}
	
	ofPushStyle();
	
	ofSetColor(255, 255, 255);
	
	//lets draw our openCV frames
	//with a nice border around them
	VideoFrame.draw(x, y, w/3, h);
	WarpedFrame.draw(x+w/3, y, w/3, h);
	PresenceFrame.draw(x+(w/3)*2, y, w/3, h);
	
	ofNoFill();
	ofRect(x, y, w/3, h);
	ofRect(x+w/3, y, w/3, h);
	ofRect(x+(w/3)*2, y, w/3, h);
	
	drawClearZone(x+w/3, y, w/3, h);
	
	//lets draw our camera quad
	QUAD.draw(x, y, w/3, h);

	ofPopStyle();
}

//---------------------------				
void laserTracking::draw(float x, float y, float w, float h){
	if( !bSetup ){
		return;
	}
	
	trackerPanel.draw(x,y,w,h);
}


//---------------------------		
void laserTracking::drawColorRange(float x, float y, float w, float h){

	trackerPanel.draw(x,y,w, h);

}

//---------------------------
void laserTracking::drawClearZone(float x, float y, float w, float h){
	ofPushStyle();
	
	ofSetLineWidth(2.0);
	ofEnableAlphaBlending();
	

	if( clearZone.bState ){
		ofFill();
		ofSetColor(255, 0, 0, ofClamp(ofMap(sin(ofGetElapsedTimef() * 4.7), -1, 1, -100, 400), 0, 255) );	

		ofBeginShape();
			for(int i = 0; i < clearZone.points.size(); i++){
				ofVertex(x + clearZone.points[i].x*w, y + clearZone.points[i].y*h);
			}
		ofEndShape(true);			
	}	
	
	glLineStipple(1, 0x00FF);
	glEnable(GL_LINE_STIPPLE);

	ofNoFill();
	ofSetColor(255, 255, 255, 255);		
	ofBeginShape();
		for(int i = 0; i < clearZone.points.size(); i++){
			ofVertex(x + clearZone.points[i].x*w, y + clearZone.points[i].y*h);
		}
	ofEndShape(true);

	glDisable(GL_LINE_STIPPLE);

	ofFill();
	
	ofPopStyle();
}

//---------------------------		
void laserTracking::drawQuadSetupImage(float x, float y, float w, float h){
	ofSetColor(0xFFFFFF);	
	ofRect(0,0,w, h);
	VideoFrame.draw(16, 12, w-32, h-24);
	ofSetColor(0xFFFF00);	
	QUAD.draw(16, 12, w-32, h-24);
}
