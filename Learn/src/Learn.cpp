#include "Learn.h"


//-------
Learn::Learn(){
    trainDuration = 2;
    trainCountdown = 2;
    instanceRate = 15;
    inputsVisible = true;

    buildGuiInputOutput();
    buildGuiMenu();
    
    startTime = -(trainDuration + trainCountdown);
    idxSelectedOutput = -1;
}

//-------
void Learn::addInput(ofxParameter<float> *parameter) {
    InputParameter *newInput = new InputParameter(parameter);
    inputs.push_back(newInput);
    buildGuiInputs();
}

//-------
void Learn::addInput(string name, float min, float max) {
    InputParameter *newInput = new InputParameter(name, min, max);
    inputs.push_back(newInput);
    buildGuiInputs();
}

//-------
void Learn::addOutput(ofxParameter<float> *parameter) {
    OutputParameter *newOutput = new OutputParameter(parameter);
    outputs.push_back(newOutput);
    buildGuiOutputs();
}

//-------
void Learn::addOutput(string name, float min, float max) {
    OutputParameter *newOutput = new OutputParameter(name, min, max);
    outputs.push_back(newOutput);
    buildGuiOutputs();
}

//-------
void Learn::update(){
    /* recording procedure */
    if (recording) {
        float elapsed = ofGetElapsedTimef() - startTime;
        if (elapsed < trainCountdown) {
            // some countdown signal?
        }
        else if (elapsed < trainCountdown + trainDuration) {
            if (ofGetFrameNum() % framesPerInstance == 0) {
                recordInstance();
            }
        }
        else if (elapsed > trainCountdown + trainDuration) {
            ((ofxUIToggle *) guiTrain->getWidget("record"))->setValue(false);
            recording = false;
        }
    }
    
    /* prediction procedure */
    if (predicting) {
        for (int i=0; i<outputs.size(); i++) {
            if (outputs[i]->getTrained())
                outputs[i]->predict();
        }
    }
}

//-------
void Learn::draw(){
    if (dataVisible) {
        outputs[idxSelectedOutput]->drawSpreadsheet(520, 145);
    }
}

//-------
void Learn::startRecording() {
    startTime = ofGetElapsedTimef();
    framesPerInstance = ofGetFrameRate() / (float) instanceRate;
    recording = true;
}

//-------
void Learn::recordInstance() {
    for (int i=0; i<outputs.size(); i++) {
        if (!outputs[i]->getRecording())    continue;
        outputs[i]->addTrainingInstance();
        ((ofxUILabelButton *) guiO->getWidget("numExamples_"+ofToString(i)))->setLabelText(ofToString(outputs[i]->getNumExamples())+" examples");
    }
}

//-------
void Learn::trainClassifiers(string trainStrategy) {
    for (int i=0; i<outputs.size(); i++) {
        if (outputs[i]->getNumExamples() == 0)  continue;
        if (trainStrategy == "fast") {
            outputs[i]->trainClassifierFast();
        }
        else if (trainStrategy == "accurate") {
            outputs[i]->trainClassifierAccurate();
        }
    }
    ((ofxUIToggle *) guiMenu->getWidget("predicting"))->setValue(true);
    recording = false;
    predicting = true;
}

//-------
void Learn::setPredictingMode(bool predicting) {
    this->predicting = predicting;
    if (predicting) {
        guiI->setVisible(false);
        guiO->setVisible(false);
        guiIP->setVisible(true);
        guiOP->setVisible(true);
        ((ofxUIToggle *) guiMenu->getWidget("inputs"))->setValue(false);
        ((ofxUIToggle *) guiMenu->getWidget("outputs"))->setValue(false);
        ((ofxUIToggle *) guiMenu->getWidget("preview"))->setValue(true);
    }
}

