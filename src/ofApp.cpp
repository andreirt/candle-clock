#include "ofApp.h"
#include <string>

const string ofApp::CAMERA_WIDTH_LABEL = "Largura da câmera";
const string ofApp::CAMERA_HEIGHT_LABEL = "Altura da câmera";
const string ofApp::COLUMNS_LABEL = "Colunas";
const string ofApp::SECONDS_PER_IMAGE_LABEL = "Tempo para completar a imagem (em segundos)";
const string ofApp::SAVE_IMAGE_LABEL = "Salvar imagem a cada";
const string ofApp::RESET_IMAGE_LABEL = "Limpar imagem";
const string ofApp::SUPPORT_BUTTON_NAME = "support";
const string ofApp::SAVE_LABEL = "Salvar";
const string ofApp::CANCEL_LABEL = "Cancelar";

//--------------------------------------------------------------
void ofApp::setup(){

    this->videoGrabber = new ofVideoGrabber();

    this->gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());
    this->gui->setWidgetSpacing(15);
    gui->setFontSize(OFX_UI_FONT_SMALL, 8);
    gui->setFontSize(OFX_UI_FONT_LARGE, 24);

    ofxUIColor backgroundColor = ofxUIColor::lightCoral;
    ofxUIColor fillColor = ofxUIColor::black;
    ofxUIColor fillHightlightColor = ofxUIColor::black;
    ofxUIColor outline = ofxUIColor::black;
    ofxUIColor outlineHighlight = ofxUIColor::red;
    ofxUIColor paddedColor = ofxUIColor::blue;
    ofxUIColor paddedOutlineColor = ofxUIColor::orange;

    this->gui->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );

    this->gui->addLabel("title", "Relógio de Vela", OFX_UI_FONT_LARGE);
    this->gui->addSpacer();

    this->cameraPanel = new ofxUICanvas(0, 0, 330, 240);
    this->cameraPanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->cameraPanel->setWidgetSpacing(15);
    this->gui->addWidgetDown(this->cameraPanel);

    // lista as câmeras conectadas a este computador
    vector< ofVideoDevice > devices = this->videoGrabber->listDevices();

    vector<string> *cameras = new vector<string>();
    vector<ofVideoDevice>::iterator it;

    for (it = devices.begin(); it != devices.end(); ++it) {
        ofVideoDevice device = *it;
        cameras->push_back(device.deviceName);
    }

    this->cameraList = this->cameraPanel->addDropDownList("Escolha a câmera", *cameras, 300, 10);
    this->cameraList->setAllowMultiple(false);
    this->cameraList->setDrawOutline(true);

    this->cameraPanel->addWidgetDown( new ofxUILabel(170, ofApp::CAMERA_WIDTH_LABEL, OFX_UI_FONT_SMALL) );
    this->cameraWidthTextInput = new ofxUITextInput("CameraWidth", "1920", 80, 18) ;
    this->cameraWidthTextInput->setOnlyNumericInput(true);
    this->cameraWidthTextInput->setDrawOutline(true);
    this->cameraWidthTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( cameraWidthTextInput );

    ofxUILabel* cameraHeightLabel = new ofxUILabel(170, ofApp::CAMERA_HEIGHT_LABEL, OFX_UI_FONT_SMALL);
    this->cameraPanel->addWidgetDown( cameraHeightLabel );
    this->cameraHeightTextInput = new ofxUITextInput("CameraHeight", "1080", 80, 18);
    this->cameraHeightTextInput->setOnlyNumericInput(true);
    this->cameraHeightTextInput->setDrawOutline(true);
    this->cameraHeightTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( this->cameraHeightTextInput );

    this->rotations = 0;

    this->cameraPanel->addLabel("Rotação da imagem", OFX_UI_FONT_SMALL);
    this->rotationToggleMatrix = this->cameraPanel->addToggleMatrix("rotation", 4, 1);
    this->rotationToggleMatrix->setAllowMultiple(false);

    ofxUIToggle* zeroRotationToggle = this->rotationToggleMatrix->getToggle(0, 0);
    zeroRotationToggle->setName("zero");
    zeroRotationToggle->setDrawOutline(true);
    zeroRotationToggle->setLabelVisible(true);
    zeroRotationToggle->setLabelPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    zeroRotationToggle->getLabelWidget()->setLabel("0 graus");

    ofxUIToggle* ninetyRotationToggle = this->rotationToggleMatrix->getToggle(1, 0);
    ninetyRotationToggle->setName("ninety");
    ninetyRotationToggle->setDrawOutline(true);
    ninetyRotationToggle->setLabelVisible(true);
    ninetyRotationToggle->setLabelPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ninetyRotationToggle->getLabelWidget()->setLabel("90 graus");

    ofxUIToggle* oneHundredEightyRotationToggle = this->rotationToggleMatrix->getToggle(2, 0);
    oneHundredEightyRotationToggle->setName("180");
    oneHundredEightyRotationToggle->setDrawOutline(true);
    oneHundredEightyRotationToggle->setLabelVisible(true);
    oneHundredEightyRotationToggle->setLabelPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    oneHundredEightyRotationToggle->getLabelWidget()->setLabel("180 graus");

    ofxUIToggle* twoHundredSeventyRotationToggle = this->rotationToggleMatrix->getToggle(3, 0);
    twoHundredSeventyRotationToggle->setName("270");
    twoHundredSeventyRotationToggle->setDrawOutline(true);
    twoHundredSeventyRotationToggle->setLabelVisible(true);
    twoHundredSeventyRotationToggle->setLabelPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    twoHundredSeventyRotationToggle->getLabelWidget()->setLabel("270 graus");

    this->rotationToggleMatrix->setAllToggles(false, false);
    this->rotationToggleMatrix->setToggle(0, 0, true, false);

    this->imagePanel = new ofxUICanvas(0, 0, 430, 240);
    this->imagePanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->imagePanel->setWidgetSpacing(15);
    this->gui->addWidgetRight(this->imagePanel);

    this->columns = 1;

    ofxUILabel* columnsLabel = new ofxUILabel(170, ofApp::COLUMNS_LABEL, OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetDown( columnsLabel );

    this->columnsTextInput = new ofxUITextInput(ofApp::COLUMNS_LABEL, "1", 80, 18);
    this->columnsTextInput->setOnlyNumericInput(true);
    this->columnsTextInput->setDrawOutline(true);
    this->columnsTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( this->columnsTextInput );

    ofxUILabel* secondsPerImageLabel = new ofxUILabel(390, ofApp::SECONDS_PER_IMAGE_LABEL, OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetDown( secondsPerImageLabel );

    this->secondsPerImageTextInput = new ofxUITextInput(ofApp::SECONDS_PER_IMAGE_LABEL, "360", 80, 18);
    this->secondsPerImageTextInput->setOnlyNumericInput(true);
    this->secondsPerImageTextInput->setDrawOutline(true);
    this->secondsPerImageTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetDown( this->secondsPerImageTextInput );

    this->saveImageToggle = new ofxUIToggle(ofApp::SAVE_IMAGE_LABEL, true, 16, 16);
    this->saveImageToggle->setDrawOutline(true);
    this->imagePanel->addWidgetDown(this->saveImageToggle);

    this->intervalToSaveImage = 15;

    this->intervalToSaveTextInput = new ofxUITextInput("A cada", "15", 60, 18);
    this->intervalToSaveTextInput->setOnlyNumericInput(true);
    this->intervalToSaveTextInput->setDrawOutline(true);
    this->intervalToSaveTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( this->intervalToSaveTextInput );

    ofxUILabel* minutesLabel = new ofxUILabel(170, "minutos", OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetRight( minutesLabel );

    ofxUILabelButton* clearButton = this->gui->addLabelButton(ofApp::RESET_IMAGE_LABEL, false, 380, 20);
    clearButton->setDrawFill(true);
    clearButton->setDrawOutline(true);
    this->imagePanel->addWidgetDown( clearButton );

    this->settingsPanel = new ofxUICanvas(0, 0, 300, 90);
    this->settingsPanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->settingsPanel->setWidgetSpacing(15);
    this->gui->addWidgetRight(this->settingsPanel);

    this->showAtStartupToggle = this->settingsPanel->addToggle("Exibir esta tela ao iniciar", true, 16, 16);
    this->showAtStartupToggle->setDrawOutline(true);
    this->settingsPanel->addSpacer();

    this->fullScreenToggle = this->settingsPanel->addToggle("Tela cheia", true, 16, 16);
    this->fullScreenToggle->setDrawOutline(true);

    this->gui->addSpacer();

    ofxUILabelButton* saveButton = this->gui->addLabelButton(ofApp::SAVE_LABEL, false, 100, 20);
    saveButton->setDrawFill(true);
    saveButton->setDrawOutline(true);

    ofxUILabelButton* cancelButton = new ofxUILabelButton(ofApp::CANCEL_LABEL, false, 100, 20);
    cancelButton->setDrawFill(true);
    cancelButton->setDrawOutline(true);
    this->gui->addWidgetRight(cancelButton);
    this->gui->addSpacer();

    this->gui->addLabel("Ampulheta - Andrei Thomaz, 2015");
    this->gui->addLabel("Integrante do projeto Máquinas do Tempo, desenvolvido com apoio da Bolsa de Artes Visual da Funarte 2014");
    this->gui->addLabel("Desenvolvido em C++ / OpenFrameworks e distribuído sob a licença MPL");
    this->gui->addLabel("Programação por Andrei Thomaz e Vitor Andrioli");
    this->gui->addSpacer();

    this->gui->addLabel("Realização");
    this->gui->addImageButton(ofApp::SUPPORT_BUTTON_NAME, "funarte.png", false, 509, 60);

    ofAddListener(this->gui->newGUIEvent, this, &ofApp::guiEvent);
    ofAddListener(this->cameraPanel->newGUIEvent, this, &ofApp::cameraPanelEvent);
    ofAddListener(this->imagePanel->newGUIEvent, this, &ofApp::imagePanelEvent);

    this->settingsPanel->loadSettings("settings.xml");
    this->cameraPanel->loadSettings("camera.xml");
    this->imagePanel->loadSettings("image.xml");

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
        this->videoGrabber->setDeviceID( selectedCamera[0] - 1 );
    }
    else {
        this->videoGrabber->setDeviceID( 0 );
    }

    this->videoGrabber->setDesiredFrameRate(30);
    this->videoGrabber->initGrabber(this->cameraWidthTextInput->getIntValue(), this->cameraHeightTextInput->getIntValue());

    // image to be drawn
    this->screenImage.allocate(this->cameraWidthTextInput->getIntValue(), this->cameraHeightTextInput->getIntValue(), OF_IMAGE_COLOR_ALPHA);
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
    ofLog() << "e.getName(): " << e.getName();

    if (e.getName() == ofApp::SAVE_LABEL) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {

            this->cameraPanel->saveSettings("camera.xml");
            this->imagePanel->saveSettings("image.xml");
            this->settingsPanel->saveSettings("settings.xml");
            this->applyConfigurationChanges();
            this->reset();
            this->hideConfigurationPanel();

        }
    }
    else if (e.getName() == ofApp::CANCEL_LABEL) {
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
}

void ofApp::cameraPanelEvent(ofxUIEventArgs &e)
{
    ofLog() << "e.getName(): " << e.getName();
}

void ofApp::imagePanelEvent(ofxUIEventArgs &e)
{
    if (e.getName() == ofApp::RESET_IMAGE_LABEL) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            this->fillImageWithWhite( &this->screenImage );
            this->screenImage.saveImage("candle_clock.png");
        }
    }
}

