#pragma once

namespace Assassin
{
	using Platform::String;
	
	public ref class TransferManager sealed
	{
	public:
		TransferManager();

		property String^ Version
		{
			String^ get();
		}

	};
}
