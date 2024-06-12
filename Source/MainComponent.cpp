#include "MainComponent.h"
#include <fstream>

//==============================================================================
MainComponent::MainComponent()
{
    setSize (600, 260);
    loadNotesFile();
    if (!notesFile.exists()) {
        DBG("Creating notes file");
        createNotesFile();
        DBG(notesFile.getFullPathName());
    }
    else
    {
        DBG("Notes file already exists");
        DBG(notesFile.getFullPathName());
    }
	if (!controllerFile.exists())
    {
		DBG("Creating controller file");
		createControllerFile();
		DBG(controllerFile.getFullPathName());
	}
    else
    {
		DBG("Controller file already exists");
		DBG(controllerFile.getFullPathName());
	}

    loadActions();
	DBG("Actions loaded");
	DBG("Actions size: " << noteActions.size());
    settingsButton.addListener(this);
	auto midiInputs = juce::MidiInput::getAvailableDevices();
    if (!midiInputs.isEmpty())
    {
        DBG("MIDI Inputs available!");
        DBG("MIDI Input: " << midiInputs[0].name << " Identifier: " << midiInputs[0].identifier);

        // Keep a reference to the opened MIDI input device to prevent it from being destroyed
        midiInput = juce::MidiInput::openDevice(midiInputs[0].identifier, this);

        if (midiInput)
        {
            DBG("MIDI Input opened!");
            midiInput->start();
        }
    }
    else
    {
        DBG("No MIDI Inputs available!");
    }
	startTimerHz(30);
    addButton.setButtonText("+");
    addButton.addListener(this);
    action.setFont(juce::Font(11.0f, juce::Font::bold));
    action.setText("Action: ", juce::dontSendNotification);
    action.attachToComponent(&textInput2, true);
	note.setFont(juce::Font(11.0f, juce::Font::bold));
    note.setText("Note: ", juce::dontSendNotification);
    note.attachToComponent(&textInput1, true);
	Notes.setFont(juce::Font(20.0f, juce::Font::bold));
	Notes.setText("Notes", juce::dontSendNotification);
	Controllers.setFont(juce::Font(20.0f, juce::Font::bold));
	Controllers.setText("Controllers", juce::dontSendNotification);
    noteOrController.addItem("Note", 1);
    noteOrController.addItem("Controller", 2);
    noteOrController.onChange = [this] { comboBoxChanged(&noteOrController); };
    addAndMakeVisible(noteOrController);
	listIdentifiers();
	createComponets();


}

MainComponent::~MainComponent()
{
	if (midiInput)
	{
		midiInput->stop();
		midiInput.reset();
	}
   
}

void MainComponent::timerCallback()
{
	repaint();
}

