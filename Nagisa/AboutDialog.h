/*
 * PROJECT:   Nagisa
 * FILE:      AboutDialog.h
 * PURPOSE:   Definition for the About Dialog.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */
#pragma once

#include "AboutDialog.g.h"

#include "winrt/Assassin.h"

namespace winrt::Nagisa::implementation
{
    using Assassin::ITransferManager;
    using Windows::UI::Xaml::Controls::ContentDialog;
    using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
    using Windows::UI::Xaml::RoutedEventArgs;

    struct AboutDialog : AboutDialogT<AboutDialog>
    {
    private:
        Assassin::ITransferManager m_TransferManager = nullptr;

    public:
        AboutDialog(
            Assassin::ITransferManager const& TransferManager);

        void ContentDialog_Loaded(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void GitHubButtonClick(
            ContentDialog const& sender,
            ContentDialogButtonClickEventArgs const& args);
    };
}

namespace winrt::Nagisa::factory_implementation
{
    struct AboutDialog :
        AboutDialogT<AboutDialog, implementation::AboutDialog>
    {
    };
}
