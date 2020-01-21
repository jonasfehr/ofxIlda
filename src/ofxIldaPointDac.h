//
//  ofxIldaPointDac.h
//  ofxIlda
//
//  Created by Memo Akten on 09/05/2013.
//
//

#pragma once

namespace ofxIlda {
    
#define kIldaMinPoint -32768
#define kIldaMaxPoint 32767
#define kIldaDimension (kIldaMaxPoint - kIldaMinPoint)
#define kIldaMaxIntensity 65535
    
    class PointDac {
    public:
        PointDac() : x(0), y(0), r(0), g(0), b(0), a(0), u1(0), u2(0) {}
        PointDac(int16_t x, int16_t y, int16_t r=0, int16_t g=0, int16_t b=0, int16_t a=0): x(x), y(y), r(r), g(g), b(b), a(a), u1(0), u2(0) {}
        PointDac(glm::vec2 p, ofFloatColor c, glm::vec2 pmin = {0,0}, glm::vec2 pmax = {1,1}) { set(p, c, pmin, pmax); }
        PointDac(glm::vec2 p, glm::vec2 pmin = {0,0}, glm::vec2 pmax = {1,1}) { setPosition(p, pmin, pmax); }
        
        int16_t x;
        int16_t y;
        uint16_t r;
        uint16_t g;
        uint16_t b;
        uint16_t a;
        uint16_t u1;  // what are these for? standard ILDA or just etherdream?
        uint16_t u2;
        
        //--------------------------------------------------------------
        void set(int16_t x, int16_t y) {
            this->x = x;
            this->y = y;
        }
        
        //--------------------------------------------------------------
        void set(int16_t x, int16_t y, int16_t r, int16_t g, int16_t b, int16_t a) {
            this->x = x;
            this->y = y;
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
        
        //--------------------------------------------------------------
        void set(int16_t x, int16_t y, int16_t r, int16_t g, int16_t b, int16_t a, int16_t u1, int16_t u2) {
            this->x = x;
            this->y = y;
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
            this->u1 = u1;
            this->u2 = u2;
        }

        
        
        
        //--------------------------------------------------------------
        // set color and position mapped from custom range (defaults to normalized)
        void set(glm::vec2 p, ofFloatColor c, glm::vec2 pmin = {0,0}, glm::vec2 pmax = {1,1}) {
            set(
                ofMap(p.x, pmin.x, pmax.x, kIldaMinPoint, kIldaMaxPoint),
                ofMap(p.y, pmin.y, pmax.y, kIldaMinPoint, kIldaMaxPoint),
                c.r * kIldaMaxIntensity,
                c.g * kIldaMaxIntensity,
                c.b * kIldaMaxIntensity,
                0,
                0,
                0
                );
        }
        
        //--------------------------------------------------------------
        // set position mapped from custom range (defaults to normalized)
        void setPosition(glm::vec2 p, glm::vec2 pmin = {0,0}, glm::vec2 pmax = {1,1}) {
            set(
                ofMap(p.x, pmin.x, pmax.x, kIldaMinPoint, kIldaMaxPoint),
                ofMap(p.y, pmin.y, pmax.y, kIldaMinPoint, kIldaMaxPoint)
                );
        }
        
        
        //--------------------------------------------------------------
        // gets position of point mapped to desired range (defaults to normalized)
        glm::vec2 getPosition(glm::vec2 pmin = {0,0}, glm::vec2 pmax = {1,1}) {
            return glm::vec2(
                           ofMap(x, kIldaMinPoint, kIldaMaxPoint, pmin.x, pmax.x),
                           ofMap(y, kIldaMinPoint, kIldaMaxPoint, pmin.y, pmax.y)
                             );
        }
        
    };
}
