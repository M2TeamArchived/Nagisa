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
    using Assassin::ITransferManager;
    using Windows::Foundation::IAsyncOperation;
    using Windows::UI::Xaml::RoutedEventArgs;
    using Windows::UI::Xaml::Controls::AutoSuggestBox;
    using Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs;
    using Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs;
    using Windows::UI::Xaml::Controls::ContentDialog;
    using Windows::UI::Xaml::Controls::ContentDialogResult;
    using Windows::UI::Xaml::Controls::ListView;
    using Windows::UI::Xaml::Data::INotifyPropertyChanged;
    using Windows::UI::Xaml::Data::PropertyChangedEventHandler;

    struct MainPage : MainPageT<MainPage>
    {
    private:
        ITransferManager m_TransferManager = nullptr;

        void RefreshTaskList();
        void RefreshTaskListAsync();
        void SearchTaskList(
            hstring const& SearchFilter);
        IAsyncOperation<ContentDialogResult> ShowContentDialogAsync(
            ContentDialog const& Dialog);

    public:
        MainPage();

        ITransferManager TransferManager();

        void AboutButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void NewTaskButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void CopyLinkMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void SearchAutoSuggestBox_LostFocus(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void SearchAutoSuggestBox_QuerySubmitted(
            AutoSuggestBox const& sender,
            AutoSuggestBoxQuerySubmittedEventArgs const& args);
        void SearchAutoSuggestBox_TextChanged(
            AutoSuggestBox const& sender,
            AutoSuggestBoxTextChangedEventArgs const& args);
        void RetryButton_Click(
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
        void RemoveMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void OpenFolderMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void StartAllAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void PauseAllAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void ClearListAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void SearchAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void OpenDownloadsFolderAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void SettingsAppBarButton_Click(
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
