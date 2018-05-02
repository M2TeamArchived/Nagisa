/******************************************************************************
Project: Assassin
Description: Implementation for BackgroundWorker.
File Name: BackgroundWorker.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "BackgroundWorker.h"

using namespace winrt::Assassin::implementation;

winrt::hstring BackgroundWorkerSocketID = L"Assassin.BackgroundWorker";

BackgroundWorker::BackgroundWorker()
{
	throw winrt::hresult_not_implemented();
}

BackgroundWorker::~BackgroundWorker()
{

}

void BackgroundWorker::Run(
	winrt::IBackgroundTaskInstance const& taskInstance) const
{
	auto deferral = taskInstance.GetDeferral();

	try
	{
		winrt::IInspectable triggerDetails = taskInstance.TriggerDetails();

		winrt::SocketActivityTriggerDetails details =
			triggerDetails.try_as<winrt::SocketActivityTriggerDetails>();

		winrt::SocketActivityInformation socketInformation =
			details.SocketInformation();

		switch (details.Reason())
		{
		case winrt::SocketActivityTriggerReason::SocketActivity:
		{
			winrt::StreamSocket Socket = socketInformation.StreamSocket();

			//CRYPTO_malloc_debug_init();

			/*
			DataReader reader = new DataReader(socket.InputStream);
			reader.InputStreamOptions = InputStreamOptions.Partial;
			await reader.LoadAsync(250);
			var dataString = reader.ReadString(reader.UnconsumedBufferLength);
			ShowToast(dataString);*/

			Socket.TransferOwnership(socketInformation.Id());

			break;
		}
		case winrt::SocketActivityTriggerReason::KeepAliveTimerExpired:
		{
			winrt::StreamSocket Socket = socketInformation.StreamSocket();

			/*
			DataWriter writer = new DataWriter(socket.OutputStream);
			writer.WriteBytes(Encoding.UTF8.GetBytes("Keep alive"));
			await writer.StoreAsync();
			writer.DetachStream();
			writer.Dispose();*/

			Socket.TransferOwnership(socketInformation.Id());

			break;
		}
		case winrt::SocketActivityTriggerReason::SocketClosed:
		{
			winrt::StreamSocket Socket = winrt::StreamSocket();

			/*socket = new StreamSocket();
			socket.EnableTransferOwnership(taskInstance.Task.TaskId, SocketActivityConnectedStandbyAction.Wake);
			if (ApplicationData.Current.LocalSettings.Values["hostname"] == null)
			{
			break;
			}
			var hostname = (String)ApplicationData.Current.LocalSettings.Values["hostname"];
			var port = (String)ApplicationData.Current.LocalSettings.Values["port"];
			await socket.ConnectAsync(new HostName(hostname), port);*/

			Socket.TransferOwnership(BackgroundWorkerSocketID);

			break;
		}
		default:
			break;
		}
	}
	catch (winrt::hresult_error const& ex)
	{
		UNREFERENCED_PARAMETER(ex);

		//ShowToast(exception.Message);	
	}

	deferral.Complete();
}
