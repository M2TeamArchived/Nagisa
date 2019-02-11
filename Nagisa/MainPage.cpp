/*
 * PROJECT:   Nagisa
 * FILE:      MainPage.cpp
 * PURPOSE:   Implementation for the Main Window.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)          
 */

#include "pch.h"
#include "MainPage.h"

#include "MainPage.h"
#include "AboutDialog.h"
#include "NewTaskDialog.h"
#include "SettingsDialog.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Nagisa::implementation
{
    MainPage::MainPage() :
        m_TransferManager(Assassin::TransferManager(true))
    {
        InitializeComponent();
    }

    TransferManager MainPage::TransferManager()
    {
        return this->m_TransferManager;
    }

    IAsyncOperation<ContentDialogResult> MainPage::ShowContentDialogAsync(
        ContentDialog const& Dialog)
    {
        double PageActualWidth = this->ActualWidth();
        if (Dialog.MaxWidth() > PageActualWidth)
            Dialog.MaxWidth(PageActualWidth);

        double PageActualHeight = this->ActualHeight();
        if (Dialog.MaxHeight() > PageActualHeight)
            Dialog.MaxHeight(PageActualHeight);

        return Dialog.ShowAsync();
    }

    ITransferTask MainPage::GetTransferTaskFromEventSender(
        IInspectable const& sender)
    {
        using Windows::UI::Xaml::FrameworkElement;

        return sender.try_as<FrameworkElement>(
            ).DataContext().try_as<ITransferTask>();
    }

    void MainPage::Page_Loaded(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Windows::ApplicationModel::Core::CoreApplication;
        using Windows::UI::Colors;
        using Windows::UI::ViewManagement::ApplicationView;
        using Windows::UI::ViewManagement::ApplicationViewTitleBar;
        using Windows::UI::Xaml::Application;
        using Windows::UI::Xaml::Media::SolidColorBrush;
        using Windows::UI::Xaml::ResourceDictionary;
        using Windows::UI::Xaml::Visibility;
        using Windows::UI::Xaml::Window;

        //Extend main grid to titlebar
        CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(
            true);
        ApplicationViewTitleBar titleBar =
            ApplicationView::GetForCurrentView().TitleBar();
        ResourceDictionary GlobalResources =
            Application::Current().Resources();
        //Change titlebar button colors to match DimButton style
        titleBar.ButtonBackgroundColor(Colors::Transparent());
        titleBar.ButtonInactiveBackgroundColor(Colors::Transparent());
        titleBar.ButtonPressedBackgroundColor(GlobalResources.Lookup(box_value(
            L"SystemControlHighlightListMediumBrush")).try_as<SolidColorBrush>(
                ).Color());
        titleBar.ButtonHoverBackgroundColor(GlobalResources.Lookup(box_value(
            L"SystemControlHighlightListLowBrush")).try_as<SolidColorBrush>(
                ).Color());
        //Set real titlebar area
        Window::Current().SetTitleBar(this->realTitle());

        this->SearchAutoSuggestBox().Visibility(Visibility::Collapsed);
    }

    void MainPage::TaskList_ContainerContentChanging(
        ListViewBase const& sender,
        ContainerContentChangingEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->TaskListNoItemsTextBlock().Visibility(
            !sender.ItemsSource()
            ? Visibility::Visible
            : Visibility::Collapsed);
    }

    IAsyncAction MainPage::AboutButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        co_await this->ShowContentDialogAsync(
            make<AboutDialog>(this->m_TransferManager));
    }

    IAsyncAction MainPage::NewTaskButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        co_await this->ShowContentDialogAsync(
            make<NewTaskDialog>(this->m_TransferManager));
    }

    void MainPage::CopyLinkMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Windows::ApplicationModel::DataTransfer::Clipboard;
        using Windows::ApplicationModel::DataTransfer::DataPackage;
        using Windows::Foundation::Uri;

        ITransferTask Task = this->GetTransferTaskFromEventSender(sender);

        Uri UriObject = Task.SourceUri();
        if (UriObject)
        {
            DataPackage data = DataPackage();
            data.SetText(UriObject.RawUri());

            Clipboard::SetContent(data);
        }
    }

    void MainPage::SearchAutoSuggestBox_LostFocus(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Windows::UI::Xaml::Visibility;

        this->SearchAutoSuggestBox().Visibility(Visibility::Collapsed);
        this->SearchAppBarButton().Visibility(Visibility::Visible);
    }

    IAsyncAction MainPage::RetryButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        ITransferTask Task = this->GetTransferTaskFromEventSender(sender);

        auto SourceUri = Task.SourceUri();
        auto FileName = Task.FileName();
        auto SaveFolder = co_await Task.SaveFolder();

        co_await this->m_TransferManager.RemoveTaskAsync(Task);

        co_await this->m_TransferManager.AddTaskAsync(
            SourceUri,
            FileName,
            SaveFolder);
    }

    void MainPage::ResumeButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->GetTransferTaskFromEventSender(sender).Resume();
    }

    void MainPage::PauseButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->GetTransferTaskFromEventSender(sender).Pause();
    }

    void MainPage::CancelMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        try
        {
            this->GetTransferTaskFromEventSender(sender).Cancel();
        }
        catch (...)
        {

        }
    }

    IAsyncAction MainPage::RemoveMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        co_await this->m_TransferManager.RemoveTaskAsync(
            this->GetTransferTaskFromEventSender(sender));
    }

    IAsyncAction MainPage::OpenFolderMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Windows::Storage::IStorageFile;
        using Windows::Storage::IStorageFolder;
        using Windows::System::Launcher;
        using Windows::System::FolderLauncherOptions;

        ITransferTask Task = this->GetTransferTaskFromEventSender(sender);

        IStorageFile FileObject = co_await Task.SaveFile();
        IStorageFolder FolderObject = co_await Task.SaveFolder();

        if (FileObject && FolderObject)
        {
            FolderLauncherOptions Options = FolderLauncherOptions();
            Options.ItemsToSelect().Append(FileObject);

            co_await Launcher::LaunchFolderAsync(FolderObject, Options);
        }
    }

    void MainPage::StartAllAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->m_TransferManager.ResumeAllTasks();
    }

    void MainPage::PauseAllAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->m_TransferManager.PauseAllTasks();
    }

    IAsyncAction MainPage::ClearListAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        co_await this->m_TransferManager.ClearTaskListAsync();
    }

    void MainPage::SearchAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Windows::UI::Xaml::FocusState;
        using Windows::UI::Xaml::Visibility;

        this->SearchAppBarButton().Visibility(Visibility::Collapsed);
        this->SearchAutoSuggestBox().Visibility(Visibility::Visible);
        this->SearchAutoSuggestBox().Focus(FocusState::Programmatic);
    }

    IAsyncAction MainPage::OpenDownloadsFolderAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Windows::Storage::IStorageFolder;
        using Windows::System::Launcher;

        IStorageFolder Folder =
            co_await this->m_TransferManager.DefaultFolder();
        if (!Folder) Folder =
            co_await this->m_TransferManager.LastusedFolder();

        if (Folder)
            co_await Launcher::LaunchFolderAsync(Folder);
    }

    IAsyncAction MainPage::SettingsAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        co_await this->ShowContentDialogAsync(
            winrt::make<SettingsDialog>(this->m_TransferManager));
    }
}
