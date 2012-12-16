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

    void            processImage(ofPixels & input, int _threshold);
    void            processImage(ofImage & input, int _threshold);
    double          ColourDistance(ofColor _src, ofColor _dst);
    
	ofImage         sourceImage;
    ofVideoPlayer   sourceVideo;
    
    vector < ofPoint > offsetPts;
    
    
    ofFloatPixels   offSet;
    ofTexture       offSetTexture;
    ofShader        flatShader;
    ofShader        medianShader;
    ofFbo           targetFbo;

    ofxPanel        gui;
    ofxFloatSlider  horizon;
    ofxFloatSlider  threshold;
    ofxFloatSlider  thresholdSmoothing;
    ofxFloatSlider  transitionSmoothing;
    
    bool            bImage;
    bool            bRecord;
};
