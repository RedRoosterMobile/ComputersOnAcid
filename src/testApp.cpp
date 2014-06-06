#include "testApp.h"



const int testApp::ELECTRIC_SHEEP = 4;
const int testApp::RATIONAL_ROSES = 3;
const int testApp::WHITE_FLOWER_NOISE = 2;
const int testApp::INCREDIBLE_HULK = 1;
const int testApp::HOT_BLUE = 0;




#define THRESHOLD 0.2
//--------------------------------------------------------------
void testApp::setup(){

    colorFX[0] = HOT_BLUE;
    colorFX[1]=INCREDIBLE_HULK;
    colorFX[2]=WHITE_FLOWER_NOISE;
    colorFX[3]=RATIONAL_ROSES;
    colorFX[4]=ELECTRIC_SHEEP;

    mColorFX=0;

	mWidth=640;
	mHeight=480;
	mMirror=true;
	mScreenshots=false;
	mFullScreen=false;
	mRealImage=false;
	counter=0;
	mSlitCount=60;
	mSmoothSize=1.0f;
	mDropColors=true;
	mIntervalLengthS=2;
	mSlitActive=true;
	isSleeping=false;
	mWidthStep=mHeightStep=5;



	//time
	timeStart=0;
	timeEnd=0;

	ofSetFrameRate(40);
	ofSetVerticalSync(true);

	slitScanner.allocate(mWidth,mHeight,mSlitCount,mSmoothSize);
	//@FIXME: make this more dynamic
	vidGrabber.setDeviceID(1);
	//vidGrabber.
	//vidGrabber.setPixelFormat(OF_PIXELS_MONO);

	vidGrabber.initGrabber(mWidth,mHeight);
	videoTexture.allocate(mWidth,mHeight, GL_RGB);

	// Create a buffer
	buffer.allocate (ofGetWidth (), ofGetHeight (),GL_RGBA); // no autoclean every drawing - since will be there to collect pictures
    buffer.clear (0, 0, 0, 255); // clear black

    //buffer_tmp.allocate (ofGetWidth (), ofGetHeight (),GL_RGBA); // no autoclean every drawing - since will be there to collect pictures
    //buffer_tmp.clear (0, 0, 0, 255); // clear black
    menuFont.loadFont("monof55.ttf", 14);
    textOffset = 10;
}

//--------------------------------------------------------------
void testApp::update(){

	vidGrabber.update();
	if(vidGrabber.isFrameNew()){

	    ofImage img;
        img.allocate(mWidth, mHeight, OF_IMAGE_COLOR);

        // start draw buffer // http://de.scribd.com/doc/51534945/OpenFrameworks-lection-2d-graphics
        buffer.begin (); // start drawing a buffer

            ofEnableAlphaBlending();

            // draw real image?
            if (mRealImage==true) {

                ofSetupGraphicDefaults();
                vidGrabber.draw(0,0);
            } else {

                // draw other stuff
                unsigned char * slitPix = vidGrabber.getPixels();
                // drop background?
                if (mDropColors==true)
                    ofBackground(127,127,127,0);//ofSetupGraphicDefaults();
                float corr=1;

                for (int y=0; y<mHeight; y+=mHeightStep) {
                        for (int x=0; x<mWidth; x+=mWidthStep) {
                            //get pixel color
                            int i = (y*mWidth+x)*3;
                            int r = slitPix[i+0];
                            int g = slitPix[i+1];
                            int b = slitPix[i+2];

                            //calculate brightness
                            float br = (r+b+g) / 765.0;

                            if (br<THRESHOLD) {
                                br=THRESHOLD;
                            }

                            ofPushMatrix();
                                // HSB farbton, sättigung, dunkelstufe
                                switch (mColorFX) {
                                    case HOT_BLUE:
                                        ofSetColor(ofColor::fromHsb(br*255,br*255,255));
                                        break;
                                    case INCREDIBLE_HULK:
                                        ofSetColor(ofColor::fromHsb(cos(br*TWO_PI/4)*360,185,255));
                                        break;
                                    case WHITE_FLOWER_NOISE:
                                        ofSetColor(ofColor::fromHsb(cos(br*PI)*360,128,255));
                                        break;
                                    case RATIONAL_ROSES:
                                        ofSetColor(ofColor::fromHsb(sin(br*TWO_PI)*360,128,255));
                                        break;
                                    case ELECTRIC_SHEEP:
                                        ofSetColor(ofColor::fromHsb(br*360,128,255));
                                        break;
                                }

                                //ofSetColor(r,g,b,255-round(br*255));

                                //calculate sine and cosine of brightness
                                float co = cos(br*TWO_PI);
                                float si = sin(br*TWO_PI);
                                //vidGrabber.
                                //draw rotated line based on brightness
                                //ofLine((x-2.5*co)*corr, (y-2.5*si)*corr, (x+2.5*co)*corr, (y+2.5*si)*corr);
                                ofCircle(x*corr,y*corr,5*co);

                            ofPopMatrix();
                            //buffer_tmp.end();

                        }
                }
            }

            img.grabScreen(0,0,mWidth,mHeight);

            ofDisableAlphaBlending();

        // close draw butter
        buffer.end (); // end drawing to clipboard
        if (mSlitActive) {
            // feed buffer image into slitscanner
            slitScanner.update(img.getPixels());
            videoTexture.loadData(slitScanner.getPixels(), mWidth,mHeight, GL_RGB);
        } else {
            // just my buffer
            videoTexture.loadData(img.getPixels(), mWidth,mHeight, GL_RGB);
        }
	}//http://vormplus.be/blog/article/mirroring-video-with-openframeworks

}

