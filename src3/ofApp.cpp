#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){




    receiver.setup(PORT);
    cout << "listening for osc messages on port " << PORT << "\n";
    ofSetFrameRate(60);


    //---goes at the end
    
    
   	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(0);

	//we need to call this for textures to work on models
    ofDisableArbTex();
   // ofLoadImage(texture, "earthTex.jpg");
    texture.load("earthTex.jpg");

	//this makes sure that the back of the model doesn't show through the front
	ofEnableDepthTest();

	//now we load our model
	model.loadModel("dog/earth_tex2.fbx");
	model.setPosition(ofGetWidth()*.5, ofGetHeight() * 0.75, 0);
    model.enableColors();
    glEnable(GL_POINT_SMOOTH);
    glPointSize(2);
    //model.addColor();
	light.enable();
    light.setPosition(model.getPosition() + ofPoint(0, 0, 1600));
    //prepare quadric for sphere
    quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
}



//--------------------------------------------------------------
void ofApp::update(){
    

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(210, 20, 210, 255);
    //if (texture.isAllocated()()) {
        // draw video into fbo
      //  model.begin();}
        
        int alpha = 255; // amount of smoothing
       ofEnableAlphaBlending();
        ofSetColor(255, 255, 255, alpha);
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2, 0);

    ofRotateY(ofGetFrameNum());
    ofRotateX(-90); //north pole facing up
    //bind and draw texture
    texture.getTextureReference().bind();
    gluSphere(quadric, 200, 100, 100);
        texture.draw(0, 0);

        // draw image into fbo
        ofDisableAlphaBlending();
    //    model.end();
    
	//first let's just draw the model with the model object
	//drawWithModel();
	//then we'll learn how to draw it manually so that we have more control over the data
	drawWithMesh();
}

//draw the model the built-in way
void ofApp::drawWithModel(){

	//get the position of the model
	ofVec3f position = model.getPosition();

	//save the current view
	ofPushMatrix();

	//center ourselves there
	//ofTranslate(position);
    ofTranslate(ofGetWindowWidth()/2, ofGetWindowWidth() /2, 0);
    ofSetColor(120, 20, 120, 60);
	//ofRotate(-ofGetMouseX(), 0, 1, 0);
	//ofRotate(90,1,0,0);
	//ofTranslate(-position);
    ofRotateY(ofGetFrameNum());
    ofRotateX(-90); //north pole facing up

    //bind and draw texture
    texture.getTextureReference().bind();
    gluSphere(quadric, 200, 100, 100);
    texture.draw(0, 0);
    //texture.getTextureReference().bind();
    // now draw
	//draw the model
    //texture.draw(0, 0);
	model.drawFaces();

	//restore the view position
    ofPopMatrix();
}

