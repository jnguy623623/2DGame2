#pragma once

#include "ofMain.h"

class Shape {
public:
	Shape() {}
	virtual void draw() {

		// draw a box by defaultd if not overridden
		//
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		ofDrawBox(defaultSize);
		ofPopMatrix();
	}

	virtual bool inside(glm::vec3 p) {
		return false;
	}

	glm::mat4 getMatrix() {
		glm::mat4 m1 = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 m2 = glm::rotate(m1, glm::radians(rot), glm::vec3(0, 0, 1));
		glm::mat4 M = glm::scale(m2, scale);      // added this scale if you want to change size of object
		return M;
	}
	glm::vec3 pos;
	float rot = 0.0;    // degrees 
	glm::vec3 scale = glm::vec3(1, 1, 1);
	float defaultSize = 20.0;

	vector<glm::vec3> verts;
};