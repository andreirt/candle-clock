#include "ofApp.h"
#include <string>

const string ofApp::ENGLISH_LABEL = "English";
const string ofApp::PORTUGUESE_LABEL = "Portugues";

const float ofApp::MAX_STRENGTH_AROUND_PIXEL = .15;
const string ofApp::SUPPORT_BUTTON_NAME = "support";
const string ofApp::CHANGE_LOCALE_BUTTON_NAME = "changeLocale";


//--------------------------------------------------------------
void ofApp::setup(){
    
    this->hideButtonReleased = false;
    this->selectedCameraIndex = 0;
    this->cameraWidth = 640;
    this->cameraHeight = 480;
    this->lastTimeImageWasSaved = 0;
    this->columns = 0;
    this->intervalToSaveImage = 15;
    this->currentLocale = LOCALE_PORTUGUESE;
    
    // reads strings.xml
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
    
    this->selectedCameraIndex = 1;

    this->videoGrabber = new ofVideoGrabber();

    this->gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());
    this->gui->setWidgetSpacing(10);
    gui->setFontSize(OFX_UI_FONT_SMALL, 8);
    gui->setFontSize(OFX_UI_FONT_LARGE, 18);

    ofxUIColor backgroundColor = ofxUIColor::white;
    ofxUIColor fillColor = ofxUIColor::black;
    ofxUIColor fillHightlightColor = ofxUIColor::black;
    ofxUIColor outline = ofxUIColor::black;
    ofxUIColor outlineHighlight = ofxUIColor::red;
    ofxUIColor paddedColor = ofxUIColor::blue;
    ofxUIColor paddedOutlineColor = ofxUIColor::orange;

    this->gui->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );

    this->titleLabel = this->gui->addLabel("title", this->currentStrings["candleclock"], OFX_UI_FONT_LARGE);
    this->changeLocaleButton = new ofxUILabelButton(ofApp::CHANGE_LOCALE_BUTTON_NAME, true, 200, 30, 500, 10);
    this->gui->addWidgetRight(this->changeLocaleButton);
    this->changeLocaleButton->getLabelWidget()->setLabel(ENGLISH_LABEL);
    this->gui->addSpacer();

    this->cameraPanel = new ofxUICanvas(0, 0, 300, 260);
    this->cameraPanel->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );
    this->cameraPanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->cameraPanel->setWidgetSpacing(10);
    this->gui->addWidgetDown(this->cameraPanel);

    // lista as câmeras conectadas a este computador
    vector< ofVideoDevice > devices = this->videoGrabber->listDevices();

    vector<string> *cameras = new vector<string>();
    vector<ofVideoDevice>::iterator it;

    for (it = devices.begin(); it != devices.end(); ++it) {
        ofVideoDevice device = *it;
        cameras->push_back(device.deviceName);
    }

    this->pickCameraLabel = this->cameraList = this->cameraPanel->addDropDownList(this->currentStrings["pickCamera"], *cameras, 300, 10);
    this->cameraList->setAllowMultiple(false);
    this->cameraList->setDrawOutline(true);
    this->cameraList->setAutoClose(true);

    vector<ofxUILabelToggle *> toggles = this->cameraList->getToggles();
    vector<ofxUILabelToggle *>::iterator togglesIterator;

    for (togglesIterator = toggles.begin(); togglesIterator != toggles.end(); ++togglesIterator) {
        ofxUILabelToggle* labelToggle = *togglesIterator;
        labelToggle->setColorFillHighlight(ofxUIColor::black);
        labelToggle->setColorFill(ofxUIColor::white);
        labelToggle->setColorOutlineHighlight(ofxUIColor::black);
        labelToggle->setColorBack(ofxUIColor::white);
    }

    this->cameraWidthLabel = new ofxUILabel(180, this->currentStrings["cameraWidth"], OFX_UI_FONT_SMALL);
    this->cameraPanel->addWidgetDown( this->cameraWidthLabel );
    this->cameraWidthTextInput = new ofxUITextInput("CameraWidth", "1920", 80, 18) ;
    this->cameraWidthTextInput->setOnlyNumericInput(true);
    this->cameraWidthTextInput->setDrawOutline(true);
    this->cameraWidthTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( cameraWidthTextInput );
    this->textInputs.push_back(this->cameraWidthTextInput);
    
    this->cameraHeightLabel = new ofxUILabel(180, this->currentStrings["cameraHeight"], OFX_UI_FONT_SMALL);
    this->cameraPanel->addWidgetDown( cameraHeightLabel );
    this->cameraHeightTextInput = new ofxUITextInput("CameraHeight", "1080", 80, 18);
    this->cameraHeightTextInput->setOnlyNumericInput(true);
    this->cameraHeightTextInput->setDrawOutline(true);
    this->cameraHeightTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( this->cameraHeightTextInput );
    this->textInputs.push_back(this->cameraHeightTextInput);

    this->rotations = 0;

    this->imageRotationLabel = this->cameraPanel->addLabel( this->currentStrings["imageRotation"], OFX_UI_FONT_SMALL);
    
    this->zeroRotationToggle = new ofxUIToggle( this->currentStrings["zeroDegress"], true, 16, 16);
    this->zeroRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->zeroRotationToggle);
    
    this->ninetyRotationToggle = new ofxUIToggle( this->currentStrings["ninetyDegress"], true, 16, 16);
    this->ninetyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->ninetyRotationToggle);

    this->oneHundredEightyRotationToggle = new ofxUIToggle( this->currentStrings["oneHundredEightyDegress"], true, 16, 16);
    this->oneHundredEightyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->oneHundredEightyRotationToggle);

    this->twoHundredSeventyRotationToggle = new ofxUIToggle( this->currentStrings["twoHundredSeventyDegress"], true, 16, 16);
    this->twoHundredSeventyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->twoHundredSeventyRotationToggle);

    this->imagePanel = new ofxUICanvas(320, 0, 430, 250);
    this->imagePanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->imagePanel->setWidgetSpacing(10);
    this->imagePanel->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );

    this->columns = 1;

    this->columnsLabel = new ofxUILabel(170, this->currentStrings["columns"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetDown( columnsLabel );

    this->columnsTextInput = new ofxUITextInput("columns", "1", 80, 18);
    this->columnsTextInput->setOnlyNumericInput(true);
    this->columnsTextInput->setDrawOutline(true);
    this->columnsTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( this->columnsTextInput );
    this->textInputs.push_back(this->columnsTextInput);

    this->secondsPerImageLabel = new ofxUILabel(390, this->currentStrings["secondsPerImage"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetDown( secondsPerImageLabel );

    this->secondsPerImageTextInput = new ofxUITextInput("seconds", "360", 80, 18);
    this->secondsPerImageTextInput->setOnlyNumericInput(true);
    this->secondsPerImageTextInput->setDrawOutline(true);
    this->secondsPerImageTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetDown( this->secondsPerImageTextInput );
    this->textInputs.push_back(this->secondsPerImageTextInput);

    this->saveImageToggle = new ofxUIToggle(this->currentStrings["saveImage"], true, 16, 16);
    this->saveImageToggle->setDrawOutline(true);
    this->imagePanel->addWidgetDown(this->saveImageToggle);

    this->intervalToSaveImage = 15;

    this->intervalToSaveTextInput = new ofxUITextInput(this->currentStrings["each"], "15", 60, 18);
    this->intervalToSaveTextInput->setOnlyNumericInput(true);
    this->intervalToSaveTextInput->setDrawOutline(true);
    this->intervalToSaveTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( this->intervalToSaveTextInput );
    this->textInputs.push_back(this->intervalToSaveTextInput);

    this->minutesLabel = new ofxUILabel(90, this->currentStrings["minutes"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetRight( minutesLabel );

    this->clearButton = this->gui->addLabelButton( this->currentStrings["resetImage"], false, 150, 20);
    clearButton->setDrawFill(true);
    clearButton->setDrawOutline(true);
    this->imagePanel->addWidgetDown( clearButton );
    this->imagePanel->addSpacer();

    this->showAtStartupToggle = this->imagePanel->addToggle( this->currentStrings["showAtStartup"], true, 16, 16);
    this->showAtStartupToggle->setDrawOutline(true);
    this->imagePanel->addSpacer();

    this->fullScreenToggle = this->imagePanel->addToggle( this->currentStrings["fullScreen"], true, 16, 16 );
    this->fullScreenToggle->setDrawOutline(true);

    this->gui->addWidgetRight(this->imagePanel);

    this->gui->addSpacer();

    this->saveButton = this->gui->addLabelButton( this->currentStrings["save"], false, 100, 20);
    saveButton->setDrawFill(true);
    saveButton->setDrawOutline(true);

    this->cancelButton = new ofxUILabelButton( this->currentStrings["cancel"], false, 100, 20);
    cancelButton->setDrawFill(true);
    cancelButton->setDrawOutline(true);
    this->gui->addWidgetRight(cancelButton);
    this->gui->addSpacer();

    this->credits1Label = this->gui->addLabel( this->currentStrings["credits1"], OFX_UI_FONT_SMALL );
    this->credits2Label = this->gui->addLabel( this->currentStrings["credits2"], OFX_UI_FONT_SMALL );
    this->credits3Label = this->gui->addLabel( this->currentStrings["credits3"], OFX_UI_FONT_SMALL );
    this->credits4Label = this->gui->addLabel( this->currentStrings["credits4"], OFX_UI_FONT_SMALL );
    this->credits5Label = this->gui->addLabel( this->currentStrings["credits5"], OFX_UI_FONT_SMALL );
    this->gui->addSpacer();

    this->supportLabel = this->gui->addLabel( this->currentStrings["support"] );
    this->gui->addImageButton(ofApp::SUPPORT_BUTTON_NAME, "funarte.png", false, 509, 60);

    ofAddListener(this->gui->newGUIEvent, this, &ofApp::guiEvent);
    ofAddListener(this->cameraPanel->newGUIEvent, this, &ofApp::cameraPanelEvent);
    ofAddListener(this->imagePanel->newGUIEvent, this, &ofApp::imagePanelEvent);

    this->cameraPanel->loadSettings("camera.xml");
    this->imagePanel->loadSettings("image.xml");
    this->gui->loadSettings("settings");

    // reads values from controls and stores them into properties
    this->applyConfigurationChanges();

    if (this->showAtStartup) {
        this->showConfigurationPanel();
    }
    else {
        this->hideConfigurationPanel();
    }

    this->reset();
}

void ofApp::reset()
{
    if (this->videoGrabber != NULL) {
        if (this->videoGrabber->isInitialized()) {
            this->videoGrabber->close();
            delete this->videoGrabber;
            this->videoGrabber = new ofVideoGrabber();
        }
    }

    // set camera
    vector<int> selectedCamera = this->cameraList->getSelectedIndeces();
    if (selectedCamera.size() > 0) {
        this->videoGrabber->setDeviceID( selectedCamera[0] );
    }
    else {
        this->videoGrabber->setDeviceID( 0 );
    }

    this->videoGrabber->setDesiredFrameRate(30);
    this->videoGrabber->initGrabber(this->cameraWidthTextInput->getIntValue(), this->cameraHeightTextInput->getIntValue());

    // image to be drawn
    this->screenImage.allocate(this->imageWidth, this->imageHeight, OF_IMAGE_COLOR_ALPHA);
    this->fillImageWithWhite( &this->screenImage );

    // check if we have a image to load
    ofImage previousImage;
    if (previousImage.loadImage("candle_clock.png")) {

        if (previousImage.getWidth() == this->screenImage.getWidth() && previousImage.getHeight() == this->screenImage.getHeight()) {

            this->screenImage = previousImage;

        }

    }

    this->columnWidth = this->imageWidth / this->columns;
    float numberOfPixels = this->cameraWidthTextInput->getIntValue() * this->cameraHeightTextInput->getIntValue();
    float numberOfMilisseconds = this->secondsPerImageTextInput->getIntValue() * 1000;
    this->millisecondsPerPixel = numberOfMilisseconds / numberOfPixels;
    this->x = 0;
    this->y = 0;
    this->lastTimePixelWasDrawn = 0;
    this->lastTimeImageWasSaved = 0;
    this->currentColumn = 0;
}

//--------------------------------------------------------------
void ofApp::update(){

    this->videoGrabber->update();
    if (this->videoGrabber->isFrameNew()) {
        ofPixels pixels = this->videoGrabber->getPixelsRef();

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
    float hScale = (float) ofGetWidth() / (float) this->screenImage.width;
    float vScale = (float) ofGetHeight() / (float) this->screenImage.height;

    float scale = max(hScale, vScale);

    int scaledWidth = (int) roundf((float) this->screenImage.width * scale);
    int scaledHeight = (int) roundf((float) this->screenImage.height * scale);

    int x = (ofGetWidth() - scaledWidth) / 2;
    int y = (ofGetHeight() - scaledHeight) / 2;

    this->screenImage.draw(x, y, scaledWidth, scaledHeight);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == OF_KEY_TAB) {

        if (this->gui->isVisible()) {
            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();
        }
        else {
            this->showConfigurationPanel();
        }

    }
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
void ofApp::mouseReleased(int x, int y, int button)
{

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

void ofApp::exit()
{
    //delete gui;
}

void ofApp::saveCurrentImage() {
    char filename[64];
    sprintf( filename, "candle_clock_%04d_%02d_%02d_%02d_%02d_%02d.png", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds() );
    this->screenImage.saveImage(filename);
}

void ofApp::fillImageWithWhite( ofImage* image )
{
    // paints with white
    unsigned char* pixels = image->getPixels();
    int bytesPerPixel = 3;
    if (image->type == OF_IMAGE_COLOR_ALPHA)
        bytesPerPixel = 4;
    else if (image->type == OF_IMAGE_GRAYSCALE)
        bytesPerPixel = 1;
    for (int i =0; i < image->getWidth() * image->getHeight() * bytesPerPixel; i++) {
        pixels[i] = 255;
    }
}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == this->ptStrings["save"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            
            this->cameraPanel->saveSettings("camera.xml");
            this->imagePanel->saveSettings("image.xml");
            this->gui->saveSettings("settings.xml");
            this->applyConfigurationChanges();
            this->reset();
            this->hideConfigurationPanel();
            
        }
    }
    else if (e.getName() == this->ptStrings["cancel"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            
            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();
            
        }
    }
    else if (e.getName() == ofApp::SUPPORT_BUTTON_NAME) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            ofLaunchBrowser("http://www.funarte.gov.br/");
        }
    }
    else if (e.getName() == ofApp::CHANGE_LOCALE_BUTTON_NAME) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            if (this->currentLocale == LOCALE_ENGLISH) {
                this->currentLocale = LOCALE_PORTUGUESE;
                this->currentStrings = this->ptStrings;
            }
            else {
                this->currentLocale = LOCALE_ENGLISH;
                this->currentStrings = this->enStrings;
            }
            
            this->titleLabel->setLabel( this->currentStrings["candleclock"] );
            if (this->currentLocale == LOCALE_ENGLISH) {
                this->changeLocaleButton->getLabelWidget()->setLabel(PORTUGUESE_LABEL);
            }
            else {
                this->changeLocaleButton->getLabelWidget()->setLabel(ENGLISH_LABEL);
            }
            
            
            this->pickCameraLabel->setLabelText( this->currentStrings["pickCamera"] );
            this->cameraWidthLabel->setLabel( this->currentStrings["cameraWidth"] );
            this->cameraHeightLabel->setLabel( this->currentStrings["cameraHeight"] );
            
            this->saveButton->setLabelText( this->currentStrings["save"] );
            this->cancelButton->setLabelText( this->currentStrings["cancel"] );
            
            this->credits1Label->setLabel( this->currentStrings["credits1"] );
            this->credits2Label->setLabel( this->currentStrings["credits2"] );
            this->credits3Label->setLabel( this->currentStrings["credits3"] );
            this->credits4Label->setLabel( this->currentStrings["credits4"] );
            this->credits5Label->setLabel( this->currentStrings["credits5"] );
            
            this->zeroRotationToggle->getLabelWidget()->setLabel( this->currentStrings["zeroDegress"] );
            this->ninetyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["ninetyDegress"] );
            this->oneHundredEightyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["oneHundredEightyDegress"] );
            this->twoHundredSeventyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["twoHundredSeventyDegress"] );
            
            this->imageRotationLabel->setLabel( this->currentStrings["imageRotation"]);
            this->secondsPerImageLabel->setLabel( this->currentStrings["secondsPerImage"] );
            this->columnsLabel->setLabel( this->currentStrings["columns"] );
            this->minutesLabel->setLabel( this->currentStrings["minutes"] );
            this->clearButton->setLabelText( this->currentStrings["resetImage"] );
            
            this->saveImageToggle->getLabelWidget()->setLabel( this->currentStrings["saveImage"] );
            this->showAtStartupToggle->getLabelWidget()->setLabel( this->currentStrings["showAtStartup"] );
            this->fullScreenToggle->getLabelWidget()->setLabel( this->currentStrings["fullScreen"] );
            
            this->supportLabel->setLabel( this->currentStrings["support"] );
            
        }
        
    }
    if (e.getKind() == OFX_UI_WIDGET_TEXTINPUT){
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            this->unfocusAllTextInputs(ti);
        }
    }
}

