/*
 * PROJECT:   Nagisa
 * FILE:      MainPage.h
 * PURPOSE:   Definition for the Main Window.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#include "MainPage.g.h"

namespace winrt::Nagisa::implementation
{
    using Assassin::TransferManager;
    using Assassin::ITransferTask;
    using Windows::Foundation::IAsyncOperation;
    using Windows::UI::Xaml::RoutedEventArgs;
    using Windows::UI::Xaml::Controls::AutoSuggestBox;
    using Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs;
    using Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs;
    using Windows::UI::Xaml::Controls::ContainerContentChangingEventArgs;
    using Windows::UI::Xaml::Controls::ContentDialog;
    using Windows::UI::Xaml::Controls::ContentDialogResult;
    using Windows::UI::Xaml::Controls::ListView;
    using Windows::UI::Xaml::Controls::ListViewBase;
    using Windows::UI::Xaml::Data::INotifyPropertyChanged;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;

    struct MainPage : MainPageT<MainPage>
    {
    private:
        TransferManager m_TransferManager = nullptr;

        IAsyncOperation<ContentDialogResult> ShowContentDialogAsync(
            ContentDialog const& Dialog);

        ITransferTask GetTransferTaskFromEventSender(
            IInspectable const& sender);

    public:
        MainPage();

        TransferManager TransferManager();

        void Page_Loaded(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void TaskList_ContainerContentChanging(
            ListViewBase const& sender,
            ContainerContentChangingEventArgs const& e);
        IAsyncAction AboutButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        IAsyncAction NewTaskButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void CopyLinkMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void SearchAutoSuggestBox_LostFocus(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        IAsyncAction RetryButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void ResumeButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void PauseButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void CancelMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        IAsyncAction RemoveMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        IAsyncAction OpenFolderMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void StartAllAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void PauseAllAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        IAsyncAction ClearListAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void SearchAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        IAsyncAction OpenDownloadsFolderAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        IAsyncAction SettingsAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
    };
}

namespace winrt::Nagisa::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
