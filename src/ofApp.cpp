/*
 VA0517-MSAFuids data visualisation
 Receive OSC from SuperCollider
 */
#include "ofApp.h"

char sz[] = "[Rd9?-2XaUP0QY[hO%9QTYQ`-W`QZhcccYQY[`b";


float tuioXScaler = 1;
float tuioYScaler = 1;

//--------------------------------------------------------------
void ofApp::setup(){
    
    receiver.setup(PORT);
    cout << "listening for osc messages on port " << PORT << "\n";
    ofSetFrameRate(60);
    current_msg_strings = 0;
    mouseX = 0;
    mouseY = 0;
    for(int i=0; i<strlen(sz); i++) sz[i] += 20;
    
    // setup fluid stuff
    fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.002).setDeltaT(0.5).setVisc(0.00015).setColorDiffusion(0);
    fluidDrawer.setup(&fluidSolver);
    
    fluidCellsX			= 150;
    
    drawFluid			= true;
    drawParticles		= true;
    
    ofSetFrameRate(60);
    ofBackground(0, 0, 0);
    ofSetVerticalSync(false);
    
#ifdef USE_TUIO
    tuioClient.start(3333);
#endif
    
    
#ifdef USE_GUI
    gui.addSlider("fluidCellsX", fluidCellsX, 20, 400);
    gui.addButton("resizeFluid", resizeFluid);
    gui.addSlider("colorMult", colorMult, 0, 100);
    gui.addSlider("velocityMult", velocityMult, 0, 100);
    gui.addSlider("fs.viscocity", fluidSolver.viscocity, 0.0, 0.01);
    gui.addSlider("fs.colorDiffusion", fluidSolver.colorDiffusion, 0.0, 0.0003);
    gui.addSlider("fs.fadeSpeed", fluidSolver.fadeSpeed, 0.0, 0.1);
    gui.addSlider("fs.solverIterations", fluidSolver.solverIterations, 1, 50);
    gui.addSlider("fs.deltaT", fluidSolver.deltaT, 0.1, 5);
    gui.addComboBox("fd.drawMode", (int&)fluidDrawer.drawMode, msa::fluid::getDrawModeTitles());
    gui.addToggle("fs.doRGB", fluidSolver.doRGB);
    gui.addToggle("fs.doVorticityConfinement", fluidSolver.doVorticityConfinement);
    gui.addToggle("drawFluid", drawFluid);
    gui.addToggle("drawParticles", drawParticles);
    gui.addToggle("fs.wrapX", fluidSolver.wrap_x);
    gui.addToggle("fs.wrapY", fluidSolver.wrap_y);
    gui.addSlider("tuioXScaler", tuioXScaler, 0, 2);
    gui.addSlider("tuioYScaler", tuioYScaler, 0, 2);
    
    gui.currentPage().setXMLName("ofxMSAFluidSettings.xml");
    gui.loadFromXML();
    gui.setDefaultKeys(true);
    gui.setAutoSave(true);
    gui.show();
