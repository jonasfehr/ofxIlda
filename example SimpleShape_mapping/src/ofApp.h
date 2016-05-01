#pragma once

#include "ofMain.h"
#include "ofxRayComposer.h"
#include "ofxIldaFrame.h"
#include "ofxGui.h"



class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed  (int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    
    
    
    ofxIlda::Frame ildaFrame;   // stores and manages ILDA frame drawings

    ofxRayComposer ildaDAC;
        
    // GUI / Controlpanel
    bool showGui;
    ofxPanel gui;
    ofParameterGroup parameters;
    
    ofParameter<int> LASER_numPoints;
    ofParameter<int> LASER_endCount;
    ofParameter<int> LASER_blankCount;
};
