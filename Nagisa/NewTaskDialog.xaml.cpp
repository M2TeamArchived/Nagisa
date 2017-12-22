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

// https://go.microsoft.com/fwlink/?LinkId=234238 上介绍了“内容对话框”项模板

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

	M2SetAsyncCompletedHandler(
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

LPWSTR WINAPI PathFindFileNameW(LPCWSTR lpszPath)
{
	LPCWSTR lastSlash = lpszPath;

	while (lpszPath && *lpszPath)
	{
		if ((*lpszPath == '\\' || *lpszPath == '/' || *lpszPath == ':') &&
			lpszPath[1] && lpszPath[1] != '\\' && lpszPath[1] != '/')
			lastSlash = lpszPath + 1;
		lpszPath++;
	}
	return (LPWSTR)lastSlash;
}

const wchar_t* GetPathFileName(const wchar_t* Path)
{
	const wchar_t* result = nullptr;

	result = wcsrchr(Path, L'\\') + 1;
	if (nullptr == result)
	{

	}

	return result;
}

void NewTaskDialog::DownloadSourceTextBox_LostFocus(
	Object^ sender,
	RoutedEventArgs^ e)
{
	try
	{
		Uri^ DownloadSource = ref new Uri(DownloadSourceTextBox->Text);
		this->FileNameTextBox->Text = ref new String(
			PathFindFileNameW(DownloadSource->Path->Data()));
		this->m_DownloadSource = DownloadSource;
	}
	catch (Exception^ ex)
	{
		
	}
}


void Nagisa::NewTaskDialog::FileNameTextBox_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->m_FileName = this->FileNameTextBox->Text;
}
