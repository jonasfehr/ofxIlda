#pragma once

#include "ofMain.h"
//#include "ofxEtherdream.h"
#include "ofxRayComposer.h"
#include "ofxSyphon.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    
    void drawInFbo();

    //ofxEtherdream ildaDAC;
    ofxRayComposer ildaDAC;
    
    ofxSyphonClient syphonIn;

};