//-------
void Learn::buildGuiInputOutput() {
    /* inputs */
    guiI = new ofxUIScrollableCanvas(5, 144, 500, 400);
    guiI->setDrawOutlineHighLight(true);
    guiI->setScrollableDirections(false, true);
    guiI->setFont("AndaleMono.ttf");
    guiI->setVisible(inputsVisible);
    
    /* outputs */
    guiO = new ofxUIScrollableCanvas(5, 144, 500, 400);
    guiO->setDrawOutlineHighLight(true);
    guiO->setScrollableDirections(false, true);
    guiO->setFont("AndaleMono.ttf");
    guiO->setVisible(!inputsVisible);
    
    /* preview guis */
    guiIP = new ofxUIScrollableCanvas(5, 144, 305, 400);
    guiIP->setDrawOutlineHighLight(true);
    guiIP->setScrollableDirections(false, true);
    guiIP->setFont("AndaleMono.ttf");
    guiIP->setVisible(false);
    guiOP = new ofxUIScrollableCanvas(340, 144, 305, 400);
    guiOP->setDrawOutlineHighLight(true);
    guiOP->setScrollableDirections(false, true);
    guiOP->setFont("AndaleMono.ttf");
    guiOP->setVisible(false);
    
    /* input selector gui */
    guiInputSelector = new ofxUICanvas(530, 144, 180, 400);
    guiInputSelector->addButton("name", false);
    guiInputSelector->setVisible(false);
    
    /* listeners */
    ofAddListener(guiI->newGUIEvent, this, &Learn::guiInputsEvent);
    ofAddListener(guiO->newGUIEvent, this, &Learn::guiOutputsEvent);
    ofAddListener(guiInputSelector->newGUIEvent, this, &Learn::guiInputSelectorEvent);
}

//-------
void Learn::buildGuiInputs() {
    guiI->clearWidgets();
    guiI->addLabel("Inputs")->setPadding(100);
    guiI->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiI->addButton("touchOSC", false);
    guiI->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    guiI->addSpacer(480, 2)->setPadding(12);
    for (int i=0; i<inputs.size(); i++) {
        guiI->addSlider(inputs[i]->getName(),
                        inputs[i]->getMin(),
                        inputs[i]->getMax(),
                        &inputs[i]->getValueRef(),
                        150.0f, 10.0f)->setLabelVisible(true);
        guiI->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        guiI->addLabel("min");
        guiI->addTextInput("min_"+ofToString(i), ofToString(inputs[i]->getMin()), 45.0f, 20.0f)->setAutoClear(false);
        guiI->addLabel("max");
        guiI->addTextInput("max_"+ofToString(i), ofToString(inputs[i]->getMax()), 45.0f, 20.0f)->setAutoClear(false);

        if (inputs[i]->getType() == PARAMETER) {
            // nothing
        }
        else if (inputs[i]->getType() == OSC) {
            guiI->addLabel("osc");
            guiI->addTextInput("oscAddress_"+ofToString(i), inputs[i]->getAddress(), 110.0f, 20.0f)->setAutoClear(false);
        }
        else if (inputs[i]->getType() == MIDI) {
            // channel
        }
        else if (inputs[i]->getType() == MANTA) {
            // pad #

        }
        else if (inputs[i]->getType() == KINECT) {
            // joint selection
        }
        
        guiI->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        guiI->addSpacer(480, 1)->setPadding(8);
    }
    guiI->autoSizeToFitWidgets();

    buildGuiPreview();
}

