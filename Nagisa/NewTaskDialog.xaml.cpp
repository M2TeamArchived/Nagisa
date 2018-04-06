//
// NewTaskDialog.xaml.cpp
// NewTaskDialog 类的实现
//

#include "pch.h"
#include "NewTaskDialog.xaml.h"

using namespace Nagisa;
using namespace Assassin;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

NewTaskDialog::NewTaskDialog(
	ITransferManager^ TransferManager) :
	m_TransferManager(TransferManager),
	m_NormalBrush(ref new SolidColorBrush(Colors::Gray)), 
	m_NoticeableBrush(ref new SolidColorBrush(Colors::Red))
{
	InitializeComponent();
}

void NewTaskDialog::DownloadButtonClick(
	ContentDialog^ sender, 
	ContentDialogButtonClickEventArgs^ args)
{
	bool IsDownloadSourceEmpty = (nullptr == this->m_DownloadSource);
	bool IsFileNameEmpty = this->m_FileName->IsEmpty();
	bool IsSaveFolderEmpty = (nullptr == this->m_SaveFolder);

	this->DownloadSourceTextBox->BorderBrush = IsDownloadSourceEmpty
		? this->m_NoticeableBrush : this->m_NormalBrush;
	this->FileNameTextBox->BorderBrush = IsFileNameEmpty
		? this->m_NoticeableBrush : this->m_NormalBrush;
	this->SaveFolderTextBox->BorderBrush = IsSaveFolderEmpty
		? this->m_NoticeableBrush : this->m_NormalBrush;

	if (IsDownloadSourceEmpty || IsFileNameEmpty || IsSaveFolderEmpty)
	{
		args->Cancel = true;
	}
	else
	{
		M2AsyncWait(this->m_TransferManager->AddTaskAsync(
			this->m_DownloadSource,
			this->m_FileName,
			this->m_SaveFolder));
	}
}

void NewTaskDialog::BrowseButtonClick(
	Object^ sender, 
	RoutedEventArgs^ e)
{
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
				this->m_SaveFolder = Folder;
				this->SaveFolderTextBox->Text = Folder->Path;
			});	
		}	
	});
}

void NewTaskDialog::DownloadSourceTextBox_LostFocus(
	Object^ sender,
	RoutedEventArgs^ e)
{
	String^ DownloadSourceText = this->DownloadSourceTextBox->Text;
	Uri^ DownloadSource = nullptr;
	
	try
	{
		if (nullptr != DownloadSourceText && !DownloadSourceText->IsEmpty())
		{
			DownloadSource = ref new Uri(DownloadSourceText);
		}	
	}
	catch (...)
	{
		DownloadSource = ref new Uri(L"http://" + DownloadSourceText);
	}

	if (nullptr != DownloadSource)
	{
		this->FileNameTextBox->Text = 
			ref new String(M2PathFindFileName(DownloadSource->Path->Data()));

		this->FileNameTextBox_LostFocus(sender, e);
	}

	this->m_DownloadSource = DownloadSource;
}

void NewTaskDialog::FileNameTextBox_LostFocus(
	Object^ sender,
	RoutedEventArgs^ e)
{
	this->m_FileName = this->FileNameTextBox->Text;
}

void NewTaskDialog::ContentDialog_Loaded(
	Object^ sender,
	RoutedEventArgs^ e)
{
	this->m_SaveFolder = this->m_TransferManager->DefaultFolder;
	if (nullptr == this->m_SaveFolder)
	{
		this->m_SaveFolder = this->m_TransferManager->LastusedFolder;
	}

	if (nullptr != this->m_SaveFolder)
	{
		this->SaveFolderTextBox->Text = this->m_SaveFolder->Path;
	}
}
