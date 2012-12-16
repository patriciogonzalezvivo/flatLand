#include "testApp.h"
#define STRINGIFY(A) #A

bool sortColor (const ofColor &a, const ofColor &b){
    return a.getBrightness() > b.getBrightness();
}

bool sortPtsByY( const ofPoint & a, const ofPoint & b){
    return a.y > b.y;
}


//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    gui.setup("panel");
    gui.add(horizon.setup("horizon",0.5,0.0,1.0));
    gui.add(threshold.setup("threshold",80.0f,0.0f,255.0f));
    gui.add(thresholdSmoothing.setup("threshold_Smoothing",0.1f, 0.0f, 0.2f));
    gui.add(transitionSmoothing.setup("transition_Smoothing", 0.6f, 0.0f, 1.0f));
    
    bImage = sourceImage.loadImage("03.jpeg");
    offSetTexture.allocate(sourceImage.width,1, GL_RGB16F);
    offSet.allocate(sourceImage.width, 1, 3);
    targetFbo.allocate(sourceImage.getWidth(), sourceImage.getHeight()*2);
    
    string fragShader = STRINGIFY(uniform sampler2DRect offsetTexture;
                                  uniform sampler2DRect sourceTexture;
                                  uniform float height;
                                  uniform float horizon;
                                  uniform float transitionSmoothing;
                                  
                                  void main(){
                                      vec2 st = gl_TexCoord[0].st;
                                      
                                      float horizonLine = horizon * height;
                                      
                                      float actualOffset = texture2DRect(offsetTexture, vec2(st.x,0.5)).r;
                                      
                                      float horizonOffset = actualOffset * height;
                                      
                                      horizonOffset = clamp(horizonOffset,0.0,height);
                                      float offSet = st.y + horizonOffset - horizonLine;
                                      
                                      vec4 color = texture2DRect(sourceTexture, vec2(st.x, offSet));
                                      
                                      if ( (offSet > height) || (offSet < 0.0 )){
                                          color = vec4(0.0,0.0,0.0,0.0);
                                      }
                                      
                                      gl_FragColor = color;
                                  }
                                  );
    
    flatShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
    flatShader.linkProgram();
    
    
    medianShader.load("", "median.frag");
    
    bRecord = false;
}

void testApp::processImage(ofImage & input, int _threshold){
    processImage(input.getPixelsRef(), _threshold);
}

void  testApp::processImage(ofPixels & srcPixels, int _threshold){
    
    offsetPts.clear();
    
    int width = srcPixels.getWidth();
    int height = srcPixels.getHeight();
    
    for (int x = 0; x < width; x++){
        
        ofColor a = srcPixels.getColor(x, 0);
        //ofFloatColor prevOffSet = offSet.getColor(x, 0);
        //offSet.setColor(x, 0, ofFloatColor(0.0));
        
        
        offsetPts.push_back(ofPoint(x, 0));
        
        for(int y = 0; y < height; y++){
            
            ofFloatColor b = srcPixels.getColor(x, y);
            
            if (ColourDistance(a,b) > _threshold){
                
                //ofFloatColor newOffset;
                //newOffset.r = (float)y/(float)height;
                
                offsetPts[x].y = y;
                
                //newOffset.g = prevOffSet.r; 
                //newOffset.b = prevOffSet.g;
                
                //offSet.setColor(x, 0, newOffset);
                break;
            } else {
                a.lerp(b,thresholdSmoothing);
            }
        }
    }
    
    
    // now let's clean up offsetPts;
    
    // let's make a copy of the points to median filter into  (Can't do inplace)
    vector < ofPoint > offsetPointsCopy;
    offsetPointsCopy.reserve(offsetPts.size());
    
    // size of our kernel (take me +/- kernel size, so 3 = 1+ 3+3, 7 values).
    int kernelSize = 15;
    
    for (int i = 0; i < offsetPts.size(); i++){
        
        // fill an array with values around i.
        vector < ofPoint > median;
        for (int j = i - kernelSize; j <= i + kernelSize; j++){
            int jSafe = ofClamp(j, 0, offsetPts.size() -1);
            median.push_back(offsetPts[jSafe]);            
            
        }
        
        // sort the array
        sort(median.begin(), median.end(), sortPtsByY);
        
        // take the middle value
        ofPoint medVal = median[kernelSize];  // take the middle value;
        
        offsetPointsCopy[i] = medVal;
    }
    
    
    
    for (int x = 0; x < width; x++){
         ofFloatColor newOffset;
         newOffset.r = (float)offsetPointsCopy[x].y/(float)height;
        offSet.setColor(x, 0, newOffset);
    }

    
    

    offSetTexture.loadData(offSet);
}