void ofApp::hideConfigurationPanel()
{
    this->gui->setVisible(false);
    this->cameraPanel->setVisible(false);
    this->imagePanel->setVisible(false);
    this->settingsPanel->setVisible(false);
    ofHideCursor();
}

void ofApp::showConfigurationPanel()
{
    this->gui->setVisible(true);
    this->cameraPanel->setVisible(true);
    this->imagePanel->setVisible(true);
    this->settingsPanel->setVisible(true);
    ofShowCursor();
}

void ofApp::cancelConfigurationChanges()
{
    this->rotationToggleMatrix->setAllToggles(false, false);
    if (this->rotations == 0) {
        this->rotationToggleMatrix->getToggle(0, 0)->setValue(true);
    }
    else if (this->rotations == 1) {
        this->rotationToggleMatrix->getToggle(1, 0)->setValue(true);
    }
    else if (this->rotations == 2) {
        this->rotationToggleMatrix->getToggle(2, 0)->setValue(true);
    }
    else if (this->rotations == 3) {
        this->rotationToggleMatrix->getToggle(3, 0)->setValue(true);
    }

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
    if (this->rotationToggleMatrix->getState(1, 0)) {
        this->rotations = 0;
    }
    if (this->rotationToggleMatrix->getState(1, 0)) {
        this->rotations = 1;
    }
    else if (this->rotationToggleMatrix->getState(2, 0)) {
        this->rotations = 2;
    }
    else if (this->rotationToggleMatrix->getState(3, 0)) {
        this->rotations = 3;
    }

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