void MainComponent::saveContentToFile(juce::TextEditor* note, int index, int type )
{
    if (type == 1) {

        juce::File file(juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyNotes.txt")); // Substitua pelo caminho correto do arquivo
        juce::String text = note->getText() + " " + textEditors[index]->getText() + "\n";
        file.appendText(text);
    }
    else
	{
		juce::File file(juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyControllers.txt")); // Substitua pelo caminho correto do arquivo
		juce::String text = note->getText() + " " + controllersTextEditors[index]->getText() + "\n";
		file.appendText(text);
	}
	
}

bool MainComponent::keyPressed(const juce::KeyPress& key, Component* originatingComponent)
{
	
	repaint();
	return true;
}

void MainComponent::createNotesFile()
{
    //Create a txt file to store notes
    notesFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyNotes.txt");
    notesFile.create();
}

void MainComponent::createControllerFile()
{
    
    //Create a txt file to store notes
    controllerFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyControllers.txt");
    controllerFile.create();
    
}

void MainComponent::addLine(juce::String note, juce::String action)
{
    if (noteOrController.getSelectedId() == 1)
    {
        juce::File file(juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyNotes.txt")); // Substitua pelo caminho correto do arquivo
        juce::String text = note + " " + action + "\n";
        file.appendText(text);
        reload();
		
	}
    else
    {
		juce::File file(juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyControllers.txt")); // Substitua pelo caminho correto do arquivo
		juce::String text = note + " " + action + "\n";
		file.appendText(text);
		reload();
		
	}
}

void MainComponent::deleteLine(int index, int type)
{
	DBG("Deleting line: " << index);
    if (type == 1) {
        juce::File file(juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyNotes.txt")); // Substitua pelo caminho correto do arquivo
        juce::String text = "";
        for (int i = 0; i < textEditors.size(); ++i)
        {
            if (i != index)
            {
                text += notesTextEditors[i]->getText() + " " + textEditors[i]->getText() + "\n";
            }
        }
        file.replaceWithText(text);
        
    }
	else
	{
		juce::File file(juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyControllers.txt")); // Substitua pelo caminho correto do arquivo
		juce::String text = "";
		for (int i = 0; i < controllersTextEditors.size(); ++i)
		{
			if (i != index)
			{
				text += controllersNotesTextEditors[i]->getText() + " " + controllersTextEditors[i]->getText() + "\n";
			}
		}
		file.replaceWithText(text);
		
	}
    reload();
}

void MainComponent::loadNotesFile()
{
   
    notesFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyNotes.txt");
    
}

void MainComponent::loadControllerFile()
{
	controllerFile = juce::File::getSpecialLocation(juce::File::currentApplicationFile).getParentDirectory().getChildFile("StrokeyControllers.txt");
}

void MainComponent::loadActions()
{
	DBG("Actions size: " << noteActions.size());
    DBG("controller size" << controllerActions.size());
    juce::File file(notesFile.getFullPathName());
	juce::File file2(controllerFile.getFullPathName());

    std::ifstream inputfile(notesFile.getFullPathName().toStdString());
    if (!inputfile.is_open())
    {
        DBG("Error opening file");
        return;
    }
    std::string line;
    while (std::getline(inputfile, line))
    {
        std::istringstream iss(line);
        int note;
        std::string action;
        if (!(iss >> note >> action)) { break; } // error
        noteActions[note] = action;
    }
    inputfile.close();

	std::ifstream inputfile2(controllerFile.getFullPathName().toStdString());
    if (!inputfile2.is_open())
    {
        DBG("Error opening file");
        return;
    }
	std::string line2;
	while (std::getline(inputfile2, line2))
	{
		std::istringstream iss(line2);
		int controller;
		std::string action;
		if (!(iss >> controller >> action)) { break; } // error
		controllerActions[controller] = action;
	}
	inputfile2.close();

    DBG("Actions size: " << noteActions.size());
	DBG("controller size" << controllerActions.size());
}

void MainComponent::recreateFile()
{
}

void MainComponent::loadActionsIntoDisplay()
{
    juce::String content;
    for (auto const& action : noteActions)
    {
        content += "Note: " + juce::String(action.first) + "           Action: " + action.second + "\n";
    }
    actionsDisplay.setText(content);
}

void MainComponent::reload()
{
    removeAllChildren();
    resetActions();
    loadNotesFile();
	loadControllerFile();
	loadActions();
    createComponets();
    resized();
}

void MainComponent::resetActions()
{
    DBG("actions size: " << noteActions.size());
    noteActions.clear();
	controllerActions.clear();
    DBG("actions size: " << noteActions.size());
    textEditors.clear();
    editButtons.clear();
    saveButtons.clear();
    deleteButtons.clear();
    notesTextEditors.clear();
	controllersTextEditors.clear();
	controllersEditButtons.clear();
	controllersSaveButtons.clear();
	controllersDeleteButtons.clear();
	controllersNotesTextEditors.clear();

}

void MainComponent::createComponets()
{
    addAndMakeVisible(settingsButton);
	container.removeAllChildren();
    container.addAndMakeVisible(addButton);
    container.addAndMakeVisible(textInput1);
    container.addAndMakeVisible(textInput2);
	container.addAndMakeVisible(noteOrController);
	if (noteActions.size() > 0)
	    container.addAndMakeVisible(Notes);
	if (controllerActions.size() > 0)
	    container.addAndMakeVisible(Controllers);

    for (auto& action : noteActions)
    {
        auto* editButton = new juce::TextButton("Edit");
        auto* saveButton = new juce::TextButton("Save");
        auto* deleteButton = new juce::TextButton("Del");

        editButton->addListener(this);
        saveButton->addListener(this);
        deleteButton->addListener(this);

        editButtons.add(editButton);
        saveButtons.add(saveButton);
        deleteButtons.add(deleteButton);
        auto* editor = new juce::TextEditor();
        auto* note = new juce::TextEditor();
        editor->setText(action.second);
        note->setText(juce::String(action.first), juce::dontSendNotification);
        editor->setMultiLine(false);
        editor->setReturnKeyStartsNewLine(false);
        editor->setScrollbarsShown(false);
        editor->setCaretVisible(true);
        editor->setReadOnly(true);
        editor->addListener(this); // Adicione MainComponent como ouvinte se necessário
        note->setMultiLine(false);
        note->setReturnKeyStartsNewLine(false);
        note->setScrollbarsShown(false);
        note->setCaretVisible(true);
        note->setReadOnly(true);
        note->addListener(this);

        container.addAndMakeVisible(editor);
        container.addAndMakeVisible(note);
        container.addAndMakeVisible(editButton);
        container.addAndMakeVisible(saveButton);
        container.addAndMakeVisible(deleteButton);

        textEditors.add(editor); // Armazene os editores em um vetor para gerenciamento
        notesTextEditors.add(note);
    }
	for (auto& action : controllerActions)
    {
		auto* editButton = new juce::TextButton("Edit");
		auto* saveButton = new juce::TextButton("Save");
		auto* deleteButton = new juce::TextButton("Del");

		editButton->addListener(this);
		saveButton->addListener(this);
		deleteButton->addListener(this);

		controllersEditButtons.add(editButton);
		controllersSaveButtons.add(saveButton);
		controllersDeleteButtons.add(deleteButton);
		auto* editor = new juce::TextEditor();
		auto* note = new juce::TextEditor();
		editor->setText(action.second);
		note->setText(juce::String(action.first), juce::dontSendNotification);
		editor->setMultiLine(false);
		editor->setReturnKeyStartsNewLine(false);
		editor->setScrollbarsShown(false);
		editor->setCaretVisible(true);
		editor->setReadOnly(true);
		editor->addListener(this); // Adicione MainComponent como ouvinte se necessário
		note->setMultiLine(false);
		note->setReturnKeyStartsNewLine(false);
		note->setScrollbarsShown(false);
		note->setCaretVisible(true);
		note->setReadOnly(true);
		note->addListener(this);

		container.addAndMakeVisible(editor);
		container.addAndMakeVisible(note);
		container.addAndMakeVisible(editButton);
		container.addAndMakeVisible(saveButton);
		container.addAndMakeVisible(deleteButton);

		controllersTextEditors.add(editor); // Armazene os editores em um vetor para gerenciamento
		controllersNotesTextEditors.add(note);
	}
    viewport.setViewedComponent(&container, false);
    //viewport.setViewPosition(0, 0);
    addAndMakeVisible(viewport);
    int yPosition = 130;
    int position = 0;
	Notes.setBounds(0, 100, getWidth(), 20);
	Notes.setJustificationType(juce::Justification::centred);
    for (auto* editor : textEditors)
    {
        auto* note = notesTextEditors[position];
        note->setBounds(10, yPosition, 40, 20);
        editor->setBounds(60, yPosition, getWidth() - 230, 20);
        yPosition += 25; // Ajuste conforme necessário para o espaçasmento
        position++;
    }
    for (int i = 0; i < textEditors.size(); ++i)
    {
        auto editorBounds = textEditors[i]->getBounds();
        editButtons[i]->setBounds(editorBounds.getRight() + 10, editorBounds.getY(), 40, 20);
        saveButtons[i]->setBounds(editorBounds.getRight() + 60, editorBounds.getY(), 40, 20);
        deleteButtons[i]->setBounds(editorBounds.getRight() + 110, editorBounds.getY(), 40, 20);
    }
    position = 0;
	Controllers.setBounds(0, yPosition, getWidth(), 20);
	Controllers.setJustificationType(juce::Justification::centred);
	yPosition += 30;
    for (auto* editor : controllersTextEditors) {
        auto* note = controllersNotesTextEditors[position];
        note->setBounds(10, yPosition, 40, 20);
        editor->setBounds(60, yPosition, getWidth() - 230, 20);
        yPosition += 25; // Ajuste conforme necessário para o espaçasmento
        position++;
    }
    for (int i = 0; i < controllersTextEditors.size(); ++i)
    {
        auto editorBounds = controllersTextEditors[i]->getBounds();
        controllersEditButtons[i]->setBounds(editorBounds.getRight() + 10, editorBounds.getY(), 40, 20);
        controllersSaveButtons[i]->setBounds(editorBounds.getRight() + 60, editorBounds.getY(), 40, 20);
        controllersDeleteButtons[i]->setBounds(editorBounds.getRight() + 110, editorBounds.getY(), 40, 20);
    }
	resized();
}

void MainComponent::updateActions(int note, std::string action)
{
    noteActions[note] = action;
    std::ofstream file(notesFile.getFullPathName().toStdString());
    for (auto const& x : noteActions)
    {
        file << x.first << " " << x.second << std::endl;
    }
    file.close();
    loadActionsIntoDisplay();
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &noteOrController)
	{
		if (noteOrController.getSelectedId() == 1)
		{
			DBG("Note selected");
			
		}
		else
		{
			DBG("Controller selected");
			
		}
	}

}

void MainComponent::increaseVolume()
{
    CoInitialize(nullptr);
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);

    IMMDevice* defaultDevice = nullptr;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    // Obtém a interface IAudioEndpointVolume
    IAudioEndpointVolume* endpointVolume = nullptr;
    defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (void**)&endpointVolume);

    // Aumenta o volume
    float currentVolume = 0;
    endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
    endpointVolume->SetMasterVolumeLevelScalar(currentVolume + 0.1f, nullptr); // Aumenta o volume em 10%

    // Limpeza
    endpointVolume->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();
}

void MainComponent::setVolume(float volume)
{
	CoInitialize(nullptr);
	IMMDeviceEnumerator* deviceEnumerator = nullptr;
	CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);

	IMMDevice* defaultDevice = nullptr;
	deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

	// Obtém a interface IAudioEndpointVolume
	IAudioEndpointVolume* endpointVolume = nullptr;
	defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (void**)&endpointVolume);

	// Ajusta o volume
	endpointVolume->SetMasterVolumeLevelScalar(volume, nullptr);

	// Limpeza
	endpointVolume->Release();
	defaultDevice->Release();
	deviceEnumerator->Release();
	CoUninitialize();
}

void MainComponent::setAppVolume(float volume, juce::String app)
{
    CoInitialize(nullptr);

    // Obtém o dispositivo de áudio padrão
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);

    IMMDevice* defaultDevice = nullptr;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    IAudioSessionManager2* sessionManager2 = nullptr;
    defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&sessionManager2);

    IAudioSessionEnumerator* sessionEnumerator = nullptr;
    sessionManager2->GetSessionEnumerator(&sessionEnumerator);

    int sessionCount = 0;
    sessionEnumerator->GetCount(&sessionCount);

    // Itera pelas sessões de áudio para encontrar a sessão do aplicativo especificado
    for (int i = 0; i < sessionCount; ++i) {
        IAudioSessionControl* sessionControl = nullptr;
        sessionEnumerator->GetSession(i, &sessionControl);

        IAudioSessionControl2* sessionControl2 = nullptr;
        sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sessionControl2);

        LPWSTR sessionIdentifier = nullptr;
        sessionControl2->GetSessionIdentifier(&sessionIdentifier);

        // Verifica se a sessão contém o identificador do aplicativo
        if (juce::String(sessionIdentifier).contains(app))
        {
            ISimpleAudioVolume* simpleAudioVolume = nullptr;
            sessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&simpleAudioVolume);

            // Define o volume da sessão
            simpleAudioVolume->SetMasterVolume(volume, nullptr);

            // Limpeza
            simpleAudioVolume->Release();
        }

        // Limpeza
        CoTaskMemFree(sessionIdentifier);
        sessionControl2->Release();
        sessionControl->Release();
    }

    // Mais limpeza
    sessionEnumerator->Release();
    sessionManager2->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();
}

