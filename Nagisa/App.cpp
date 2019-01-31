/*
 * PROJECT:   Nagisa
 * FILE:      App.cpp
 * PURPOSE:   Implementation for the App Entry.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"

#include "App.h"
#include "MainPage.h"

using namespace winrt;
using namespace Nagisa;
using namespace Nagisa::implementation;

/**
 * Initializes the singleton application object. This is the first line of 
 * authored code executed, and as such is the logical equivalent of main() 
 * or WinMain().
 */
App::App()
{
    InitializeComponent();
    Suspending({ this, &App::OnSuspending });

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    using Windows::UI::Xaml::UnhandledExceptionEventArgs;

    UnhandledException([this](
        IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

/**
 * Invoked when the application is launched normally by the end user. Other
 * entry points will be used such as when the application is launched to open
 * a specific file.
 *
 * @param e Details about the launch request and process.
 */
void App::OnLaunched(
    LaunchActivatedEventArgs const& e)
{
    using Windows::ApplicationModel::Activation::ApplicationExecutionState;
    using Windows::UI::Xaml::Controls::Frame;
    using Windows::UI::Xaml::Interop::TypeName;
    using Windows::UI::Xaml::Navigation::NavigationFailedEventHandler;
    using Windows::UI::Xaml::Window;

    Frame rootFrame{ nullptr };
    auto content = Window::Current().Content();
    if (content)
    {
        rootFrame = content.try_as<Frame>();
    }

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (rootFrame == nullptr)
    {
        // Create a Frame to act as the navigation context and associate it 
        // with a SuspensionManager key
        rootFrame = Frame();

        rootFrame.NavigationFailed({ this, &App::OnNavigationFailed });

        if (e.PreviousExecutionState() == ApplicationExecutionState::Terminated)
        {
            // TODO: Restore the saved session state only when appropriate, 
            // scheduling the final launch steps after the restore is complete.
        }

        if (e.PrelaunchActivated() == false)
        {
            if (rootFrame.Content() == nullptr)
            {
                // When the navigation stack isn't restored navigate to the 
                // first page, configuring the new page by passing required 
                // information as a navigation parameter.
                rootFrame.Navigate(
                    xaml_typename<Nagisa::MainPage>(),
                    box_value(e.Arguments()));
            }
            // Place the frame in the current Window
            Window::Current().Content(rootFrame);
            // Ensure the current window is active
            Window::Current().Activate();
        }
    }
    else
    {
        if (e.PrelaunchActivated() == false)
        {
            if (rootFrame.Content() == nullptr)
            {
                // When the navigation stack isn't restored navigate to the 
                // first page, configuring the new page by passing required 
                // information as a navigation parameter.
                rootFrame.Navigate(
                    xaml_typename<Nagisa::MainPage>(),
                    box_value(e.Arguments()));
            }
            // Ensure the current window is active
            Window::Current().Activate();
        }
    }
}

/**
 * Invoked when application execution is being suspended. Application state is
 * saved without knowing whether the application will be terminated or resumed
 * with the contents of memory still intact.
 *
 * @param sender The source of the suspend request.
 * @param e Details about the suspend request.
 */
void App::OnSuspending(
    IInspectable const& sender,
    SuspendingEventArgs const& e)
{
    UNREFERENCED_PARAMETER(sender);  // Unused parameter.
    UNREFERENCED_PARAMETER(e);   // Unused parameter.

    // Save application state and stop any background activity
}

// 
// Parameters:
//   
//   
// Return value:
//   The function does not return a value.
/**
 * Invoked when Navigation to a certain page fails
 *
 * @param sender The Frame which failed navigation.
 * @param e Details about the navigation failure.
 */
void App::OnNavigationFailed(
    IInspectable const& sender,
    NavigationFailedEventArgs const& e)
{
    UNREFERENCED_PARAMETER(sender);  // Unused parameter.

    throw hresult_error(
        E_FAIL,
        hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}
