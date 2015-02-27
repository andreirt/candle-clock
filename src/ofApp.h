#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

    private:
        ofVideoGrabber grabber;
        int selectedCameraIndex = 1;

        ofImage screenImage;

        int cameraWidth = 1920;
        int cameraHeight = 1080;

        int columns = 1;
        int currentColumn = 0;
        long secondsToComplete = 24 * 60 * 60;
        float millisecondsPerPixel = 0;
        float lastTimePixelWasDrawn = 0;
        int x = 0;
        int y = 0;
        int columnWidth = 0;

        float lastTimeImageWasSaved = 0;
        int intervalToSaveImage = 15;

        void saveCurrentImage();

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
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

};
