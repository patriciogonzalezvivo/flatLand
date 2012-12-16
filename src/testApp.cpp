#include "testApp.h"
#define STRINGIFY(A) #A

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    gui.setup("panel");
    gui.add(horizon.setup("horizon",0.5,0.0,1.0));
    gui.add(threshold.setup("threshold",80.0f,0.0f,255.0f));
    gui.add(thresholdSmoothing.setup("threshold_Smoothing",0.1f, 0.0f, 1.0f));
    gui.add(transitionSmoothing.setup("transition_Smoothing", 0.6f, 0.0f, 1.0f));
    
    bImage = sourceImage.loadImage("03.jpeg");
    offSetTexture.allocate(sourceImage.width,1, GL_RGB32F);
    targetFbo.allocate(sourceImage.getWidth(), sourceImage.getHeight()*2);
    
    string fragShader = STRINGIFY(uniform sampler2DRect offsetTexture;
                                  uniform sampler2DRect sourceTexture;
                                  uniform float height;
                                  uniform float horizon;
                                  
                                  void main(){
                                      vec2 st = gl_TexCoord[0].st;
                                      
                                      float horizonOffset = texture2DRect(offsetTexture, vec2(st.x,1.0)).r * height;
                                      float horizonLine = horizon * height;
                                      
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
}

void testApp::processImage(ofImage & input, int _threshold){
    processImage(input.getPixelsRef(), _threshold);
}

void  testApp::processImage(ofPixels & srcPixels, int _threshold){
    
    int width = srcPixels.getWidth();
    int height = srcPixels.getHeight();
    
    ofFloatPixels offSet;
    offSet.allocate(width, 1, 3);
    
    for (int x = 0; x < width; x++){
        ofColor a = srcPixels.getColor(x, 0);
        offSet.setColor(x, 0, ofFloatColor(0.0));
        
        for(int y = 0; y < height; y++){
            ofColor b = srcPixels.getColor(x, y);
            
            if (ColourDistance(a,b) > _threshold){
                offSet.setColor(x, 0, ofFloatColor((float)y/(float)height));
                break;
            } else {
                a.lerp(b,thresholdSmoothing);
            }
        }
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
        bImage = sourceImage.loadImage(dragInfo.files[0]);
        if (bImage){
            targetFbo.allocate(sourceImage.getWidth(), sourceImage.getHeight()*2);
            offSetTexture.clear();
            offSetTexture.allocate(sourceImage.width,1, GL_RGB32F);
        } else {
            if (sourceVideo.loadMovie(dragInfo.files[0])){
                sourceVideo.play();
                targetFbo.allocate(sourceVideo.getWidth(), sourceVideo.getHeight()*2);
                offSetTexture.clear();
                offSetTexture.allocate(sourceVideo.getWidth(),1, GL_RGB32F);
            }
        }
    }

}