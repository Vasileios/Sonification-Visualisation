#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxOsc.h"
#define HOST "localhost"
#define PORT 12345
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


        //this is our model we'll draw
        ofxAssimpModelLoader model;

        ofLight light;
        //ofTexture texture;
    ofImage texture;
    ofxOscReceiver receiver;
    ofPoint points [4];
    
    GLUquadricObj *quadric;

    int cornerIndex = 0;
        //we added these functions to make it easier to switch between the two methods of drawing
        void drawWithModel();
        void drawWithMesh();

};
