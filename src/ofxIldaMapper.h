//
//  ofxIldaMapper.h
//  ofxILDA demo
//
//  Created by Jonas Fehr on 29/04/2016.
//  inspired and based on ofxQuadWarp created by lukasz karluk on 19/06/11.
//  https://github.com/julapy/ofxQuadWarp
//  the reimplementation was needed, as the ILDA range is from 0. to 1. instead of the pixel on the canvas.
//

#pragma once

#include "ofMain.h"
#include "ofxIldaPoly.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

namespace ofxIlda {
    class Mapper {
    public:
        struct {
            bool bMouseEnabled;
            bool bKeyboardShortcuts;
            bool bShow;
            bool bAutoSave;
        } params;
        
        
        //--------------------------------------------------------------
        Mapper() {
            memset(&params, 0, sizeof(params));
            params.bMouseEnabled = false;
            params.bKeyboardShortcuts = false;
            params.bShow = false;
            params.bAutoSave = true;
            
            
            srcPoints[0] = dstPoints[0] = dstPointsOld[0] = ofPoint(0,0);
            srcPoints[1] = dstPoints[1] = dstPointsOld[1] = ofPoint(1,0);
            srcPoints[2] = dstPoints[2] = dstPointsOld[2] = ofPoint(1,1);
            srcPoints[3] = dstPoints[3] = dstPointsOld[3] = ofPoint(0,1);
            
            if(params.bAutoSave){
                load("settings/mapping.xml");
            }
            
            position = ofVec2f(0,0);
            
            matrix = getMatrix();
            
            anchorSize = 10;
            anchorSizeHalf = anchorSize * 0.5;
            selectedCornerIndex = -1;
            highlightCornerIndex = -1;
            
            
            setup();
            
        }
        ~Mapper() {
            disableMouseControls();
            disableKeyboardShortcuts();
            
            if(params.bAutoSave){
                save("settings/mapping.xml");
            }
        }
        
        //----------------------------------------------------- setup.
        void setup() {
            enableMouseControls();
            enableKeyboardShortcuts();
        }
        
        void reset() {
            dstPoints[0].set(srcPoints[0]);
            dstPoints[1].set(srcPoints[1]);
            dstPoints[2].set(srcPoints[2]);
            dstPoints[3].set(srcPoints[3]);
        }
        
        
        //--------------------------------------------------------------
        string getString() {
            stringstream s;
            s << "Mapper.params:" << endl;
            s << "bMouseEnabled : " << params.bMouseEnabled << endl;
            s << "bKeyboardShortcuts : " << params.bKeyboardShortcuts << endl;
            s << "bShow : " << params.bShow << endl;
            s << endl;
            //            s << "dstPoint 1 : " << dstPoints[0] << endl;
            //            s << "dstPoint 2 : " << dstPoints[1] << endl;
            //            s << "dstPoint 3 : " << dstPoints[2] << endl;
            //            s << "dstPoint 4 : " << dstPoints[3] << endl;
            s << "selected Corner: " << selectedCornerIndex << endl;
            
            return s.str();
        }
        
        //--------------------------------------------------------------
        void update(const vector<Poly> &origPolys, vector<Poly> &mappedPolys) {
            for(int i = 0; i<4; i++){ // avoid calculating matrix each timeM
                if(dstPoints[i]!=dstPointsOld[i]){
                    matrix = getMatrix();
                    dstPointsOld[i] = dstPoints[i];
                    i = 4; // matrix is recalculated;
                }
            }
            
            mappedPolys = origPolys;
            for(int i=0; i<mappedPolys.size(); i++) {
                for(int p = 0; p < mappedPolys[i].getVertices().size(); p++){
                    mappedPolys[i].getVertices()[p] = matrix.preMult(mappedPolys[i].getVertices()[p]);
                }
            }
            
        }
        //--------------------------------------------------------------
        // functions to controll the mapping are also copied from ofxQuadWrap
        void enableMouseControls() {
            if(params.bMouseEnabled == true) {
                return;
            }
            params.bMouseEnabled = true;
            ofAddListener(ofEvents().mouseMoved, this, &Mapper::onMouseMoved);
            ofAddListener(ofEvents().mousePressed, this, &Mapper::onMousePressed);
            ofAddListener(ofEvents().mouseDragged, this, &Mapper::onMouseDragged);
            ofAddListener(ofEvents().mouseReleased, this, &Mapper::onMouseReleased);
            ofLogNotice() << "ofxIldaMapper - Mouse control enabled";
            
        }
        
