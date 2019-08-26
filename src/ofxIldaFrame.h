//
//  ofxIldaFrame.h
//  ofxIlda
//
//  Created by Memo Akten on 09/05/2013.
//  Updated by Mitsuru Takeuchi on 02/06/2013.
//  Added colorMaps by Jonas Fehr 02/06/2013
//


// a single ILDA frame, contains multiple polys
// coordinates are NORMALIZED (0...1, 0...1)

#pragma once

#include "ofMain.h"
#include "ofxIldaPoly.h"
#include "ofxIldaPoint.h"
#include "ofxIldaPointDac.h"
#include "ofxIldaPolyProcessor.h"

namespace ofxIlda {
	
    class Frame {
public:
		ofParameterGroup parameters;
        struct {
            struct {
//                ofParameter<bool> lines; // draw lines
                ofParameter<bool> points;    // draw points
                ofParameter<bool> finalPoints;  // draw the Final Points
                ofParameter<bool> moveInBlack;  // draw the Final Points
                ofParameter<bool> pointNumbers;  // draw point numbers (not implemented yet)

            } draw;
            
            struct {
                ofParameter<ofFloatColor> masterColor; // color
                ofPixels colorMap;
                ofParameter<int> startBlanks;     // how many blank points to send at path ends
                ofParameter<int> startCount;       // how many end repeats to send in start
                ofParameter<int> endCount;       // how many end repeats to send
                ofParameter<int> endBlanks;     // how many blank points to send at path ends
                ofParameter<float> blackMoveMinDist;     // at which minimum distance do we create additional point to move in back
                ofParameter<float> galvoCorrection;     // Correcting one of the axis to compensate for mirror distortion based on the offset
                ofParameter<bool> doCapX;        // cap out of range on x (otherwise wraps around)
                ofParameter<bool> doCapY;        // cap out of range on y (otherwise wraps around)
                ofParameter<bool> useColorMap;
                ofParameter<bool> doDisplace;
                ofParameter<float> displaceAmountHorizontal;
                ofParameter<float> displaceAmountVertical;
//                ofParameter<bool> doFade;
//                ofParameter<float> fadeLength;

                ofPixels displacementMap;

                struct {
//                    ofParameter<bool> doMap;
                    ofParameter<bool> doFlipX;
                    ofParameter<bool> doFlipY;
                    ofParameter<glm::vec2> offset;
                    ofParameter<glm::vec2> scale;
                } transform;
            } output;
        } params;
        
//        Mapper mapper;

        PolyProcessor polyProcessor; // params and functionality for processing the polys
        
        struct {
            int pointCountInput;    // READONLY current total number of points across all paths (excluding blanks and end repititons)
            int pointCountProcessed; // same as above, except AFTER being processed
        } stats;
        
        
        //--------------------------------------------------------------
        Frame() {
            setDefaultParams();
        }
        
