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
        TransferManager const& TransferManager)
    {
        InitializeComponent();

        this->VersionText().Text(TransferManager.Version());
    }

    fire_and_forget AboutDialog::GitHubButtonClick(
        ContentDialog const& sender,
        ContentDialogButtonClickEventArgs const& args)
    {
        UNREFERENCED_PARAMETER(sender);  // Unused parameter.

        using Windows::Foundation::Uri;
        using Windows::System::Launcher;

        co_await Launcher::LaunchUriAsync(
            Uri(L"https://github.com/Project-Nagisa/Nagisa"));

        args.Cancel(true);
    }
}