        void disableMouseControls() {
            if(params.bMouseEnabled == false) {
                return;
            }
            params.bMouseEnabled = false;
            try {
                ofRemoveListener(ofEvents().mouseMoved, this, &Mapper::onMouseMoved);
                ofRemoveListener(ofEvents().mousePressed, this, &Mapper::onMousePressed);
                ofRemoveListener(ofEvents().mouseDragged, this, &Mapper::onMouseDragged);
                ofRemoveListener(ofEvents().mouseReleased, this, &Mapper::onMouseReleased);
                ofLogNotice() << "ofxIldaMapper - Mouse control disabled";
                
            }
            catch(Poco::SystemException) {
                return;
            }
        }
        
        void enableKeyboardShortcuts() {
            if(params.bKeyboardShortcuts == true) {
                return;
            }
            params.bKeyboardShortcuts = true;
            ofAddListener(ofEvents().keyPressed, this, &Mapper::keyPressed);
            ofLogNotice() << "ofxIldaMapper - Key control enabled";
            
        }
        
        void disableKeyboardShortcuts() {
            if(params.bKeyboardShortcuts == false) {
                return;
            }
            params.bKeyboardShortcuts = false;
            try {
                ofRemoveListener(ofEvents().keyPressed, this, &Mapper::keyPressed);
                ofLogNotice() << "ofxIldaMapper - Key control disabled";
                
            }
            catch(Poco::SystemException) {
                return;
            }
        }
        
        // DRAW funktions
        //----------------------------------------------------- show / hide.
        void draw() {
            if(params.bShow == false) {
                return;
            }
            
            drawQuadOutline();
            drawCorners();
            drawHighlightedCorner();
            drawSelectedCorner();
        }
        
        void drawQuadOutline() {
            if(params.bShow == false) {
                return;
            }
            ofSetColor(ofColor::magenta);
            
            for(int i=0; i<4; i++) {
                int j = (i+1) % 4;
                ofDrawLine(dstPoints[i].x * ofGetWidth() + position.x,
                           dstPoints[i].y * ofGetHeight() + position.y,
                           dstPoints[j].x * ofGetWidth() + position.x,
                           dstPoints[j].y * ofGetHeight() + position.y);
            }
        }
        
        void drawCorners() {
            if(params.bShow == false) {
                return;
            }
            ofSetColor(ofColor::yellow);
            
            for(int i=0; i<4; i++) {
                ofPoint point;
                point.x = dstPoints[i].x * ofGetWidth();
                point.y = dstPoints[i].y * ofGetHeight();
                
                drawCornerAt(point);
            }
        }
        
        void drawHighlightedCorner() {
            if(params.bShow == false) {
                return;
            }
            if(highlightCornerIndex < 0 || highlightCornerIndex > 3) {
                return;
            }
            ofSetColor(ofColor::magenta);
            
            ofPoint point;
            point.x = dstPoints[highlightCornerIndex].x * ofGetWidth();
            point.y = dstPoints[highlightCornerIndex].y * ofGetHeight();
            drawCornerAt(point);
        }
        
        void drawSelectedCorner() {
            if(params.bShow == false) {
                return;
            }
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            ofSetColor(ofColor::red);
            
            ofPoint point;
            point.x = dstPoints[highlightCornerIndex].x * ofGetWidth();
            point.y = dstPoints[highlightCornerIndex].y * ofGetHeight();
            drawCornerAt(point);
        }
        
        void drawCornerAt(const ofPoint & point) {
            ofDrawRectangle(point.x + position.x - anchorSizeHalf,
                            point.y + position.y - anchorSizeHalf,
                            anchorSize, anchorSize);
        }
        
        
        //---------------------------------
        
    protected:
        ofxXmlSettings settingsXML;
        
        
        ofPoint srcPoints[4] = {ofPoint(0,0),ofPoint(1,0),ofPoint(1,1),ofPoint(0,1)};
        ofPoint dstPoints[4];
        ofPoint dstPointsOld[4];
        
        ofMatrix4x4 matrix;
        
