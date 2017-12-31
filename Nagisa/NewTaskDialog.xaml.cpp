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

NewTaskDialog::NewTaskDialog()
{
	InitializeComponent();
}

void NewTaskDialog::DownloadButtonClick(
	ContentDialog^ sender, 
	ContentDialogButtonClickEventArgs^ args)
{
	using Windows::UI::Colors;
	SolidColorBrush^ NormalBrush = ref new SolidColorBrush(Colors::Gray);
	SolidColorBrush^ NoticeableBrush = ref new SolidColorBrush(Colors::Red);

	this->DownloadSourceTextBox->BorderBrush = NormalBrush;
	this->FileNameTextBox->BorderBrush = NormalBrush;
	this->SaveFolderTextBox->BorderBrush = NormalBrush;

	bool IsSuccess = true;

	if (nullptr == this->m_DownloadSource)
	{
		IsSuccess = false;
		this->DownloadSourceTextBox->BorderBrush = NoticeableBrush;
	}

	if (this->m_FileName->IsEmpty())
	{
		IsSuccess = false;
		this->FileNameTextBox->BorderBrush = NoticeableBrush;
	}

	if (nullptr == this->m_SaveFolder)
	{
		IsSuccess = false;
		this->SaveFolderTextBox->BorderBrush = NoticeableBrush;
	}

	if (false == IsSuccess)
	{
		args->Cancel = true;
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

	M2AsyncSetCompletedHandler(
		picker->PickSingleFolderAsync(),
		[this](
			IAsyncOperation<StorageFolder^>^ asyncInfo,
			AsyncStatus asyncStatus)
	{
		M2ExecuteOnUIThread([this, asyncInfo, asyncStatus]()
		{
			if (AsyncStatus::Completed == asyncStatus)
			{
				if (StorageFolder^ Folder = asyncInfo->GetResults())
				{
					this->m_SaveFolder = Folder;
					this->SaveFolderTextBox->Text = Folder->Path;
				}
			}
		});	
	});
}

template<typename CharType>
CharType M2PathFindFileName(CharType Path)
{
	CharType FileName = Path;

	while (Path && *Path)
	{
		if (L'\\' == *Path || L'/' == *Path)
		{
			FileName = Path + 1;
		}

		++Path;
	}

	return FileName;
}

void NewTaskDialog::DownloadSourceTextBox_LostFocus(
	Object^ sender,
	RoutedEventArgs^ e)
{
	Uri^ DownloadSource = nullptr;
	
	try
	{
		DownloadSource = ref new Uri(DownloadSourceTextBox->Text);	
	}
	catch (Exception^ ex)
	{
		DownloadSource = ref new Uri(L"http://" + DownloadSourceTextBox->Text);
	}

	this->FileNameTextBox->Text = ref new String(
		M2PathFindFileName(DownloadSource->Path->Data()));
	this->m_DownloadSource = DownloadSource;
}

void NewTaskDialog::FileNameTextBox_LostFocus(
	Object^ sender, 
	RoutedEventArgs^ e)
{
	this->m_FileName = this->FileNameTextBox->Text;
}
