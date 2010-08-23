#pragma once

#include "ofMain.h"
#include "ofxFft.h"
//#include "aubioAnalyzer.h"
#include "statsRecorder.h"
#include "ofxControlPanel.h"

#define NUM_BANDS 256


class AudioAnalyzer{

	public:

		AudioAnalyzer();
		~AudioAnalyzer();
		
		void setup();
		void update();
		void draw();
		
		void plot(float* array, int length, float scale, float offset);
		
		int 	bufferCounter;
		int 	drawCounter;
	
		// fft
		float averageVal;
		float maxVal;
		
		ofxFft	* fft;
		float	* eqFunction;
		float	* eqOutput;
		float	* ifftOutput;
		float	* fftOutput;
		float	* audioInput;
		float	* blendfft;
		
		float	* avgBands;
		
		int		bufferSize;
		bool	bSetup;
		
		// aubio stuff
		//aubioAnalyzer	aubio;
		
		// stat recorders
		statsRecorder	stats_pitch;
		
		
		// average of fft bands
		int numAvgBands;
		void calculateAverageFFTBands(float * vals, int totalVals);
		
		vector<float> peakFades;
		float peakFadeVal;
		float peakThreshold;
		void updatePeaks(float threshold, float * vals, int numVals );
		void plotPeaks( int length, float scale, float offset);
		
		ofSoundPlayer music;
		
		ofxControlPanel panel;
};