#endif
    
    windowResized(ofGetWidth(), ofGetHeight());		// force this at start (cos I don't think it is called)
   pMouse = msa::getWindowCenter();// change here with msg osc from SuperCollider
   // receiver = msa::getWindowCenter();
    resizeFluid			= true;
    
    ofEnableAlphaBlending();
    ofSetBackgroundAuto(false);
    
    
    ///////------mesh here
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    mesh.enableColors();
    
    ofVec3f  v0(-100, -100, -100);
    ofVec3f v1(100, -100, -100);
    ofVec3f v2(100, 100, -100);
    ofVec3f v3(-100, 100, -100);
    ofVec3f v4(-100, -100, 100);
    ofVec3f v5(100, -100, 100);
    ofVec3f v6(100, 100, 100);
    ofVec3f v7(-100, 100, 100);
    ofVec3f v8(-150, 140, 300);
    ofVec3f v9(-190, 130, 320);
    ofVec3f v10(-200, 135, 360);
    ofVec3f v11(-230, 100, 380);
    ofVec3f v12(-250, 80, 390);
    ofVec3f v13(-300, 70, 410);
    //
    mesh.addVertex(v0);
    mesh.addColor(ofFloatColor(0.0, 0.0, 0.0));
    
    mesh.addVertex(v1);
    mesh.addColor(ofFloatColor(1.0, 0.0, 0.0));
    
    mesh.addVertex(v2);
    mesh.addColor(ofFloatColor(1.0, 1.0, 0.0));
    
    mesh.addVertex(v3);
    mesh.addColor(ofFloatColor(0.0, 1.0, 1.0));
    
    mesh.addVertex(v4);
    mesh.addColor(ofFloatColor(0.0, 0.0, 1.0));
    
    mesh.addVertex(v5);
    mesh.addColor(ofFloatColor(1.0, 0.0, 1.0));
    
    mesh.addVertex(v6);
    mesh.addColor(ofFloatColor(1.0, 1.0, 1.0));
    
    mesh.addVertex(v7);
    mesh.addColor(ofFloatColor(0.4, 0.8, 0.9));
    mesh.addVertex(v8);
    mesh.addColor(ofFloatColor(0.7, 1.0, 0.2));
    mesh.addVertex(v9);
    mesh.addColor(ofFloatColor(0.0, 1.0, 1.0));
    mesh.addVertex(v10);
    mesh.addColor(ofFloatColor(0.0, 1.0, 1.0));
    mesh.addVertex(v11);
    mesh.addColor(ofFloatColor(0.0, 1.0, 1.0));
    mesh.addVertex(v12);
    mesh.addColor(ofFloatColor(0.0, 1.0, 1.0));
    mesh.addVertex(v13);
    mesh.addColor(ofFloatColor(0.0, 1.0, 1.0));
    
    
    
/////////////////////////
}


void ofApp::fadeToColor(float r, float g, float b, float speed) {
    glColor4f(r, g, b, speed);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
}


// add force and dye to fluid, and create particles
void ofApp::addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce) {
    float speed = vel.x * vel.x  + vel.y * vel.y * msa::getWindowAspectRatio() * msa::getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if(speed > 0) {
        pos.x = ofClamp(pos.x, 0.0f, 1.0f);
        pos.y = ofClamp(pos.y, 0.0f, 1.0f);
        
        int index = fluidSolver.getIndexForPos(pos);
        
        if(addColor) {
            //			Color drawColor(CM_HSV, (getElapsedFrames() % 360) / 360.0f, 1, 1);
            ofColor drawColor;
            drawColor.setHsb((ofGetFrameNum() % 255), 255, 255);
            
            fluidSolver.addColorAtIndex(index, drawColor * colorMult);
            
            if(drawParticles)
                particleSystem.addParticles(pos * ofVec2f(ofGetWindowSize()), 10);
        }
        
        if(addForce)
            fluidSolver.addForceAtIndex(index, vel * velocityMult);
        
    }
}


//--------------------------------------------------------------
void ofApp::update(){
    // hide old messages
  //  for(int i = 0; i < NUM_MSG_STRINGS; i++){
    //    if(timers[i] < ofGetElapsedTimef()){
      //      msg_strings[i] = "";
            
       // }
   // }
    if(resizeFluid) 	{
        fluidSolver.setSize(fluidCellsX, fluidCellsX / msa::getWindowAspectRatio());
        fluidDrawer.setup(&fluidSolver);
        resizeFluid = false;
    }
   

    
    // osc receive from SuperCollider
    //while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        //for(int i = 0; i < m.getNumArgs(); i++){
        //for(int i = 0; i < NUM_MSG_STRINGS; i++){
        //  ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
        //}
        
        
       // int x, y;
        
        
        //if (m.getAddress() == "/data") {
            receiver.getNextMessage(&m);
            
            ofVec2f eventPos = ofVec2f(m.getArgAsFloat(0), m.getArgAsFloat(1));
            ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
            ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
            addToFluid(mouseNorm, mouseVel, true, true);
            pMouse = eventPos;
         
            cout << m.getArgAsFloat(0) << endl;
            cout << m.getArgAsFloat(1) << endl;

            //   string msg_string = m.getAddress();
            //     cout << msg_string << endl;
            // x = m.getArgType(current_msg_strings);
            //   cout << msg_string << endl;
       // }
        
    //}
   
#ifdef USE_TUIO
    tuioClient.getMessage();
    
    // do finger stuff
    list<ofxTuioCursor*>cursorList = tuioClient.getTuioCursors();
    for(list<ofxTuioCursor*>::iterator it=cursorList.begin(); it != cursorList.end(); it++) {
        ofxTuioCursor *tcur = (*it);
        float vx = tcur->getXSpeed() * tuioCursorSpeedMult;
        float vy = tcur->getYSpeed() * tuioCursorSpeedMult;
        if(vx == 0 && vy == 0) {
            vx = ofRandom(-tuioStationaryForce, tuioStationaryForce);
            vy = ofRandom(-tuioStationaryForce, tuioStationaryForce);
        }
        addToFluid(ofVec2f(tcur->getX() * tuioXScaler, tcur->getY() * tuioYScaler), ofVec2f(vx, vy), true, true);
    }
#endif
    
 
    
    
    fluidSolver.update();
}


