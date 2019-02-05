/*
 * PROJECT:   Assassin
 * FILE:      BackgroundWorker.cpp
 * PURPOSE:   Implementation for the BackgroundWorker
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"
#include "BackgroundWorker.h"

namespace winrt::Assassin::implementation
{
    using Windows::Foundation::IInspectable;
    using Windows::ApplicationModel::Background::IBackgroundTask;
    using Windows::Networking::Sockets::SocketActivityInformation;
    using Windows::Networking::Sockets::SocketActivityTriggerDetails;
    using Windows::Networking::Sockets::SocketActivityTriggerReason;
    using Windows::Networking::Sockets::StreamSocket;

    hstring BackgroundWorkerSocketID = L"Assassin.BackgroundWorker";

    BackgroundWorker::BackgroundWorker()
    {
        throw hresult_not_implemented();
    }

    BackgroundWorker::~BackgroundWorker()
    {

    }

    void BackgroundWorker::Run(
        IBackgroundTaskInstance const& taskInstance) const
    {
        auto deferral = taskInstance.GetDeferral();

        try
        {
            IInspectable triggerDetails = taskInstance.TriggerDetails();

            SocketActivityTriggerDetails details =
                triggerDetails.try_as<SocketActivityTriggerDetails>();

            SocketActivityInformation socketInformation =
                details.SocketInformation();

            switch (details.Reason())
            {
            case SocketActivityTriggerReason::SocketActivity:
            {
                StreamSocket Socket = socketInformation.StreamSocket();

                //CRYPTO_malloc_debug_init();

                /*
                DataReader reader = new DataReader(socket.InputStream);
                reader.InputStreamOptions = InputStreamOptions.Partial;
                await reader.LoadAsync(250);
                var dataString = reader.ReadString(
                    reader.UnconsumedBufferLength);
                ShowToast(dataString);*/

                Socket.TransferOwnership(socketInformation.Id());

                break;
            }
            case SocketActivityTriggerReason::KeepAliveTimerExpired:
            {
                StreamSocket Socket = socketInformation.StreamSocket();

                /*
                DataWriter writer = new DataWriter(socket.OutputStream);
                writer.WriteBytes(Encoding.UTF8.GetBytes("Keep alive"));
                await writer.StoreAsync();
                writer.DetachStream();
                writer.Dispose();*/

                Socket.TransferOwnership(socketInformation.Id());

                break;
            }
            case SocketActivityTriggerReason::SocketClosed:
            {
                StreamSocket Socket = StreamSocket();

                /*socket = new StreamSocket();
                socket.EnableTransferOwnership(
                    taskInstance.Task.TaskId,
                    SocketActivityConnectedStandbyAction.Wake);
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
        catch (hresult_error const& ex)
        {
            UNREFERENCED_PARAMETER(ex);

            //ShowToast(exception.Message);	
        }

        deferral.Complete();
    }
}
