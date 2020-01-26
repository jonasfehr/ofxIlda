//
//  ofxIldaPolyProcessor.h
//  ofxILDA demo
//
//  Created by Memo Akten on 21/05/2013.
//
//

#pragma once

#include "ofxIldaPoly.h"
#include "ContentBase.hpp"

namespace ofxIlda {
    class PolyProcessor {
    public:
        ofParameterGroup parameters;
        struct {
            ofParameter<int> smoothAmount;   // how much to smooth the path (zero to ignore)
            ofParameter<float> optimizeTolerance;    // howmuch to optimize the path, based on curvature (zero to ignore)
//            ofParameter<bool> collapse;  // (not implemented yet)
            ofParameter<int> targetPointCount;   // how many points in total should ALL paths in this frame be resampled to (zero to ignore)
            ofParameter<float> spacing;  // desired spacing between points. Set automatically by targetPointCount, or set manually. (zero to ignore)
//            ofParameter<bool> doMoveSlowBlack;
//            ofParameter<int> moveSlowSpacingMult;
        } params;
        
        
        //--------------------------------------------------------------
        PolyProcessor() {
//            memset(&params, 0, sizeof(params));
            params.smoothAmount = 0;
            params.optimizeTolerance = 0;
//            params.collapse = false;
            params.targetPointCount = 500;
            params.spacing = 0;
//            params.doMoveSlowBlack = false;
//            params.moveSlowSpacingMult = 3;
        }
        
        //--------------------------------------------------------------
        string getString() {
            stringstream s;
            s << "polyProcessor.params:" << endl;
            s << "smoothAmount : " << params.smoothAmount << endl;
            s << "optimizeTolerance : " << params.optimizeTolerance << endl;
//            s << "collapse : " << params.collapse << endl;
            s << "targetPointCount : " << params.targetPointCount << endl;
            s << "spacing : " << params.spacing << endl;
//            s << "doMoveSlowBlack : " << params.doMoveSlowBlack << endl;
//            s << "moveSlowSpacingMult : " << params.moveSlowSpacingMult << endl;

            return s.str();
        }
        
        //--------------------------------------------------------------
        void setup(){
            parameters.setName("PolyProcessor");
            
            parameters.add(params.smoothAmount.set("smoothAmount",0, 0, 100));
            parameters.add(params.optimizeTolerance.set("optimizeTolerance",0., 0., 1.));
//            parameters.add(params.collapse.set("collapse",false));
            parameters.add(params.targetPointCount.set("targetPointCount",500, 0, 1000));
            parameters.add(params.spacing.set("spacing",0., 0., 1.));
//            parameters.add(params.doMoveSlowBlack.set("doMoveSlowBlack", false));
//            parameters.add(params.moveSlowSpacingMult.set("moveSlowSpacingMult",3, 1, 10));

        }
        //--------------------------------------------------------------
        void update(const vector<Poly> &origPolys, vector<ContentBase::PointGroup> &processedPoints) {
            float totalLength = 0;
            vector<int> pathLengths;
            vector<Poly> processedPolys;
            processedPolys = origPolys;
            for(size_t i=0; i<processedPolys.size(); i++) {
                if(processedPolys[i].size()) {
                    // smooth paths
                    if(params.smoothAmount > 0) processedPolys[i].setFromPolyline(processedPolys[i].getSmoothed(params.smoothAmount));
                    
                    // optimize paths
                    if(params.optimizeTolerance > 0) processedPolys[i].simplify(params.optimizeTolerance);
                    
                    // calculate total length (needed for auto spacing calculation)
                    if(params.targetPointCount > 0) {
                        float l = processedPolys[i].getPerimeter();
                        totalLength += l;
                        pathLengths.push_back(l);
                    }
                } else {
                    pathLengths.push_back(0);
                }
            }

            // calculate spacing based on desired total number of points
            if(params.targetPointCount > 0 && totalLength > 0) {
                params.spacing = totalLength / params.targetPointCount;
            }
            
            
            // resample paths based on spacing (either as calculated by targetPointCount, or set by user)
            if(params.spacing && totalLength > params.spacing) {
                for(size_t i=0; i<processedPolys.size(); i++) {
                    processedPolys[i].setFromPolyline(processedPolys[i].getResampledBySpacing(params.spacing));
                }
            }
            
            // convert to points
            for (auto & poly : processedPolys){
                vector<Point> points;
                for ( auto & point : poly.getVertices()){
                    points.push_back(Point(point, poly.color));
                }
                processedPoints.push_back(points);
            }
        }
    };
}
