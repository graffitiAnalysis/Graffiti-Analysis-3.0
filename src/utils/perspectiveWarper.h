/*
 *  perspectiveWarper.h
 *  GA_Interactive
 *
 *  Created by Chris on 6/11/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "cv.h"

class PerspectiveWarper
{

	public:
		PerspectiveWarper();
		~PerspectiveWarper();
		
		/// orig_width and _height are the original width and height (or the target width
		/// and height, if doing inverse transform)
		virtual void initWarp( float orig_width, float orig_height );
		virtual void initWarp( float orig_width, float orig_height,int num_x_sections, int num_y_sections);
		
		/// if set to true, we are warping from a square to a
		/// perspective-compensated rect - eg for squaring a non-aligned beamer
		/// aka 'keystone'
		void setInvert( bool tf ) { invert = tf; }
		bool getInvert() { return invert; }
		
		/// recalculate the warping
		virtual void recalculateWarp();
		virtual void recalculateWarp( float new_points[8] );
		virtual void recalculateWarp( ofPoint src[4], ofPoint dst[4] );
		
		/// shutdown warping
		virtual void shutdownWarp();
		
		/// warp the given point by the current warp matrix
		ofPoint warpPoint( const ofPoint& p );
		ofPoint warpPointInv( const ofPoint& p );
		
		/// draw at given x,y pos
		void drawEditInterface( float x, float y, float scale );
		void drawUV( float x, float y, float scale );
		
		
		void mouseMoved(ofMouseEventArgs& event);
		void mouseDragged(ofMouseEventArgs& event);
		
		
		/// serialise to/from xml
		void saveToXml(string filename );
		void loadFromXml( string filename );
		
		/// dump to console
		void dumpWarpMatrix();
		
		// for texture warping
		float * u;
		float * v;
		int num_x_sections;
		int num_y_sections;
	
		void enableEditing(){ bEditing = true; }
		void disableEditing(){ bEditing = false; }
		bool isEditing(){ return bEditing; }
		int getMouseIndex(){ return mouse_index; }
	
	protected:
	
		void resetPoints();
		void reCalculateUV();
			
		bool invert;
		
		// editing?
		bool bEditing;
		
		float orig_width;
		float orig_height;
		
		ofPoint points[4];
		ofPoint dest_points[4];
		
		// ui
		int mouse_index;
	
		CvMat* warp_translate;
		CvMat* warp_translate_inv;
		CvMat* warp_transform_storage_in;
		CvMat* warp_transform_storage_out;
		
		// for editable interface
		ofPoint editOffset;
		float editScale;
		
};



