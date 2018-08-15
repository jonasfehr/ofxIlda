//
//  ofxIldaTarget.h
//  ofxIlda
//
//  Created by Memo Akten on 09/05/2013.
//
//


// wraps functionality of an offscreen render target (i.e. FBO)
// which you can draw anything into, and it gets converted to an ILDA Frame


#pragma once

#include "ofMain.h"
#include "ofxIldaFrame.h"
#include "ofxOpenCv.h"

namespace ofxIlda {
    class RenderTarget {
    public:
        ofParameterGroup parameters;

        struct {
            // cv
            struct {
                ofParameter<int> blurAmount;
                ofParameter<int> bottomThreshold;
                ofParameter<int> thresholdAmount;
                ofParameter<int> adaptiveThresholdAmount;
                ofParameter<int> adaptiveThresholdBlock;
                ofParameter<int> erodeAmount;
                ofParameter<bool> doCanny;
                ofParameter<float> cannyThresh1;
                ofParameter<float> cannyThresh2;
                ofParameter<int> cannyWindow;
                ofParameter<bool> doInvert;
                ofParameter<bool> doFindHoles;
            } cv;
            
            struct {
                ofParameter<bool> fbo;
                ofParameter<int> fboAlpha;
                ofParameter<bool> linesRaw;
            } draw;
        } params;
        
        
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void setup(float w, float h, int internalformat = GL_RGB) {
            fbo.allocate(w, h, internalformat);
            fbo.begin();
            ofClear(0);
            fbo.end();
            pixels.allocate(fbo.getWidth(), fbo.getHeight(), 3);
            
            colorImage.allocate(fbo.getWidth(), fbo.getHeight());
            greyImage.allocate(fbo.getWidth(), fbo.getHeight());
            
            parameters.setName("IldaRenderTarget");
            parameters.add(params.cv.blurAmount.set("blurAmount",0, 0, 20));
            parameters.add(params.cv.bottomThreshold.set("bottomThreshold", 0, 0, 20));
            parameters.add(params.cv.thresholdAmount.set("thresholdAmount", 120, 0, 255));
            parameters.add(params.cv.adaptiveThresholdAmount.set("adaptiveThresholdAmount",0, 0, 50));
            parameters.add(params.cv.adaptiveThresholdBlock.set("adaptiveThresholdBlock",0, 0, 10));
            parameters.add(params.cv.erodeAmount.set("erodeAmount",0, 0, 20));
            parameters.add(params.cv.doCanny.set("doCanny", true));
            parameters.add(params.cv.cannyThresh1.set("cannyThresh1",0, 0, 10));
            parameters.add(params.cv.cannyThresh2.set("cannyThresh2",0, 0, 10));
            parameters.add(params.cv.cannyWindow.set("cannyWindow",1, 1, 3));
            parameters.add(params.cv.doInvert.set("doInvert", false));
            parameters.add(params.cv.doFindHoles.set("doFindHoles", true));
            
            parameters.add(params.draw.fbo.set("fbo", true));
            parameters.add(params.draw.fboAlpha.set("fboAlpha",0, 0, 255));
            parameters.add(params.draw.linesRaw.set("linesRaw", true));


        }
        
        
        //--------------------------------------------------------------
        void begin() { fbo.begin(); }
        
        //--------------------------------------------------------------
        void end() { fbo.end(); }
        
        //--------------------------------------------------------------
        void clear(float r=0, float g=0, float b=0, float a=0) { begin(); ofClear(r, g, b, a); end(); }

        
        //--------------------------------------------------------------
        float getWidth() { return fbo.getWidth(); }
        
        //--------------------------------------------------------------
        float getHeight() { return fbo.getHeight(); }
        
        
        
        //--------------------------------------------------------------
        void update(Frame &ildaFrame) {
            fbo.readToPixels(pixels);
            colorImage.setFromPixels(pixels);
            greyImage.setFromColorImage(colorImage);
            
            if(params.cv.blurAmount) greyImage.blurGaussian(params.cv.blurAmount * 2 + 1);
            if(params.cv.bottomThreshold) {
                cvThreshold(greyImage.getCvImage(), greyImage.getCvImage(), params.cv.bottomThreshold*2+1, 0, CV_THRESH_TOZERO); greyImage.flagImageChanged();
            }
            
            if(params.cv.thresholdAmount) greyImage.threshold(params.cv.thresholdAmount);
            if(params.cv.adaptiveThresholdBlock) greyImage.adaptiveThreshold(params.cv.adaptiveThresholdBlock*2+1, params.cv.adaptiveThresholdAmount, true);
            for(int i=0; i<params.cv.erodeAmount; i++) greyImage.erode();
            
            if(params.cv.doCanny) {
                cvCanny(greyImage.getCvImage(), greyImage.getCvImage(), params.cv.cannyThresh1, params.cv.cannyThresh2, params.cv.cannyWindow*2+1);
                greyImage.flagImageChanged();
            }
            
            if(params.cv.doInvert) greyImage.invert();
            
            contourFinder.findContours(greyImage, 5*5, greyImage.getWidth() * greyImage.getHeight(), 1000, params.cv.doFindHoles, false);
            
            glm::vec2 normalizer(1.0f/contourFinder.getWidth(), 1.0f/contourFinder.getHeight());
            for(int i=0; i<contourFinder.blobs.size(); i++) {
                vector<glm::vec3> &pts = contourFinder.blobs[i].pts;
                ofPolyline &poly = ildaFrame.addPoly();
                for(int j=0; j<pts.size(); j++) {
                    poly.lineTo(pts[j] * normalizer);  // coordinates are 0...1
                }
                poly.close();
            }
        }
        
        
        //--------------------------------------------------------------
        void draw(float x, float y, float w, float h) {
            ofNoFill();
            ofSetColor(255);
            ofDrawRectangle(x, y, w, h);
            
            if(params.draw.fbo) {
                ofSetColor(params.draw.fboAlpha);
                fbo.draw(x, y, w, h);
            }
            
            if(params.draw.linesRaw) {
                ofPushStyle();
                ofPushMatrix();
                ofTranslate(x, y);
                ofScale(w/contourFinder.getWidth(), h/contourFinder.getHeight() );
                
                ofSetColor(255, 0, 0);
                ofNoFill();
                for(int i=0; i<contourFinder.blobs.size(); i++ ) {
                    vector<glm::vec3> &pts = contourFinder.blobs[i].pts;
                    ofBeginShape();
                    for(int j=0; j<pts.size(); j++) {
                        ofVertex(pts[j].x, pts[j].y);
                    }
                    ofEndShape();
                    
                }
                ofPopMatrix();
                ofPopStyle();
            }
            
        }
        
        //--------------------------------------------------------------
        ofFbo &getFbo() { return fbo; }
        ofPixels &getPixels() { return pixels; }
        ofxCvColorImage &getColorImage() { return colorImage; }
        ofxCvGrayscaleImage &getGreyImage() { return greyImage; }
        ofxCvContourFinder &getContourFinder() { return contourFinder; }
        
        
        
    protected:
        ofFbo fbo;  // fbo to draw into
        ofPixels pixels;    // pixels to read fbo into
        ofxCvColorImage colorImage;
        ofxCvGrayscaleImage greyImage;
        ofxCvContourFinder contourFinder;
    };
}
