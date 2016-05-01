#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    
    ildaDAC.setup();
    ildaDAC.setPPS(30000);
    
    // initialising GUI
    parameters.add(LASER_numPoints.set("LASER_numPoints", 200, 1, 500));
    parameters.add(LASER_endCount.set("LASER_endCount", 0, 0, 100));
    parameters.add(LASER_blankCount.set("LASER_blankCount", 0, 0, 100));
    
    gui.setup(parameters);
    showGui = true;
    
    ildaFrame.params.output.transform.doMap = true;
    ildaFrame.mapper.params.bShow = true;

    
}

//--------------------------------------------------------------
void ofApp::update(){
    // draw Shape
    ofPolyline shape;
    ofPoint center = ofPoint(0.5,0.5);
    
    bool bCircle = false;
    if(bCircle){
        float radius = 0.5;
        int circleRes = 50;
        float segAng = TWO_PI/circleRes;
        for(int i = 0; i < circleRes; i++){
            float x = sin(segAng*i)*radius;
            float y = cos(segAng*i)*radius;
            shape.addVertex(x+center.x, y+center.y);
        }
        shape.close();
    }
    
    bool bTriangle = true;
    if(bTriangle){
        shape.addVertex(0.5, 0);
        shape.addVertex(1, 1);
        shape.addVertex(0, 1);
        shape.close();
    }
    
    
    
    ildaFrame.params.output.endCount = LASER_endCount;
    ildaFrame.params.output.blankCount = LASER_blankCount;
    ildaFrame.polyProcessor.params.targetPointCount = LASER_numPoints;
    
    // transmit shape to ildaFrame
    ildaFrame.clear();
    ildaFrame.addPoly(shape, ofFloatColor::white);
    ildaFrame.update();
    
    
    // send points to the DAC
    ildaDAC.setPoints(ildaFrame);
}



//--------------------------------------------------------------
void ofApp::draw() {
    
    
    
    // draw to the screen
    ildaFrame.draw(0, 0, ofGetWidth(), ofGetHeight());
    

    

    
    
    if(showGui){
        gui.draw();
    }
    
    ofSetColor(255);
    ofDrawBitmapString(ildaFrame.getString(), 10, gui.getHeight()+20);
}

//--------------------------------------------------------------
void ofApp::exit(){
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case 'f': ofToggleFullscreen(); break;
            
            
            // clear the frame
        case 'c': ildaFrame.clear(); break;
            
            // change color
        case 'R': ildaFrame.params.output.color.r = 1 - ildaFrame.params.output.color.r; break;
        case 'G': ildaFrame.params.output.color.g = 1 - ildaFrame.params.output.color.g; break;
        case 'B': ildaFrame.params.output.color.b = 1 - ildaFrame.params.output.color.b; break;

            // toggle draw lines (on screen only)
        case 'l': ildaFrame.params.draw.lines ^= true; break;
            
            // toggle loop for last poly
        case 'o': ildaFrame.getLastPoly().setClosed(ildaFrame.getLastPoly().isClosed()); break;
            
            // toggle draw points (on screen only)
        case 'p': ildaFrame.params.draw.points ^= true; break;
            
            // adjust point count
        case '.': ildaFrame.polyProcessor.params.targetPointCount++; break;
        case ',': if(ildaFrame.polyProcessor.params.targetPointCount > 10) ildaFrame.polyProcessor.params.targetPointCount--; break;

            // adjust point count quicker
        case '>': ildaFrame.polyProcessor.params.targetPointCount += 10; break;
        case '<': if(ildaFrame.polyProcessor.params.targetPointCount > 20) ildaFrame.polyProcessor.params.targetPointCount -= 10; break;
            
            // flip image
        case 'x': ildaFrame.params.output.transform.doFlipX ^= true; break;
        case 'y': ildaFrame.params.output.transform.doFlipY ^= true; break;

            // cap image
        case 'X': ildaFrame.params.output.doCapX ^= true; break;
        case 'Y': ildaFrame.params.output.doCapY ^= true; break;
            
            // scale output
        case 'w': ildaFrame.params.output.transform.scale.y += 0.05; break;
        case 's': ildaFrame.params.output.transform.scale.y -= 0.05; break;
        case 'a': ildaFrame.params.output.transform.scale.x -= 0.05; break;
        case 'd': ildaFrame.params.output.transform.scale.x += 0.05; break;
            
        case 'C': ildaFrame.drawCalibration(); break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    // draw a line to the mouse cursor (normalized coordinates) in the last poly created
    ildaFrame.getLastPoly().lineTo(x / (float)ofGetWidth(), y / (float)ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    // create a new poly in the ILDA frame
    ildaFrame.addPoly();
}