//--------------------------------------------------------------
void testApp::update(){
    
    if (bImage){
        processImage(sourceImage.getPixelsRef(), threshold);
    } else {
        processImage(sourceVideo.getPixelsRef(), threshold);
        sourceVideo.update();
    }
    
    targetFbo.begin();
    ofClear(255);
    
    flatShader.begin();
    flatShader.setUniformTexture("offsetTexture", offSetTexture, 0);
    flatShader.setUniform1f("transitionSmoothing", transitionSmoothing);
    flatShader.setUniform1f("horizon", horizon);
    
    if (bImage){
        flatShader.setUniformTexture("sourceTexture", sourceImage.getTextureReference(), 1);
        flatShader.setUniform1f("height", (float)sourceImage.getHeight());
    } else {
        flatShader.setUniformTexture("sourceTexture", sourceVideo.getTextureReference(), 1);
        flatShader.setUniform1f("height", (float)sourceVideo.getHeight());
    }
    
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(targetFbo.getWidth(), 0); glVertex3f(targetFbo.getWidth(), 0, 0);
    glTexCoord2f(targetFbo.getWidth(), targetFbo.getHeight()); glVertex3f(targetFbo.getWidth(), targetFbo.getHeight(), 0);
    glTexCoord2f(0,targetFbo.getHeight());  glVertex3f(0,targetFbo.getHeight(), 0);
    glEnd();
    
    flatShader.end();
    targetFbo.end();
    
    if (bRecord){
        ofImage newFrame;
        newFrame.allocate(targetFbo.getWidth(), targetFbo.getHeight(), OF_IMAGE_COLOR_ALPHA);
        ofPixels pixels;
        targetFbo.readToPixels(pixels);
        newFrame.setFromPixels(pixels);
        newFrame.saveImage( ofToString(ofGetFrameNum())+".png" );
        sourceVideo.nextFrame();
    }
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
    ofPushMatrix();
    ofScale(0.5, 0.5);
    if(bImage){
        sourceImage.draw(0, 0);
        targetFbo.draw(sourceImage.getWidth(),0);
    } else {
        sourceVideo.draw(0, 0);
        targetFbo.draw(sourceVideo.getWidth(),0);
    }
    
    ofSetColor(255,0,0);
    ofPolyline line;
    line.addVertices(offsetPts);
    line.draw();
    ofSetColor(255,255,255);
    
    ofPopMatrix();

    ofPushMatrix();
    ofTranslate(0, ofGetHeight()-10);
    ofScale(10, 10);
    offSetTexture.draw(0,0);
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
    
    if (key == ' '){
        sourceVideo.stop();
        bRecord = true;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if (key == ' '){
        bRecord = false;
        sourceVideo.play();
    }
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
        bImage = sourceImage.loadImage(dragInfo.files[0]);
        if (bImage){
            targetFbo.allocate(sourceImage.getWidth(), sourceImage.getHeight()*2);
            offSetTexture.clear();
            offSetTexture.allocate(sourceImage.width,1, GL_RGB16F);
            offSet.allocate(sourceImage.width, 1, 3);
        } else {
            if (sourceVideo.loadMovie(dragInfo.files[0])){
                sourceVideo.play();
                targetFbo.allocate(sourceVideo.getWidth(), sourceVideo.getHeight()*2);
                offSetTexture.clear();
                offSetTexture.allocate(sourceVideo.getWidth(),1, GL_RGB16F);
                offSet.allocate(sourceVideo.width, 1, 3);
            }
        }
    }

}