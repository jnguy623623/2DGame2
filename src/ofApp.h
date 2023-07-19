#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"

class Player: public Sprite{
	using Sprite::Sprite;

public:
	
	glm::vec3 heading() { //get the direction the player is heading in
		glm::vec3 heading = glm::vec3(glm::sin(rot * PI / 180), -glm::cos(rot * PI / 180), 0);
		return glm::normalize(heading);
	}
	

	bool checkBounds(glm::vec3 pos) {
		if (pos.x > 0 && pos.x < ofGetWindowWidth() && pos.y > 0 &&
			pos.y < ofGetWindowHeight()) {
			return true;
		}
		else return false;
	}

	bool checkBounds(glm::vec3 pos, glm::vec3 offset) {
		glm::vec3 temp = pos;
		glm::vec3 temp2 = pos;
		temp += offset;
		temp2 -= offset;
		if (checkBounds(temp) && checkBounds(temp2)) {
			return true;
		}
		else return false;
	}

	bool checkCollision(Sprite* agent) { //overriding the sprite version

		if (isColliding(agent)) {
			energy--;
			agent->lifespan = 0; //removes agent
			return true;
		}
		else return false;
	}

	int energy; //this is player HP
	glm::vec3 center;
	glm::vec3 headingPt;

};


class Agent : public Sprite {
	//use shape emitter example src
public:

	Agent(Player* player) {
		target = player;
	}

	glm::vec3 heading() { //direction sprite is looking at
		glm::vec3 heading = glm::vec3(glm::sin(rot * PI / 180), -glm::cos(rot * PI / 180), 0);
		return glm::normalize(heading);
	}

	Player* target;
	int eSpeed;
	//float hitbox;
	float headingLength = 75; //in px
};




class AgentEmitter : public Emitter {
public:

	AgentEmitter(Player* player) {
		target = player;
		eSpeed = 60;
		groupSize = 1;
	}

	void setTarget(Player* player) {
		target = player;
	}

	void setSpeed(int speed) {
		eSpeed = speed;
	}

	void setPhysics(ofVec3f force, float m, float d,
		float angVel, float angAccel, float angForce) {

		//data is set based on sliders
		this->force = force;
		this->mass = m;
		this->damping = d;

		this->angularVel = angVel;
		this->angularAccel = angAccel;
		this->angularFor = angForce;
	}

	void moveSprite(Sprite* sprite) {//integrate physics here

		//int speed = eSpeed;
		//glm::vec3 offset;
		/*move forward - commented out code was used for non - physics
		//if (ofGetFrameRate != 0) {
			//offset = dir * speed / ofGetFrameRate();
		//}
		//else offset = sprite->heading() * speed;

		//offset = sprite->heading() * speed; //this is actually velocity
		//sprite->vel = offset; //every time we move sprite, velocity is reset to this

		//sprite->pos += offset; -this was for non-physics
		//sprite->vel = sprite->heading() * vel;//velocity;
		*/

		glm::vec3 dir = (target->pos - sprite->pos);
		dir = glm::normalize(dir);

		//angle from agent heading to player in radians
		float theta = atan2(dir.y, dir.x) + 90;
		sprite->rot = theta * 180/PI; //this is the angle to the player
		//then the physics integration will give rotation some effects

		ofVec3f vector = sprite->vel;
		double mag = sqrt(vector.x * vector.x + vector.y*vector.y);
		//mag is like a pushing force based on the magnitude of inital velocity
		sprite->vel = sprite->heading() * mag;
		//initial velocity is determined by this "force" pointing towards the player
		sprite->integrate();
		if (mag <= 20) {
			sprite->vel = sprite->heading() * eSpeed;
			//increase velocity again so agents don't stop because of damping
		}
		
	}

	void spawnSprite() {
		//can override.
		Agent sprite(target);

		if (haveChildImage) sprite.setImage(childImage);
		//sprite.velocity = velocity;
		

		sprite.lifespan = lifespan;
		sprite.pos = glm::vec3(ofRandom(0, ofGetWindowWidth()), ofRandom(0,ofGetWindowHeight()), 0);
		sprite.rot = ofRandom(0, 360); //start in a random direction
		sprite.eSpeed = eSpeed;
		sprite.birthtime = ofGetElapsedTimeMillis();

		//physics
		sprite.damping = this->damping;
		sprite.mass = this->mass;
		sprite.vel = sprite.heading() * eSpeed; //initial velocity is the heading * speed
		sprite.accel = this->accel;
		sprite.force = this->force;
		sprite.angularVel = this->angularVel;
		sprite.angularAccel = this->angularAccel;
		sprite.angularFor = this->angularFor;

		sys->add(sprite);

	}

	Player* target;
	int eSpeed;

