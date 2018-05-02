/******************************************************************************
Project: Assassin
Description: Definition for the BackgroundWorker.
File Name: BackgroundWorker.h
License: The MIT License
******************************************************************************/

#pragma once

#include "BackgroundWorker.g.h"

namespace winrt
{
	using Windows::Foundation::IInspectable;
	using Windows::ApplicationModel::Background::IBackgroundTask;
	using Windows::ApplicationModel::Background::IBackgroundTaskInstance;
	using Windows::Networking::Sockets::SocketActivityInformation;
	using Windows::Networking::Sockets::SocketActivityTriggerDetails;
	using Windows::Networking::Sockets::SocketActivityTriggerReason;
	using Windows::Networking::Sockets::StreamSocket;
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