void MainComponent::listIdentifiers()
{
    CoInitialize(nullptr);

    // Obtém o dispositivo de áudio padrão
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);

    IMMDevice* defaultDevice = nullptr;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    IAudioSessionManager2* sessionManager2 = nullptr;
    defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, (void**)&sessionManager2);

    IAudioSessionEnumerator* sessionEnumerator = nullptr;
    sessionManager2->GetSessionEnumerator(&sessionEnumerator);

    int sessionCount = 0;
    sessionEnumerator->GetCount(&sessionCount);

    // Itera pelas sessões de áudio
    for (int i = 0; i < sessionCount; ++i) {
        IAudioSessionControl* sessionControl = nullptr;
        sessionEnumerator->GetSession(i, &sessionControl);

        IAudioSessionControl2* sessionControl2 = nullptr;
        sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sessionControl2);

        LPWSTR sessionIdentifier = nullptr;
        sessionControl2->GetSessionIdentifier(&sessionIdentifier);

        // Mostra o identificador da sessão
		DBG("Session Identifier: " << juce::String(sessionIdentifier));

        // Limpeza
        CoTaskMemFree(sessionIdentifier);
        sessionControl2->Release();
        sessionControl->Release();
    }

    // Mais limpeza
    sessionEnumerator->Release();
    sessionManager2->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();
}

