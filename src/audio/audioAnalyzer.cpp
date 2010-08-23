#include "audioAnalyzer.h"

//--------------------------------------------------------------
AudioAnalyzer::AudioAnalyzer(){
	
	bSetup = false;

}

AudioAnalyzer::~AudioAnalyzer(){

	delete [] eqFunction;
	delete [] eqOutput;
	delete [] ifftOutput;
	delete [] fftOutput;
	delete [] audioInput;
	delete [] avgBands;
	delete [] blendfft;
	
	delete fft;
}



//--------------------------------------------------------------
void AudioAnalyzer::setup(){


	bufferSize = NUM_BANDS;
	numAvgBands = 64;
	peakFadeVal = .05;
	peakThreshold = .1;

	
	fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_BARTLETT);
	
	audioInput	= new float[bufferSize];
	fftOutput	= new float[fft->getBinSize()];
	eqFunction	= new float[fft->getBinSize()];
	eqOutput	= new float[fft->getBinSize()];
	ifftOutput	= new float[bufferSize];
	blendfft	= new float[bufferSize];
	avgBands	= new float[numAvgBands];
	
	bufferCounter = 0;
	
	// this describes a linear low pass filter
	for(int i = 0; i < fft->getBinSize(); i++)
		eqFunction[i] = .25*powf((float)i,1.1);
		
	//(float) (fft->getBinSize() - i) / (float) fft->getBinSize();
	//eqFunction[i] = (float) (fft->getBinSize() - i) / (float) fft->getBinSize();
	
	//aubio.setup();
	
	stats_pitch.setup(100,0,5000);

	bSetup = true;
	
	panel.setup("audio mover",16,16,400,75);
	panel.update();
}


//--------------------------------------------------------------
void AudioAnalyzer::update(){

	fft->setSignal( audioInput );
	
	memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * fft->getBinSize());
	
	for(int i = 0; i < fft->getBinSize(); i++)
		eqOutput[i] = fftOutput[i] * eqFunction[i];
	
	fft->setPolar(eqOutput, fft->getPhase());
	
	fft->clampSignal();
	memcpy(ifftOutput, fft->getSignal(), sizeof(float) * fft->getSignalSize());
	
	for(int i = 0; i < bufferSize; i++)
		blendfft[i] = .9*blendfft[i] + .1*ifftOutput[i];
		
	//aubio.processAudio(audioInput, NUM_BANDS);
	
	//averageVal = aubio.amplitude;
		
	//stats_pitch.update(aubio.pitch);
	
	calculateAverageFFTBands( eqOutput, fft->getBinSize() );
	
	updatePeaks(peakThreshold, eqOutput, fft->getBinSize());
	
	panel.update();
}

//--------------------------------------------------------------
void AudioAnalyzer::draw(){

	int plotHeight = 128;
	
	ofEnableAlphaBlending();
	
	ofSetColor(0xffffff);
	ofPushMatrix();
	
	glTranslatef(panel. boundingBox.x,panel.boundingBox.y+10,0);//16, 16, 0);
	ofDrawBitmapString("Audio Input", 0, 0);
	plot(audioInput, bufferSize, plotHeight / 2, 0);
	
	glTranslatef(0, plotHeight + 16, 0);
	ofDrawBitmapString("FFT Output", 0, 0);
	plot(fftOutput, fft->getBinSize(), -plotHeight, plotHeight / 2);
	
	
	ofSetColor(255,0,0,200);
	ofLine(0,-averageVal*plotHeight+plotHeight,fft->getBinSize(),-averageVal*plotHeight+plotHeight);
	ofSetColor(255,255,255,255);
	
	ofPushMatrix();
		glTranslatef(fft->getBinSize(), 0, 0);
		ofDrawBitmapString("EQd FFT Output", 0, 0);
		plotPeaks(fft->getBinSize(), -plotHeight, plotHeight / 2);//plot(avgBands, numAvgBands-1,-plotHeight, plotHeight / 2);
		plot(eqOutput, fft->getBinSize(), -plotHeight, plotHeight / 2);
		
		glPushMatrix();
			glTranslatef(0, plotHeight / 2 + (plotHeight / 2), 0);
			ofBeginShape();
				for (int i = 0; i < numAvgBands; i++)
					ofVertex(i * 2, avgBands[i] * (-plotHeight));
			ofEndShape();
		glPopMatrix();
	
	ofPopMatrix();
	
	glTranslatef(0, plotHeight + 16, 0);
	ofDrawBitmapString("IFFT Output", 0, 0);
	plot(ifftOutput, fft->getSignalSize(), plotHeight / 2, 0);
	
	glTranslatef(0, plotHeight + 16, 0);
	ofDrawBitmapString("Blend FFT Output", 0, 0);
	plot(blendfft, fft->getSignalSize(), plotHeight / 2, 0);	
	ofPopMatrix();
	
	//ofDrawBitmapString( "pitch is : " + ofToString((int)aubio.pitch) + "\namplitude is : " + ofToString(aubio.amplitude,3), 10,600);

	//stats_pitch.draw(16,460,100,100,10);
	
}

