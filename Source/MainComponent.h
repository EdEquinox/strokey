#pragma once

#include <JuceHeader.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <Audiopolicy.h>
#include <Mmdeviceapi.h>
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::Component, public juce::MidiInputCallback, public juce::Button::Listener, public juce::KeyListener, public juce::Timer, public juce::Label::Listener, public juce::TextEditor::Listener, public juce::ComboBox::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;
	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    void timerCallback() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
  
    void MainComponent::saveContentToFile(juce::TextEditor* note, int index, int type);
    
    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;
    void createNotesFile();
    void createControllerFile();
    void addLine(juce::String note, juce::String action);
    void deleteLine(int index, int type);
	
    juce::File getNotesFile() { return notesFile; }
    void loadNotesFile();
    void loadControllerFile();
    void loadActions();
    void recreateFile();
    void loadActionsIntoDisplay();
    void reload();
    void resetActions();
	void createComponets();
    void updateActions(int note, std::string action);
    void mouseDown(const juce::MouseEvent& event) override;
	void labelTextChanged(juce::Label* labelThatHasChanged) override;
	void increaseVolume();
	void setVolume(float volume);
    void setAppVolume(float volume, juce::String app);
	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
	void listIdentifiers();
private:
    //==============================================================================
    // Your private member variables go here...
    juce::TextButton settingsButton{ "Settings" };
    juce::AudioDeviceManager deviceManager;
    std::unique_ptr<juce::MidiInput> midiInput;
    int lastNote = -1;
    juce::TextButton addButton;
    juce::TextEditor textInput1;
    juce::TextEditor textInput2;
    juce::TextButton minimizeButton;
    juce::File notesFile = "";
	juce::File controllerFile = "";
    std::map<int, std::string> noteActions;
	std::map<int, std::string> controllerActions;
    juce::TextEditor actionsDisplay;
    juce::Label editableLabel;
    juce::OwnedArray<juce::TextEditor> textEditors;
    juce::OwnedArray<juce::TextButton> editButtons;
    juce::OwnedArray<juce::TextButton> saveButtons;
    juce::OwnedArray<juce::TextButton> deleteButtons;
    juce::OwnedArray<juce::TextEditor> notesTextEditors;
    juce::OwnedArray<juce::TextEditor> controllersTextEditors;
    juce::OwnedArray<juce::TextButton> controllersEditButtons;
    juce::OwnedArray<juce::TextButton> controllersSaveButtons;
    juce::OwnedArray<juce::TextButton> controllersDeleteButtons;
    juce::OwnedArray<juce::TextEditor> controllersNotesTextEditors;
    juce::Component container; // Componente que contém todas as linhas
    juce::Viewport viewport;   // O Viewport que permite a rolagem
	juce::Label notesLabel;
    juce::Label note;
	juce::Label action;
    juce::ComboBox noteOrController;
    juce::Label Notes;
	juce::Label Controllers;

    void showAudioSettings()
    {
        auto* audioSettingsComp = new juce::AudioDeviceSelectorComponent(deviceManager,
            0, 2, 0, 2,
            true, true, true, false);
        audioSettingsComp->setSize(500, 450);

        juce::DialogWindow::LaunchOptions o;
        o.content.setOwned(audioSettingsComp);
        o.dialogTitle = "Audio Settings";
        o.componentToCentreAround = this;
        o.dialogBackgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
        o.escapeKeyTriggersCloseButton = true;
        o.useNativeTitleBar = true;
        o.resizable = false;

        o.launchAsync();
    }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
