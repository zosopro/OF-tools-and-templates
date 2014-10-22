#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "LearnParameter.h"
#include "ThreadedLearner.h"
#include "Control.h"



/* Constants?
 
 trainDuration = 2;
 trainCountdown = 2;
 instanceRate = 15;
 ---
 inputs/outputs X, Y
 ---
 osc default ports, default host

 */




class Learn
{
public:
    ~Learn();
    Learn(bool init=true);
    
    void update();
    void draw();
    
    // managing parameters
    virtual LearnInputParameter  * addInput (string name, float *value, float min, float max);
    virtual LearnOutputParameter * addOutput(string name, float *value, float min, float max);
    LearnInputParameter  * addInput (string name, float min, float max);
    LearnOutputParameter * addOutput(string name, float min, float max);
    void addParameterAsInput(string name, LearnInputParameter* newInput);
    void addParametersAsInput(string name, vector<LearnInputParameter*> &newInputs);
    void initializeOutput(LearnOutputParameter *output);
    
    // osc
    void setupOscSender(string host, int port);
    void setupOscReceiver(int port);
    void enableOscInputs(bool enable);
    void enableOscOutputs(bool enable);

    // visibility
    void setVisible(bool visible);
    void setGuiInputsVisible(bool visible);
    void setGuiOutputsVisible(bool visible);
    void toggleVisible() {setVisible(!visible);}
    
    // touch osc
    void saveInputsToTouchOsc();
    void saveOutputsToTouchOsc();
    void saveInputsAndOutputsToTouchOsc();
    
    // events
    virtual void inputParameterChanged(LearnParameter & input);
    virtual void inputParameterDeleted(LearnParameter & input);
    virtual void outputParameterChanged(LearnParameter & output);
    virtual void outputParameterDeleted(LearnParameter & output);
    virtual void outputParameterViewed(LearnOutputParameter & output);
    virtual void parameterSelected(LearnParameter & parameter);
    
    
protected:
    
    // managing parameters
    virtual void resetInputs();
    virtual void resetInputGroups();
    virtual void resetOutputs();

    // osc events
    void oscEventSetRecording(bool &b);
    void oscEventSetPredicting(bool &b);

    // gui
    virtual void setupGui();
    virtual void resetGuiPositions();
    void gui1Event(ofxUIEventArgs &e);
    void gui2Event(ofxUIEventArgs &e);
    void gui3Event(ofxUIEventArgs &e);
    
    // record + learn
    void startRecording();
    void stopRecording();
    void recordInstance();
    void trainClassifiers(string trainStrategy);
    
    // presets
    bool savePreset(string filename="");
    void saveInputs(string filename, ofXml &xml);
    void saveOutputs(string filename, ofXml &xml);
    string loadPresetDialog(string filename);
    void loadPreset(string path);
    void loadInputs(ofXml &xml);
    void loadOutputs(ofXml &xml, bool loadExamples=true, bool loadClassifier=true);
    virtual void resetPresets();
    
    // style
    void setFont(string path);
    void setFontSizes(int small, int medium, int large);
    
    // parameters and groups
    vector<LearnInputParameter *> inputs;
    vector<LearnOutputParameter *> outputs;
    vector<LearnOutputParameter::GuiInputGroup> inputGroups;
    
    // osc management
    OscManager oscManager;
    bool oscInActive, oscOutActive;
    string oscOutputHost;
    int oscOutputPort, oscInputPort;
    
    // gui
    ofxUICanvas *gui1, *gui2, *gui3;
    ofxUIDropDownList *guiSelector;
    ofxUILabel *guiStatusLabel;
    bool visible, inputsVisible, outputsVisible;
    
    // recording variables
    float startTime, trainCountdown, trainDuration;
    int instanceRate, framesPerInstance, currentNewInstances;
    bool inRecording, countingDown, recording, predicting, training;
    int newInputCounter, newOutputCounter;
    
    // style
    bool customFont;
    string fontPath;
    int fontSmall, fontMedium, fontLarge;
    
    // learn
    ThreadedLearner threadedLearn;
    string trainStrategy;
    int idxTraining;
};
