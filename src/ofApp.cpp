#include "ofApp.h"
#include<string>

const string ofApp::ENGLISH_LABEL = "English";
const string ofApp::PORTUGUESE_LABEL = "Portugus";
const string ofApp::CHANGE_LOCALE_BUTTON_NAME = "changeLocale";

int ofApp::intervalToSaveImage = 15;
int ofApp::degreesButtonValue = 0;
int ofApp::columns = 1;
int ofApp::secondsPerImage = 900;
bool ofApp::saveImageToggle = false;
bool ofApp::showAtStartup = true;
bool ofApp::fullscreen = true;
bool ofApp::configurationPanelShow = true;

//--------------------------------------------------------------
void ofApp::setup(){
    camera.setup();
    
    gui = new ofxImGui;
    gui->setup();
    imageButtonID = gui->loadImage("funarte.png");
    this->cameraWidth = 5184;
    this->cameraHeight = 3456;
    this->imageWidth = 800;
    this->imageHeight = 600;
    this->lastTimeImageWasSaved = 0;
    this->rotations = 0;
    this->currentLocale = LOCALE_PORTUGUESE;
    this->changeLocaleLabel = PORTUGUESE_LABEL;
    
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
    
    this->applyConfigurationChanges();
    
    if (this->showAtStartup) {
        this->showConfigurationPanel();
    }
    else {
        this->hideConfigurationPanel();
    }
    
    this->reset();
    this->camera.takePhoto();
}
//--------------------------------------------------------------
void ofApp::update(){
    this->camera.update();
    
    if (this->configurationPanelShow == true) {
        ofShowCursor();
        return;
    }
    
    if(camera.isPhotoNew()) {
        this->cameraPixels = camera.getPhotoPixels();
        this->cameraPixels.rotate90( this->rotations );
        
        if (this->cameraPixels.getWidth() != this->imageWidth || this->cameraPixels.getHeight() != this->imageHeight) {
            this->cameraWidth = this->cameraPixels.getWidth();
            this->cameraHeight = this->cameraPixels.getHeight();
            this->reset();
        }
        
        this->lastTimePhotoWasTaken = ofGetElapsedTimeMillis();
        camera.takePhoto();
    }
    
    float now = ofGetElapsedTimeMillis();
    while (this->lastTimePixelWasDrawn < now){
        
        ofColor c = cameraPixels.getColor( this->x, this->y );
        this->screenImage.setColor( this->x, this->y, c );
        this->lastTimePixelWasDrawn += this->millisecondsPerPixel;
            
        ++this->x;
        if (this->x >= (this->currentColumn + 1) * this->columnWidth) {
            this->x = this->currentColumn * this->columnWidth;
            ++this->y;
            
            if (this->y >= this->imageHeight) {
                ++this->currentColumn;
                this->x = this->currentColumn * this->columnWidth;
                this->y = 0;

				// saves finished column / image
				this->saveCurrentImage();
                
                if (this->currentColumn == this->columns) {
                    this->x = 0;
                    this->currentColumn = 0;
                    
                    ofResetElapsedTimeCounter();

					this->lastTimePixelWasDrawn = 0;
					this->lastTimePhotoWasTaken = 0;
					this->lastTimeImageWasSaved = 0;
					break;

                }                    
                
            }
        }
    }
        
    this->screenImage.update();
        
    if (ofGetElapsedTimef() - this->lastTimeImageWasSaved > this->intervalToSaveImage * 60) {
        this->saveCurrentImage();
        this->lastTimeImageWasSaved = ofGetElapsedTimef();
    }
        
    unsigned long intervalSinceLastPhoto = ofGetElapsedTimeMillis() - this->lastTimePhotoWasTaken;
    ofLog() << "intervalSinceLastPhoto: " << intervalSinceLastPhoto;
    
    // if one image fails to be taken, we try to restart after one minute
    if (intervalSinceLastPhoto > 1000 * 60 && intervalSinceLastPhoto < 1000 * 60 * 3) {
        this->camera.takePhoto();
    }
        
    // we give two minutes to app finish to save images and other tasks
    // before we quit
    
    // if the last image was taken five minutes ago,
    // we quit, so the program can be restarted by an utility like
    // https://www.raymond.cc/blog/keep-application-running-by-automatically-rerun-when-closed/
    if (intervalSinceLastPhoto > 1000 * 60 * 5) {
        this->camera.close();
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
        
        ImGui::Text(this->currentStrings["imageRotation"].c_str());
        ImGui::RadioButton(this->currentStrings["zeroDegress"].c_str(), &degreesButtonValue, 0); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["ninetyDegress"].c_str(), &degreesButtonValue, 1); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["oneHundredEightyDegress"].c_str(), &degreesButtonValue, 2); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["twoHundredSeventyDegress"].c_str(), &degreesButtonValue, 3);
        ImGui::PushItemWidth(100);
        ImGui::SliderInt(this->currentStrings["columns"].c_str(),&columns, 1, 15);
        ImGui::SliderInt(this->currentStrings["secondsPerImage"].c_str(), &secondsPerImage, 900, 604800);
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
        
        ImGui::End();
        gui->end();
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'c') {
        
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
    // image to be drawn
	if (!this->screenImage.isAllocated()) {
		this->screenImage.allocate(this->imageWidth, this->imageHeight, OF_IMAGE_COLOR_ALPHA);
	}
    this->fillImageWithWhite( &this->screenImage );
    
    this->columnWidth = this->imageWidth / this->columns;
    float numberOfPixels = this->cameraWidth * this->cameraHeight;
    float numberOfMilisseconds = this->secondsPerImage * 1000;
    this->millisecondsPerPixel = (numberOfMilisseconds / numberOfPixels);
    
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
    secondsPerImage = 900;
    columns = 1;
    intervalToSaveImage= 15;
    degreesButtonValue = 0;
    saveImageToggle = false;
    showAtStartup = true;
    fullscreen = true;
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
void ofApp::saveXmlSettings(){
    settings.clear();
    settings.addChild("MAIN");
    settings.setTo("MAIN");
    settings.addValue("SECONDS_PER_IMAGE", ofToString(secondsPerImage));
    settings.addValue("COLUMNS", ofToString(columns));
    settings.addValue("INTERVAL_TO_SAVE", ofToString(intervalToSaveImage));
    settings.addValue("DEGREES_BUTTON_VAL", ofToString(degreesButtonValue));
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
        secondsPerImage = 900;
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