//draw the model manually
void ofApp::drawWithMesh(){
/*
	//get the model attributes we need
	ofVec3f scale = model.getScale();
	ofVec3f position = model.getPosition();
	float normalizedScale = model.getNormalizedScale();
	ofVboMesh mesh = model.getMesh(0);
	ofTexture texture;
    ofxAssimpMeshHelper& meshHelper = model.getMeshHelper( 0 );
    bool bHasTexture = meshHelper.hasTexture();
    if( bHasTexture ) {
        texture = model.getTextureForMesh(0);
    }

	ofMaterial material = model.getMaterialForMesh(0);

    ofPushMatrix();

	//translate and scale based on the positioning.
	ofTranslate(position);
	ofRotate(-ofGetMouseX(), 0, 1, 0);
	ofRotate(90,1,0,0);


	ofScale(normalizedScale, normalizedScale, normalizedScale);
//	ofScale(scale.x,scale.y,scale.z);

	//modify mesh with some noise
	float liquidness = 5;
	//float amplitude = mouseY/100.0;
    float amplitudem.getArgAsFloat(0)
    cout << amplitude << endl;
	float speedDampen = 5;
	vector<ofVec3f>& verts = mesh.getVertices();
	for(unsigned int i = 0; i < verts.size(); i++){
		verts[i].x += ofSignedNoise(verts[i].x/liquidness, verts[i].y/liquidness,verts[i].z/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
		verts[i].y += ofSignedNoise(verts[i].z/liquidness, verts[i].x/liquidness,verts[i].y/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
		verts[i].z += ofSignedNoise(verts[i].y/liquidness, verts[i].z/liquidness,verts[i].x/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
	}

	//draw the model manually
	if(bHasTexture) texture.bind();
	material.begin();
	//mesh.drawWireframe(); //you can draw wireframe too
	mesh.drawFaces();
	material.end();
	if(bHasTexture) texture.unbind();

	ofPopMatrix();

    */
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        cout << "got message from OSC\n";
        
        
        cout << "message was data as expected\n";
        //   cout << m.getArgAsFloat(0) << endl;
           //  cout << m.getArgAsFloat(1) << endl;
       //  ofVec2f eventPos = ofVec2f(m.getArgAsFloat(0), m.getArgAsFloat(1));
         //ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
         //ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
         //addToFluid(mouseNorm, mouseVel, true, true);
         //pMouse = eventPos;
     
        
        // if (m.getAddress() == "/data") {
        //   cout << "message was vertex as expected\n";
        // cout << m.getArgAsFloat(0) << endl;
        //cout << m.getArgAsFloat(1) << endl;
        //cout << m.getArgAsFloat(2) << endl;
        //cout << m.getArgAsFloat(3) << endl;
        //cout << m.getArgAsFloat(4) << endl;
        //cout << m.getArgAsFloat(5) << endl;
        
        //get the model attributes we need
        ofVec3f scale = model.getScale();
        ofVec3f position = model.getPosition();
        float normalizedScale = model.getNormalizedScale();
        ofVboMesh mesh = model.getMesh(0);
        ofTexture texture;
        ofxAssimpMeshHelper& meshHelper = model.getMeshHelper( 0 );
        bool bHasTexture = meshHelper.hasTexture();
        if( bHasTexture ) {
            texture = model.getTextureForMesh(0);
        }
        
        ofMaterial material = model.getMaterialForMesh(0);
        
        ofPushMatrix();
        
        //translate and scale based on the positioning.
       // ofTranslate(position);
        //  ofRotate(-ofGetMouseX(), 0, 1, 0);
        //ofRotate(90,1,0,0);
        
        ofSetColor(120, 120, 120, 255);
        //ofScale(normalizedScale, normalizedScale, normalizedScale);
        //ofScale(scale.x,scale.y,scale.z);
        if (m.getAddress() == "/data") {
            ofScale(normalizedScale, normalizedScale, normalizedScale);
            ofScale(scale.x,scale.y,scale.z);
            //modify mesh with some noise
            float liquidness = 5;
             float speedDampen = 5;
            float amplitude = m.getArgAsFloat(0)*0.2;

            //float speedDampen = m.getArgAsFloat(1);
            vector<ofVec3f>& verts = mesh.getVertices();
            for(unsigned int i = 0; i < verts.size(); i++){
                ofSetColor(220, 124, 220, 220);
                verts[i].x += ofSignedNoise(verts[i].x/liquidness, verts[i].y/liquidness,verts[i].z/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
                verts[i].y += ofSignedNoise(verts[i].z/liquidness, verts[i].x/liquidness,verts[i].y/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
                verts[i].z += ofSignedNoise(verts[i].y/liquidness, verts[i].z/liquidness,verts[i].x/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
            }
        }
        //draw the model manually
        if(bHasTexture) texture.bind();
        material.begin();
        //mesh.drawWireframe(); //you can draw wireframe too
        mesh.drawFaces();
        material.end();
        if(bHasTexture) texture.unbind();
        
        ofPopMatrix();
    }
    
    }

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
