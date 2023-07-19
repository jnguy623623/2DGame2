#include "ofApp.h"

/*
* CS 134 - Project 2: Dynamic Pursuit
* @author Jonathan Nguyen
* @version 11/6/2022
* 
* matoi.png, enemy.png, gradient.png, gameOver.png were created by me from scratch
* 
* 
*/

//--------------------------------------------------------------
void ofApp::setup() {

	if (!playerImage.load("images/matoi.png")) { //Sprite drawn by me
		cout << "Can't load image" << endl;
		ofExit();
	}

	if (!enemyImage.load("images/enemy.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}

	if (!bulletImage.load("images/grantBullet.png")) { //testBullet.png or grantBullet.png
		cout << "Can't load image" << endl;
		ofExit();
	}

	if (!background.load("images/gradient.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}

	if (!gameOver.load("images/gameOver.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}

	
	if (!movSound.load("sounds/charge.mp3")) {
		cout << "Can't load sound" << endl;
		ofExit();
	}
	
	if (!explSound.load("sounds/Explosion1.ogg")) {
		cout << "Can't load sound" << endl;
		ofExit();
	}

	if (!dmgSound.load("sounds/Damage5.ogg")) {
		cout << "Can't load sound" << endl;
		ofExit();
	}

	//misc data
	ratio_factor = 1; //factor to scale by
	headingLength = 75; //in px
	player.center = glm::vec3(player.pos.x, player.pos.y, 0);
	player.headingPt = player.center + player.heading() * headingLength;
	speed = 300; //initial speed. Divide by 60 to get px moved per frame
	enemyLimit = 1;
	enemySpeed = 60;
	time0 = ofGetElapsedTimeMillis(); //will be used for scoring

	//starting player data
	player.pos = glm::vec3(ofGetWindowWidth() / 2.0 - 100, ofGetWindowHeight() / 2.0 - 100, 0);
	player.rot = 0;
	player.scale = glm::vec3(1 * ratio_factor, 1 * ratio_factor, 1);//x and y can be multiplied by ratio factor
	player.setImage(playerImage);
	
	// create an array of emitters and set their position;
	//
	turret = new GunEmitter(&player);
	turret->setGroupSize(3);
	turret->pos = glm::vec3(0, 0, 0);
	turret->drawable = false; //set to false if we don't want to see a blu square
	turret->setChildImage(bulletImage); //set image of particles/agents
	turret->start();	//start spawning entities


	//emitters and agents
	spawner = new AgentEmitter(&player);
	spawner->drawable = false;
	spawner->setChildImage(enemyImage);

	//explosion emitter
	radialForce = new ImpulseRadialForce(100);
	explEmitter.sys->addForce(radialForce);
	explEmitter.setVelocity(ofVec3f(0, 0, 0));
	explEmitter.setOneShot(true);
	explEmitter.setEmitterType(RadialEmitter);
	explEmitter.setGroupSize(50);
	//explEmitter.pos = player.pos;


	//GUI related data
	gui.setup();
	gui.add(energy_slider.setup("Init Energy", 4, 1, 10));
	gui.add(speed_slider.setup("Speed", speed, 60, 1200));
	gui.add(scale_slider.setup("Scale", ratio_factor, 0.25, 10));
	gui.add(toggleSprite.setup("Sprite", false));
	gui.add(toggleHeading.setup("Heading", false));
	gui.add(nEnemy.setup("nEnemy", enemyLimit, 1, 12));
	gui.add(rate.setup("Rate", 1, 0.5, 10));
	gui.add(life.setup("Life", 1, 0, 10));
	gui.add(enemy_speed_slider.setup("Enemy Speed", enemySpeed, 60, 1200));
	//gui.add(velocity_slider.setup("Velocity", ofVec3f(100, 100, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(force_slider.setup("Force", ofVec3f(0, 0, 0), ofVec3f(-500, -500, 0), ofVec3f(500, 500, 0)));
	gui.add(mass_slider.setup("Mass", 500, 1, 1000));
	gui.add(damp_slider.setup("Damping", 0.99, 0.01, 1));

	gui.add(angVel_slider.setup("Angular Velocity", 10, 0, 50));
	gui.add(angAcc_slider.setup("Angular Acceleration", 10, 0, 50));
	gui.add(angFor_slider.setup("Angular Force", 10, 0, 50));
	bHide = false; //GUI displayed

	player.energy = energy_slider;
	//movSound.setVolume();
}


void ofApp::exit() {
	//exit stuff happens here
}

//--------------------------------------------------------------
void ofApp::update() {
	
	if (spawner->started == false && player.energy > 0) {
		player.energy = energy_slider;
		initEnergy = player.energy;
	}

	//misc data
	player.center = glm::vec3(player.pos.x, player.pos.y, 0);
	player.headingPt = player.center + headingLength * player.heading();
	speed = speed_slider;
	enemySpeed = enemy_speed_slider;
	ratio_factor = scale_slider;
	player.scale = glm::vec3(1 * ratio_factor, 1 * ratio_factor, 1);

	if (ofGetFrameRate() > 0) {
		pxPerFrame = speed / ofGetFrameRate();
	}
	else pxPerFrame = speed;

	if (toggleSprite == true) {
		player.bShowImage = true;
		for (auto& a : spawner->sys->sprites) {
			a.bShowImage = true;
		}
		
		for (auto& b : turret->sys->sprites) {
			b.bShowImage = true;
		}
		
	}
	else {
		player.bShowImage = false;
		for (auto& a : spawner->sys->sprites) {
			a.bShowImage = false;
		}
		
		for (auto& b : turret->sys->sprites) {
			b.bShowImage = false;
		}
		
		
	}

	spawner->setRate(rate);
	spawner->setLifespan(life * 1000);
	spawner->setSpeed(enemySpeed);
	spawner->setGroupSize(nEnemy);
	spawner->setPhysics(ofVec3f(force_slider), mass_slider, 
		damp_slider, angVel_slider, angAcc_slider, angFor_slider);
	spawner->update(); //create, kill, move, whatever
	spawner->pos = glm::vec3(ofRandom(0, ofGetWindowWidth()), ofRandom(0, ofGetWindowHeight()), 0);

	turret->setRate(2);
	turret->setLifespan(3000);
	turret->update();
	turret->pos = player.pos;

	for (auto& a : spawner->sys->sprites) {
		if (player.checkCollision(&a) == true) {
			dmgSound.play();
		}
		for (auto& b : turret->sys->sprites) {	//probably set this up in sprite.h and .cpp


			if (b.checkCollision(&a) == true) { //this is using sprite class' method and not bullet
				//b.emitter->start()
				b.lifespan = 0;
				ParticleEmitter *cloneEmitter = &explEmitter;
				explEmitter.pos = a.pos;
				cloneEmitter->pos = explEmitter.pos;
				for (auto& e : cloneEmitter->sys->particles) { //explEmitter.sys
					//e.position = b.pos;
					e.position.set(ofVec3f(a.pos));
					
				}
				cloneEmitter->start(); //explEmitter
				explSound.play();
				canRecover = true;
			}
		}
	}
	if (canRecover == true && player.energy < initEnergy) {
		player.energy++;
		canRecover = false;
	}

	if (spawner->started == true) {
		float time = ofGetElapsedTimeMillis() - time0;
		if (time >= 1000) {
			points++;
			time0 = ofGetElapsedTimeMillis();
		}
	}

	explEmitter.setLifespan(1);
	explEmitter.setVelocity(ofVec3f(50, 50, 50));
	explEmitter.setRate(1);
	explEmitter.setParticleRadius(5);
	explEmitter.update();



	//movement and movement sounds
	if (keymap[OF_KEY_LEFT]) {
		player.rot -= 6;
	}
	if (keymap[OF_KEY_RIGHT]) {
		player.rot += 6;
	}

	if (keymap[OF_KEY_UP]) {
		//move forward
		glm::vec3 offset = (player.heading() * pxPerFrame);
		player.pos += offset;
		if (player.checkBounds(player.pos, offset) == false && player.checkBounds(player.pos, -offset) == false) {
			player.pos -= 2 * offset; //bounce back
		}

		if (!movSound.getIsPlaying()) {
			movSound.play();
		}
	
	}
	else if (!keymap[OF_KEY_UP] && movSound.getIsPlaying() == false) {
		movSound.stop();
	}

	if (keymap[OF_KEY_DOWN]) {
		//move backwards
		glm::vec3 offset = (player.heading() * pxPerFrame);
		player.pos -= offset;
		if (player.checkBounds(player.pos, offset) == false && player.checkBounds(player.pos, -offset) == false) {
			player.pos += 2 * offset; //bounce back
		}

		if (!movSound.getIsPlaying()) {
			movSound.play();
		}

	}
	else if (!keymap[OF_KEY_DOWN] && movSound.getIsPlaying() == false) {
		movSound.stop();
	}

	//check game over
	if (player.energy == 0) {
		player.energy--;
		spawner->stop();
		time0 = 0;
		spawner->sys->sprites.clear();
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(ofColor::white);

	if (player.energy > 0) {
		background.draw(0, 0);
		if (!bHide) {
			gui.draw();
		}

	if (spawner->started == false && player.energy > 0) {
		ofSetColor(ofColor::black);
		ofDrawBitmapString("Press [Space Bar] to Start", ofGetWindowWidth()/2 - 100, ofGetWindowHeight()/2);
	}

	ofSetColor(ofColor::white);
	ofDrawBitmapString("Current Energy: " + ofToString(player.energy), 220, 25);
	ofDrawBitmapString("Firing Mode: " + ofToString(turret->mode), 220, 45);
	ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), ofGetWindowWidth() - 100, 25);
	ofDrawBitmapString("Time: " + ofToString(points), ofGetWindowWidth() - 100, 45);


	ofPushMatrix();
	ofMultMatrix(player.getMatrix());

	if (toggleSprite == true) { //there is probably some weird error here. Invincible because this actually isn't where the player is, but rather the image
		playerImage.draw(-playerImage.getWidth() / 2, -playerImage.getHeight() / 2);
		ofPopMatrix();
		spawner->drawable = false;
		spawner->draw();
		turret->draw();
		explEmitter.draw();
	}
	else {
		ofPopMatrix();
		player.draw();
		spawner->draw(); //if sprites are disabled
		turret->draw();
		explEmitter.draw();
	}

		if (toggleHeading == true) {
			ofDrawLine(player.center, player.headingPt);
			for (auto& s : spawner->sys->sprites) {
				ofDrawLine(s.pos, s.pos+s.heading()*headingLength);
			}
		}
	}

	//else we're dead
	else { //just display game over and restart instructions here
		ofSetColor(ofColor::white);
		gameOver.draw(0,0);
		ofDrawBitmapString("Time: " + ofToString(points), ofGetWindowWidth() / 2 -50, ofGetWindowHeight() / 2);
		ofDrawBitmapString("[R] to restart", ofGetWindowWidth() / 2-50, ofGetWindowHeight() / 2 + 25);
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	//do keymapping here
	switch (key) {
	case(OF_KEY_CONTROL):
		rot_toggle = true;
		break;
	case 'R':
	case 'r':
		//restart game here
		if (player.energy <= 0 && spawner->started == false) {
			player.energy = energy_slider;
			points = 0;
		}
		break;
	case 'H':
	case 'h':
		bHide = !bHide;

		break;
	case 'X':
	case 'x':
		turret->changeMode2();
		break;
	case 'Z':
	case 'z':
		turret->changeMode();
		break;

	case ' ':
		spawner->start();
		turret->start();
		break;
	default:
		break;
	}

	keymap[key] = true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

	//do keymapping here
	keymap[key] = false;

	switch (key) {
	case(OF_KEY_CONTROL):
		rot_toggle = false;
		inRotate = false;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

	glm::vec3 m = glm::vec3(x, y, 0);

	if (player.inside(m)) {
		ofSetColor(ofColor::lightGreen);
	}
	else ofSetColor(ofColor::darkCyan);

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	glm::vec3 m = glm::vec3(x, y, 0);

	if (within_shape) {
		glm::vec3 offset = m - mouse;
		player.pos += offset;
		mouse = m;
	}
	else if (inRotate) {
		glm::vec3 offset = m - mouse;
		player.rot += offset.x / 2.0;
		mouse = m;
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	mouse = glm::vec3(x, y, 1);

	if (player.inside(mouse)) {
		if (rot_toggle) {
			inRotate = true;
			mouse = glm::vec3(x, y, 0);

		}
		else {
			within_shape = true;
			inRotate = false;
			mouse = glm::vec3(x, y, 0);
		}

		ofSetColor(ofColor::magenta);

	}
	else {
		ofSetColor(ofColor::darkCyan);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	glm::vec3 m = glm::vec3(x, y, 1);
	within_shape = false;

	if (player.inside(m)) {
		ofSetColor(ofColor::lightGreen);

	}
	else ofSetColor(ofColor::darkCyan);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}