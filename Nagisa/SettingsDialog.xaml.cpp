/******************************************************************************
Project: Nagisa
Description: Implementation for the Settings Dialog.
File Name: SettingsDialog.xaml.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "SettingsDialog.xaml.h"

using namespace Nagisa;

SettingsDialog::SettingsDialog(
    ITransferManager^ TransferManager) :
    m_TransferManager(TransferManager)
{
    InitializeComponent();
}

void SettingsDialog::ContentDialog_Loaded(
    Object^ sender,
    RoutedEventArgs^ e)
{
    this->m_LastusedFolder = this->m_TransferManager->LastusedFolder;

    IStorageFolder^ DefaultFolder = this->m_TransferManager->DefaultFolder;

    bool IsUseCustomFolder = (nullptr != DefaultFolder);

    this->UseCustomFolder->IsChecked = IsUseCustomFolder;

    this->DownloadFolderPathTextBox->Text =
        (IsUseCustomFolder
            ? DefaultFolder->Path
            : ((nullptr != this->m_LastusedFolder)
                ? this->m_LastusedFolder->Path
                : nullptr));
}

void SettingsDialog::CustomDownloadFolderBrowseButton_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    using Windows::Foundation::IAsyncOperation;
    using Windows::Storage::StorageFolder;
    using Windows::Storage::Pickers::FolderPicker;
    using Windows::Storage::Pickers::PickerLocationId;

    FolderPicker^ picker = ref new FolderPicker();
    picker->SuggestedStartLocation = PickerLocationId::ComputerFolder;
    picker->FileTypeFilter->Append(L"*");

    IAsyncOperation<StorageFolder^>^ Operation =
        picker->PickSingleFolderAsync();

    M2::CThread([this, Operation]()
    {
        StorageFolder^ Folder = M2AsyncWait(Operation);

        if (nullptr != Folder)
        {
            M2ExecuteOnUIThread([this, Folder]()
            {
                this->m_TransferManager->DefaultFolder = Folder;
                this->DownloadFolderPathTextBox->Text = Folder->Path;
            });
        }
    });
}

void SettingsDialog::UseCustomFolder_Click(
    Object^ sender,
    RoutedEventArgs^ e)
{
    bool IsUseCustomFolder = this->UseCustomFolder->IsChecked->Value;

    if (!IsUseCustomFolder)
    {
        this->m_TransferManager->DefaultFolder = nullptr;
    }

    this->DownloadFolderPathTextBox->Text =
        (IsUseCustomFolder
            ? nullptr
            : ((nullptr != this->m_LastusedFolder)
                ? this->m_LastusedFolder->Path
                : nullptr));
}
