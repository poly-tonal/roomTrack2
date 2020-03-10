#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
	ofSetVerticalSync(true);
	cam.setup(1280, 720);
	osc.setup(HOST, PORT);
	// imitate() will set up previous and diff
	// so they have the same size and type as cam
	imitate(previous, cam);
	imitate(diff, cam);
	gui.setup();
	gui.add(minArea.set("Min area", 70, 1, 100));
	gui.add(maxArea.set("Max area", 500, 1, 500));
	gui.add(threshold.set("Threshold", 3, 0, 255));
	gui.add(holes.set("Holes", false));
	gui.add(staticBack.set("Static Background", false));
}

void ofApp::update() {
	cam.update();
	if (cam.isFrameNew()) {
		// take the absolute difference of prev and cam and save it inside diff
		absdiff(cam, previous, diff);

		diff.setImageType(OF_IMAGE_GRAYSCALE);
		diff.update();

		if (!staticBack) {
			copy(cam, previous);
		}

		diffMean = mean(toCv(diff));

		// you can only do math between Scalars, convert int to scalar
		diffMean *= Scalar(50);
		cont.setMinAreaRadius(minArea);
		cont.setMaxAreaRadius(maxArea);
		cont.setThreshold(threshold);
		cont.findContours(diff);
		cont.setFindHoles(holes);

		for (int i = 0; i < cont.size(); i++) {
			ofxOscMessage message;
			switch (i) {
			case 0: //drum message
				message.setAddress("/loop1");
				message.addFloatArg(ofMap(cont.getCentroid(i).x, 0, ofGetWidth(), 0.0f, 200.0f, true));
				message.addFloatArg(ofMap(cont.getCentroid(i).y, 0, ofGetHeight(), 1.0f, 0.0f, true));
				osc.sendMessage(message, false);
				break;
			case 1: //arp message
				message.setAddress("/loop2"); //notes set to sectors send chords to pd
				if (cont.getCentroid(i).x < (ofGetWidth() / 4) && cont.getCentroid(i).y < (ofGetHeight()/2)) {
					message.addFloatArg(62);
				}
				else if (cont.getCentroid(i).x < (ofGetWidth() / 2) && cont.getCentroid(i).y < (ofGetHeight() / 2)) {
					message.addFloatArg(67);
				}		
				else if (cont.getCentroid(i).x < (ofGetWidth() / 4) * 3 && cont.getCentroid(i).y < (ofGetHeight() / 2)) {
					message.addFloatArg(64);
				}
				else if (cont.getCentroid(i).x < ofGetWidth() && cont.getCentroid(i).y < (ofGetHeight() / 2)) {
					message.addFloatArg(72);
				}		
				else if (cont.getCentroid(i).x < (ofGetWidth() / 4) && cont.getCentroid(i).y < ofGetHeight()) {
					message.addFloatArg(65);
				}
				else if (cont.getCentroid(i).x < (ofGetWidth() / 2) && cont.getCentroid(i).y < ofGetHeight()) {
					message.addFloatArg(60);
				}		
				else if (cont.getCentroid(i).x < (ofGetWidth() / 4) * 3 && cont.getCentroid(i).y < ofGetHeight()) {
					message.addFloatArg(69);
				}
				else if (cont.getCentroid(i).x < ofGetWidth() && cont.getCentroid(i).y < ofGetHeight()) {
					message.addFloatArg(71);
				}
				osc.sendMessage(message, false);
				break;
			case 2: //lead message
				message.setAddress("/loop3");
				message.addFloatArg(ofMap(cont.getCentroid(i).x, 0, ofGetWidth(), 60, 72, true)); //notes
				message.addFloatArg(ofMap(cont.getCentroid(i).y, 0, ofGetHeight(), 1.0f, 0.0f, true));  //reverb amount
				osc.sendMessage(message, false);
				break;
			case 3: //bass message
				message.setAddress("/loop4");
				message.addFloatArg(int(ofMap(cont.getCentroid(i).x, 0, ofGetWidth(), 36, 48, true))); //notes
				message.addFloatArg(ofMap(cont.getCentroid(i).y, 0, ofGetHeight(), 1.0f, 0.0f, true));  //controls keyswap
				osc.sendMessage(message, false);
				break;
			}
		}
	}
}

void ofApp::draw() {
	ofSetColor(255);
	//cam.draw(0, 0);
	diff.draw(0, 0);
	//cont.draw();
	for (int i = 0; i < cont.size(); i++) {
		ofDrawCircle(cont.getCentroid(i).x, cont.getCentroid(i).y, 5);
		ofDrawBitmapString("tracker: " + ofToString(i), cont.getCentroid(i).x + 5, cont.getCentroid(i).y + 5);
	}
	gui.draw();
}
