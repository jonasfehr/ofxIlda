#pragma once

#include "ofMain.h"
#include "ofxEtherdream.h"
//#include "ofxRayComposer.h"
#include "ofxSyphon.h"
#include "ofxSimpleGuiToo.h"
#include "ofxIldaRenderTarget.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    
    void drawInFbo();

    ofxEtherdream ildaDAC;
//    ofxRayComposer ildaDAC;
    
    ofxSyphonClient syphonIn;
    
    // VARS
    ofxIlda::RenderTarget ildaFbo;
    ofxIlda::Frame ildaFrame;
    
    ofVec2f mouseDownPos;   // position of mouse (normalized)
    ofVec2f lastMouseDownPos; // last position of mouse (normalized)
    
    // PARAMS
    bool doSyphonIn;
    bool doFboClear;
    bool doDrawErase;   // whether we are erasing (true) or drawing (erase)
    int brushThickness;
    
    ofImage colorMap;
    
    bool doOutput;

};
