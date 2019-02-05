/*
 * PROJECT:   Assassin
 * FILE:      BackgroundWorker.h
 * PURPOSE:   Definition for the BackgroundWorker
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#pragma once

#include "BackgroundWorker.g.h"

#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.ApplicationModel.Background.h>
#include <winrt\Windows.Networking.Sockets.h>

namespace winrt::Assassin::implementation
{
    using Windows::ApplicationModel::Background::IBackgroundTaskInstance;

    struct BackgroundWorker : BackgroundWorkerT<BackgroundWorker>
    {
    public:
        BackgroundWorker();
        virtual ~BackgroundWorker();

        void Run(
            IBackgroundTaskInstance const& taskInstance) const;
    };
}

namespace winrt::Assassin::factory_implementation
{
    struct BackgroundWorker : BackgroundWorkerT<
        BackgroundWorker, implementation::BackgroundWorker>
    {
    };
}
