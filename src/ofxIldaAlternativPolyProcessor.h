//
//  ofxIldaAlternativePolyProcessor.h
//  ofxILDA demo
//
//  Created by Memo Akten on 21/05/2013.
//
//

#pragma once

#include "ofxIldaPoly.h"

namespace ofxIlda {
    class AlternativePolyProcessor {
    public:
        ofParameterGroup parameters;
        struct {
            ofParameter<int> speed;
            ofParameter<float> acceleration;
            ofParameter<int> cornerThreshold;
            ofParameter<int> dotMaxPoints;
        } params;
        
        
        //--------------------------------------------------------------
        AlternativePolyProcessor() {
//            memset(&params, 0, sizeof(params));
            params.speed = 10;
            params.acceleration = 2;
//            params.collapse = false;
            params.cornerThreshold = 60;
            params.dotMaxPoints = 20;
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
            return s.str();
        }
        
        //--------------------------------------------------------------
        void setup(){
            parameters.setName("AlternativePolyProcessor");
            
            parameters.add(params.speed.set("speed",10, 1, 40));
            parameters.add(params.acceleration.set("acceleration",2, 0.1, 4));
            parameters.add(params.cornerThreshold.set("cornerThreshold",60, 0, 180));
            parameters.add(params.dotMaxPoints.set("dotMaxPoints",20, 0, 100));

        }
        //--------------------------------------------------------------
        void update(const vector<Poly> &origPolys, vector<Poly> &processedPolys) {
            float totalLength = 0;
            vector<int> pathLengths;
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
            if(params.spacing) {
                for(size_t i=0; i<processedPolys.size(); i++) {
                    processedPolys[i].setFromPolyline(processedPolys[i].getResampledBySpacing(params.spacing));
                }
            }
        }
    };
}
