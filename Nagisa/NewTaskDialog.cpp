/*
 * PROJECT:   Nagisa
 * FILE:      NewTaskDialog.cpp
 * PURPOSE:   Implementation for the New Task Dialog.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"
#include "NewTaskDialog.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Nagisa::implementation
{
    NewTaskDialog::NewTaskDialog(
        TransferManager const& TransferManager) :
        m_TransferManager(TransferManager),
        m_NormalBrush(SolidColorBrush(Colors::Gray())),
        m_NoticeableBrush(SolidColorBrush(Colors::Red()))
    {
        InitializeComponent();  
    }

    IAsyncAction NewTaskDialog::ContentDialog_Loaded(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        this->m_SaveFolder =
            co_await this->m_TransferManager.DefaultFolder();
        if (!this->m_SaveFolder) this->m_SaveFolder =
            co_await this->m_TransferManager.LastusedFolder();

        if (this->m_SaveFolder)
        {
            this->SaveFolderTextBox().Text(this->m_SaveFolder.Path());
        }
    }

    IAsyncAction NewTaskDialog::DownloadButtonClick(
        ContentDialog const& sender,
        ContentDialogButtonClickEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.

        hstring FileName = this->FileNameTextBox().Text();

        bool IsDownloadSourceEmpty = (nullptr == this->m_DownloadSource);
        bool IsFileNameEmpty = FileName.empty();
        bool IsSaveFolderEmpty = (nullptr == this->m_SaveFolder);

        this->DownloadSourceTextBox().BorderBrush(IsDownloadSourceEmpty
            ? this->m_NoticeableBrush : this->m_NormalBrush);
        this->FileNameTextBox().BorderBrush(IsFileNameEmpty
            ? this->m_NoticeableBrush : this->m_NormalBrush);
        this->SaveFolderTextBox().BorderBrush(IsSaveFolderEmpty
            ? this->m_NoticeableBrush : this->m_NormalBrush);

        if (IsDownloadSourceEmpty || IsFileNameEmpty || IsSaveFolderEmpty)
        {
            args.Cancel(true);
        }
        else
        {
            co_await this->m_TransferManager.AddTaskAsync(
                this->m_DownloadSource,
                FileName,
                this->m_SaveFolder);
        }
    }

    IAsyncAction NewTaskDialog::BrowseButtonClick(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        using Windows::Foundation::IAsyncOperation;
        using Windows::Storage::StorageFolder;
        using Windows::Storage::Pickers::FolderPicker;
        using Windows::Storage::Pickers::PickerLocationId;

        FolderPicker picker = FolderPicker();
        picker.SuggestedStartLocation(PickerLocationId::ComputerFolder);
        picker.FileTypeFilter().Append(L"*");

        StorageFolder Folder = co_await picker.PickSingleFolderAsync();

        if (Folder)
        {
            this->m_SaveFolder = Folder;
            this->SaveFolderTextBox().Text(Folder.Path());
        }
    }

    void NewTaskDialog::DownloadSourceTextBox_LostFocus(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        hstring DownloadSourceText = this->DownloadSourceTextBox().Text();
        Uri DownloadSource = nullptr;

        try
        {
            if (!DownloadSourceText.empty())
            {
                DownloadSource = Uri(DownloadSourceText);
            }
        }
        catch (...)
        {
            DownloadSource = Uri(L"http://" + DownloadSourceText);
        }

        if (DownloadSource)
        {
            this->FileNameTextBox().Text(
                M2PathFindFileName(DownloadSource.Path().data()));
        }

        this->m_DownloadSource = DownloadSource;
    }
}
