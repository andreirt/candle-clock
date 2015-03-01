#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    this->grabber.setDeviceID( 0 );

    this->grabber.setDesiredFrameRate(30);
    this->grabber.initGrabber(this->cameraWidth, this->cameraHeight);
    
    if (this->rotations % 2 == 0) {
        this->imageWidth = this->cameraWidth;
        this->imageHeight = this->cameraHeight;
    }
    else {
        this->imageWidth = this->cameraHeight;
        this->imageHeight = this->cameraWidth;
    }

    // image to be drawn
    this->screenImage.allocate(this->imageWidth, this->imageHeight, OF_IMAGE_COLOR_ALPHA);

    // paints with black
    unsigned char* pixels = this->screenImage.getPixels();
    for (int i =0; i<this->screenImage.getWidth() * this->screenImage.getHeight() * 4; i++) {
        pixels[i] = 0;
    }

    this->columnWidth = this->imageWidth / this->columns;
    float numberOfPixels = this->cameraWidth * this->cameraHeight;
    float numberOfMilisseconds = this->secondsToComplete * 1000;
    this->millisecondsPerPixel = numberOfMilisseconds / numberOfPixels;
}

//--------------------------------------------------------------
void ofApp::update(){

    this->grabber.update();
    if (this->grabber.isFrameNew()) {
        ofPixels pixels = this->grabber.getPixelsRef();
        pixels.rotate90( this->rotations );

        float now = ofGetElapsedTimeMillis();
        while (this->lastTimePixelWasDrawn < now) {

            ofColor c = pixels.getColor( this->x, this->y );
            this->screenImage.setColor( this->x, this->y, c );
            this->lastTimePixelWasDrawn += this->millisecondsPerPixel;

            ++this->x;
            if (this->x == (this->currentColumn + 1) * this->columnWidth) {
                this->x = this->currentColumn * this->columnWidth;
                ++this->y;

                if (this->y == this->imageHeight) {
                     ++this->currentColumn;
                    this->x = this->currentColumn * this->columnWidth;
                    this->y = 0;

                    if (this->currentColumn == this->columns) {
                        this->x = 0;
                        this->currentColumn = 0;
                    }

                    // saves finished column / image
                    this->saveCurrentImage();
                }
            }
        }

        this->screenImage.update();

        if (ofGetElapsedTimef() - this->lastTimeImageWasSaved > this->intervalToSaveImage * 60) {
            this->saveCurrentImage();
            this->lastTimeImageWasSaved = ofGetElapsedTimef();
        }

    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    this->screenImage.draw(0, 0);
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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

void ofApp::saveCurrentImage() {
    char filename[64];
    sprintf( filename, "candle_clock_%04d_%02d_%02d_%02d_%02d_%02d.png", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds() );
    this->screenImage.saveImage(filename);
}
