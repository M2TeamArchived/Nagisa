/******************************************************************************
Project: Assassin
Description: Definition for TransferManagerFactory.
File Name: TransferManagerFactory.h
License: The MIT License
******************************************************************************/

#pragma once

#include "TransferManager.h"

namespace winrt::Assassin::implementation
{
	struct TransferManagerFactory : TransferManagerFactoryT<TransferManagerFactory>
	{
	public:
		// Creates a new TransferManager object.
		// Parameters:
		//   The function does not have parameters.
		// Return value:
		//   Returns a new TransferManager object.
		winrt::ITransferManager CreateInstance() const;
	};
}

namespace winrt::Assassin::factory_implementation
{
	struct TransferManagerFactory : TransferManagerFactoryT<
		TransferManagerFactory, implementation::TransferManagerFactory>
	{
	};
}