void ofApp::cameraPanelEvent(ofxUIEventArgs &e)
{
    if (e.getName() == this->ptStrings["zeroDegress"] && e.getToggle()->getValue()) {
        this->ninetyRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->ptStrings["ninetyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->ptStrings["oneHundredEightyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->ninetyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->ptStrings["twoHundredSeventyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->ninetyRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
    }
    else {
        this->checkTextInputFocus(e);
    }
}

void ofApp::imagePanelEvent(ofxUIEventArgs &e)
{
    if (e.getName() == this->ptStrings["resetImage"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            this->fillImageWithWhite( &this->screenImage );
            this->screenImage.saveImage("hourglass.png");
        }
    }
    else {
        this->checkTextInputFocus(e);
    }
}

void ofApp::checkTextInputFocus(ofxUIEventArgs &e)
{
    if (e.getKind() == OFX_UI_WIDGET_TEXTINPUT){
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            this->unfocusAllTextInputs(ti);
        }
    }
}

void ofApp::hideConfigurationPanel()
{
    this->gui->setVisible(false);
    this->cameraPanel->setVisible(false);
    this->imagePanel->setVisible(false);
    ofHideCursor();
}

void ofApp::showConfigurationPanel()
{
    this->gui->setVisible(true);
    this->gui->disableKeyEventCallbacks();

    this->cameraPanel->setVisible(true);
    this->imagePanel->setVisible(true);
    ofShowCursor();
}

void ofApp::cancelConfigurationChanges()
{
    this->zeroRotationToggle->setValue(this->rotations == 0);
    this->ninetyRotationToggle->setValue(this->rotations == 1);
    this->oneHundredEightyRotationToggle->setValue(this->rotations == 2);
    this->twoHundredSeventyRotationToggle->setValue(this->rotations == 3);

    if (this->rotations % 2 == 0) {
        this->cameraWidthTextInput->setTextString( ofToString(this->imageWidth) );
        this->cameraHeightTextInput->setTextString( ofToString(this->imageHeight) );
    }
    else {
        this->cameraWidthTextInput->setTextString( ofToString(this->imageHeight) );
        this->cameraHeightTextInput->setTextString( ofToString(this->imageWidth) );
    }

    this->columnsTextInput->setTextString( ofToString(this->columns) );
    this->intervalToSaveTextInput->setTextString( ofToString(this->intervalToSaveImage) );
    this->showAtStartupToggle->setValue( this->showAtStartup );
    this->fullScreenToggle->setValue( this->fullScreen );
    this->saveImageToggle->setValue( this->saveImage );
}

void ofApp::applyConfigurationChanges()
{
    if (this->zeroRotationToggle->getValue()) {
        this->rotations = 0;
    }
    if (this->ninetyRotationToggle->getValue()) {
        this->rotations = 1;
    }
    else if (this->oneHundredEightyRotationToggle->getValue()) {
        this->rotations = 2;
    }
    else if (this->twoHundredSeventyRotationToggle->getValue()) {
        this->rotations = 3;
    }

    this->zeroRotationToggle->setValue(this->rotations == 0);
    this->ninetyRotationToggle->setValue(this->rotations == 1);
    this->oneHundredEightyRotationToggle->setValue(this->rotations == 2);
    this->twoHundredSeventyRotationToggle->setValue(this->rotations == 3);

    if (this->rotations % 2 == 0) {
        this->imageWidth = this->cameraWidthTextInput->getIntValue();
        this->imageHeight = this->cameraHeightTextInput->getIntValue();
    }
    else {
        this->imageWidth = this->cameraHeightTextInput->getIntValue();
        this->imageHeight = this->cameraWidthTextInput->getIntValue();
    }

    this->columns = this->columnsTextInput->getIntValue();
    this->intervalToSaveImage = this->intervalToSaveTextInput->getIntValue();

    this->showAtStartup = this->showAtStartupToggle->getValue();
    this->fullScreen = this->fullScreenToggle->getValue();
    this->saveImage = this->saveImageToggle->getValue();

    if (this->fullScreen) {
        ofSetFullscreen(true);
    }
    else {
        ofSetFullscreen(false);
        ofSetWindowShape(this->imageWidth, this->imageHeight);
    }
}

void ofApp::unfocusAllTextInputs(ofxUITextInput* except){
    for (int i = 0; i < this->textInputs.size(); i ++){
        if (except != this->textInputs[i]){
            this->textInputs[i]->setFocus(false);
        }
    }
}
