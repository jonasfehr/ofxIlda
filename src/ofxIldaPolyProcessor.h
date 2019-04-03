//
//  ofxIldaPolyProcessor.h
//  ofxILDA demo
//
//  Created by Memo Akten on 21/05/2013.
//
//

#pragma once

#include "ofxIldaPoly.h"

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
            ofParameter<bool> doMoveSlowBlack;
            ofParameter<int> moveSlowSpacingMult;
        } params;
        
        
        //--------------------------------------------------------------
        PolyProcessor() {
//            memset(&params, 0, sizeof(params));
            params.smoothAmount = 0;
            params.optimizeTolerance = 0;
//            params.collapse = false;
            params.targetPointCount = 500;
            params.spacing = 0;
            params.doMoveSlowBlack = true;
            params.moveSlowSpacingMult = 3;
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
            s << "doMoveSlowBlack : " << params.doMoveSlowBlack << endl;
            s << "moveSlowSpacingMult : " << params.moveSlowSpacingMult << endl;

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
            parameters.add(params.doMoveSlowBlack.set("doMoveSlowBlack", true));
            parameters.add(params.moveSlowSpacingMult.set("moveSlowSpacingMult",3, 1, 10));

        }
        //--------------------------------------------------------------
        void update(const vector<Poly> &origPolys, vector<Poly> &processedPolys, glm::vec3 &lastPointPreviousFrame) {
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
            
        // Add steps for movement in black ! EXPERIMENTAL !
            if(params.doMoveSlowBlack){
                vector<Poly> blackMovePaths;
                vector<Poly> result ;

                // Make a line from the last point of the previous frame to the first of the new frame.
                if(processedPolys.size()) {
                    if(processedPolys.front().size()) {
                        if(distance(lastPointPreviousFrame , processedPolys.front().getVertices().front()) > params.spacing*params.moveSlowSpacingMult){
                            Poly blackMovePath;
                            blackMovePath.addVertex(lastPointPreviousFrame);
                            blackMovePath.addVertex(processedPolys.front().getVertices().front());
                            if(params.spacing > 0.){
                                blackMovePath = blackMovePath.getResampledBySpacing(params.spacing*params.moveSlowSpacingMult);
                            } else {
                                blackMovePath = blackMovePath.getResampledBySpacing(0.005*params.moveSlowSpacingMult);
                            }
                            blackMovePath.color = ofFloatColor(0.0, 0.0, 0.0);
                            
                            result.push_back(blackMovePath);
                        }
                    }
                }

                for(size_t i=0; i<processedPolys.size(); i++) {
                    int numOfPoints = processedPolys[i].size();
                    Poly blackMovePath;
                    blackMovePath.addVertex(processedPolys[i].getVertices().back());
                    blackMovePath.addVertex(processedPolys[(i+1)%processedPolys.size()].getVertices().front());
                    if(params.spacing > 0.){
                        blackMovePath = blackMovePath.getResampledBySpacing(params.spacing*params.moveSlowSpacingMult);
                    } else {
                        blackMovePath = blackMovePath.getResampledBySpacing(0.005*params.moveSlowSpacingMult);
                    }
                    blackMovePath.color = ofFloatColor(0.0, 0.0, 0.0);
                    
                    if(blackMovePath.size()) blackMovePaths.push_back(blackMovePath);
                }
                
                // Combine the two lists
                
                auto m = std::min( processedPolys.size(), blackMovePaths.size() ) ;
                
                for( std::size_t i=0 ; i<m ; ++i )
                {
                    result.push_back(processedPolys[i]) ;
                    result.push_back(blackMovePaths[i]) ;
                }
                if( m < processedPolys.size() ) result.insert( result.end(), processedPolys.begin()+m, processedPolys.end() ) ;
                if( m < blackMovePaths.size() ) result.insert( result.end(), blackMovePaths.begin()+m, blackMovePaths.end() ) ;
                
                
                processedPolys.clear();
                processedPolys = result;
                
            }
        }
    };
}