//-------
void Learn::buildGuiOutputs() {
    guiO->clearWidgets();
    guiO->addLabel("Outputs")->setPadding(100);
    guiO->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiO->addButton("touchOSC", false);
    guiO->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    guiO->addSpacer(480, 2)->setPadding(12);
    for (int i=0; i<outputs.size(); i++) {
        guiO->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        guiO->addSlider(outputs[i]->getName(),
                        outputs[i]->getMin(),
                        outputs[i]->getMax(),
                        &outputs[i]->getValueRef(),
                        150.0f, 10.0f)->setLabelVisible(true);
        guiO->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        guiO->addLabel("min");
        guiO->addTextInput("min_"+ofToString(i), ofToString(outputs[i]->getMin()), 45.0f, 20.0f)->setAutoClear(false);
        guiO->addLabel("max");
        guiO->addTextInput("max_"+ofToString(i), ofToString(outputs[i]->getMax()), 45.0f, 20.0f)->setAutoClear(false);

        if (outputs[i]->getType() == PARAMETER) {
        }
        else if (outputs[i]->getType() == OSC) {
            guiO->addLabel("osc");
            guiO->addTextInput("oscAddress", outputs[i]->getAddress(), 110.0f, 20.0f)->setAutoClear(false);
        }
        
        guiO->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        guiO->addLabelButton("inputs_"+ofToString(i), false, 100.0f)->setLabelText("inputs");
        guiO->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        guiO->addLabelButton("numExamples_"+ofToString(i), false, 100.0f)->setLabelText(ofToString(outputs[i]->getNumExamples())+" examples");
        guiO->addLabelButton("clear_"+ofToString(i), false, 60.0f)->setLabelText("clear");
        guiO->addToggle("record", &outputs[i]->getRecordingRef());
        guiO->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        guiO->addSpacer(480, 1)->setPadding(8);
    }
    guiO->autoSizeToFitWidgets();
    
    buildGuiPreview();
}

//-------
void Learn::buildGuiPreview() {
    guiIP->clearWidgets();
    guiIP->addLabel("Inputs");
    guiIP->addSpacer(300, 2)->setPadding(12);
    for (int i=0; i<inputs.size(); i++) {
        guiIP->addSlider(inputs[i]->getName(),
                        inputs[i]->getMin(),
                        inputs[i]->getMax(),
                        &inputs[i]->getValueRef(),
                        300.0f, 12.0f)->setLabelVisible(true);
        guiIP->addSpacer(300, 1)->setPadding(8);
    }
    guiIP->autoSizeToFitWidgets();
    guiOP->clearWidgets();
    guiOP->addLabel("Outputs");
    guiOP->addSpacer(300, 2)->setPadding(12);
    for (int i=0; i<outputs.size(); i++) {
        guiOP->addSlider(outputs[i]->getName(),
                         outputs[i]->getMin(),
                         outputs[i]->getMax(),
                         &outputs[i]->getValueRef(),
                         300.0f, 12.0f)->setLabelVisible(true);
        guiOP->addSpacer(300, 1)->setPadding(8);
    }
    guiOP->autoSizeToFitWidgets();
}

//-------
void Learn::buildGuiMenu() {
    guiMenu = new ofxUICanvas(5, 5, 390, 135);
    guiMenu->setDrawOutlineHighLight(true);
    guiMenu->setFont("AndaleMono.ttf");
    guiMenu->setGlobalButtonDimension(18);
    guiMenu->setPadding(8);
    guiMenu->addLabel("Menu")->setColorFill(ofxUIColor(50, 200, 50));
    guiMenu->addSpacer();
    
    vector<string> names;
    names.push_back("inputs");
    names.push_back("outputs");
    names.push_back("preview");
    guiMenu->setPadding(5);
    guiMenu->addRadio("view", names);
    ((ofxUIToggle *) guiMenu->getWidget("inputs"))->setValue(true);

    guiMenu->setPadding(8);
    guiMenu->setGlobalButtonDimension(56);
    guiMenu->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiMenu->addButton("load", false)->setLabelPosition(OFX_UI_WIDGET_POSITION_DOWN);
    guiMenu->addButton("save", false)->setLabelPosition(OFX_UI_WIDGET_POSITION_DOWN);
    guiMenu->addToggle("predicting", &predicting)->setLabelPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    guiTrain = new ofxUICanvas(400, 5, 360, 135);
    guiTrain->setDrawOutlineHighLight(true);
    guiTrain->setFont("AndaleMono.ttf");
    guiTrain->setGlobalButtonDimension(56);
    guiTrain->setPadding(8);
    guiTrain->addLabel("Train")->setColorFill(ofxUIColor(50, 200, 50));
    guiTrain->addSpacer();
    guiTrain->addToggle("record", false)->setLabelPosition(OFX_UI_WIDGET_POSITION_DOWN);
    guiTrain->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiTrain->addButton("fast", false)->setLabelPosition(OFX_UI_WIDGET_POSITION_DOWN);
    guiTrain->addButton("accurate", false)->setLabelPosition(OFX_UI_WIDGET_POSITION_DOWN);
    guiTrain->addWidgetEastOf(new ofxUISlider("countdown", 0.0, 5.0, &trainCountdown, 120.0f, 8.0f), "accurate")->setPadding(2);
    guiTrain->addWidgetSouthOf(new ofxUISlider("duration", 0.0, 5.0, &trainDuration, 120.0f, 8.0f), "countdown")->setPadding(2);
    guiTrain->addWidgetSouthOf(new ofxUIIntSlider("instanceRate", 1, 30, &instanceRate, 120.0f, 8.0f), "duration")->setPadding(2);

    ofAddListener(guiMenu->newGUIEvent, this, &Learn::guiMenuEvent);
    ofAddListener(guiTrain->newGUIEvent, this, &Learn::guiTrainEvent);
}

