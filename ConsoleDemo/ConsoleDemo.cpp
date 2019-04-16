/*
 * PROJECT:   ConsoleDemo
 * FILE:      ConsoleDemo.cpp
 * PURPOSE:   Console Application Demo for developing.
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include "pch.h"
#include <iostream>

#include <winrt\Windows.Foundation.h>
#include <winrt/Windows.Web.Syndication.h>
#include <winrt/windows.Storage.h>
#include <Windows.h>
#include <exception>
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Web::Syndication;
using namespace Windows::Storage;

void PrintFeed(SyndicationFeed const& syndicationFeed)
{
    for (SyndicationItem const& syndicationItem : syndicationFeed.Items())
    {
        std::wcout << syndicationItem.Title().Text().c_str() << std::endl;
    }
}

IAsyncAction ProcessFeedAsync()
{
    try
    {
        Uri rssFeedUri(L"https://feeds.appinn.com/appinns/");
        SyndicationClient syndicationClient;
        SyndicationFeed syndicationFeed(co_await syndicationClient.RetrieveFeedAsync(rssFeedUri));
        PrintFeed(syndicationFeed);
    }
    catch (...)
    {
        std::cout << "exception" << std::endl;
    }
}

int main()
{
    try
    {
        winrt::init_apartment();
        auto processOp(ProcessFeedAsync());
        // do other work while the feed is being printed.

        processOp.get(); // no more work to do; call get() so that we see the printout before the application exits.
    }
    catch (const std::exception&)
    {

    }
    return 0;
}
