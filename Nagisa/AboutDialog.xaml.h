/******************************************************************************
Project: Nagisa
Description: Definition for the About Dialog.
File Name: AboutDialog.xaml.h
License: The MIT License
******************************************************************************/

#pragma once

#include "AboutDialog.g.h"

namespace Nagisa
{
    using Assassin::ITransferManager;
    using Platform::Object;
    using Windows::UI::Xaml::Controls::ContentDialog;
    using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
    using Windows::UI::Xaml::RoutedEventArgs;

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class AboutDialog sealed
    {
    public:
        AboutDialog(
            ITransferManager^ TransferManager);
    private:
        ITransferManager ^ m_TransferManager = nullptr;

        void ContentDialog_Loaded(
            Object^ sender,
            RoutedEventArgs^ e);
        void GitHubButtonClick(
            ContentDialog^ sender,
            ContentDialogButtonClickEventArgs^ args);
    };
}
