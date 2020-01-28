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
#include "ContentBase.hpp"

namespace ofxIlda {
	
    class Frame {
public:
		ofParameterGroup parameters;
        struct {
            struct {
//                ofParameter<bool> lines; // draw lines
                ofParameter<bool> points{"Draw points", false};    // draw points
                ofParameter<bool> finalPoints{"Draw finalPoints", true};  // draw the Final Points
                ofParameter<bool> moveInBlack{"Draw moveInBlack", true};  // draw the Final Points
                ofParameter<bool> pointNumbers{"Draw pointNumbers", false};  // draw point numbers (not implemented yet)

            } draw;
            
            struct {
                ofParameter<ofFloatColor> masterColor{"masterColor", ofFloatColor::white}; // color
                ofPixels colorMap;
                ofParameter<int> startBlanks{"start Blanks", 3, 0, 20};     // how many blank points to send at path ends
                ofParameter<int> startDelay{"start Delay", 4, 0, 20};       // how many end repeats to send in start
                ofParameter<int> endCount{"end Count", 3, 0, 20};       // how many end repeats to send
                ofParameter<int> endBlanks{"end blanks", 3, 0, 20};     // how many blank points to send at path ends
                ofParameter<float> blackMoveMinDist{"blackMoveMinDist", 0.005, 0.001, 0.01};     // at which minimum distance do we create additional point to move in back
                ofParameter<float> blackMoveMaxDist{"blackMoveMaxDist", 0.005, 0.01, 0.1};     // at which maximum distance do we create additional point to move in back
                ofParameter<float> blackMoveIncAmt{"blackMoveIncAmt", 0.1, 0.000001, 0.5};     // at which pct do we increese the to move in back distance
                ofParameter<float> galvoCorrectionDeform{"galvoCorrectionDeform", 0.0336, 0, 0.15};     // Correcting one of the axis to compensate for mirror distortion based on the offset
                ofParameter<float> galvoCorrectionDist{"galvoCorrectionDist", 0.0376, 0, 0.15};     // Correcting one of the axis to compensate for mirror distortion based on the offset

                ofParameter<bool> doCapX{"doCapX", true};        // cap out of range on x (otherwise wraps around)
                ofParameter<bool> doCapY{"doCapY", true};        // cap out of range on y (otherwise wraps around)
                ofParameter<bool> useColorMap{"useColorMap", false};
                ofParameter<bool> doDisplace{"doDisplace", false};
                ofParameter<float> displaceAmountHorizontal{"displaceAmountHorizontal", 0., -1., 1.};
                ofParameter<float> displaceAmountVertical{"displaceAmountVertical", 0., -1., 1.};
//                ofParameter<bool> doFade;
//                ofParameter<float> fadeLength;

                ofPixels displacementMap;

                struct {
//                    ofParameter<bool> doMap;
                    ofParameter<bool> doFlipX{"doFlipX", false};
                    ofParameter<bool> doFlipY{"doFlipY", false};
                    ofParameter<glm::vec2> offset{"Offset", {0, 0}, {-1,-1},{1,1}};
                    ofParameter<glm::vec2> scale{"Scale", {0, 0}, {0,0},{3,3}};
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
        }
        