        //--------------------------------------------------------------
        void setDefaultParams() {
//            memset(&params, 0, sizeof(params));  // safety catch all default to zero
//            memset(&stats, 0, sizeof(stats));  // safety catch all default to zero
			
//            params.draw.lines = false;
            params.draw.points = false;
//            params.draw.pointNumbers = false;
            params.draw.finalPoints = true;
            params.draw.moveInBlack = true;
            params.draw.pointNumbers = false;

            params.output.masterColor.set(ofFloatColor(1, 1, 1, 1)); // limits the color of the polys to it´s value
            params.output.startBlanks = 10;
            params.output.startCount = 10;
            params.output.endCount = 10;
            params.output.endBlanks = 10;
            params.output.blackMoveMinDist = 0.01;
            params.output.galvoCorrection = 0;
            params.output.doCapX = false;
            params.output.doCapY = false;
            params.output.useColorMap = false;
            params.output.doDisplace = false;
            params.output.displaceAmountHorizontal = 0.;
            params.output.displaceAmountVertical = 0.;
//            params.output.doFade = false;
//            params.output.fadeLength = 0.;
            
//            params.output.transform.doMap = false;
            params.output.transform.doFlipX = false;
            params.output.transform.doFlipY = false;
            params.output.transform.offset.set(glm::vec2(0, 0));
            params.output.transform.scale.set(glm::vec2(1, 1));
        }
		void setup(){
			parameters.setName("IldaFrame Params");
            polyProcessor.setup();
            parameters.add(polyProcessor.parameters);
//            parameters.add(params.draw.lines.set("Draw Lines", false));
            parameters.add(params.draw.points.set("Draw Points",false));
//            parameters.add(params.draw.pointNumbers.set("Draw Point nums", false));
            parameters.add(params.draw.finalPoints.set("Draw finalPoints", true));
            parameters.add(params.draw.moveInBlack.set("Draw moveInBalc (red)", true));
            parameters.add(params.draw.pointNumbers.set("Draw pointNumbers", false));

            parameters.add(params.output.masterColor.set("masterColor", {1., 1., 1., 1.}, {0.,0.,0.,0.}, {1., 1., 1., 1.}));
            parameters.add(params.output.startBlanks.set("start Blanks",10, 0, 100));
            parameters.add(params.output.startCount.set("start Count", 10, 0, 100));
            parameters.add(params.output.endCount.set("end Count", 10, 0, 100));
            parameters.add(params.output.endBlanks.set("end Blanks",10, 0, 100));
            parameters.add(params.output.blackMoveMinDist.set("blackMoveMinDist",0.01, 0.001, 0.1));
            parameters.add(params.output.galvoCorrection.set("galvoCorrection",0., -1, 1));
            parameters.add(params.output.doCapX.set("Do Cap X", false));
            parameters.add(params.output.doCapY.set("Do Cap Y", false));
            parameters.add(params.output.useColorMap.set("useColorMap", false));
            parameters.add(params.output.doDisplace.set("doDisplace", false));
            parameters.add(params.output.displaceAmountHorizontal.set("displaceAmountHorizontal", 0., -1., 1.));
            parameters.add(params.output.displaceAmountVertical.set("displaceAmountVertical", 0., -1., 1.));
//            parameters.add(params.output.doFade.set("doFade", false));
//            parameters.add(params.output.fadeLength.set("fadeLength", 0., 0., 1.));

            parameters.add(params.output.transform.doFlipX.set("Do Flip X", false));
            parameters.add(params.output.transform.doFlipY.set("Do Flip Y", false));
            parameters.add(params.output.transform.offset.set("Offset", {0, 0}, {-1,-1},{1,1}));
            parameters.add(params.output.transform.scale.set("Scale", {0, 0}, {0,0},{3,3}));
            
            params.output.colorMap.allocate(512,512,OF_IMAGE_COLOR_ALPHA);
            params.output.displacementMap.allocate(512,512,OF_IMAGE_COLOR_ALPHA);

		}
        
        
        //--------------------------------------------------------------
        string getString() {
            stringstream s;
            s << polyProcessor.getString();
            
            s << "params:" << endl;
//            s << "draw.lines : " << params.draw.lines << endl;
            s << "draw.point : " << params.draw.points << endl;
            s << "draw.finalPoint : " << params.draw.finalPoints << endl;
            s << "draw.moveInBlack : " << params.draw.moveInBlack << endl;
            s << "draw.pointNumbers : " << params.draw.pointNumbers << endl;
            
            s << "output.masterColor : " << params.output.masterColor << endl;
            s << "output.startBlanks : " << params.output.startBlanks << endl;
            s << "output.startCount : " << params.output.startCount << endl;
            s << "output.endCount : " << params.output.endCount << endl;
            s << "output.endBlanks : " << params.output.endBlanks << endl;
            s << "output.blackMoveMinDist : " << params.output.blackMoveMinDist << endl;
            s << "output.galvoCorrection : " << params.output.galvoCorrection << endl;
            s << "output.doCapX : " << params.output.doCapX << endl;
            s << "output.doCapY : " << params.output.doCapY << endl;
//            s << "output.transform.doMap : " << params.output.transform.doMap << endl;
            s << "output.transform.doFlipX : " << params.output.transform.doFlipX << endl;
            s << "output.transform.doFlipY : " << params.output.transform.doFlipY << endl;
            s << "output.transform.offset : " << params.output.transform.offset << endl;
            s << "output.transform.scale : " << params.output.transform.scale << endl;
            
            s << endl;
            
            s << "stats:" << endl;
            s << "stats.pointCountInput : " << stats.pointCountInput << endl;
            s << "stats.pointCountProcessed : " << stats.pointCountProcessed << endl;
            
            s << polyProcessor.getString();
            s << endl;
//            s << mapper.getString();
            
            return s.str();
        }
        