        // get Matrix function is directly copied from ofxQuadWarp
        ofMatrix4x4 getMatrix() const {
            return getMatrix(&srcPoints[0], &dstPoints[0]);
        }
        ofMatrix4x4 getMatrix(const ofPoint* srcPoints, const ofPoint* dstPoints) const {
            
            CvPoint2D32f cvsrc[4];
            CvPoint2D32f cvdst[4];
            
            //we set the warp coordinates
            //source coordinates as the dimensions of our window
            cvsrc[0].x = srcPoints[0].x;
            cvsrc[0].y = srcPoints[0].y;
            cvsrc[1].x = srcPoints[1].x;
            cvsrc[1].y = srcPoints[1].y;
            cvsrc[2].x = srcPoints[2].x;
            cvsrc[2].y = srcPoints[2].y;
            cvsrc[3].x = srcPoints[3].x;
            cvsrc[3].y = srcPoints[3].y;
            
            cvdst[0].x = dstPoints[0].x;
            cvdst[0].y = dstPoints[0].y;
            cvdst[1].x = dstPoints[1].x;
            cvdst[1].y = dstPoints[1].y;
            cvdst[2].x = dstPoints[2].x;
            cvdst[2].y = dstPoints[2].y;
            cvdst[3].x = dstPoints[3].x;
            cvdst[3].y = dstPoints[3].y;
            
            //we create a matrix that will store the results
            //from openCV - this is a 3x3 2D matrix that is
            //row ordered
            CvMat * translate = cvCreateMat(3,3,CV_32FC1);
            
            //this is the slightly easier - but supposidly less
            //accurate warping method
            //cvWarpPerspectiveQMatrix(cvsrc, cvdst, translate);
            
            
            //for the more accurate method we need to create
            //a couple of matrixes that just act as containers
            //to store our points  - the nice thing with this
            //method is you can give it more than four points!
            
            CvMat* src_mat = cvCreateMat(4, 2, CV_32FC1);
            CvMat* dst_mat = cvCreateMat(4, 2, CV_32FC1);
            
            //copy our points into the matrixes
            cvSetData(src_mat, cvsrc, sizeof(CvPoint2D32f));
            cvSetData(dst_mat, cvdst, sizeof(CvPoint2D32f));
            
            //figure out the warping!
            //warning - older versions of openCV had a bug
            //in this function.
            cvFindHomography(src_mat, dst_mat, translate);
            
            //get the matrix as a list of floats
            float *mat = translate->data.fl;
            
            
            //we need to copy these values
            //from the 3x3 2D openCV matrix which is row ordered
            //
            // ie:   [0][1][2] x
            //       [3][4][5] y
            //       [6][7][8] w
            
            //to openGL's 4x4 3D column ordered matrix
            //        x  y  z  w
            // ie:   [0][3][ ][6]
            //       [1][4][ ][7]
            //		 [ ][ ][ ][ ]
            //       [2][5][ ][9]
            //
            
            ofMatrix4x4 matrixTemp;
            matrixTemp.getPtr()[0]  = mat[0];
            matrixTemp.getPtr()[4]  = mat[1];
            matrixTemp.getPtr()[12] = mat[2];
            
            matrixTemp.getPtr()[1]  = mat[3];
            matrixTemp.getPtr()[5]  = mat[4];
            matrixTemp.getPtr()[13] = mat[5];
            
            matrixTemp.getPtr()[3]  = mat[6];
            matrixTemp.getPtr()[7]  = mat[7];
            matrixTemp.getPtr()[15] = mat[8];
            
            cvReleaseMat(&translate);
            cvReleaseMat(&src_mat);
            cvReleaseMat(&dst_mat);
            
            return matrixTemp;
        }
        
        //interaction
        ofPoint position;
        
        float anchorSize;
        float anchorSizeHalf;
        int selectedCornerIndex;
        int highlightCornerIndex;
        
        void onMouseMoved(ofMouseEventArgs & mouseArgs) {
            if(params.bShow == false) {
                return;
            }
            
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            for(int i=0; i<4; i++) {
                ofPoint dstPoint;
                dstPoint.x = dstPoints[i].x * ofGetWidth();
                dstPoint.y = dstPoints[i].y * ofGetHeight();
                if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
                    highlightCornerIndex = i;
                    return;
                }
            }
            highlightCornerIndex = -1;
        }
        void onMousePressed(ofMouseEventArgs & mouseArgs) {
            if(params.bShow == false) {
                return;
            }
            
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            for(int i=0; i<4; i++) {
                ofPoint dstPoint;
                dstPoint.x = dstPoints[i].x * ofGetWidth();
                dstPoint.y = dstPoints[i].y * ofGetHeight();
                if(mousePoint.distance(dstPoint) <= anchorSizeHalf) {
                    dstPoint.set(mousePoint);
                    selectedCornerIndex = i;
                    return;
                }
            }
            selectedCornerIndex = -1;
        }
        
