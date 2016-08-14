#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxEdsdk.h"

class ofApp : public ofBaseApp{
    
private:
    ofXml settings;
    ofxImGui*gui;
    GLuint imageButtonID;
    ofxEdsdk::Camera camera;
    
    static int intervalToSaveImage;
    static int degreesButtonValue;
    static int columns;
    static int secondsPerImage;
    static bool saveImageToggle;
    static bool showAtStartup;
    static bool fullscreen;
    static bool configurationPanelShow;
    
    std::map<string, string> ptStrings;
    std::map<string, string> enStrings;
    std::map<string, string> currentStrings;
    string changeLocaleLabel;
    int currentLocale;
    const static string CHANGE_LOCALE_BUTTON_NAME;
    const static string ENGLISH_LABEL;
    const static string PORTUGUESE_LABEL;
    const static int LOCALE_ENGLISH = 0;
    const static int LOCALE_PORTUGUESE = 1;
    
    ofImage screenImage;
    unsigned char* pixels;
    ofPixels cameraPixels;
    int currentColumn;
    int x, y;
    
    double millisecondsPerPixel;
    double lastTimePixelWasDrawn;
    double lastTimeImageWasSaved;
    unsigned long lastTimePhotoWasTaken;
    int columnWidth;
    int rotations;
    int cameraWidth;
    int cameraHeight;
    int imageWidth;
    int imageHeight;
    bool saveImage;
    
    void fillImageWithWhite( ofImage* image );
    void saveCurrentImage();
    void reset();
    void cancelConfigurationChanges();
    void applyConfigurationChanges();
    void hideConfigurationPanel();
    void showConfigurationPanel();
    void changeLocale();
    void saveXmlSettings();
    void loadXmlSettings();
    void setFullscreen();
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
};
