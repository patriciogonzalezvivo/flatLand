#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    offSet = NULL;
}

void testApp::processImage(string _imageFile, int _threshold){
    if(offSet != NULL)
        delete [] offSet;
    
    source.loadImage(_imageFile);
    int width = source.getWidth();
    int height = source.getHeight();
    target.allocate(width, height*2, OF_IMAGE_COLOR);
    offSet = new int[width];
    
    ofPixels srcPixels = source.getPixelsRef();
    ofPixels trgPixels = target.getPixelsRef();

    for (int x = 0; x < width; x++){
        for(int y = 0; y < height*2; y++){
            ofColor c = ofColor(255);
            trgPixels.setColor(x,y, c);
        }
    }
    
    for (int x = 0; x < width; x++){
        ofColor a = srcPixels.getColor(x, 0);
        offSet[x] = 0;
        
        for(int y = 0; y < height; y++){
            ofColor b = srcPixels.getColor(x, y);
            
            if (ColourDistance(a,b) > _threshold){
                
                offSet[x] = y;
                
                break;
                
            }
        }
    }
    
    for (int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            ofColor c = srcPixels.getColor(x, y);
            trgPixels.setColor(x, height*0.5 + y - offSet[x], c );
        }
    }
    
    target.setFromPixels(trgPixels);
    target.update();
}

//--------------------------------------------------------------
void testApp::update(){
    processImage("03.jpeg", ofMap(mouseX,0,ofGetWidth(),0,255));
}

//--------------------------------------------------------------
void testApp::draw(){
    ofScale(0.5, 0.5);
    source.draw(0, 0);
    target.draw(source.width,0);
}

//  Take from http://www.compuphase.com/cmetric.htm
//
double testApp::ColourDistance(ofColor e1, ofColor e2){
    long rmean = ( (long)e1.r + (long)e2.r ) / 2;
    long r = (long)e1.r - (long)e2.r;
    long g = (long)e1.g - (long)e2.g;
    long b = (long)e1.b - (long)e2.b;
    
    return sqrt((((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8));
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}