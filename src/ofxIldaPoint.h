//
//  ofxIldaPoint.h
//  interactivelaser
//
//  Created by 武内 満 on 2013/06/02.
//
//

#pragma once

namespace ofxIlda {
    
    class Point: public glm::vec2{
	public:
		ofFloatColor color;
		Point() : color(ofFloatColor(1, 1, 1, 1)) {}

        Point(ofFloatColor color) : color(color) {}
        
        Point(const Point& point) : glm::vec2(point), color(point.color) {}
        
        Point(const glm::vec2& pos) : glm::vec2(pos), color(ofFloatColor(1, 1, 1, 1)) {}

        Point(const glm::vec2& pos, ofFloatColor color) : glm::vec2(pos), color(color) {}
        
        Point(const float posX, const float posY, ofFloatColor color = ofFloatColor(1, 1, 1, 1)) : glm::vec2(posX, posY), color(color) {}
        
        void setPosition(const glm::vec2& pos){
            this->x = pos.x;
            this->y = pos.y;
        }
	};
}
