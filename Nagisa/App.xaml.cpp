/******************************************************************************
Project: Nagisa
Description: Implementation for the App Entry.
File Name: App.xaml.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "MainPage.xaml.h"

using namespace Nagisa;

// Initializes the singleton application object. This is the first line of 
// authored code executed, and as such is the logical equivalent of main() 
// or WinMain().
// Parameters:
//   The function does not have parameters.
// Return value:
//   The function does not return a value.
App::App()
{
	using Windows::UI::Xaml::SuspendingEventHandler;

    InitializeComponent();
    Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

// Invoked when the application is launched normally by the end user. Other 
// entry points will be used such as when the application is launched to open
// a specific file.
// Parameters:
//   e: Details about the launch request and process.
// Return value:
//   The function does not return a value.
void App::OnLaunched(
	LaunchActivatedEventArgs^ e)
{
	using Windows::ApplicationModel::Activation::ApplicationExecutionState;
	using Windows::UI::Xaml::Controls::Frame;
	using Windows::UI::Xaml::Interop::TypeName;
	using Windows::UI::Xaml::Navigation::NavigationFailedEventHandler;
	using Windows::UI::Xaml::Window;
	
	auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

	// Do not repeat app initialization when the Window already has content, 
	// just ensure that the window is active
    if (rootFrame == nullptr)
    {
		// Create a Frame to act as the navigation context and associate it 
		// with a SuspensionManager key
        rootFrame = ref new Frame();

        rootFrame->NavigationFailed += ref new NavigationFailedEventHandler(
			this, &App::OnNavigationFailed);

        if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
        {
			// TODO: Restore the saved session state only when appropriate, 
			// scheduling the final launch steps after the restore is complete.
        }

        if (e->PrelaunchActivated == false)
        {
            if (rootFrame->Content == nullptr)
            {
				// When the navigation stack isn't restored navigate to the 
				// first page, configuring the new page by passing required 
				// information as a navigation parameter.
                rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
            }
			// Place the frame in the current Window.
			Window::Current->Content = rootFrame;
			// Ensure the current window is active.
			Window::Current->Activate();
        }
    }
    else
    {
        if (e->PrelaunchActivated == false)
        {
            if (rootFrame->Content == nullptr)
            {
				// When the navigation stack isn't restored navigate to the 
				// first page, configuring the new page by passing required 
				// information as a navigation parameter.
                rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
            }
			// Ensure the current window is active.
            Window::Current->Activate();
        }
    }
}

// Invoked when application execution is being suspended. Application state is 
// saved without knowing whether the application will be terminated or resumed
// with the contents of memory still intact.
// Parameters:
//   sender: The source of the suspend request.
//   e: Details about the suspend request.
// Return value:
//   The function does not return a value.
void App::OnSuspending(
	Object^ sender, 
	SuspendingEventArgs^ e)
{
	UNREFERENCED_PARAMETER(sender);  // Unused parameter.
	UNREFERENCED_PARAMETER(e);   // Unused parameter.

	//TODO: Save application state and stop any background activity.
}

// Invoked when Navigation to a certain page fails
// Parameters:
//   sender: The Frame which failed navigation.
//   e: Details about the navigation failure.
// Return value:
//   The function does not return a value.
void App::OnNavigationFailed(
	Object^ sender, 
	NavigationFailedEventArgs^ e)
{
	using Platform::FailureException;

	throw ref new FailureException(
		"Failed to load Page " + e->SourcePageType.Name);
}