//-------
void Learn::buildGuiInputSelector(int idx) {
    idxSelectedOutput = idx;
    guiInputSelector->clearWidgets();
    
    vector<string> inputLabels;
    for (int i=0; i<inputs.size(); i++) {
        inputLabels.push_back(inputs[i]->getName());
    }
    ofxUIDropDownList *dropDown = guiInputSelector->addDropDownList("select inputs", inputLabels, 150.0f);
    dropDown->setAutoClose(false);
    dropDown->setAllowMultiple(true);
    dropDown->open();

    vector<InputParameter *> inputs = outputs[idxSelectedOutput]->getInputs();
    vector<ofxUILabelToggle *> toggles = dropDown->getToggles();
    for (int i=0; i<toggles.size(); i++) {
        string inputName = toggles[i]->getName();
        for (int j=0; j<inputs.size(); j++) {
            if (inputName == inputs[j]->getName()) {
                toggles[i]->setValue(true);
            }
        }
    }
    guiInputSelector->autoSizeToFitWidgets();
}

//-------
void Learn::guiInputsEvent(ofxUIEventArgs &e){
    for (int i=0; i<outputs.size(); i++) {
        if (e.getName() == "min_"+ofToString(i)) {
            float min = ofToFloat(((ofxUITextInput *) guiI->getWidget("min_"+ofToString(i)))->getTextString());
            inputs[i]->setMin(min);
            ((ofxUISlider *) guiI->getWidget(inputs[i]->getName()))->setMin(min);
        }
        else if (e.getName() == "max_"+ofToString(i)) {
            float max = ofToFloat(((ofxUITextInput *) guiI->getWidget("max_"+ofToString(i)))->getTextString());
            inputs[i]->setMax(max);
            ((ofxUISlider *) guiI->getWidget(inputs[i]->getName()))->setMax(max);

        }
        else if (e.getName() == "oscAddress_"+ofToString(i)) {
            inputs[i]->setAddress(((ofxUITextInput *) guiI->getWidget("oscAddress_"+ofToString(i)))->getTextString());
        }
    }
    if (e.getName() == "touchOSC") {
        if (e.getButton()->getValue() == 1) return;
        cout << "TOUCH OSC" << endl;
    }
}

