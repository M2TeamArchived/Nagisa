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
    using Windows::UI::ViewManagement::UISettings;
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
        UISettings m_UISettings = nullptr;

        IAsyncOperation<ContentDialogResult> ShowContentDialogAsync(
            ContentDialog const& Dialog);

        ITransferTask GetTransferTaskFromEventSender(
            IInspectable const& sender);

        void UpdateTitleBarColor();

        void ColorValuesChanged(
            UISettings const& sender,
            IInspectable const& e);

    public:
        MainPage();

        TransferManager TransferManager();

        void Page_Loaded(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void TaskList_ContainerContentChanging(
            ListViewBase const& sender,
            ContainerContentChangingEventArgs const& e);
        fire_and_forget AboutButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        fire_and_forget NewTaskButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void CopyLinkMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void SearchAutoSuggestBox_LostFocus(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        fire_and_forget RetryButton_Click(
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
        fire_and_forget RemoveMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        fire_and_forget OpenFolderMenuItem_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void StartAllAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void PauseAllAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        fire_and_forget ClearListAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        void SearchAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        fire_and_forget OpenDownloadsFolderAppBarButton_Click(
            IInspectable const& sender,
            RoutedEventArgs const& e);
        fire_and_forget SettingsAppBarButton_Click(
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