	ofVec3f vel;
	ofVec3f accel;
	ofVec3f force;
	float mass;
	float damping = 0.99;

	float angularVel;
	float angularAccel;
	float angularFor;


};

class Bullet : public Sprite {
public:
	//using Sprite::Sprite;
	Bullet(Player* player) {
		owner = player;
	}

	void setOwner(Player* player) {
		owner = player;
	}

	/*
	void draw() {
		ofSetColor(ofColor::red);
		ofDrawRectangle(-width / 2 + pos.x, -height / 2 + pos.y, width, height);
	}
	//maybe change draw to be a rectangle instead of a triangle
	*/

	Player* owner;

};

class GunEmitter : public Emitter {
public:
	GunEmitter(Player *player) {
		owner = player;
		mode = 1;
	}

	//not needed?
	void setBulletSize(float w, float h, Sprite* sprite) {
		bulletWidth = w;
		bulletHeight = h;
		sprite->width = w;
		sprite->height = h;
	}

	void changeMode() {
		mode++;
		if (mode > 3) {
			mode = 1;
		}
	}

	void changeMode2() {
		mode--;
		if (mode < 1) {
			mode = 3;
		}
	}

	void moveSprite(Sprite* sprite) {
		int speed = 300;
		glm::vec3 offset;
		
		//get offset
		if (ofGetFrameRate != 0) {
			offset = sprite->heading() * speed / ofGetFrameRate();
		}
		else offset = sprite->heading() * speed;

		//firing modes
		if (mode == 1) { //forward firing, as expected
			sprite->pos += offset;
		}
		else if (mode == 2) { //stationary, stops bullets in track
			sprite->lifespan++;
			sprite->rot += 90;
			sprite->pos += cos(sprite->rot) + offset;
		}

		else if (mode == 3) { // primed explosion, allows ramming and detonation
			sprite->lifespan++;
			sprite->pos = owner->pos + owner->heading() * 75;
			//sets bullets to be concentrated in front of player
		}

	}

	void spawnSprite() {

		Bullet sprite(owner);
		if (haveChildImage) sprite.setImage(childImage);
		sprite.lifespan = lifespan;
		sprite.pos = this->pos;
		sprite.rot = owner->rot;
		sprite.birthtime = ofGetElapsedTimeMillis();

		sys->add(sprite);
	}


	float bulletWidth;
	float bulletHeight;
	int mode;
	Player *owner;

};


class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);


	Player player;//player object
	AgentEmitter* spawner; //emitter object
	GunEmitter *turret = NULL;
	ParticleEmitter explEmitter;
	ofImage playerImage; //image to draw on top of player
	ofImage enemyImage; //image for agents
	ofImage bulletImage;
	ofImage background; //background image
	ofImage gameOver;

	int points;	//score, based on time ellapsed
	int initEnergy;
	float speed; //player speed in px/s
	float enemySpeed;//enemy speed in px/s
	int enemyLimit; //number of enemies at a time
	float pxPerFrame; //player distance traveled, based on player speed
	float ratio_factor; //value to scale by
	int headingLength; //should be 75px
	map<int, bool> keymap;
	float time0;
	glm::vec3 mouse;	//where mouse is clicked

	//state variables
	bool within_shape = false; //inDrag
	bool rot_toggle = false; //ctrlKeyDown
	bool inRotate = false;
	bool canRecover = false;


	//UI Controls
	bool bHide;
	ofxIntSlider level_slider; //easy, normal, hard - is this an actual slider or do we just record 3 different settings?

	//player configuration
	ofxIntSlider energy_slider; //starting player health
	ofxFloatSlider speed_slider;
	ofxFloatSlider scale_slider;

	//graphics configuration
	ofxToggle toggleSprite;
	ofxToggle toggleHeading;

	//enemy configuration
	ofxIntSlider nEnemy;
	ofxFloatSlider rate;
	ofxFloatSlider life;
	//ofxVec3Slider velocity_slider; //for physics
	ofxVec3Slider force_slider; //for physics
	ofxFloatSlider mass_slider;		//for physics
	ofxFloatSlider damp_slider;		//for physics
	ofxFloatSlider angVel_slider;
	ofxFloatSlider angAcc_slider;
	ofxFloatSlider angFor_slider;
	ofxFloatSlider enemy_speed_slider;

	//physics
	//ofVec3f velocity; //velocity
	//ofVec3f acceleration; //acceleration
	//ofVec3f turbulence;
	ImpulseRadialForce* radialForce;


	//sounds
	ofSoundPlayer dmgSound;
	ofSoundPlayer explSound;
	ofSoundPlayer movSound;


	ofxLabel screenSize;
	ofxPanel gui; //Panel to draw all GUI components on

};
