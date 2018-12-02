/******************************************************************************
Project: Assassin
Description: Definition for the BackgroundWorker.
File Name: BackgroundWorker.h
License: The MIT License
******************************************************************************/

#pragma once

#include "BackgroundWorker.g.h"

#include <winrt\Windows.Foundation.h>
#include <winrt\Windows.ApplicationModel.Background.h>
#include <winrt\Windows.Networking.Sockets.h>

namespace winrt
{
    using Windows::ApplicationModel::Background::IBackgroundTaskInstance;
}

namespace winrt::Assassin::implementation
{
    struct BackgroundWorker : BackgroundWorkerT<BackgroundWorker>
    {
    public:
        BackgroundWorker();
        virtual ~BackgroundWorker();

        void Run(
            winrt::IBackgroundTaskInstance const& taskInstance) const;
    };
}

namespace winrt::Assassin::factory_implementation
{
    struct BackgroundWorker : BackgroundWorkerT<
        BackgroundWorker, implementation::BackgroundWorker>
    {
    };
}