//--------------------------------------------------------------
void ofApp::draw(){
    if(drawFluid) {
        ofClear(0);
        glColor3f(1, 1, 1);
        fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());
    } else {
        //		if(ofGetFrameNum()%5==0)
        fadeToColor(0, 0, 0, 0.01);
    }
    if(drawParticles)
        particleSystem.updateAndDraw(fluidSolver, ofGetWindowSize(), drawFluid);
   
    ///---mesh here
    cam.begin();
    cam.setNearClip(0);
    //glPushMatrix();
    //ofSetColor(23, 220, 210, 60);
    //model.drawVertices();
    
    ofPushMatrix();
    ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2);
    mesh.draw();
    ofPopMatrix();
    cam.end();
    //ofLine(30, 40, 1, 300, 400, 1);
    glLineWidth(4);
    /////////---------

    //	ofDrawBitmapString(sz, 50, 50);
#ifdef USE_GUI
    
    gui.draw();
#endif
    
    

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case '1':
            fluidDrawer.setDrawMode(msa::fluid::kDrawColor);
            break;
            
        case '2':
            fluidDrawer.setDrawMode(msa::fluid::kDrawMotion);
            break;
            
        case '3':
            fluidDrawer.setDrawMode(msa::fluid::kDrawSpeed);
            break;
            
        case '4':
            fluidDrawer.setDrawMode(msa::fluid::kDrawVectors);
            break;
            
        case 'd':
            drawFluid ^= true;
            break;
            
        case 'p':
            drawParticles ^= true;
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'r':
            fluidSolver.reset();
            break;
            
        case 'b': {
            //			Timer timer;
            //			const int ITERS = 3000;
            //			timer.start();
            //			for(int i = 0; i < ITERS; ++i) fluidSolver.update();
            //			timer.stop();
            //			cout << ITERS << " iterations took " << timer.getSeconds() << " seconds." << std::endl;
        }
            break;
            
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
/*
void ofApp::mouseMoved(int x, int y ){
    ofVec2f eventPos = ofVec2f(x, y);
    ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
    ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
    addToFluid(mouseNorm, mouseVel, true, true);
    pMouse = eventPos;
    cout << ofGetMouseX() << endl;
    cout << ofGetMouseY() << endl;

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    ofVec2f eventPos = ofVec2f(x, y);
    ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
    ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
    addToFluid(mouseNorm, mouseVel, false, true);
    pMouse = eventPos;
}

//----------------------------------------------------

 
//----------------------------------------------------
//--------------------------------------------------------------

void ofApp::receiveOscMessages(float x, float y, float z){
    ofxOscMessage m;
    m.setAddress("data");
    m.addFloatArg(x);
    m.addFloatArg(y);
    m.getArgAsFloat(x);
    m.getArgAsFloat(y);
    m.getArgAsFloat(z);
    

    m.getArgAsBlob(x);
    m.getArgAsBlob(y);
    ofVec2f eventPos = ofVec2f(x, y);
    ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
    ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
    addToFluid(mouseNorm, mouseVel, true, true);
    pMouse = eventPos;
    receiver.getNextMessage(m);
}
*/
//------------------------------------------------------

void ofApp::mousePressed(int x, int y, int button){
    
}

