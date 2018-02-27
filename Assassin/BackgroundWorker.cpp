/******************************************************************************
Project: Assassin
Description: Implementation for BackgroundWorker.
File Name: BackgroundWorker.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "BackgroundWorker.h"

using namespace Assassin;
using namespace Platform;

using Windows::Networking::Sockets::SocketActivityInformation;
using Windows::Networking::Sockets::SocketActivityTriggerDetails;
using Windows::Networking::Sockets::SocketActivityTriggerReason;
using Windows::Networking::Sockets::StreamSocket;

BackgroundWorker::BackgroundWorker()
{
	throw ref new Platform::NotImplementedException();
}

BackgroundWorker::~BackgroundWorker()
{
	
}

void BackgroundWorker::Run(IBackgroundTaskInstance^ taskInstance)
{
	auto deferral = taskInstance->GetDeferral();

	try
	{
		SocketActivityTriggerDetails^ details = 
			dynamic_cast<SocketActivityTriggerDetails^>(
				taskInstance->TriggerDetails);

		SocketActivityInformation^ socketInformation = 
			details->SocketInformation;

		switch (details->Reason)
		{
		case SocketActivityTriggerReason::SocketActivity:
		{
			StreamSocket^ Socket = socketInformation->StreamSocket;

			//CRYPTO_malloc_debug_init();

			/*
			DataReader reader = new DataReader(socket.InputStream);
			reader.InputStreamOptions = InputStreamOptions.Partial;
			await reader.LoadAsync(250);
			var dataString = reader.ReadString(reader.UnconsumedBufferLength);
			ShowToast(dataString);*/

			Socket->TransferOwnership(socketInformation->Id);

			break;
		}			
		case SocketActivityTriggerReason::KeepAliveTimerExpired:
		{
			StreamSocket^ Socket = socketInformation->StreamSocket;
			
			/*
			DataWriter writer = new DataWriter(socket.OutputStream);
			writer.WriteBytes(Encoding.UTF8.GetBytes("Keep alive"));
			await writer.StoreAsync();
			writer.DetachStream();
			writer.Dispose();*/

			Socket->TransferOwnership(socketInformation->Id);

			break;
		}		
		case SocketActivityTriggerReason::SocketClosed:
		{
			StreamSocket^ Socket = ref new StreamSocket();

			/*socket = new StreamSocket();
			socket.EnableTransferOwnership(taskInstance.Task.TaskId, SocketActivityConnectedStandbyAction.Wake);
			if (ApplicationData.Current.LocalSettings.Values["hostname"] == null)
			{
			break;
			}
			var hostname = (String)ApplicationData.Current.LocalSettings.Values["hostname"];
			var port = (String)ApplicationData.Current.LocalSettings.Values["port"];
			await socket.ConnectAsync(new HostName(hostname), port);*/

			Socket->TransferOwnership(BackgroundWorkerSocketID);

			break;
		}
		default:
			break;
		}
	}
	catch (Exception^ exception)
	{
		//ShowToast(exception.Message);	
	}

	deferral->Complete();
}
