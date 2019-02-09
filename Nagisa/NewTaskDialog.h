/*
 * PROJECT:   Nagisa
 * FILE:      NewTaskDialog.h
 * PURPOSE:   Definition for the New Task Dialog.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#include "NewTaskDialog.g.h"

namespace winrt::Nagisa::implementation
{
    using Assassin::TransferManager;
    using Windows::Foundation::Uri;
    using Windows::Storage::IStorageFolder;
    using Windows::UI::Colors;
    using Windows::UI::Xaml::Controls::ContentDialog;
    using Windows::UI::Xaml::Controls::ContentDialogButtonClickEventArgs;
    using Windows::UI::Xaml::Media::SolidColorBrush;
    using Windows::UI::Xaml::RoutedEventArgs;

    struct NewTaskDialog : NewTaskDialogT<NewTaskDialog>
    {
    private:
        TransferManager m_TransferManager = nullptr;

        SolidColorBrush m_NormalBrush = nullptr;
        SolidColorBrush m_NoticeableBrush = nullptr;

        Uri m_DownloadSource = nullptr;

        IStorageFolder m_SaveFolder = nullptr;

    public:
        NewTaskDialog(
            TransferManager const& TransferManager);

        IAsyncAction ContentDialog_Loaded(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        IAsyncAction DownloadButtonClick(
            ContentDialog const& sender,
            ContentDialogButtonClickEventArgs const& args);
        IAsyncAction BrowseButtonClick(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void DownloadSourceTextBox_LostFocus(
            IInspectable const& sender,
            RoutedEventArgs const& e);
    };
}

namespace winrt::Nagisa::factory_implementation
{
    struct NewTaskDialog :
        NewTaskDialogT<NewTaskDialog, implementation::NewTaskDialog>
    {
    };
}
