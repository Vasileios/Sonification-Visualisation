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
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        if(timers[i] < ofGetElapsedTimef()){
            msg_strings[i] = "";
            
        }
    }
   
//}

    /*
    ofVec2f eventPos = ofVec2f(x, y);
        ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
        ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
        addToFluid(mouseNorm, mouseVel, true, true);
        pMouse = eventPos;
        //receiver.getNextMessage(&m);
        
}
    }
     */
    
    // check for waiting messages
    /*
     // check for waiting messages
     while(receiver.hasWaitingMessages()){
     // get the next message
     ofxOscMessage m;
     receiver.getNextMessage(m);
     
     // check for mouse moved message
     if(m.getAddress() == "/mouse/position"){
     // both the arguments are int32's
     mouseX = m.getArgAsInt32(0);
     mouseY = m.getArgAsInt32(1);
     }
     // check for mouse button message
     else if(m.getAddress() == "/mouse/button"){
     // the single argument is a string
     mouseButtonState = m.getArgAsString(0);
     }
     // check for an image being sent (note: the size of the image depends greatly on your network buffer sizes - if an image is too big the message won't come through )
     else if(m.getAddress() == "/image" ){
     ofBuffer buffer = m.getArgAsBlob(0);
     receivedImage.load(buffer);
     }
     else{
     // unrecognized message: display on the bottom of the screen
     string msg_string;
     msg_string = m.getAddress();
     msg_string += ": ";
     for(int i = 0; i < m.getNumArgs(); i++){
     // get the argument type
     msg_string += m.getArgTypeName(i);
     msg_string += ":";
     // display the argument - make sure we get the right type
     if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
     }
     else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
     }
     else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
     }
     else{
					msg_string += "unknown";
     }
     }

        if (m.getAddress() == "/datastart") cout << "datastart" << endl;
        
        if (m.getAddress() == "/datastart")cout << "datastart " << m.getArgAsFloat(0) << endl;
        
     
        if (m.getAddress() == "/horizontal/x") {
            oscX = m.getArgAsFloat(0);
        }
        if (m.getAddress() == "/vertical/x")
      
     oscY = m.getArgAsFloat(0);
     */
    
    
    if(resizeFluid) 	{
        fluidSolver.setSize(fluidCellsX, fluidCellsX / msa::getWindowAspectRatio());
        fluidDrawer.setup(&fluidSolver);
        resizeFluid = false;
    }
    
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
    
    // osc receive from SuperCollider
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
       //for(int i = 0; i < m.getNumArgs(); i++){
    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
    }
       // m.getAddress();
        //m.getArgType(current_msg_strings);
        //m.getArgAsString(current_msg_strings);
     //   mousex = m.getArgAsChar(current_msg_strings);
       // mousey = m.getArgAsChar(current_msg_strings);
      //  mousex = m.getArgAsInfinitum(current_msg_strings);
        //mousey = m.getArgAsInfinitum(current_msg_strings);
    //}
    

            int x, y;
            //mousex = m.getArgAsInt(0);
            //mousey = m.getArgAsInt(0);
        //m.getArgAsInt32(x);
         //m.getArgAsInt32(y);
     //  m.getArgAsInt(x);
      // m.getArgAsInt(y);
    //m.getArgAsBlob(x);
   // m.getArgAsBlob(y);
   // m.getArgAsInt64(x);
    //m.getArgAsInt64(y);
    //m.getNumArgs();
           // if (m.getAddress() == "/data") {
                receiver.getNextMessage(&m);
       // m.getArgAsFloat(0);
                //mousex = m.getArgAsInt(current_msg_strings);
                //mousey = m.getArgAsInt(current_msg_strings);
               // mousex = m.getArgAsInt(2);
                //mousey = m.getArgAsInt(3);
                
                ofVec2f eventPos = ofVec2f(m.getArgAsFloat(0), m.getArgAsFloat(1));
        ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
        ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
        addToFluid(mouseNorm, mouseVel, true, true);
        pMouse = eventPos;
        cout << m.getArgAsFloat(0) << endl;
        string msg_string = m.getAddress();
        cout << msg_string << endl;
   // x = m.getArgType(current_msg_strings);
    cout << msg_string << endl;
        }
    
       //  }
/*
        string msg_string;
        msg_string = m.getAddress();
        msg_string += ": ";
        for(int i = 0; i < m.getNumArgs(); i++){
            // get the argument type
            msg_string += m.getArgTypeName(i);
            msg_string += ":";
            // display the argument - make sure we get the right type
            if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                msg_string += ofToString(m.getArgAsInt32(i));
              //  int x, y;
                
             //   mouseX = x;
                
 
                ofVec2f eventPos = ofVec2f(x, y);
                ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
                ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
                addToFluid(mouseNorm, mouseVel, true, true);
                pMouse = eventPos;

            }
            else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                msg_string += ofToString(m.getArgAsFloat(i));
             //   float x, y;
                
               // mouseX = x;
                
                ofVec2f eventPos = ofVec2f(x, y);
                ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
                ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
                addToFluid(mouseNorm, mouseVel, true, true);
                pMouse = eventPos;

            }
            else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                msg_string += m.getArgAsString(i);
            //    string x, y;
                
                //mouseX = x;
                
                ofVec2f eventPos = ofVec2f(x, y);
                ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
                ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
                addToFluid(mouseNorm, mouseVel, true, true);
                pMouse = eventPos;

            }
            else{
                msg_string += "unknown";
            }
        }
*/
        // ofxOscMessage m;
        //string msg_string;
        //m.setAddress("/data");
        //  float x = 0.0;
        // float y = 0.0;
        //m.addFloatArg(x);
        //m.addFloatArg(y);
        //m.getArgAsFloat(x);
        //m.getArgAsFloat(y);
        //m.getArgAsFloat(z);
        // m.addInt32Arg(x);
        //m.addInt64Arg(x);
        //m.addIntArg(x);
        // m.getArgAsInt(x);
        // m.getArgAsInt(y);
        // m.getArgAsChar(x);
        //m.getArgAsChar(y);
        // m.getArgAsBlob(x);
        //m.getArgAsBlob(y);
        
        //while (m.getAddress() == "/data") {
        
        
       //m.setAddress("/data");
       // m.addStringArg("/data");
       // m.addStringArg(",");
        //m.addStringArg("mirror2_upperBoundary");
        //m.addStringArg(",");
        //m.addFloatArg(50.0f);
        //m.addFloatArg(50.0f);
        //m.addFloatArg(50.0f);
      //  while (m.getAddress() == "/data") {
            //string message = m.getArgAsString(0);
           //   int i = m.getArgAsInt32(6);
            //uint64_t j = m.getArgAsInt64(2);
            // float k = m.getArgAsInt(i);
            //    mouseX = m.getArgAsInt(0);
            //mouseY = m.getArgAsInt(0);
            //ofLog() << m.getArgAsString(0);
            // int x = m.getArgAsInt(0);
       // int x;
       // int y;
            // receiver.getNextMessage(&m);
             //  mouseX = m.getArgAsFloat(50.0f);
            // mouseY = m.getArgAsFloat(50.0f);
           // m.getArgAsInt(x);
            //m.getArgAsInt(y);
            
            // m.getArgAsBlob(x);
            //m.getArgAsBlob(y);
            
            
           //            receiver.getNextMessage(&m);
        //}
    //}
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

