#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxGui.h"

#define PORT 9001
#define HOST "localhost"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	ofVideoGrabber cam;
	ofxCv::ContourFinder cont;
	ofPixels previous;
	ofImage diff;
	// a scalar is like an ofVec4f but normally used for storing color information
	cv::Scalar diffMean;

	ofxPanel gui;
	ofParameter<float> minArea, maxArea, threshold;
	ofParameter<bool> holes;
	ofParameter<bool> staticBack;

	ofxOscSender osc;
};
