/*
 *  gaManagerApp.h
 *  GA_Interactive
 *
 *  Created by Chris on 8/22/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

#define	GA_MODE_RECORDER	0
#define GA_MODE_PLAYER		1
#define GA_MODE_LASER		2
#define GA_MODE_HELP		3
#define GA_MODE_START		4

class GaManagerApp{

	public:
		GaManagerApp();
		~GaManagerApp();
		
		void setup();
		void update();
		void draw();
		
		void setMode(int newMode){ mode = newMode; }
		int getMode(){ return mode; }
	
		int mode;
		
	protected:

};