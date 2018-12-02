/******************************************************************************
Project: Nagisa
Description: Definition for the App Entry.
File Name: App.xaml.h
License: The MIT License
******************************************************************************/

#pragma once

#include "App.g.h"

namespace Nagisa
{
    using Platform::Object;
    using Windows::ApplicationModel::Activation::LaunchActivatedEventArgs;
    using Windows::ApplicationModel::SuspendingEventArgs;
    using Windows::UI::Xaml::Navigation::NavigationFailedEventArgs;

    ref class App sealed
    {
    protected:
        virtual void OnLaunched(
            LaunchActivatedEventArgs^ e) override;

    internal:
        App();

    private:
        void OnSuspending(
            Object^ sender,
            SuspendingEventArgs^ e);
        void OnNavigationFailed(
            Object^ sender,
            NavigationFailedEventArgs^ e);
    };
}
