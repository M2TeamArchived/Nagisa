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

        this->RefreshTaskListAsync();
    }

    TransferManager MainPage::TransferManager()
    {
        return this->m_TransferManager;
    }

    void MainPage::RefreshTaskList()
    {
        auto Tasks = this->m_TransferManager.GetTasksAsync().get();

        if (nullptr != Tasks)
        {
            M2ExecuteOnUIThread([this, Tasks]()
            {
                using Windows::UI::Xaml::Visibility;

                this->TaskList().ItemsSource(Tasks);

                this->TaskListNoItemsTextBlock().Visibility(
                    (nullptr == Tasks || 0 == Tasks.Size())
                    ? Visibility::Visible
                    : Visibility::Collapsed);
            });
        }
    }

    void MainPage::RefreshTaskListAsync()
    {
        M2::CThread([this]()
        {
            this->RefreshTaskList();
        });
    }

    void MainPage::SearchTaskList(
        hstring const& SearchFilter)
    {
        this->m_TransferManager.SearchFilter(SearchFilter);

        this->RefreshTaskListAsync();
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

    void MainPage::AboutButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->ShowContentDialogAsync(
            winrt::make<AboutDialog>(this->m_TransferManager));
    }

    void MainPage::NewTaskButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Windows::UI::Xaml::Controls::ContentDialogResult;

        IAsyncOperation<ContentDialogResult> Operation =
            this->ShowContentDialogAsync(
                winrt::make<NewTaskDialog>(this->m_TransferManager));

        M2::CThread([this, Operation]()
        {
            if (ContentDialogResult::Primary == Operation.get())
            {
                this->RefreshTaskList();
            }
        });
    }

    void MainPage::CopyLinkMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        try
        {
            using Assassin::ITransferTask;
            using namespace Windows::UI::Xaml;

            ITransferTask Task = sender.try_as<FrameworkElement>(
                ).DataContext().try_as<ITransferTask>();

            using Windows::ApplicationModel::DataTransfer::Clipboard;
            using Windows::ApplicationModel::DataTransfer::DataPackage;

            DataPackage data = DataPackage();
            data.SetText(Task.SourceUri().RawUri());

            Clipboard::SetContent(data);
        }
        catch (...)
        {

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

    void MainPage::SearchAutoSuggestBox_QuerySubmitted(
        AutoSuggestBox const& sender,
        AutoSuggestBoxQuerySubmittedEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(args);   // Unused parameter.

        this->SearchTaskList(sender.Text());
    }

    void MainPage::SearchAutoSuggestBox_TextChanged(
        AutoSuggestBox const& sender,
        AutoSuggestBoxTextChangedEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(args);   // Unused parameter.

        using Windows::UI::Xaml::DispatcherTimer;
        using Windows::Foundation::EventHandler;
        using Windows::Foundation::TimeSpan;

        if (nullptr == sender.DataContext())
        {
            DispatcherTimer Timer = DispatcherTimer();
            AutoSuggestBox pSearchAutoSuggestBox = sender;

            // 10,000 ticks per millisecond.
            Timer.Interval(TimeSpan(250 * 10000));

            Timer.Tick([this, pSearchAutoSuggestBox, Timer](
                IInspectable const& sender, IInspectable const& args)
            {
                UNREFERENCED_PARAMETER(args);   // Unused parameter.

                this->SearchTaskList(pSearchAutoSuggestBox.Text());

                sender.try_as<DispatcherTimer>().Stop();
            });

            sender.DataContext(Timer);
        }

        DispatcherTimer Timer = sender.DataContext().try_as<DispatcherTimer>();

        Timer.Stop();
        Timer.Start();
    }

    void MainPage::RetryButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Assassin::ITransferTask;
        using namespace Windows::UI::Xaml;

        ITransferTask Task = sender.try_as<FrameworkElement>(
            ).DataContext().try_as<ITransferTask>();

        M2::CThread([this, Task]()
        {
            auto SourceUri = Task.SourceUri();
            auto FileName = Task.FileName();
            auto SaveFolder = Task.SaveFolder();

            this->m_TransferManager.RemoveTaskAsync(Task).get();

            this->m_TransferManager.AddTaskAsync(
                SourceUri,
                FileName,
                SaveFolder).get();

            this->RefreshTaskList();
        });
    }

    void MainPage::ResumeButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Assassin::ITransferTask;
        using namespace Windows::UI::Xaml;

        ITransferTask Task = sender.try_as<FrameworkElement>(
            ).DataContext().try_as<ITransferTask>();

        Task.Resume();

        this->RefreshTaskListAsync();
    }

    void MainPage::PauseButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Assassin::ITransferTask;
        using namespace Windows::UI::Xaml;

        ITransferTask Task = sender.try_as<FrameworkElement>(
            ).DataContext().try_as<ITransferTask>();

        Task.Pause();

        this->RefreshTaskListAsync();
    }

    void MainPage::CancelMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        try
        {
            using Assassin::ITransferTask;
            using namespace Windows::UI::Xaml;

            ITransferTask Task = sender.try_as<FrameworkElement>(
                ).DataContext().try_as<ITransferTask>();

            Task.Cancel();

            this->RefreshTaskListAsync();
        }
        catch (...)
        {

        }
    }

    void MainPage::RemoveMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Assassin::ITransferTask;
        using namespace Windows::UI::Xaml;

        ITransferTask Task = sender.try_as<FrameworkElement>(
            ).DataContext().try_as<ITransferTask>();

        M2::CThread([this, Task]()
        {
            this->m_TransferManager.RemoveTaskAsync(Task).get();

            this->RefreshTaskList();
        });
    }

    void MainPage::OpenFolderMenuItem_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Assassin::ITransferTask;
        using namespace Windows::UI::Xaml;

        ITransferTask Task = sender.try_as<FrameworkElement>(
            ).DataContext().try_as<ITransferTask>();

        try
        {
            using Windows::System::Launcher;
            using Windows::System::FolderLauncherOptions;

            FolderLauncherOptions Options = FolderLauncherOptions();
            Options.ItemsToSelect().Append(Task.SaveFile());

            Launcher::LaunchFolderAsync(Task.SaveFolder(), Options);
        }
        catch (...)
        {

        }
    }

    void MainPage::StartAllAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->m_TransferManager.StartAllTasks();

        this->RefreshTaskListAsync();
    }

    void MainPage::PauseAllAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->m_TransferManager.PauseAllTasks();

        this->RefreshTaskListAsync();
    }

    void MainPage::ClearListAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->m_TransferManager.ClearTaskList();

        this->RefreshTaskListAsync();
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

    void MainPage::OpenDownloadsFolderAppBarButton_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        try
        {
            using Windows::Storage::IStorageFolder;
            using Windows::System::Launcher;

            IStorageFolder Folder = this->m_TransferManager.DefaultFolder();
            if (nullptr == Folder)
            {
                Folder = this->m_TransferManager.LastusedFolder();
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
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->ShowContentDialogAsync(
            winrt::make<SettingsDialog>(this->m_TransferManager));
    }
}