//-------
void Learn::guiOutputsEvent(ofxUIEventArgs &e){
    for (int i=0; i<outputs.size(); i++) {
        if (e.getName() == "min_"+ofToString(i)) {
            float min = ofToFloat(((ofxUITextInput *) guiO->getWidget("min_"+ofToString(i)))->getTextString());
            outputs[i]->setMin(min);
            ((ofxUISlider *) guiO->getWidget(outputs[i]->getName()))->setMin(min);
        }
        else if (e.getName() == "max_"+ofToString(i)) {
            float max = ofToFloat(((ofxUITextInput *) guiO->getWidget("max_"+ofToString(i)))->getTextString());
            outputs[i]->setMax(max);
            ((ofxUISlider *) guiO->getWidget(outputs[i]->getName()))->setMax(max);
        }
        else if (e.getName() == "numExamples_"+ofToString(i)) {
            if (e.getButton()->getValue() == 1) return;
            if (i == idxSelectedOutput && dataVisible) {
                dataVisible = false;
            }
            else {
                guiInputSelector->setVisible(false);
                dataVisible = true;
                idxSelectedOutput = i;
            }
        }
        else if (e.getName() == "inputs_"+ofToString(i)) {
            if (e.getButton()->getValue() == 1) return;
            if (i == idxSelectedOutput && guiInputSelector->isVisible()) {
                guiInputSelector->setVisible(false);
            }
            else {
                buildGuiInputSelector(i);
                guiInputSelector->setVisible(true);
                dataVisible = false;
            }
        }
        else if (e.getName() == "clear_"+ofToString(i)) {
            if (e.getButton()->getValue() == 1) return;
            outputs[i]->clearTrainingExamples();
            ((ofxUILabelButton *) guiO->getWidget("numExamples_"+ofToString(i)))->setLabelText("0 examples");
        }
        else if (e.getName() == "record_"+ofToString(i)) {
            // done
        }
    }
    if (e.getName() == "touchOSC") {
        if (e.getButton()->getValue() == 1) return;
        cout << "TOUCH OSC" << endl;
    }
}

//-------
void Learn::guiInputSelectorEvent(ofxUIEventArgs &e){
    if (e.getParentName() == "select inputs") {
        outputs[idxSelectedOutput]->clearInputs();
        ofxUIDropDownList *dropDown = (ofxUIDropDownList *) guiInputSelector->getWidget("select inputs");
        vector<ofxUILabelToggle *> toggles = dropDown->getToggles();
        for (int j=0; j<toggles.size(); j++) {
            if (toggles[j]->getValue()) {
                outputs[idxSelectedOutput]->addInput(inputs[j]);
            }
        }
    }
}

//-------
void Learn::guiTrainEvent(ofxUIEventArgs &e) {
    if (e.getName() == "record") {
        startRecording();
    }
    else if (e.getName() == "fast" || e.getName() == "recording") {
        if (e.getButton()->getValue() == 1) return;
        trainClassifiers(e.getName());
    }
}

//-------
void Learn::guiMenuEvent(ofxUIEventArgs &e) {
    if (e.getName() == "view") {
        if (predicting) return;
        guiI->setVisible(inputsVisible);
        guiO->setVisible(!inputsVisible);
    }
    
    
    if (e.getName() == "inputs") {
        guiI->setVisible(true);
        guiO->setVisible(false);
        guiIP->setVisible(false);
        guiOP->setVisible(false);
    }
    else if (e.getName() == "outputs") {
        guiI->setVisible(false);
        guiO->setVisible(true);
        guiIP->setVisible(false);
        guiOP->setVisible(false);
    }
    else if (e.getName() == "preview") {
        guiI->setVisible(false);
        guiO->setVisible(false);
        guiIP->setVisible(true);
        guiOP->setVisible(true);
    }
    
    
    else if (e.getName() == "save") {
        if (e.getButton()->getValue() == 1) return;
        /*
         
         SAVING ROUTINE
         
         */
    }
    else if (e.getName() == "predicting") {
        setPredictingMode(e.getButton()->getValue());
    }
    else if (e.getName() == "load") {
        if (e.getButton()->getValue() == 1) return;
        ofFileDialogResult loadFile = ofSystemLoadDialog("load presets file");
        if (loadFile.bSuccess){
            //presets.load(loadFile.filePath);
            /*
             
             LOADING ROUTINE
             
             */
        }
    }
}

//-------
Learn::~Learn(){
    for (int i=0; i<inputs.size(); i++) {
        delete inputs[i];
    }
    for (int i=0; i<outputs.size(); i++) {
        delete outputs[i];
    }
    delete guiI;
    delete guiO;
    delete guiIP;
    delete guiOP;
    delete guiTrain;
    delete guiMenu;
    delete guiInputSelector;
}