void MainComponent::mouseDown(const juce::MouseEvent& event)
{
}

void MainComponent::labelTextChanged(juce::Label* labelThatHasChanged)
{
}

void MainComponent::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
    DBG("Midi Message: " << message.getDescription());
	lastNote = message.getNoteNumber();
    if (message.isNoteOn())
    {
        int note = message.getNoteNumber();

        if (noteActions.find(note) != noteActions.end())
        {
            DBG("Note On: " + juce::String(note));
            juce::String action = noteActions[note];
            DBG("Action: " + action);
            if (action == "master") {
                setVolume(50 / 127.0f);
            }
            else
            {
                juce::File newfile(action);
                if (newfile.exists())
                    newfile.startAsProcess();
                else
                {
                    DBG("File does not exist");
                }
            }
        }
        else
        {
            DBG("nada");
            DBG("Note On: " + juce::String(note));
        }

        DBG("Note On: " + juce::String(note));

        //system("start calc.exe");
    }
    else if (message.isController())
    {
        int controller = message.getControllerNumber();
        int value = message.getControllerValue();


        if (controllerActions.find(controller) != controllerActions.end()) {
            DBG("Controller On: " + juce::String(controller));
            juce::String action = controllerActions[controller];
            DBG("Action: " + action);
            if (action == "master") {
                setVolume(value / 127.0f);
            }
            else if (!action.contains("\\")) {
				setAppVolume(value / 127.0f, action);
            }
            else
            {
                juce::File newfile(action);
                if (newfile.exists())
                    newfile.startAsProcess();
                else
                {
                    DBG("File does not exist");
                }
            }
        }
        else
        {
            DBG("nada");
            DBG("Controller: " + juce::String(controller));
        }

    }
    else if (message.isSysEx())
    {
        DBG("SysEx");
    }

}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));



    // Screen to show the key pressed
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
	juce::Rectangle<int> area(0, 5, getWidth(), 20);
    g.drawFittedText("Note Pressed: " + juce::String(lastNote), area, juce::Justification::centred, 1);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
	settingsButton.setBounds(10, 5, 50, 20);
	noteOrController.setBounds(10, 70, 100, 20);
	addButton.setBounds(getWidth() - 80, 10, 50, 50);
	note.setBounds(10, 10, 40, 20);
	action.setBounds(10, 40, 40, 20);
    textInput1.setBounds(60, 10, getWidth() - 150, 20);
    textInput2.setBounds(60, 40, getWidth() - 150, 20);

	int totalHeight = 0;
    for (auto* child : container.getChildren()) {
		totalHeight += child->getHeight();
    }
    container.setSize(getWidth(), totalHeight);
    
    viewport.setScrollBarsShown(true, false);
    viewport.setBounds(0, 30, getWidth(), getHeight());
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &settingsButton)
    {
        showAudioSettings();
    }
    else if (button == &addButton)
    {
        DBG("Button clicked!");
        DBG("Text Input 1: " << textInput1.getText());
        DBG("Text Input 2: " << textInput2.getText());
        addLine(textInput1.getText(), textInput2.getText());
        textInput1.clear();
        textInput2.clear();
        
    }

    for (int i = 0; i < editButtons.size(); ++i)
    {
        if (button == editButtons[i])
        {
            // Torna o TextEditor correspondente editável
            textEditors[i]->setReadOnly(false);
            textEditors[i]->setCaretVisible(true);
            textEditors[i]->grabKeyboardFocus();
        }
        else if (button == saveButtons[i])
        {
            textEditors[i]->setReadOnly(true);
            textEditors[i]->setCaretVisible(false);
            // Salva o conteúdo do TextEditor no arquivo
            DBG("note: " << notesTextEditors[i]->getText());
            saveContentToFile(notesTextEditors[i], i,1);

        }
        else if (button == deleteButtons[i])
        {
            deleteLine(i, 1);

        }
    }
	for (int i = 0; i < controllersEditButtons.size(); ++i)
	{
		if (button == controllersEditButtons[i])
		{
			// Torna o TextEditor correspondente editável
			controllersTextEditors[i]->setReadOnly(false);
			controllersTextEditors[i]->setCaretVisible(true);
			controllersTextEditors[i]->grabKeyboardFocus();
		}
		else if (button == controllersSaveButtons[i])
		{
			controllersTextEditors[i]->setReadOnly(true);
			controllersTextEditors[i]->setCaretVisible(false);
			// Salva o conteúdo do TextEditor no arquivo
			DBG("note: " << controllersNotesTextEditors[i]->getText());
			saveContentToFile(controllersNotesTextEditors[i], i,2);

		}
		else if (button == controllersDeleteButtons[i])
		{
			deleteLine(i,2);

		}
	}

    loadActions();

}