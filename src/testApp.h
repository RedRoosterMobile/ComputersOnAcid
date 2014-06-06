#pragma once

/**
 * ACID EYES
 * @author: Thomas Ranker
 * @TODO
 * -more modes
 * -define outputpath of images
 * -make photo timer interval adjustable
 * -set a mode to adjust color values on the fly
 *
 *
 */


#include "ofMain.h"
#include "dkVideoSlitScan.h"
#include "ofxFBOTexture.h"

//http://stackoverflow.com/questions/2298242/callback-functions-in-c

//typedef int (*cbSetInterval)(int);

class testApp : public ofBaseApp{
public:

    static const int ELECTRIC_SHEEP;
    static const int RATIONAL_ROSES;
    static const int WHITE_FLOWER_NOISE;
    static const int INCREDIBLE_HULK;
    static const int HOT_BLUE;

	void setup();
	void update();
	void draw();
	void drawMenu(int textDistance);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	//void DoWorkObject(testApp::cbClearInterval callback);

	ofVideoGrabber 		vidGrabber;
	ofTexture			videoTexture;
	dkVideoSlitScan     slitScanner;

    //cbSetInterval* cbSI;

	size_t mWidth;
	size_t mHeight;
	bool mMirror;
	bool mScreenshots;
	bool mRealImage;
	bool mFullScreen;
	float mSmoothSize;
	bool mSlitActive;
	int mSlitCount;
	int mColorFX;
	int mWidthStep;
	int mHeightStep;
	unsigned long counter;
	ofTrueTypeFont menuFont;
	int textOffset;



	//in seconds
	int mIntervalLengthS;
	unsigned int timeStart;
	unsigned int timeEnd;

	bool mDropColors;

    // for callback
    bool isSleeping;

    // color modes
	int  colorFX[5];

	ofxFBOTexture buffer; // buffer for drawing off-screen

};