void AudioAnalyzer::plot(float* array, int length, float scale, float offset)
{
	float plotHeight = 128;
	
	ofFill();
	ofSetColor(0,0,0,200);
	ofRect(0, 0, length, plotHeight);
	
	ofSetColor(255,255,255,255);

	ofNoFill();
	ofRect(0, 0, length, plotHeight);
	
	glPushMatrix();
		glTranslatef(0, plotHeight / 2 + offset, 0);
		ofBeginShape();
			for (int i = 0; i < length; i++)
				ofVertex(i, array[i] * scale);
		ofEndShape();
	glPopMatrix();
}

void AudioAnalyzer::plotPeaks(int length, float scale, float offset)
{
	if(peakFades.size() <= 0) return;
	
	float plotHeight = 128;
	int nPeaks = (int)( peakFades.size() );
	int psize = (int)(length / (float)nPeaks);
	
	ofFill();
	ofSetColor(0,0,0,200);
	//ofRect(0, 0, length, plotHeight);
	
	ofSetColor(255,255,255,255);
	
	ofNoFill();
	//ofRect(0, 0, length, plotHeight);
	
	ofFill();
	glPushMatrix();
		glTranslatef(0, plotHeight / 2 + offset, 0);
			for (int i = 0; i < nPeaks; i++)
			{
				ofSetColor(255, 0, 0, 255*(peakFades[i]*2));
				ofRect(i*psize, 0, psize-1, scale);
			}
	glPopMatrix();
}

void AudioAnalyzer::calculateAverageFFTBands(float * vals, int totalVals)
{
	
	for( int i = 0; i < numAvgBands; i++)
	{
		
		float average = 0;
		
		for( int j = i; j < (i+numAvgBands); j++)
		{
			if(j < totalVals) average += vals[j];
		}
		
		average /= (float)numAvgBands;
		avgBands[i] = average;
		
	}
	
}


void AudioAnalyzer::updatePeaks( float threshold, float * vals, int numVals )
{
	if( peakFades.size() < numAvgBands)
	{
		peakFades.clear();
		for( int i = 0; i < numAvgBands; i++)
		{
			peakFades.push_back(0);
		}
	}
	
	float averageForVals = 0;
	for( int i = 0; i < numVals; i++)
	{
		averageForVals+= vals[i];
	}
	averageForVals /= (float)numVals;
	
	// find if any of the fft vals are higher than the threshold
	// if the peak for that field is <= 0, then make a new peak
	// fade outall peaks
	for( int i = 0; i < peakFades.size(); i++)
	{
		if(peakFades[i] > 0 ) peakFades[i] -= peakFadeVal;
	}
	
	
	// for each averaged bands
	for( int i = 0; i < numAvgBands; i++)
	{
		if( peakFades[i] > 0 ) continue;
		
		float hiVal = 0;
		for( int j = i; j < numAvgBands; j++)
		{
			if( j < numVals )
			{
				if( vals[j] > averageForVals+threshold && vals[j] > hiVal)
				//if( vals[j] > (avgBands[i]*threshold) && vals[j] > hiVal)
					hiVal = vals[j];
			}
		} 
		
		if( hiVal > 0 ) peakFades[i] = 1;
	}
	
	
	
	
}