        void onMouseDragged(ofMouseEventArgs & mouseArgs){
            if(params.bShow == false) {
                return;
            }
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            dstPoints[selectedCornerIndex].set(ofPoint(mousePoint.x/ofGetWidth(), mousePoint.y/ofGetHeight()));
        }
        void onMouseReleased(ofMouseEventArgs & mouseArgs) {
            if(params.bShow == false) {
                return;
            }
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            
            ofPoint mousePoint(mouseArgs.x, mouseArgs.y);
            mousePoint -= position;
            dstPoints[selectedCornerIndex].set(ofPoint(mousePoint.x/ofGetWidth(), mousePoint.y/ofGetHeight()));
        }
        void keyPressed(ofKeyEventArgs & keyArgs){
            if(params.bShow == false) {
                return;
            }
            
            switch (keyArgs.key) {
                case '1':
                    selectedCornerIndex = 0;
                    break;
                case '2':
                    selectedCornerIndex = 1;
                    break;
                case '3':
                    selectedCornerIndex = 2;
                    break;
                case '4':
                    selectedCornerIndex = 3;
                    break;
                case 'r':
                    reset();
                    break;
                default:
                    break;
            }
            
            if(selectedCornerIndex < 0 || selectedCornerIndex > 3) {
                return;
            }
            
            float nudgeAmount = 0.01;
            ofPoint & selectedPoint = dstPoints[selectedCornerIndex];
            
            switch (keyArgs.key) {
                case OF_KEY_LEFT:
                    selectedPoint.x -= nudgeAmount;
                    break;
                case OF_KEY_RIGHT:
                    selectedPoint.x += nudgeAmount;
                    break;
                case OF_KEY_UP:
                    selectedPoint.y -= nudgeAmount;
                    break;
                case OF_KEY_DOWN:
                    selectedPoint.y += nudgeAmount;
                    break;
                default:
                    break;
            }
        }
        //----------------------------------------------------- save / load.
        void save(const string& path) {
            ofXml xml;
            xml.addChild("mapping");
            xml.setTo("mapping");
            xml.addChild("src");
            xml.setTo("src");
            for(int i=0; i<4; i++) {
                xml.addChild("point");
                xml.setToChild(i);
                xml.setAttribute("x", ofToString(srcPoints[i].x));
                xml.setAttribute("y", ofToString(srcPoints[i].y));
                xml.setToParent();
            }
            xml.setToParent();
            xml.addChild("dst");
            xml.setTo("dst");
            for(int i=0; i<4; i++) {
                xml.addChild("point");
                xml.setToChild(i);
                xml.setAttribute("x", ofToString(dstPoints[i].x));
                xml.setAttribute("y", ofToString(dstPoints[i].y));
                xml.setToParent();
            }
            xml.setToParent();
            
            xml.setToParent();
            xml.save(path);
        }
        
        void load(const string& path) {
            ofXml xml;
            bool bOk = xml.load(path);
            if(bOk == false) {
                return;
            }
            
            bOk = xml.setTo("mapping");
            if(bOk == false) {
                return;
            }
            
            bOk = xml.setTo("src");
            if(bOk == false) {
                return;
            }
            
            for(int i=0; i<xml.getNumChildren(); i++) {
                bOk = xml.setToChild(i);
                if(bOk == false) {
                    continue;
                }
                srcPoints[i].x = ofToFloat(xml.getAttribute("x"));
                srcPoints[i].y = ofToFloat(xml.getAttribute("y"));
                xml.setToParent();
            }
            xml.setToParent();
            
            bOk = xml.setTo("dst");
            if(bOk == false) {
                return;
            }
            
            for(int i=0; i<xml.getNumChildren(); i++) {
                bOk = xml.setToChild(i);
                if(bOk == false) {
                    continue;
                }
                dstPoints[i].x = ofToFloat(xml.getAttribute("x"));
                dstPoints[i].y = ofToFloat(xml.getAttribute("y"));
                xml.setToParent();
            }
            xml.setToParent();
            xml.setToParent();
        }
        
    };
}