        //--------------------------------------------------------------
        void update() {

            stats.pointCountInput = 0;
            for(auto & origPoly : origPolys) {
                stats.pointCountInput += origPoly.size();
            }
            for(auto & points : pointGroups) {
                stats.pointCountInput += points.size();
            }
            
            polyProcessor.update(origPolys, pointGroups);

            updateFinalPoints();
            
            if(params.output.galvoCorrection !=0){
                for(auto & point : pointsDac){
                    float offYZero = abs(float(point.y)/kIldaMaxPoint);
                    float offXZero = (float(point.x)/kIldaMaxPoint);
                    point.x = point.x + (5000*params.output.galvoCorrection)*cos(offYZero*PI/2)*offXZero;
                }
            }
            
            stats.pointCountProcessed = 0;
            stats.pointCountProcessed = pointsDac.size();
        }
        
        
        //--------------------------------------------------------------
        void draw(ofRectangle rect) {
            draw(rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight());
        }

        
        void draw(float x=0, float y=0, float w=0, float h=0) {
            if(w==0) w = ofGetWidth();
            if(h==0) h = ofGetHeight();
            
            ofPushStyle();
            ofPushMatrix();
            ofTranslate(x, y);
            ofScale(w, h);
            
//            if(params.draw.lines) {
//                drawLines();
//            }
            if(params.draw.points) {
                drawPoints();
            }
            if(params.draw.finalPoints) {
                drawFinalPoints();
            }
            ofPopMatrix();
            ofPopStyle();
            
//            mapper.draw(x,y,w,h);
        }
        
//        void drawLines(){
//            ofSetLineWidth(2);
//            for(size_t i=0; i<processedPolys.size(); i++) {
//                ofPolyline &poly = processedPolys[i];
//                ofFloatColor &pcolor = processedPolys[i].color;
//                ofSetColor(pcolor.r*255, pcolor.g*255, pcolor.b*255);
//                poly.draw();
//            }
//        }
        
        void drawPoints(){
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_POINTS);
            glEnable(GL_POINT_SMOOTH);
            glPointSize(5);
            ofSetColor(255);

            for(auto & points : pointGroups){
                for(auto & point : points){
                    mesh.addColor(point.color);
                    mesh.addVertex(glm::vec3(point,0.));
                }
                mesh.drawVertices();
                mesh.clearVertices();
            }
            
//            for(size_t i=0; i<processedPolys.size(); i++) {
//                ofPolyline &poly = processedPolys[i];
//                ofFloatColor &pcolor = processedPolys[i].color;
//                ofSetColor(pcolor.r*255, pcolor.g*255, pcolor.b*255);
//
//                for(size_t i=0; i<poly.size(); i++) {
//                    auto &p = poly[i];
//                    mesh.addVertex(glm::vec3(p.x, p.y,0.));
//                }
//                mesh.drawVertices();
//                mesh.clearVertices();
//
//            }
        }
        
        void drawFinalPoints(){
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_POINTS);
            glEnable(GL_POINT_SMOOTH);
            glPointSize(5);
            ofSetColor(255);
            
