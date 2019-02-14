/*
 * PROJECT:   Nagisa
 * FILE:      SettingsDialog.cpp
 * PURPOSE:   Implementation for the Settings Dialog.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"
#include "SettingsDialog.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Nagisa::implementation
{
    SettingsDialog::SettingsDialog(
        TransferManager const& TransferManager) :
        m_TransferManager(TransferManager)
    {
        InitializeComponent();
    }

    fire_and_forget SettingsDialog::ContentDialog_Loaded(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        this->m_LastusedFolder =
            co_await this->m_TransferManager.LastusedFolder();

        IStorageFolder DefaultFolder =
            co_await this->m_TransferManager.DefaultFolder();

        bool IsUseCustomFolder = (nullptr != DefaultFolder);

        this->UseCustomFolder().IsChecked(IsUseCustomFolder);

        this->CustomDownloadFolderBrowseButton().IsEnabled(IsUseCustomFolder);

        this->DownloadFolderPathTextBox().Text((
            IsUseCustomFolder
            ? DefaultFolder.Path()
            : ((nullptr != this->m_LastusedFolder)
                ? this->m_LastusedFolder.Path()
                : L"")));
    }

    fire_and_forget SettingsDialog::CustomDownloadFolderBrowseButton_Click(
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

        if (nullptr != Folder)
        {
            this->m_TransferManager.SetDefaultFolder(Folder);
            this->DownloadFolderPathTextBox().Text(Folder.Path());
        }
    }

    void SettingsDialog::UseCustomFolder_Click(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        bool IsUseCustomFolder = this->UseCustomFolder().IsChecked().Value();

        this->CustomDownloadFolderBrowseButton().IsEnabled(IsUseCustomFolder);

        if (!IsUseCustomFolder)
        {
            this->m_TransferManager.SetDefaultFolder(nullptr);
        }

        this->DownloadFolderPathTextBox().Text(
            (IsUseCustomFolder
                ? L""
                : ((nullptr != this->m_LastusedFolder)
                    ? this->m_LastusedFolder.Path()
                    : L"")));
    }
}
