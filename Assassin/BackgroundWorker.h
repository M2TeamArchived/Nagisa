/******************************************************************************
Project: Assassin
Description: Definition for the BackgroundWorker.
File Name: BackgroundWorker.h
License: The MIT License
******************************************************************************/

#pragma once

namespace Assassin
{
	using Platform::String;
	using Platform::StringReference;
	using Windows::ApplicationModel::Background::IBackgroundTask;
	using Windows::ApplicationModel::Background::IBackgroundTaskInstance;

	String^ BackgroundWorkerSocketID = 
		StringReference(L"Assassin.BackgroundWorker");
	
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class BackgroundWorker sealed : public IBackgroundTask
	{
	private:

	public:
		BackgroundWorker();
		virtual ~BackgroundWorker();

		virtual void Run(IBackgroundTaskInstance^ taskInstance);
	};
}