            for(size_t i=0; i<pointsDac.size(); i++) {
                PointDac &point = pointsDac[i];
                mesh.addColor(ofFloatColor(point.r/(float)kIldaMaxIntensity, point.g/(float)kIldaMaxIntensity, point.b/(float)kIldaMaxIntensity, point.a/(float)kIldaMaxIntensity));
                mesh.addVertex(glm::vec3(point.getPosition(), 0));

                if(params.draw.moveInBlack && point.r == 0 && point.g == 0 && point.b == 0 ){
                    mesh.addColor(ofFloatColor(0,1,1,0.25));
                    mesh.addVertex(glm::vec3(point.getPosition(), 0));
                }
                
                if(params.draw.pointNumbers) ofDrawBitmapString(ofToString(i),point.getPosition()+glm::vec2(0.01,0.01));

                
            }
            ofSetColor(255);
            mesh.drawVertices();
            mesh.clearVertices();
        }
        
        //--------------------------------------------------------------
        void clear() {
            origPolys.clear();
            pointGroups.clear();
            pointsDac.clear();
        }
        
        //--------------------------------------------------------------
        int size() {
            return origPolys.size();
        }
        
        //--------------------------------------------------------------
        Poly& addPoly(const Poly& poly) {
            origPolys.push_back(poly);
            return origPolys.back();
        }
        
        //--------------------------------------------------------------
        Poly& addPoly() {
            return addPoly(Poly());
        }
        
        //--------------------------------------------------------------
        Poly& addPoly(const ofPolyline& polyline) {
            return addPoly(polyline, ofFloatColor::white);
        }
        
        //--------------------------------------------------------------
        Poly& addPoly(const ofPolyline& polyline, ofFloatColor color) {
            return addPoly(Poly(polyline, color));
        }

        //--------------------------------------------------------------
        Poly& addPoly(const vector<glm::vec3> points) {
            return addPoly(Poly(points));
        }
        
        //--------------------------------------------------------------
        Poly& addPoly(const vector<glm::vec3> points, ofFloatColor color) {
            return addPoly(Poly(points, color));
        }
        
        //--------------------------------------------------------------
        void addPolys(const vector<ofPolyline> &polylines) {
            for(size_t i=0; i<polylines.size(); i++) addPoly(polylines[i]);
        }
        
        //--------------------------------------------------------------
        void addPolys(const vector<ofPolyline> &polylines, ofFloatColor color) {
            for(size_t i=0; i<polylines.size(); i++) addPoly(polylines[i], color);
        }

        
        //--------------------------------------------------------------
        void addPolys(const vector<Poly> &polys) {
            for(size_t i=0; i<polys.size(); i++) addPoly(polys[i]);
        }
        
        //--------------------------------------------------------------
        Poly& getPoly(int i) {
            return origPolys[i];
        }
        
        //--------------------------------------------------------------
        vector<Point>& getPointGroup(int i) {
            return pointGroups[i];
        }

        //--------------------------------------------------------------
        vector<Poly> &getPolys() {
            return origPolys;
        }
        
        //--------------------------------------------------------------
        vector<vector<Point>> &getPointGroups() {
            return pointGroups;
        }
        
        //--------------------------------------------------------------
        Poly& getLastPoly() {
            if(origPolys.empty()) addPoly();
            return origPolys.back();
        }
        
        //--------------------------------------------------------------
        void addPointGroups(const vector<vector<Point>> & pointG) {
            for(auto & pG : pointG){
                pointGroups.push_back(pG);
            }
        }
        
        //--------------------------------------------------------------
        vector<Point>& addPoints(const vector<Point> & points) {
            pointGroups.push_back(points);
            return pointGroups.back();
        }
        
        //--------------------------------------------------------------
        vector<Point>& addPoints(const Point & point, const int repeat = 1) {
            vector<Point> points;
            for(int i = 0; i < repeat; i++){
                points.push_back(point);
            }
            return addPoints(points);
        }
        
        //--------------------------------------------------------------
        vector<Point>& addPoints(const glm::vec2 pos, ofFloatColor color = ofFloatColor(1,1,1,1) , const int repeat = 1) {
            return addPoints(Point(pos, color), repeat);
        }
        
        //--------------------------------------------------------------
        const vector<PointDac>& getPointsDac() const {
            return pointsDac;
        }
        
        //--------------------------------------------------------------
        void drawCalibration() {
            addPoly(Poly::fromRectangle(ofRectangle(0, 0, 1, 1)));
            ofPolyline &p1 = addPoly();
            p1.lineTo(0.25, 0.25);
            p1.lineTo(0.75, 0.75);
            
            ofPolyline &p2 = addPoly();
            p2.lineTo(0.75, 0.25);
            p2.lineTo(0.25, 0.75);
            
            ofPolyline &p3 = addPoly();
            float r = .25 * sqrt(2.0f);
            p3.arc(0.5, 0.5, r, r, 0, 360, 60);

        }

        //--------------------------------------------------------------
        Point transformPoint(Point & p) const {
            // flip
            if(params.output.transform.doFlipX) p.x = 1 - p.x;
            if(params.output.transform.doFlipY) p.y = 1 - p.y;
            
            // scale
            if(glm::length2(params.output.transform.scale.get()) > 0) {
                p -= glm::vec2(0.5, 0.5);
                p *= glm::vec2(params.output.transform.scale.get());
                p += glm::vec2(0.5, 0.5);
            }
            
            // offset
            p += glm::vec2(params.output.transform.offset.get());
            
            // cap or wrap
            if(p.x < 0) {
                p.x = params.output.doCapX ? 0 : 1 + p.x - ceil(p.x);
            } else if(p.x > 1) {
                p.x = params.output.doCapX ? 1 : p.x - floor(p.x);
            }
            
            if(p.y < 0) {
                p.y = params.output.doCapY ? 0 : 1 + p.y - ceil(p.y);
            } else if(p.y > 1) {
                p.y = params.output.doCapY ? 1 : p.y - floor(p.y);
            }
            
            return p;
        }

        
        //--------------------------------------------------------------
        void updateFinalPoints() {
            pointsDac.clear();
            for(auto & pointGroup : pointGroups) {
                
                if(pointGroup.size() > 0) {
                    
                    Point startPoint = pointGroup.front();
                    Point endPoint = pointGroup.back();
                    transformPoint(startPoint);
                    transformPoint(endPoint);

                    
                    if(params.output.doDisplace){
                        displaceFromMap(startPoint);
                        displaceFromMap(endPoint);
                    }
                
                    
                    // Move slowly in black to startpoint
                    addMoveBlack(startPoint);
                    
                    // blanking at start
                    for(int n=0; n<params.output.startBlanks; n++) {
                        pointsDac.push_back( PointDac(startPoint, ofFloatColor(0, 0, 0, 0)));
                    }
                    
                    // repeat at start
                    for(int n=0; n<params.output.startCount; n++) {
                        if(params.output.useColorMap) startPoint.color = getColorFromMap(startPoint);
                        pointsDac.push_back( PointDac(startPoint, limitColor(startPoint.color, params.output.masterColor)) );
                    }
                    
                    
                    // add points
                    for(auto & point : pointGroup) {
                        transformPoint(point);
                        if(params.output.useColorMap) point.color = getColorFromMap(point);
                        if(params.output.doDisplace) displaceFromMap(point);
                        pointsDac.push_back( PointDac(point, limitColor(point.color, params.output.masterColor)) );
                    }
                    
                    // repeat at end
                    for(int n=0; n<params.output.endCount; n++) {
                        if(params.output.useColorMap) endPoint.color = getColorFromMap(endPoint);
                        pointsDac.push_back( PointDac(endPoint, limitColor(endPoint.color, params.output.masterColor)) );
                    }
                    
                    // blanking at end
                    for(int n=0; n<params.output.endBlanks; n++) {
                        pointsDac.push_back( PointDac(endPoint, ofFloatColor(0, 0, 0, 0) ));
                    }
                    
                    lastPointPoly = endPoint;
                }
            }
            
            
            if(pointGroups.size()==0){ // for safety
                // Move slowly in black to blackPoint
                addMoveBlack(glm::vec2(0.5,0.5));
                ofxIlda::PointDac point;
                point.set(glm::vec3(0.5,0.5,0.0), ofFloatColor(0));
                pointsDac.push_back(point);
                lastPointPoly = Point(0.5,0.5);
                
            }
        }
        
        void addMoveBlack(glm::vec2 target){
            // Move slowly in black to blackPoint
            if(glm::distance(glm::vec3(lastPointPoly, 0), glm::vec3(target, 0)) > params.output.blackMoveMinDist){
                Poly blackMovePath;
                blackMovePath.addVertex(glm::vec3(lastPointPoly, 0));
                blackMovePath.addVertex(glm::vec3(target, 0));
                blackMovePath = blackMovePath.getResampledBySpacing(params.output.blackMoveMinDist);
                
                if(polyProcessor.params.spacing == 0) {
                    blackMovePath.getResampledByCount(200);
                }
                
                for(auto & vertex : blackMovePath.getVertices()) {
                    pointsDac.push_back( PointDac(vertex, ofFloatColor(0, 0, 0, 0)));
                }
            }
        }
        
        Point displaceFromMap(Point & point){
            ofFloatColor displaceColor = ofFloatColor(params.output.displacementMap.getColor(point.x*params.output.displacementMap.getWidth(), point.y*params.output.displacementMap.getHeight()));
            glm::vec2 displace = glm::vec2(0.);
            displace.y = displaceColor.getBrightness()*(float)params.output.displaceAmountHorizontal;
            displace.x = displaceColor.getBrightness()*-(float)params.output.displaceAmountVertical;
            
            point.setPosition(point-displace);
            return point;
        }
        
        ofFloatColor getColorFromMap(glm::vec2 pos){
            return ofFloatColor(params.output.colorMap.getColor(pos.x*params.output.colorMap.getWidth(), pos.y*params.output.colorMap.getHeight()));
        }
        
        ofFloatColor limitColor(ofFloatColor a, ofFloatColor b){
            ofFloatColor color;
            color.r = a.r * b.r * b.a;
            color.g = a.g * b.g * b.a;
            color.b = a.b * b.b * b.a;
            return color;
        }
        
    protected:
        vector<Poly> origPolys;   // stores the original polys
        vector<vector<Point>> pointGroups;   // groups of Raw Points

        vector<PointDac> pointsDac;   // final points to send
        Point lastPointPoly;
    };
}