//--------------------------------------------------------------
void testApp::draw() {
    //ofSetupScreen();
	if(mMirror){
		videoTexture.draw(ofGetWidth(),0,-ofGetWidth(),ofGetHeight());
	} else {
		videoTexture.draw(0,0,ofGetWidth(),ofGetHeight());
	}


    // save screen?
    if (mScreenshots && !isSleeping) {
        // do interrrut callback replacement
        timeStart = ofGetUnixTime();
        timeEnd = timeStart + mIntervalLengthS;
        isSleeping = true;
        ofLog() << "the photographer is..waking UP!";

        ofImage myImage;
        myImage.grabScreen(0,0,ofGetWidth(),ofGetHeight());
        myImage.saveImage("partOfTheScreen"+ofToString(counter)+".png");
        counter++;
        //ofExit
    } else if ( mScreenshots && (ofGetUnixTime() > timeEnd )) {
        isSleeping = false;
        ofLog() << "the photographer is..going to sleep..";
    }


    // text in white
    ofSetColor(254,254,254);
    ofSetLineWidth(5.0);
    // draw menu
    drawMenu(15);


}

void testApp::drawMenu(int textGap) {
    menuFont.drawString ("FrameRate              : "+ofToString(ofGetFrameRate()), textOffset ,textGap*3);
	menuFont.drawString ("Fullscreen       [F]   : "+ofToString(mFullScreen?"YES":"NO"), textOffset ,textGap*4);
	menuFont.drawString ("Mirror           [M]   : "+ofToString(mMirror?"YES":"NO"), textOffset ,textGap*5);
	menuFont.drawString ("Direction        [R]   : "+ofToString(slitScanner.getReverse()?"UP":"DOWN"), textOffset ,textGap*6);
	menuFont.drawString ("Slit num         [A/Z] : "+ofToString(mSlitCount)+" -> "+ofToString(slitScanner.getSlitCount()), textOffset ,textGap*7);
	menuFont.drawString ("Smoothing        [S/X] : "+ofToString(mSmoothSize), textOffset ,textGap*8);
	menuFont.drawString ("Makescreenshots  [P]   : "+ofToString(mScreenshots?"YES":"NO"), textOffset ,textGap*9);
	menuFont.drawString ("Show real colors [C]   : "+ofToString(mRealImage?"YES":"NO"), textOffset ,textGap*10);
	menuFont.drawString ("Drop prev. Colors[D]   : "+ofToString(mDropColors?"YES":"NO"), textOffset ,textGap*11);
	menuFont.drawString ("Color fX         [O/L] : "+ofToString(colorFX[mColorFX]), textOffset ,textGap*12);
	menuFont.drawString ("Wobble fX        [W]   : "+ofToString(mSlitActive?"YES":"NO"), textOffset ,textGap*13);
	menuFont.drawString ("Width step size  [1/2] : "+ofToString(mWidthStep), textOffset ,textGap*14);
	menuFont.drawString ("Height step size [3/4] : "+ofToString(mHeightStep), textOffset ,textGap*15);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key == 'e' || key == 'E') {
		vidGrabber.videoSettings();
	}
	switch(key){
	case 'm':
	case 'M':
		mMirror=!mMirror;
		break;
    case 'd':
	case 'D':
		mDropColors=!mDropColors;
		break;
    case 'c':
	case 'C':
		mRealImage=!mRealImage;
		break;
    case 'p':
	case 'P':
		mScreenshots=!mScreenshots;
		break;
	case 'f':
	case 'F':
		mFullScreen=!mFullScreen;
		ofSetFullscreen(mFullScreen);
		break;
	case 'r':
	case 'R':
		slitScanner.setReverse(!slitScanner.getReverse());
		break;

	case 'a':
	case 'A':
		mSlitCount++;
		slitScanner.allocate(mWidth,mHeight,mSlitCount,mSmoothSize);
		break;
	case 'z':
	case 'Z':
		mSlitCount--;
		mSlitCount=mSlitCount<0?0:mSlitCount;
		slitScanner.allocate(mWidth,mHeight,mSlitCount,mSmoothSize);
		break;

    case 'o':
	case 'O':
		mColorFX++;
		if (mColorFX>4)
            mColorFX=4;
		break;
	case 'l':
	case 'L':
		mColorFX--;
		if (mColorFX<0)
            mColorFX=0;
		break;

	case 's':
	case 'S':
		mSmoothSize+=0.1f;
		mSmoothSize=mSmoothSize>1.0f?1.0f:mSmoothSize;
		slitScanner.allocate(mWidth,mHeight,mSlitCount,mSmoothSize);
		break;
	case 'x':
	case 'X':
		mSmoothSize-=0.1f;
		mSmoothSize=mSmoothSize<.0f?.0f:mSmoothSize;
		slitScanner.allocate(mWidth,mHeight,mSlitCount,mSmoothSize);
		break;
    case 'w':
	case 'W':
		mSlitActive=!mSlitActive;
		break;
    case '1':
		mWidthStep++;
		break;
    case '2':
		mWidthStep--;
		if (mWidthStep<=0)
            mWidthStep=1;
		break;
    case '3':
		mHeightStep++;
		break;
    case '4':
		mHeightStep--;
		if (mHeightStep<=0)
            mHeightStep=1;
		break;
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
