#pragma once

#include "ofMain.h"
#include "ofxLearn.h"
#include "ofxSpreadsheet.h"
#include "Control.h"



//-----------
class LearnParameter : public Parameter<float>
{
public:
    LearnParameter(string name, float *value, float min=0, float max=1);
    ~LearnParameter();
    
    void set(float value);
    void setName(string name);
    void setOscAddress(string oscAddress);
    void setMin(float min);
    void setMax(float max);
    
    virtual void setVisible(bool visible);
    void setGuiPosition(int x, int y) {gui->setPosition(x, y);}
    bool isVisible() {return visible;}
    void deselect();

    template<typename ListenerClass, typename ListenerMethod>
    void addParameterSelectedListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(pSelectedEvent, listener, method);
    }

    template<typename ListenerClass, typename ListenerMethod>
    void addParameterChangedListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(pChangedEvent, listener, method);
    }
    
    template<typename ListenerClass, typename ListenerMethod>
    void addParameterDeletedListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(pDeletedEvent, listener, method);
    }

protected:
    
    virtual void setupGui() { }

    void guiEvent(ofxUIEventArgs &e);
    void guiSetName();
    void guiSetOscAddress();
    void guiSetMin();
    void guiSetMax();
    void guiSetValueText();

    ofxUICanvas *gui;
    ofxUISlider *guiValue;
    ofxUITextInput *guiValueText, *guiMin, *guiMax, *guiOsc, *guiName;
    bool visible;
    
    ofEvent<LearnParameter> pSelectedEvent, pChangedEvent, pDeletedEvent;
};



//-----------
class LearnInputParameter : public LearnParameter
{
public:
    LearnInputParameter(string name, float *value, float min=0, float max=1);

protected:
    void setupGui();
    void guiEvent(ofxUIEventArgs &e);
};



//-----------
class LearnOutputParameter : public LearnParameter
{
public:
    ~LearnOutputParameter();
    LearnOutputParameter(string name, float *value, float min=0, float max=1);
    
    virtual void draw();

    void setInputParameters(vector<LearnInputParameter *> &allInputs);
    void setupHeaders(int p=-1);
    void addDataPage();
    void setPage(int p);

    void addInstance(vector<float> instance);
    void addInstance();
    void clearInstances();
    int getNumInstances();
    vector<vector<vector<float> > > getInstances();

    void addInput(LearnInputParameter * input);
    bool removeInput(LearnInputParameter * input);
    int getNumInputs() {return activeInputs.size();}
    vector<LearnInputParameter *> & getActiveInputs() {return activeInputs;}
    bool getInputActive(LearnInputParameter * input);
    
    void setVisible(bool visible);
    void setGuiPosition(int x, int y);
    void setupGui();
    void setupGuiInputSelector();
    void setInputsVisible(bool b);
    void setExamplesVisible(bool b);
    void setDataSize(int width, int height);

    bool getRecording() {return record;}
    bool getTrained() {return trained;}
    
    void trainClassifierFast();
    void trainClassifierAccurate();
    void predict();
    
    template<typename ListenerClass, typename ListenerMethod>
    void addParameterViewedListener(ListenerClass *listener, ListenerMethod method) {
        ofAddListener(pViewedEvent, listener, method);
    }
    
protected:
    
    template <typename T> vector<T> grabFeatureVector(bool labelFirst);
    void addSpreadsheetDataToLearn();
    
    void guiEvent(ofxUIEventArgs &e);
    void guiInputSelectEvent(ofxUIEventArgs &e);
    void guiDataEvent(ofxUIEventArgs &e);
    void dataChangedEvent(bool &b);
    
    ofxUICanvas *guiInputSelect, *guiData;
    ofxUIDropDownList *guiSelector;
    ofxUILabelToggle *guiInputs, *guiExamples;
    ofxUILabelButton *guiDataStatus, *guiDataPage;
    
    ofxLearn learn;
    vector<LearnInputParameter *> allInputs, activeInputs;

    vector<ofxSpreadsheet *> data;
    int page, dataWidth, dataHeight;
    
    bool record, trained;
    bool viewExamples, viewInputs;
    
    ofEvent<LearnOutputParameter> pViewedEvent;
};


//--------
template <typename T> vector<T> LearnOutputParameter::grabFeatureVector(bool labelFirst) {
    vector<T> instance;
    if (labelFirst) instance.push_back((T) get());
    for (int i=0; i<activeInputs.size(); i++) {
        instance.push_back(activeInputs[i]->get());
    }
    return instance;
}


