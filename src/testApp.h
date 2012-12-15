#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void    processImage(ofPixels & input, ofImage & output, int _threshold);
    void    processImage(ofImage & input, ofImage & output, int _threshold);
    double  ColourDistance(ofColor _src, ofColor _dst);
    
	ofImage source;
    ofImage target;

    vector < float > offsetSmooth;
    
    ofxPanel gui;
    ofxFloatSlider threshold;
    
    string fileName;

    ofVideoPlayer player;
};
