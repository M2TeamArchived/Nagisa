/******************************************************************************
Project: Assassin
Description: Implementation for TransferManagerFactory.
File Name: TransferManagerFactory.cpp
License: The MIT License
******************************************************************************/

#include "pch.h"
#include "TransferManagerFactory.h"

using namespace winrt::Assassin::implementation;

// Creates a new TransferManager object.
// Parameters:
//   The function does not have parameters.
// Return value:
//   Returns a new TransferManager object.
winrt::ITransferManager TransferManagerFactory::CreateInstance() const
{
	return winrt::make<TransferManager>(true);
}
