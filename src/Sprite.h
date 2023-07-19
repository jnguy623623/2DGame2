#pragma once

#include "Shape.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"



class TriangleShape : public Shape {
public:
	TriangleShape() {
		// default data
		set(glm::vec3(20, 20, 0), glm::vec3(0, -40, 0), glm::vec3(-20, 20, 0));
	}
	virtual void draw();
	virtual bool inside(const glm::vec3 p);

	void set(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
		verts.clear();
		verts.push_back(v1);
		verts.push_back(v2);
		verts.push_back(v3);
	}
};

class Sprite : public TriangleShape {
public:
	// some functions for highlighting when selected
	//
	// 
	float checkCollision(Sprite* agent) { //{
		
		if (isColliding(agent)) {
			//energy--;
			agent->lifespan = 0; //removes agent
			return true;
		}
		else return false;
	}
	//virtual void checkCollision(Sprite* agent);

	void draw() {
		if (bShowImage) {
			ofPushMatrix();
			ofSetColor(ofColor::white);
			ofMultMatrix(getMatrix());
			spriteImage.draw(-spriteImage.getWidth() / 2, -spriteImage.getHeight() / 2.0);
			ofPopMatrix();
		}
		else
		{
			if (bHighlight) ofSetColor(ofColor::white);
			else ofSetColor(ofColor::yellow);
			TriangleShape::draw();
		}
	}

	float age() {
		return (ofGetElapsedTimeMillis() - birthtime);
	}

	void setImage(ofImage img) {
		spriteImage = img;
		bShowImage = true;
		width = img.getWidth();
		height = img.getHeight();
	}

	bool inside(const glm::vec3 p);
	//bool isHeal;

	void setSelected(bool state) { bSelected = state; }
	void setHighlight(bool state) { bHighlight = state; }
	bool isSelected() { return bSelected; }
	bool isHighlight() { return bHighlight; }

	
	glm::vec3 heading() {
		glm::vec3 heading = glm::vec3(glm::sin(rot * PI / 180), -glm::cos(rot * PI / 180), 0);
		return glm::normalize(heading);
		//return (glm::rotate(glm::mat4(1.0), glm::radians(rot), glm::vec3(0, 0, 1)) * glm::vec4(0, 1, 0, 1));
	}

	float getHitRadius() {
		
		//note: verts[] are untransformed
		glm::vec4 temp = glm::vec4(this->verts[1], 1);
		glm::vec3 temp2 = glm::vec3(this->getMatrix() * temp);
		//glm::vec3 temp = this->getMatrix() * this->verts[1]; 
		float radius = glm::distance(this->pos, temp2);
		return radius;

		//will want a smaller hitbox if doing bullet hell

	}

	bool isColliding(Sprite* agent) {

		float d = glm::distance(this->pos, agent->pos);
		//float d = sqrt(pow(distance.x, 2) + pow(distance.y, 2));
		if (d <= (this->getHitRadius() + agent->getHitRadius())) {
			return true;
		}
		else return false;

	}

	void integrate();

	bool bHighlight = false;
	bool bSelected = false;
	bool bShowImage = false;

	glm::vec3 velocity = glm::vec3(0, 0, 0);
	float birthtime = 0; // elapsed time in ms
	float lifespan = -1;  //  time in ms
	string name =  "UnammedSprite";
	float width = 40;
	float height = 40;

	ofImage spriteImage;

	//physics stuff
	ofVec3f vel;
	ofVec3f accel;
	ofVec3f force;
	float mass;
	float damping = 0.99;

	float angularVel;
	float angularAccel;
	float angularFor;
	bool bThrust;
};