        //--------------------------------------------------------------
		void setup(){
			parameters.setName("IldaFrame Params");
            polyProcessor.setup();
            parameters.add(polyProcessor.parameters);
//            parameters.add(params.draw.lines);
            parameters.add(params.draw.points);
//            parameters.add(params.draw.pointNumbers);
            parameters.add(params.draw.finalPoints);
            parameters.add(params.draw.moveInBlack);
            parameters.add(params.draw.pointNumbers);

            parameters.add(params.output.masterColor);
            parameters.add(params.output.startBlanks);
            parameters.add(params.output.startDelay);
            parameters.add(params.output.endCount);
            parameters.add(params.output.endBlanks);
            parameters.add(params.output.blackMoveMinDist);
            parameters.add(params.output.blackMoveMaxDist);
            parameters.add(params.output.blackMoveIncAmt);
            parameters.add(params.output.galvoCorrectionDeform);
            parameters.add(params.output.galvoCorrectionDist);
            parameters.add(params.output.doCapX);
            parameters.add(params.output.doCapY);
            parameters.add(params.output.useColorMap);
            parameters.add(params.output.doDisplace);
            parameters.add(params.output.displaceAmountHorizontal);
            parameters.add(params.output.displaceAmountVertical);
//            parameters.add(params.output.doFade);
//            parameters.add(params.output.fadeLength);

            parameters.add(params.output.transform.doFlipX);
            parameters.add(params.output.transform.doFlipY);
            parameters.add(params.output.transform.offset);
            parameters.add(params.output.transform.scale);
            
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
            s << "output.startCount : " << params.output.startDelay << endl;
            s << "output.endCount : " << params.output.endCount << endl;
            s << "output.endBlanks : " << params.output.endBlanks << endl;
            s << "output.blackMoveMinDist : " << params.output.blackMoveMinDist << endl;
            s << "output.blackMoveMaxDist : " << params.output.blackMoveMaxDist << endl;
            s << "output.blackMoveIncAmt : " << params.output.blackMoveIncAmt << endl;
            s << "output.galvoCorrectionX : " << params.output.galvoCorrectionDeform << endl;
            s << "output.galvoCorrectionY : " << params.output.galvoCorrectionDist << endl;
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

            for(auto & pointGroup : pointGroups){
                for(auto & point : pointGroup.points){
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
            return pointGroups[i].points;
        }

        //--------------------------------------------------------------
        vector<Poly> &getPolys() {
            return origPolys;
        }
        
        //--------------------------------------------------------------
        vector<ContentBase::PointGroup> &getPointGroups() {
            return pointGroups;
        }
        
        //--------------------------------------------------------------
        Poly& getLastPoly() {
            if(origPolys.empty()) addPoly();
            return origPolys.back();
        }
        
        //--------------------------------------------------------------
        void addPointGroups(const vector<ContentBase::PointGroup> & pointG) {
            for(auto & pG : pointG){
                pointGroups.push_back(pG);
            }
        }
        
//        //--------------------------------------------------------------
//        vector<Point>& addPoints(const vector<Point> & points) {
//            pointGroups.back().points.push_back(points);
//            return pointGroups.back().points;
//        }
//        
//        //--------------------------------------------------------------
//        vector<Point>& addPoints(const Point & point, const int repeat = 1) {
//            vector<Point> points;
//            for(int i = 0; i < repeat; i++){
//                points.push_back(point);
//            }
//            return addPoints(points);
//        }
//        
//        //--------------------------------------------------------------
//        vector<Point>& addPoints(const glm::vec2 pos, ofFloatColor color = ofFloatColor(1,1,1,1) , const int repeat = 1) {
//            return addPoints(Point(pos, color), repeat);
//        }
        
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
            
            // Galvo correction
            if(params.output.galvoCorrectionDeform !=0 || params.output.galvoCorrectionDist !=0 ){
                glm::vec2 point = p;
                point*=2;
                point-=1;
                p.x = p.x + (params.output.galvoCorrectionDeform)*cos(abs(point.y)*PI/2)*point.x - point.x*(1.-cos(abs(point.x)*PI/2))*params.output.galvoCorrectionDist ;
                p.y = p.y - point.y*(1.-cos(abs(point.y)*PI/2))*params.output.galvoCorrectionDist;

                //p.x = p.x + (params.output.galvoCorrection)*cos(offYZero*PI/2)*offXZero;
            }
            
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
            bool first = true;
            for(auto & pointGroup : pointGroups) {
                // select profile
                int startBlanks = 0;
                int startDelay = 0;
                int endCount = 0;
                int endBlanks = 0;
                bool isDirectlyConnected = false;
                bool checkIsConnected = false;
                
                switch( pointGroup.type ){
                    case CT_UNDEFINED:
                        startBlanks = params.output.startBlanks;
                        startDelay = params.output.startDelay;
                        endCount = params.output.endCount;
                        endBlanks = params.output.endBlanks;
                        break;
                        
                    case CT_DOT:
                        startBlanks = 3;
                        startDelay = 0;
                        endCount = 0;
                        endBlanks = 0;
                        break;
                        
                    case CT_LINE:
                        startBlanks = 0;
                        startDelay = 3;
                        endCount = 3;
                        endBlanks = 0;
                        break;
                        
                    case CT_FLOW:
                        startBlanks = 0;
                        startDelay = 3;
                        endCount = 3;
                        endBlanks = 0;
                        break;
                        
                    case CT_CIRCLE:
                        startBlanks = 0;
                        startDelay = 6;
                        endCount = 1;
                        endBlanks = 0;
                        break;
                         
                    case CT_CONNECTEDLINES:
                        startBlanks = 3;
                        startDelay = 3;
                        endCount = 4;
                        endBlanks = 0;
                        checkIsConnected = true;
                        
                        break;
                        
                        case CT_CONNECTEDDOTS:
                        startBlanks = 3;
                        startDelay = 1;
                        endCount = 0;
                        endBlanks = 0;
                        
                        break;
                }
                
                
                if(pointGroup.size() > 0) {
                    

                    Point startPoint = pointGroup.points.front();
                    Point endPoint = pointGroup.points.back();
                    transformPoint(startPoint);
                    transformPoint(endPoint);

                    
                    if(params.output.doDisplace){
                        displaceFromMap(startPoint);
                        displaceFromMap(endPoint);
                    }
                
                    if(checkIsConnected){
                        if(glm::distance((glm::vec2)startPoint, (glm::vec2)lastPointPoly)<params.output.blackMoveMinDist){
                            isDirectlyConnected = true;
                        }
                    }
                    // Move slowly in black to startpoint
                    if(first) first = false;
                    else if(!isDirectlyConnected) addMoveBlack(lastPointPoly, startPoint, params.output.blackMoveMinDist, params.output.blackMoveMaxDist, params.output.blackMoveIncAmt);
                    
                    // blanking at start
                    if(!isDirectlyConnected){
                        for(int n=0; n<startBlanks; n++) {
                            pointsDac.push_back( PointDac(startPoint, ofFloatColor(0, 0, 0, 0)));
                        }
                    }
                    
                    // add points
                    int i = 0;
                    for(auto & point : pointGroup.points) {
                        transformPoint(point);
                        if(params.output.useColorMap) point.color = getColorFromMap(point);
                        if(params.output.doDisplace) displaceFromMap(point);
                        if(i < startDelay && !isDirectlyConnected){
                            // remove the color of first points in the pattern to allow the galvos to reach position.
                            point.color = ofFloatColor(0,0,0,0);
                            i++;
                        }
                        pointsDac.push_back( PointDac(point, limitColor(point.color, params.output.masterColor)) );
                    }
                    
                    // repeat at end
                    if(!isDirectlyConnected){
                        for(int n=0; n<endCount; n++) {
                            if(params.output.useColorMap) endPoint.color = getColorFromMap(endPoint);
                            pointsDac.push_back( PointDac(endPoint, limitColor(endPoint.color, params.output.masterColor)) );
                        }
                        
                        
                        // blanking at end
                        for(int n=0; n<endBlanks; n++) {
                            pointsDac.push_back( PointDac(endPoint, ofFloatColor(0, 0, 0, 0) ));
                        }
                    }
                    lastPointPoly = endPoint;
                }
            }
            
            
            if(pointGroups.size()==0){ // for safety
                // Move slowly in black to center
                addMoveBlack(lastPointPoly, glm::vec2(0.5,0.5), params.output.blackMoveMinDist, params.output.blackMoveMaxDist, params.output.blackMoveIncAmt);
                lastPointPoly = Point(0.5,0.5);
            }
            
            
            //force 500 points
//            if(pointsDac.size()<500){
//                for(int i = 0; i < 500 - pointsDac.size(); i++){
//                ofxIlda::PointDac point;
//                point.set(glm::vec3(lastPointPoly,0.0), ofFloatColor(0));
//                pointsDac.push_back(point);
//                }
//            }
        }
        
        glm::vec2 lerp(glm::vec2 a, glm::vec2 b, float amt){
            return (1.-amt)*a + amt*b;
        }
        
        void addMoveBlack(glm::vec2 origin, glm::vec2 target, float startStep = 0.0025, float maxStep = 0.02, float incAmt = 0.1){
            float distance = glm::distance(origin, target);
            float inc = startStep;
            float distPassed = inc;
            // speedUp
            while((inc < maxStep) && ((distPassed/distance) < 0.5)){
                glm::vec2 pos = lerp(origin, target, distPassed/distance);
                pointsDac.push_back( PointDac(pos, ofFloatColor(0, 0, 0, 0)));
                inc*=(1+incAmt);
                distPassed += inc;
            }
            float distToMaxSpeed = distPassed;
            // keep
            while(distPassed < (distance-distToMaxSpeed)){
                glm::vec2 pos = lerp(origin, target, distPassed/distance);
                pointsDac.push_back( PointDac(pos, ofFloatColor(0, 0, 0, 0)));
                distPassed += inc;
            }
            // slowDown
            while( distPassed<distance){
                glm::vec2 pos = lerp(origin, target, distPassed/distance);
                pointsDac.push_back( PointDac(pos, ofFloatColor(0, 0, 0, 0)));
                inc*=(1-incAmt);
                if(inc < startStep) inc = startStep;
                distPassed += inc;
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
//        vector<vector<Point>> pointGroups;   // groups of Raw Points
        vector<ContentBase::PointGroup> pointGroups;

        vector<PointDac> pointsDac;   // final points to send
        Point lastPointPoly;
    };
}
