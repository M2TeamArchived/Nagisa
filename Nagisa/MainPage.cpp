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
        m_TransferManager(Assassin::TransferManager(true)),
        m_UISettings(UISettings()),
        m_CurrentApplication(Application::Current()),
        m_ApplicationViewTitleBar(
            ApplicationView::GetForCurrentView().TitleBar())
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
        return sender.try_as<Windows::UI::Xaml::FrameworkElement>(
            ).DataContext().try_as<ITransferTask>();
    }

    void MainPage::UpdateTitleBarColor()
    {
        using Windows::UI::Color;
        using Windows::UI::Colors;
        
        Color ButtonForegroundColor =
            (this->m_CurrentApplication.RequestedTheme()
                != Windows::UI::Xaml::ApplicationTheme::Dark)
            ? Colors::Black()
            : Colors::White();

        this->m_ApplicationViewTitleBar.ButtonForegroundColor(
            ButtonForegroundColor);
        this->m_ApplicationViewTitleBar.ButtonHoverForegroundColor(
            ButtonForegroundColor); 
        this->m_ApplicationViewTitleBar.ButtonPressedForegroundColor(
            ButtonForegroundColor);
    }

    void MainPage::InitializeCustomTitleBar()
    {
        using Windows::ApplicationModel::Core::CoreApplication;
        using Windows::UI::Color;
        using Windows::UI::Colors;
        using Windows::UI::Xaml::Window;

        // Extend main grid to titlebar
        CoreApplication::GetCurrentView().TitleBar().ExtendViewIntoTitleBar(
            true);

        // Color settings.
        Color DarkGray = Colors::DarkGray();
        Color Transparent = Colors::Transparent();
        this->m_ApplicationViewTitleBar.ButtonBackgroundColor(
            Transparent);
        this->m_ApplicationViewTitleBar.ButtonHoverBackgroundColor(
            DarkGray);
        this->m_ApplicationViewTitleBar.ButtonInactiveBackgroundColor(
            Transparent);
        this->m_ApplicationViewTitleBar.ButtonInactiveForegroundColor(
            DarkGray);
        this->m_ApplicationViewTitleBar.ButtonPressedBackgroundColor(
            DarkGray);
        this->UpdateTitleBarColor();
        this->m_UISettings.ColorValuesChanged(
            { this, &MainPage::ColorValuesChanged });

        // Set real titlebar area
        Window::Current().SetTitleBar(this->realTitle());
    }

    void MainPage::ColorValuesChanged(
        UISettings const& sender,
        IInspectable const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using winrt::Windows::UI::Core::CoreDispatcherPriority;

        this->Dispatcher().RunAsync(
            CoreDispatcherPriority::Normal,
            { this, &MainPage::UpdateTitleBarColor });
    }

    void MainPage::Page_Loaded(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->InitializeCustomTitleBar();

        using Windows::UI::Xaml::Visibility;
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

    fire_and_forget MainPage::AboutButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        co_await this->ShowContentDialogAsync(
            make<AboutDialog>(this->m_TransferManager));
    }

    fire_and_forget MainPage::NewTaskButton_Click(
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

    fire_and_forget MainPage::RetryButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        ITransferTask Task = this->GetTransferTaskFromEventSender(sender);

        auto SourceUri = Task.SourceUri();
        auto FileName = Task.FileName();
        auto SaveFolder = co_await Task.SaveFolder();

        this->m_TransferManager.RemoveTask(Task);

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

        this->GetTransferTaskFromEventSender(sender).Cancel();
    }

    void MainPage::RemoveMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->m_TransferManager.RemoveTask(
            this->GetTransferTaskFromEventSender(sender));
    }

    fire_and_forget MainPage::OpenFolderMenuItem_Click(
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

    void MainPage::ClearListAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->m_TransferManager.ClearTaskList();
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

    fire_and_forget MainPage::OpenDownloadsFolderAppBarButton_Click(
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

    fire_and_forget MainPage::SettingsAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        co_await this->ShowContentDialogAsync(
            winrt::make<SettingsDialog>(this->m_TransferManager));
    }
}
