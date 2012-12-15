#include "testApp.h"

bool sortColor (const ofColor &a, const ofColor &b){
    return a.getBrightness() > b.getBrightness();
}

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    gui.setup("panel");
    gui.add(threshold.setup("threshold",80.0f,0.0f,255.0f));
    gui.add(topBottomSmoothing.setup("threshold_Smoothing",0.1f, 0.0f, 1.0f));
    gui.add(offSetSmoothing.setup("transition_Smoothing", 0.6f, 0.0f, 1.0f));
    
    
    bImage = true;
    source.loadImage("03.jpg");
    target.allocate(source.width, player.height*2, OF_IMAGE_COLOR);  
    for (int i = 0; i < source.width; i++)
        offsetSmooth.push_back(0);
}

void testApp::processImage(ofImage & input, ofImage & output, int _threshold){
    processImage(input.getPixelsRef(), output, _threshold);
}

void  testApp::processImage(ofPixels & srcPixels, ofImage & output, int _threshold){
    
    
    bool bUseMedian = true;
    vector < ofColor > colors;
    int kernelSize = 3;  // use me +/- kernel (so 4 = 4+4+1 = 9);
    
    
    int width = srcPixels.getWidth();
    int height = srcPixels.getHeight();
    
    vector < int > offSet;
    
    ofPixels trgPixels = output.getPixelsRef();
    
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
            
            ofColor b;
            b = srcPixels.getColor(x, y);
            
            
            if (ColourDistance(a,b) > _threshold){
                
//                colors.clear();
//                for (int k = (y - kernelSize); k <= (y+kernelSize); k++){
//                    // printf("checking %i \n", k);
//                    int kSafe = ofClamp(k, 0, height-1);
//                    colors.push_back(srcPixels.getColor(x, kSafe));
//                }
//                sort(colors.begin(), colors.end(), sortColor);
//                ofColor c = colors[0 + kernelSize];
//        
//                if (ColourDistance(a,c) > _threshold){
                    offSet[x] = y;
                    break;
                //}
                
            } else {
                a.lerp(b, topBottomSmoothing);
            }
        }
    }
    
    for (int i =0; i < width; i++){
        offsetSmooth[i] = offSetSmoothing * offsetSmooth[i] + (1.0f - offSetSmoothing ) * offSet[i];
    }
    
    for (int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
            ofColor c = srcPixels.getColor(x, y);
            
            int h = height*0.5 + y - offsetSmooth[x];
            h = ofClamp(h, 0, height *2 - 1);
            
            trgPixels.setColor(x, h, c );
        }
    }
    output.setFromPixels(trgPixels);
}

//--------------------------------------------------------------
void testApp::update(){
    
    if (bImage){
        processImage(source.getPixelsRef(), target, threshold);
    } else {
        processImage(player.getPixelsRef(), target, threshold);
        player.update();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofPushMatrix();
    ofScale(0.5, 0.5);
    if(bImage){
        source.draw(0, 0);
        target.draw(source.width,0);
    } else {
        player.draw(0, 0);
        target.draw(player.width,0);
    }
    
    ofPopMatrix();
    
    glDisable(GL_DEPTH);
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
    
    if(dragInfo.files.size() > 0){
        bImage = source.loadImage(dragInfo.files[0]);
        if (bImage){
            cout << "Loading Image " << dragInfo.files[0] << endl;
            target.allocate(source.width, source.height*2, OF_IMAGE_COLOR);
            offsetSmooth.clear();
            for (int i = 0; i < source.width; i++){
                offsetSmooth.push_back(0);
            }
        } else {
            
            if (player.loadMovie(dragInfo.files[0])){
                player.play();
                cout << "Loading Video " << dragInfo.files[0] << endl;
                target.allocate(player.width, player.height*2, OF_IMAGE_COLOR);
                offsetSmooth.clear();
                for (int i = 0; i < player.width; i++){
                    offsetSmooth.push_back(0);
                }
            }
        }
    }

}