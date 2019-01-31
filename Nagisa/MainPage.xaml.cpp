﻿/******************************************************************************
Project: Nagisa
Description: Implementation for the Main Window.
File Name: MainPage.xaml.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "MainPage.xaml.h"
#include "AboutDialog.xaml.h"
#include "NewTaskDialog.xaml.h"
#include "SettingsDialog.xaml.h"

using namespace Nagisa;

MainPage::MainPage() :
    m_TransferManager(ref new Assassin::TransferManager(true))
{
    InitializeComponent();
}

ITransferManager^ MainPage::TransferManager::get()
{
    return this->m_TransferManager;
}

void MainPage::RefreshTaskList()
{
    auto Tasks = M2AsyncWait(this->m_TransferManager->GetTasksAsync());

    if (nullptr != Tasks)
    {
        M2ExecuteOnUIThread([this, Tasks]()
        {
            this->TaskList->ItemsSource = Tasks;
        });
    }
}

void Nagisa::MainPage::RefreshTaskListAsync()
{
    M2::CThread([this]()
    {
        this->RefreshTaskList();
    });
}

void Nagisa::MainPage::SearchTaskList(String^ SearchFilter)
{
    this->m_TransferManager->SearchFilter = SearchFilter;

    this->RefreshTaskListAsync();
}

IAsyncOperation<ContentDialogResult>^ MainPage::ShowContentDialogAsync(
    ContentDialog^ Dialog)
{
    double PageActualWidth = this->ActualWidth;
    if (Dialog->MaxWidth > PageActualWidth)
        Dialog->MaxWidth = PageActualWidth;

    double PageActualHeight = this->ActualHeight;
    if (Dialog->MaxHeight > PageActualHeight)
        Dialog->MaxHeight = PageActualHeight;

    return Dialog->ShowAsync();
}

void MainPage::AboutButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    this->ShowContentDialogAsync(
        ref new AboutDialog(this->m_TransferManager));
}

void MainPage::NewTaskButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Windows::UI::Xaml::Controls::ContentDialogResult;

    IAsyncOperation<ContentDialogResult>^ Operation =
        this->ShowContentDialogAsync(
            ref new NewTaskDialog(this->m_TransferManager));

    M2::CThread([this, Operation]()
    {
        if (ContentDialogResult::Primary == M2AsyncWait(Operation))
        {
            this->RefreshTaskList();
        }
    });
}

void MainPage::Page_Loaded(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Windows::ApplicationModel::Core::CoreApplication;
    using Windows::UI::Colors;
    using Windows::UI::ViewManagement::ApplicationView;
    using Windows::UI::ViewManagement::ApplicationViewTitleBar;
    using Windows::UI::Xaml::Application;
    using Windows::UI::Xaml::Media::SolidColorBrush;
    using Windows::UI::Xaml::ResourceDictionary;
    using Windows::UI::Xaml::Visibility;
    using Windows::UI::Xaml::Window;

    this->RefreshTaskListAsync();
    //Extend main grid to titlebar
    CoreApplication::GetCurrentView()->TitleBar->ExtendViewIntoTitleBar = true;
    ApplicationViewTitleBar^ titleBar =
        ApplicationView::GetForCurrentView()->TitleBar;
    ResourceDictionary^ GlobalResources =
        Application::Current->Resources;
    //Change titlebar button colors to match DimButton style
    titleBar->ButtonBackgroundColor = Colors::Transparent;
    titleBar->ButtonInactiveBackgroundColor = Colors::Transparent;
    titleBar->ButtonPressedBackgroundColor =
        dynamic_cast<SolidColorBrush^>(GlobalResources->Lookup(
            "SystemControlHighlightListMediumBrush"))->Color;
    titleBar->ButtonHoverBackgroundColor =
        dynamic_cast<SolidColorBrush^>(GlobalResources->Lookup(
            "SystemControlHighlightListLowBrush"))->Color;
    //Set real titlebar area
    Window::Current->SetTitleBar(realTitle);

    this->SearchAutoSuggestBox->Visibility = Visibility::Collapsed;
}

void MainPage::CopyLinkMenuItem_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    try
    {
        using Assassin::ITransferTask;
        using namespace Windows::UI::Xaml;

        ITransferTask^ Task = dynamic_cast<ITransferTask^>(
            dynamic_cast<FrameworkElement^>(sender)->DataContext);

        using Windows::ApplicationModel::DataTransfer::Clipboard;
        using Windows::ApplicationModel::DataTransfer::DataPackage;

        DataPackage^ data = ref new DataPackage();
        data->SetText(Task->SourceUri->RawUri);

        Clipboard::SetContent(data);
    }
    catch (...)
    {

    }
}

void MainPage::SearchAutoSuggestBox_LostFocus(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Windows::UI::Xaml::Visibility;

    this->SearchAutoSuggestBox->Visibility = Visibility::Collapsed;
    this->SearchAppBarButton->Visibility = Visibility::Visible;
}

void MainPage::SearchAutoSuggestBox_QuerySubmitted(
    AutoSuggestBox^ sender,
    AutoSuggestBoxQuerySubmittedEventArgs^ args)
{
    this->SearchTaskList(sender->Text);
}

void MainPage::SearchAutoSuggestBox_TextChanged(
    AutoSuggestBox^ sender,
    AutoSuggestBoxTextChangedEventArgs^ args)
{
    using Windows::UI::Xaml::DispatcherTimer;
    using Windows::Foundation::EventHandler;
    using Windows::Foundation::TimeSpan;

    if (nullptr == sender->DataContext)
    {
        DispatcherTimer^ Timer = ref new DispatcherTimer();
        AutoSuggestBox^ pSearchAutoSuggestBox = sender;

        TimeSpan Interval;
        Interval.Duration = 250 * 10000; // 10,000 ticks per millisecond.

        Timer->Interval = Interval;

        Timer->Tick += ref new EventHandler<Object^>(
            [this, pSearchAutoSuggestBox, Timer](Object^ sender, Object^ args)
        {
            this->SearchTaskList(pSearchAutoSuggestBox->Text);

            dynamic_cast<DispatcherTimer^>(sender)->Stop();
        });

        sender->DataContext = Timer;
    }

    DispatcherTimer^ Timer = dynamic_cast<DispatcherTimer^>(
        sender->DataContext);

    Timer->Stop();
    Timer->Start();
}

void MainPage::RetryButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Assassin::ITransferTask;
    using namespace Windows::UI::Xaml;

    ITransferTask^ Task = dynamic_cast<ITransferTask^>(
        dynamic_cast<FrameworkElement^>(sender)->DataContext);

    M2::CThread([this, Task]()
    {
        auto SourceUri = Task->SourceUri;
        auto FileName = Task->FileName;
        auto SaveFolder = Task->SaveFolder;

        M2AsyncWait(this->m_TransferManager->RemoveTaskAsync(Task));

        M2AsyncWait(this->m_TransferManager->AddTaskAsync(
            SourceUri,
            FileName,
            SaveFolder));

        this->RefreshTaskList();
    });
}

void MainPage::ResumeButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Assassin::ITransferTask;
    using namespace Windows::UI::Xaml;

    ITransferTask^ Task = dynamic_cast<ITransferTask^>(
        dynamic_cast<FrameworkElement^>(sender)->DataContext);

    Task->Resume();

    this->RefreshTaskListAsync();
}

void MainPage::PauseButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Assassin::ITransferTask;
    using namespace Windows::UI::Xaml;

    ITransferTask^ Task = dynamic_cast<ITransferTask^>(
        dynamic_cast<FrameworkElement^>(sender)->DataContext);

    Task->Pause();

    this->RefreshTaskListAsync();
}

void MainPage::CancelMenuItem_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    try
    {
        using Assassin::ITransferTask;
        using namespace Windows::UI::Xaml;

        ITransferTask^ Task = dynamic_cast<ITransferTask^>(
            dynamic_cast<FrameworkElement^>(sender)->DataContext);

        Task->Cancel();

        this->RefreshTaskListAsync();
    }
    catch (...)
    {

    }
}

void MainPage::RemoveMenuItem_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Assassin::ITransferTask;
    using namespace Windows::UI::Xaml;

    ITransferTask^ Task = dynamic_cast<ITransferTask^>(
        dynamic_cast<FrameworkElement^>(sender)->DataContext);

    M2::CThread([this, Task]()
    {
        M2AsyncWait(this->m_TransferManager->RemoveTaskAsync(Task));

        this->RefreshTaskList();
    });
}

void MainPage::OpenFolderMenuItem_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Assassin::ITransferTask;
    using namespace Windows::UI::Xaml;

    ITransferTask^ Task = dynamic_cast<ITransferTask^>(
        dynamic_cast<FrameworkElement^>(sender)->DataContext);

    try
    {
        using Windows::System::Launcher;
        using Windows::System::FolderLauncherOptions;

        FolderLauncherOptions^ Options = ref new FolderLauncherOptions();
        Options->ItemsToSelect->Append(Task->SaveFile);

        Launcher::LaunchFolderAsync(Task->SaveFolder, Options);
    }
    catch (...)
    {

    }
}

void MainPage::StartAllAppBarButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    this->m_TransferManager->StartAllTasks();

    this->RefreshTaskListAsync();
}

void MainPage::PauseAllAppBarButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    this->m_TransferManager->PauseAllTasks();

    this->RefreshTaskListAsync();
}

void MainPage::ClearListAppBarButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    this->m_TransferManager->ClearTaskList();

    this->RefreshTaskListAsync();
}

void MainPage::SearchAppBarButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Windows::UI::Xaml::FocusState;
    using Windows::UI::Xaml::Visibility;

    this->SearchAppBarButton->Visibility = Visibility::Collapsed;
    this->SearchAutoSuggestBox->Visibility = Visibility::Visible;
    this->SearchAutoSuggestBox->Focus(FocusState::Programmatic);
}

void MainPage::OpenDownloadsFolderAppBarButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    try
    {
        using Windows::Storage::IStorageFolder;
        using Windows::System::Launcher;

        IStorageFolder^ Folder = this->m_TransferManager->DefaultFolder;
        if (nullptr == Folder)
        {
            Folder = this->m_TransferManager->LastusedFolder;
        }

        if (nullptr != Folder)
        {
            Launcher::LaunchFolderAsync(Folder);
        }
    }
    catch (...)
    {

    }
}

void MainPage::SettingsAppBarButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    this->ShowContentDialogAsync(
        ref new SettingsDialog(this->m_TransferManager));
}
