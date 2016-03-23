#include "ofApp.h"
#include<string>

const string ofApp::ENGLISH_LABEL = "English";
const string ofApp::PORTUGUESE_LABEL = "Português";

const float  ofApp::MAX_STRENGTH_AROUND_PIXEL = .15;
const string ofApp::CHANGE_LOCALE_BUTTON_NAME = "changeLocale";

int ofApp::intervalToSaveImage = 15;
int ofApp::degreesButtonValue = 0;
int ofApp::currentResolution = 2;
int ofApp::columns = 1;
int ofApp::secondsPerImage = 360;
bool ofApp::saveImageToggle = false;
bool ofApp::showAtStartup = true;
bool ofApp::fullscreen = true;
bool ofApp::configurationPanelShow = true;

//--------------------------------------------------------------
void ofApp::setup(){
    gui = new ofxImGui;
    gui->setup();
    imageButtonID = gui->loadImage("funarte.png");
    this->imageWidth = 800;
    this->imageHeight = 600;
    this->lastTimeImageWasSaved = 0;
    this->rotations = 0;
    this->selectedCameraIndex = 0;
    this->currentLocale = LOCALE_PORTUGUESE;
    this->changeLocaleLabel = PORTUGUESE_LABEL;
    this->videoGrabber = new ofVideoGrabber();
    
    this->loadXmlSettings();
    
    ofFile stringsFile;
    stringsFile.open("strings.xml");
    ofBuffer stringsBuffer = stringsFile.readToBuffer();
    ofXml strings;
    strings.loadFromBuffer( stringsBuffer );
    strings.setTo("data");
    int numberOfStrings = strings.getNumChildren();
    for (int i = 0; i < numberOfStrings; i++) {
        strings.setToChild(i);
        
        string tagName = strings.getName();
        
        strings.setTo("pt");
        this->ptStrings[tagName ] = strings.getValue();
        
        strings.setTo("../en");
        this->enStrings[tagName ] = strings.getValue();
        strings.setToParent();
        
        strings.setToParent();
    }
    this->currentStrings = this->ptStrings;

    this->selectResolution();
    this->applyConfigurationChanges();
    
    if (this->showAtStartup) {
        this->showConfigurationPanel();
    }
    else {
        this->hideConfigurationPanel();
    }
    
    this->reset();
}
//--------------------------------------------------------------
void ofApp::update(){
    this->videoGrabber->update();
    
    if (this->configurationPanelShow == true) {
        ofShowCursor();
        return;
    }
    
    if (this->videoGrabber->isFrameNew()) {
        ofPixels pixels = this->videoGrabber->getPixels();
        
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
    // We scale the image so it covers all the screen.
    // The image is croppred if necessary.
    float hScale = (float) ofGetWidth() / (float) this->screenImage.getWidth();
    float vScale = (float) ofGetHeight() / (float) this->screenImage.getHeight();
    
    float scale = max(hScale, vScale);
    
    int scaledWidth = (int) roundf((float) this->screenImage.getWidth() * scale);
    int scaledHeight = (int) roundf((float) this->screenImage.getHeight() * scale);
    
    int x = (ofGetWidth() - scaledWidth) / 2;
    int y = (ofGetHeight() - scaledHeight) / 2;
    
    this->screenImage.draw(x, y, scaledWidth, scaledHeight);
    
    //Interface
    if(configurationPanelShow == true){
        gui->begin();
        ImGui::SetNextWindowSize(ofVec2f(800,500));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin(this->currentStrings["candleclock"].c_str());
        ImGui::Text(this->currentStrings["candleclock"].c_str());
        if(ImGui::Button(this->changeLocaleLabel.c_str())){
            this->changeLocale();
        }
        
        vector< ofVideoDevice > devices = this->videoGrabber->listDevices();
        vector<string> *cameras = new vector<string>();
        vector<ofVideoDevice>::iterator it;
        
        for (it = devices.begin(); it != devices.end(); ++it) {
            ofVideoDevice device = *it;
            cameras->push_back(device.deviceName);
        }
        
        char* nameDevices = (char*) malloc(512 * cameras->size());
        char* current = nameDevices;
        for (int i = 0; i < cameras->size(); i++) {
            strcpy(current, cameras->at(i).c_str());
            current += strlen(cameras->at(i).c_str());
            *current = '\0';
            ++current;
        }
        ImGui::PushItemWidth(200);
        ImGui::Combo(this->currentStrings["pickCamera"].c_str(), &selectedCameraIndex, nameDevices, cameras->size());
        free(nameDevices);
        ImGui::PushItemWidth(100);
        ImGui::Combo(this->currentStrings["deviceResolution"].c_str(), &currentResolution, deviceResolution, 3);
        ImGui::Text(this->currentStrings["imageRotation"].c_str());
        ImGui::RadioButton(this->currentStrings["zeroDegress"].c_str(), &degreesButtonValue, 0); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["ninetyDegress"].c_str(), &degreesButtonValue, 1); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["oneHundredEightyDegress"].c_str(), &degreesButtonValue, 2); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["twoHundredSeventyDegress"].c_str(), &degreesButtonValue, 3);
        ImGui::PushItemWidth(100);
        ImGui::SliderInt(this->currentStrings["columns"].c_str(),&columns, 1, 15);
        ImGui::SliderInt(this->currentStrings["secondsPerImage"].c_str(), &secondsPerImage, 1, 100);
        ImGui::Checkbox(this->currentStrings["saveImage"].c_str(), &saveImageToggle); ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::SliderInt(this->currentStrings["minutes"].c_str(), &intervalToSaveImage, 1, 60);
        
        if(ImGui::Button(this->currentStrings["resetImage"].c_str())){
            this->fillImageWithWhite( &this->screenImage );
            this->screenImage.save("hourglass.png");
        }
        
        ImGui::Checkbox(this->currentStrings["showAtStartup"].c_str(), &showAtStartup);
        ImGui::Checkbox(this->currentStrings["fullScreen"].c_str(), &fullscreen);
        
        if(ImGui::Button(this->currentStrings["save"].c_str())){
            this->saveXmlSettings();
            this->applyConfigurationChanges();
            this->reset();
            this->hideConfigurationPanel();
        }
        ImGui::SameLine();
        if(ImGui::Button(this->currentStrings["cancel"].c_str())){
            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();
        }
        
        ImGui::Text(this->currentStrings["credits1"].c_str());
        ImGui::Text(this->currentStrings["credits2"].c_str());
        ImGui::Text(this->currentStrings["credits3"].c_str());
        ImGui::Text(this->currentStrings["credits4"].c_str());
        ImGui::Text(this->currentStrings["credits5"].c_str());
        ImGui::Text(this->currentStrings["support"].c_str());
        if(ImGui::ImageButton((ImTextureID)(uintptr_t)imageButtonID, ImVec2(509, 60))){
            ofLaunchBrowser("http://www.funarte.gov.br/");
        }
        
        setFullscreen();
        selectResolution();
        
        ImGui::End();
        gui->end();
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_TAB) {
        
        if (this->configurationPanelShow == true) {
            this->hideConfigurationPanel();
        }
        else {
            this->showConfigurationPanel();
        }
    }
}
//--------------------------------------------------------------
void ofApp::reset(){
    if (this->videoGrabber != nullptr) {
        if (this->videoGrabber->isInitialized()) {
            this->videoGrabber->close();
            delete this->videoGrabber;
            this->videoGrabber = new ofVideoGrabber();
        }
    }
    
    // set camera
    if (selectedCameraIndex > 0) {
        this->videoGrabber->setDeviceID( selectedCameraIndex );
    }
    else {
        this->videoGrabber->setDeviceID( 0 );
    }
    
//  this->videoGrabber->setDesiredFrameRate(30);
    this->videoGrabber->initGrabber(this->cameraWidth, this->cameraHeight);
    
    // image to be drawn
    this->screenImage.allocate(this->imageWidth, this->imageHeight, OF_IMAGE_COLOR_ALPHA);
    this->fillImageWithWhite( &this->screenImage );
    
    // check if we have a image to load
    ofImage previousImage;
    if (previousImage.load("hourglass.png")) {
        
        if (previousImage.getWidth() == this->screenImage.getWidth() && previousImage.getHeight() == this->screenImage.getHeight()) {
            
            this->screenImage = previousImage;
        }
    }
    
    this->columnWidth = this->imageWidth / this->columns;
    float numberOfPixels = this->cameraWidth * this->cameraHeight;
    float numberOfMilisseconds = this->secondsPerImage * 1000;
    this->millisecondsPerPixel = numberOfMilisseconds / numberOfPixels;
    this->x = 0;
    this->y = 0;
    this->lastTimePixelWasDrawn = 0;
    this->lastTimeImageWasSaved = 0;
    this->currentColumn = 0;
}
//--------------------------------------------------------------
void ofApp::saveCurrentImage(){
    char filename[64];
    sprintf( filename, "candle_clock_%04d_%02d_%02d_%02d_%02d_%02d.png", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds() );
    
    this->screenImage.save(filename);
}
//--------------------------------------------------------------
void ofApp::fillImageWithWhite(ofImage * image){
    image->setColor(ofColor::whiteSmoke);
    this->lastTimePixelWasDrawn = 0;
    this->currentColumn = 0;
    this->x = 0; this->y = 0;
}
//--------------------------------------------------------------
void ofApp::hideConfigurationPanel(){
    this->configurationPanelShow = false;
    ofHideCursor();
}
//--------------------------------------------------------------
void ofApp::showConfigurationPanel(){
    this->configurationPanelShow = true;
    ofShowCursor();
}
//--------------------------------------------------------------
void ofApp::cancelConfigurationChanges(){
    secondsPerImage = 360;
    columns = 1;
    intervalToSaveImage= 15;
    degreesButtonValue = 0;
    saveImageToggle = false;
    showAtStartup = true;
    fullscreen = true;
    currentResolution = 2;
    selectedCameraIndex = 0;
}
//--------------------------------------------------------------
void ofApp::applyConfigurationChanges(){
    if (degreesButtonValue == 0) {
        this->rotations = 0;
    }
    if (degreesButtonValue == 1) {
        this->rotations = 1;
    }
    else if (degreesButtonValue == 2) {
        this->rotations = 2;
    }
    else if (degreesButtonValue == 3) {
        this->rotations = 3;
    }
    
    if (this->rotations % 2 == 0) {
        this->imageWidth = this->cameraWidth;
        this->imageHeight = this->cameraHeight;
    }
    else {
        this->imageWidth = this->cameraHeight;
        this->imageHeight = this->cameraWidth;
    }
    
    this->selectResolution();
    this->setFullscreen();
}
//--------------------------------------------------------------
void ofApp::setFullscreen(){
    if (fullscreen == true){
        ofSetFullscreen(true);
    } else {
        ofSetFullscreen(false);
        ofSetWindowShape(cameraWidth, cameraHeight);
    }
}
//--------------------------------------------------------------
void ofApp::selectResolution(){
    this->deviceResolution[0] = "800x600";
    this->deviceResolution[1] = "1280x720";
    this->deviceResolution[2] = "1920x1080";
    
    if(currentResolution == 0){
        cameraWidth = 800;
        cameraHeight = 600;
    } else if (currentResolution == 1){
        cameraWidth = 1280;
        cameraHeight = 720;
    } else if (currentResolution == 2){
        cameraWidth = 1920;
        cameraHeight = 1080;
    }
}
//--------------------------------------------------------------
void ofApp::saveXmlSettings(){
    settings.clear();
    settings.addChild("MAIN");
    settings.setTo("MAIN");
    settings.addValue("SECONDS_PER_IMAGE", ofToString(secondsPerImage));
    settings.addValue("COLUMNS", ofToString(columns));
    settings.addValue("INTERVAL_TO_SAVE", ofToString(intervalToSaveImage));
    settings.addValue("DEGREES_BUTTON_VAL", ofToString(degreesButtonValue));
    settings.addValue("CURRENT_RESOLUTION", ofToString(currentResolution));
    settings.addValue("SAVE_IMAGE_TOGGLE", ofToString(saveImageToggle));
    settings.addValue("SHOW_AT_STARTUP", ofToString(showAtStartup));
    settings.addValue("FULLSCREEN", ofToString(fullscreen));
    settings.addValue("SHOW_CONFIGURATION_PANEL", ofToString(configurationPanelShow));
    settings.save("settings.xml");
}
//--------------------------------------------------------------
void ofApp::loadXmlSettings(){
    this->settings.load("settings.xml");
    
    if(settings.exists("//SECONDS_PER_IMAGE")){
        this->secondsPerImage = settings.getValue<int>("//SECONDS_PER_IMAGE");
    } else {
        secondsPerImage = 360;
    }
    
    if(settings.exists("//COLUMNS")){
        this->columns = settings.getValue<int>("//COLUMNS");
    } else {
        columns = 1;
    }
    
    if(settings.exists("//INTERVAL_TO_SAVE")){
        this->intervalToSaveImage = settings.getValue<int>("//INTERVAL_TO_SAVE");
    } else {
        intervalToSaveImage = 15;
    }
    
    if(settings.exists("//DEGREES_BUTTON_VAL")){
        this->degreesButtonValue = settings.getValue<int>("//DEGREES_BUTTON_VAL");
    } else {
        degreesButtonValue = 0;
    }
    
    if(settings.exists("//CURRENT_RESOLUTION")){
        this->currentResolution = settings.getValue<int>("//CURRENT_RESOLUTION");
    } else {
        this->currentResolution = 2;
    }
    
    if(settings.exists("//SAVE_IMAGE_TOGGLE")){
        this->saveImageToggle = settings.getValue<bool>("//SAVE_IMAGE_TOGGLE");
    } else {
        this->saveImageToggle = false;
    }
    
    if(settings.exists("//SHOW_AT_STARTUP")){
        this->showAtStartup = settings.getValue<bool>("//SHOW_AT_STARTUP");
    } else {
        this->showAtStartup = true;
    }
    
    if(settings.exists("//FULLSCREEN")){
        this->fullscreen = settings.getValue<bool>("//FULLSCREEN");
    } else {
        this->fullscreen = true;
    }
    
    if(settings.exists("//SHOW_CONFIGURATION_PANEL")){
        this->configurationPanelShow = settings.getValue<bool>("//SHOW_CONFIGURATION_PANEL");
    } else {
        this->configurationPanelShow = true;
    }

}
//--------------------------------------------------------------
void ofApp::changeLocale(){
    if (this->currentLocale == LOCALE_ENGLISH) {
        this->currentLocale = LOCALE_PORTUGUESE;
        this->currentStrings = this->ptStrings;
    }
    else {
        this->currentLocale = LOCALE_ENGLISH;
        this->currentStrings = this->enStrings;
    }
    
    if (this->currentLocale == LOCALE_ENGLISH) {
        this->changeLocaleLabel = PORTUGUESE_LABEL;
    }
    else {
        this->changeLocaleLabel = ENGLISH_LABEL;
    }
}
//--------------------------------------------------------------