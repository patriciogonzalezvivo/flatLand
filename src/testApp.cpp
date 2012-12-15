#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    gui.setup("panel");
    gui.add(threshold.setup("threshold",80.0f,0.0f,255.0f));
    
    source.loadImage("03.jpeg");
    target.allocate(source.width, source.height*2, OF_IMAGE_COLOR);    
}

void testApp::processImage(ofImage & input, ofImage & output, int _threshold){
    processImage(input.getPixelsRef(), output, _threshold);
}

void  testApp::processImage(ofPixels & srcPixels, ofImage & output, int _threshold){
    
    int width = srcPixels.getWidth();
    int height = srcPixels.getHeight();
    
    vector < int > offSet;
    
    ofPixels trgPixels = output.getPixelsRef();
    
    //cout << output.getPixelsRef().getWidth() << endl;
    
    for (int x = 0; x < width; x++){
        for(int y = 0; y < height*2; y++){
            ofColor c = ofColor(255);
            trgPixels.setColor(x,y, c);
        }
    }
    
    for (int x = 0; x < width; x++){
        ofColor a = srcPixels.getColor(x, 0);
        offSet.push_back(0);
        
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
            
            int h = height*0.5 + y - offSet[x];
            h = ofClamp(h, 0, height *2 - 1);
            
            trgPixels.setColor(x, h, c );
        }
    }
    output.setFromPixels(trgPixels);
}

//--------------------------------------------------------------
void testApp::update(){
    processImage(source, target, threshold);
}

//--------------------------------------------------------------
void testApp::draw(){
    ofPushMatrix();
    ofScale(0.5, 0.5);
    source.draw(0, 0);
    target.draw(source.width,0);
    ofPopMatrix();
    
    gui.draw();
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
    if(key == 's') {
        gui.saveToFile("settings.xml");
    }
    
    if(key == 'l') {
        gui.loadFromFile("settings.xml");
    }
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