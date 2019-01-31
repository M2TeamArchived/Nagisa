/*
 * PROJECT:   Nagisa
 * FILE:      App.h
 * PURPOSE:   Definition for the App Entry.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once
#include "App.xaml.g.h"

namespace winrt::Nagisa::implementation
{
    using Windows::ApplicationModel::Activation::LaunchActivatedEventArgs;
    using Windows::ApplicationModel::SuspendingEventArgs;
    using Windows::UI::Xaml::Navigation::NavigationFailedEventArgs;

    struct App : AppT<App>
    {
        App();

        void OnLaunched(
            LaunchActivatedEventArgs const& e);
        void OnSuspending(
            IInspectable const& sender,
            SuspendingEventArgs const& e);
        void OnNavigationFailed(
            IInspectable const& sender,
            NavigationFailedEventArgs const& e);
    };
}
