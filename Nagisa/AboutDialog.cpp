/*
 * PROJECT:   Nagisa
 * FILE:      AboutDialog.cpp
 * PURPOSE:   Implementation for the About Dialog.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"
#include "AboutDialog.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Nagisa::implementation
{
    AboutDialog::AboutDialog(
        ITransferManager const& TransferManager) :
        m_TransferManager(TransferManager)
    {
        InitializeComponent();
    }

    void AboutDialog::ContentDialog_Loaded(
        IInspectable const& sender,
        RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.
        UNREFERENCED_PARAMETER(e);   // Unused parameter.

        this->VersionText().Text(m_TransferManager.Version());
    }

    void AboutDialog::GitHubButtonClick(
        ContentDialog const& sender,
        ContentDialogButtonClickEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.

        using Windows::Foundation::Uri;
        using Windows::System::Launcher;

        Launcher::LaunchUriAsync(
            Uri(L"https://github.com/Project-Nagisa/Nagisa"));

        args.Cancel(true);
    }
}
