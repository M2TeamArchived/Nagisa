//
// MainPage.xaml.h
// MainPage 类的声明。
//

#pragma once

#include "MainPage.g.h"

namespace Nagisa
{
	using Assassin::TransferManager;
	using Platform::Object;
	using Windows::UI::Xaml::RoutedEventArgs;

	public ref class MainPage sealed
	{
	public:
		MainPage();
	internal:
		TransferManager ^ m_TransferManager = nullptr;
	private:
		void AboutButtonClick(Object^ sender, RoutedEventArgs^ e);
		void NewTaskButtonClick(Object^ sender, RoutedEventArgs^ e);
	};
}
