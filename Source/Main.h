/*
  ==============================================================================

    Main.h
    Created: 11 Jun 2024 6:55:35pm
    Author:  edequ

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MainComponent.h"
#include "BinaryData.h"
#include "Main.h"

class teste2Application;

class MainWindow : public juce::DocumentWindow
{
public:
    typedef void (teste2Application::* AppWindowFunc)();
    MainWindow(juce::String name, teste2Application* window, AppWindowFunc func)
        : DocumentWindow(name,
            juce::Desktop::getInstance().getDefaultLookAndFeel()
            .findColour(juce::ResizableWindow::backgroundColourId),
            DocumentWindow::allButtons), appWindow(window), function(func)
    {
        setUsingNativeTitleBar(false);
        setContentOwned(new MainComponent(), true);

#if JUCE_IOS || JUCE_ANDROID
        setFullScreen(true);
#else
        setResizable(false, false);
        centreWithSize(getWidth(), getHeight());
#endif

        setTitleBarButtonsRequired(DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);


    }

    void minimiseButtonPressed() override{
        (appWindow->*function)();
    }

    void closeButtonPressed() override
    {
        // This is called when the user tries to close this window. Here, we'll just
        // ask the app to quit when this happens, but you can change this to do
        // whatever you need.
		juce::JUCEApplication::quit();
    }

    /* Note: Be careful if you override any DocumentWindow methods - the base
       class uses a lot of them, so by overriding you might break its functionality.
       It's best to do all your work in your content component instead, but if
       you really have to override any DocumentWindow methods, make sure your
       subclass also calls the superclass's method.
    */

private:
    teste2Application* appWindow;
    AppWindowFunc function;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};



class MyAppIconTray : public juce::SystemTrayIconComponent
{
public:
    typedef void (teste2Application::* AppWindowFunc)();

    MyAppIconTray(teste2Application* window, AppWindowFunc func) : appWindow(window), function(func)
    {
        juce::Image myIcon = juce::ImageCache::getFromMemory(BinaryData::tray_arrow_png, BinaryData::tray_arrow_pngSize);
        setIconImage(myIcon, myIcon);
        setIconTooltip("Strokey");
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        // Right-click: show context menu
        if (event.mods.isLeftButtonDown())
        {

            (appWindow->*function)();

        }
        else if (event.mods.isRightButtonDown()) {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    }


private:
    teste2Application* appWindow;
    AppWindowFunc function;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyAppIconTray)
};

//==============================================================================
class teste2Application : public juce::JUCEApplication
{
public:
    //==============================================================================
    teste2Application() {}

    const juce::String getApplicationName() override { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    //==============================================================================
    void initialise(const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

        mainWindow.reset(new MainWindow(getApplicationName(), this, &teste2Application::minimizeToTray));
		mainWindow->setVisible(true);
		trayIcon = std::make_unique<MyAppIconTray>(this, &teste2Application::restoreFromTray);

    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
        trayIcon = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    void minimizeToTray()
    {
        if (mainWindow != nullptr)
            juce::MessageManager::callAsync([this] { mainWindow->setVisible(false); });

    }

    void restoreFromTray()
    {
        if (mainWindow != nullptr)
            juce::MessageManager::callAsync([this] { mainWindow->setVisible(true); });

    }

	static teste2Application* getInstance()
	{
		return dynamic_cast<teste2Application*>(juce::JUCEApplication::getInstance());
	}


    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */


private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<MyAppIconTray> trayIcon;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(teste2Application)
};
