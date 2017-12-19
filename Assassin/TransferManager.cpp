#include "pch.h"
#include "TransferManager.h"

using namespace Assassin;
using namespace Platform;

TransferManager::TransferManager()
{
}

String^ TransferManager::Version::get()
{
	return NAGISA_VERSION_STRING;
}