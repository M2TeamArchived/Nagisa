/******************************************************************************
Project: M2-Team Common Library
Description: Definition for the M2::CThread class
File Name: M2Thread.h
License: The MIT License
******************************************************************************/

#pragma once

#ifndef _M2_THREAD_
#define _M2_THREAD_

#include <Windows.h>

#include <assert.h>
#include <process.h>

#include "M2Object.h"

namespace M2
{
	// The implementation of thread.
	class CThread
	{
	private:
		// Internal implemention for creating the thread.
		HANDLE CreateThreadInternal(
			_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
			_In_ SIZE_T dwStackSize,
			_In_ LPTHREAD_START_ROUTINE lpStartAddress,
			_In_opt_ __drv_aliasesMem LPVOID lpParameter,
			_In_ DWORD dwCreationFlags,
			_Out_opt_ LPDWORD lpThreadId)
		{
			// sanity check for lpThreadId
			assert(sizeof(DWORD) == sizeof(unsigned));

			typedef unsigned(__stdcall* routine_type)(void*);

			// _beginthreadex calls CreateThread which will set the last error
			// value before it returns.
			return reinterpret_cast<HANDLE>(_beginthreadex(
				lpThreadAttributes,
				static_cast<unsigned>(dwStackSize),
				reinterpret_cast<routine_type>(lpStartAddress),
				lpParameter,
				dwCreationFlags,
				reinterpret_cast<unsigned*>(lpThreadId)));
		}

		// The thread object.
		CHandle m_Thread;

	public:
		// Create the thread.
		template<class TFunction>
		CThread(
			_In_ TFunction&& workerFunction,
			_In_ DWORD dwCreationFlags = 0)
		{
			auto ThreadFunctionInternal = [](LPVOID lpThreadParameter) -> DWORD
			{
				auto function = reinterpret_cast<TFunction*>(
					lpThreadParameter);
				(*function)();
				delete function;
				return 0;
			};

			this->m_Thread = CreateThreadInternal(
				nullptr,
				0,
				ThreadFunctionInternal,
				reinterpret_cast<void*>(
					new TFunction(std::move(workerFunction))),
				dwCreationFlags,
				nullptr);
		}

		// Detach the thread object.
		HANDLE Detach()
		{
			return this->m_Thread.Detach();
		}

		// Resume the thread.
		DWORD Resume()
		{
			return ResumeThread(this->m_Thread);
		}

		// Suspend the thread.
		DWORD Suspend()
		{
			return SuspendThread(this->m_Thread);
		}

		// Wait the thread.
		DWORD Wait(
			_In_ DWORD dwMilliseconds = INFINITE,
			_In_ BOOL bAlertable = FALSE)
		{
			return WaitForSingleObjectEx(
				this->m_Thread, dwMilliseconds, bAlertable);
		}

	};
}

#endif // _M2_THREAD_
