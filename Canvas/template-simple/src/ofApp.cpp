#include "ofApp.h"

//-----------
void ofApp::setup(){
    ofSetBackgroundAuto(false);
    
    canvas.setup(1024, 768, 2, 2);
}

//-----------
void ofApp::update(){
    canvas.update();
}

//-----------
void ofApp::draw() {
    canvas.draw(0, 0);
}

//-----------
void ofApp::keyPressed(int key){
    if (key=='g')   canvas.toggleGuiVisible();
}