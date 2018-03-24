/******************************************************************************
Project: M2-Team Common Library
Description: Definition for the asynchronous helper functions.
File Name: M2AsyncHelpers.h
License: The MIT License
******************************************************************************/

#pragma once

#ifndef _M2_ASYNC_HELPERS_
#define _M2_ASYNC_HELPERS_

#ifdef __cplusplus_winrt

#include <Windows.h>
#include <wrl\client.h>

#include "M2RemoveReference.h"
#include "M2Object.h"
#include "M2CXHelpers.h"
#include "M2Thread.h"

#include <memory>

// Set the completed handler function for asynchronous call.
// Parameters:
//   Async: The asynchronous call you want to set.
//   Function: The completed handler function you want to set.
// Return value:
//   This function does not return a value.
template<typename TAsync, typename... TFunction>
inline void M2AsyncSetCompletedHandler(
	TAsync Async, const TFunction&... Function)
{
	Async->Completed =
		ref new M2RemoveReference<decltype(Async->Completed)>::Type(
			Function...);
}

// Set the progress handler function for asynchronous call.
// Parameters:
//   Async: The asynchronous call you want to set.
//   Function: The progress handler function you want to set.
// Return value:
//   This function does not return a value.
template<typename TAsync, typename... TFunction>
inline void M2AsyncSetProgressHandler(
	TAsync Async, const TFunction&... Function)
{
	Async->Progress =
		ref new M2RemoveReference<decltype(Async->Progress)>::Type(
			Function...);
}

// Try to wait asynchronous call.
// Parameters:
//   Async: The asynchronous call you want to wait.
//   Timeout: The maximum time interval for waiting the asynchronous call, in 
//   milliseconds. A value of -1 indicates that the suspension should not time 
//   out.
// Return value:
//   The return value is determined by the asynchronous call.
//   The function will throw a COM exception if the function fails. 
template<typename TAsync>
auto M2AsyncWait(
	TAsync Async, int32 Timeout = -1) -> decltype(Async->GetResults())
{
	HRESULT M2AsyncHandleCompleted(Platform::Object^ Async);

	using M2::CHandle;
	using Platform::COMException;
	using Windows::Foundation::AsyncStatus;
	using Windows::Foundation::IAsyncInfo;

	HRESULT hr = S_OK;

	// Create an event object for wait the asynchronous call.
	CHandle CompletedEvent(
		CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
	if (!CompletedEvent.IsInvalid())
	{
		// Set the completed handler.
		M2AsyncSetCompletedHandler(
			Async, [&](IAsyncInfo^ /*asyncInfo*/, AsyncStatus /*asyncStatus*/)
		{
			// Set event signaled to end wait when completed.
			SetEvent(CompletedEvent);
		});

		// Wait event object signaled.
		WaitForSingleObjectEx(CompletedEvent, Timeout, FALSE);

		// Handle the completed asynchronous call.
		hr = M2AsyncHandleCompleted(Async);
	}
	else
	{
		// Get error code if failed to create an event object.
		hr = M2GetLastError();
	}

	// Throw a COM exception if failed.
	M2ThrowPlatformExceptionIfFailed(hr);

	// Return the result of asynchronous call.
	return Async->GetResults();
}

// Execute function on the UI thread with normal priority.
// Parameters:
//   Function: The function you want to execute.
// Return value:
//   The return value is Windows::Foundation::IAsyncAction^.
template<typename... TFunction>
inline Windows::Foundation::IAsyncAction^ M2ExecuteOnUIThread(
	const TFunction&... Function)
{
	Windows::Foundation::IAsyncAction^ M2ExecuteOnUIThread(
		Windows::UI::Core::DispatchedHandler^ agileCallback);

	return M2ExecuteOnUIThread(
		ref new Windows::UI::Core::DispatchedHandler(Function...));
}

// Async Controller Interface

interface class IM2AsyncController
{
	void CancelCurrentTask();

	bool IsTaskCancellationRequested();
};

template<typename ProgressType>
interface class IM2AsyncControllerEx : IM2AsyncController
{
	void ReportProgress(const typename ProgressType& ProgressValue);
};

// Implementions for M2AsyncCreate function.
namespace M2AsyncCreateInternal
{
	ref class M2EmptyRefClass
	{
	};

	template<typename TProgress, typename TReturn, bool NeedProgressHandler>
	struct M2AsyncType
	{
	};

	using Windows::Foundation::IAsyncOperationWithProgress;
	using Windows::Foundation::AsyncOperationProgressHandler;
	using Windows::Foundation::AsyncOperationWithProgressCompletedHandler;

	template<typename TProgress, typename TReturn>
	struct M2AsyncType<TProgress, TReturn, true>
	{
		using BaseType = IAsyncOperationWithProgress<TReturn, TProgress>;
		using ProgressHandlerType =
			AsyncOperationProgressHandler<TReturn, TProgress>;
		using CompletedHandlerType =
			AsyncOperationWithProgressCompletedHandler<TReturn, TProgress>;

		using ReturnType = TReturn;
		using ProgressType = TProgress;

		static const bool NeedProgressHandler = true;
	};

	using Windows::Foundation::IAsyncOperation;
	using Windows::Foundation::AsyncOperationCompletedHandler;

	template<typename TProgress, typename TReturn>
	struct M2AsyncType<TProgress, TReturn, false>
	{
		using BaseType = IAsyncOperation<TReturn>;
		using ProgressHandlerType = M2EmptyRefClass;
		using CompletedHandlerType = AsyncOperationCompletedHandler<TReturn>;

		using ReturnType = TReturn;
		using ProgressType = TProgress;

		static const bool NeedProgressHandler = false;
	};

	using Windows::Foundation::IAsyncActionWithProgress;
	using Windows::Foundation::AsyncActionProgressHandler;
	using Windows::Foundation::AsyncActionWithProgressCompletedHandler;

	template<typename TProgress>
	struct M2AsyncType<TProgress, void, true>
	{
		using BaseType = IAsyncActionWithProgress<TProgress>;
		using ProgressHandlerType = AsyncActionProgressHandler<TProgress>;
		using CompletedHandlerType =
			AsyncActionWithProgressCompletedHandler<TProgress>;

		using ReturnType = void;
		using ProgressType = TProgress;

		static const bool NeedProgressHandler = true;
	};

	using Windows::Foundation::IAsyncAction;
	using Windows::Foundation::AsyncActionCompletedHandler;

	template<typename TProgress>
	struct M2AsyncType<TProgress, void, false>
	{
		using BaseType = IAsyncAction;
		using ProgressHandlerType = M2EmptyRefClass;
		using CompletedHandlerType = AsyncActionCompletedHandler;

		using ReturnType = void;
		using ProgressType = TProgress;

		static const bool NeedProgressHandler = false;
	};

	enum M2AsyncStatusInternal
	{
		// Client visible states. (Must match AsyncStatus exactly)
		Started = Windows::Foundation::AsyncStatus::Started,
		Completed = Windows::Foundation::AsyncStatus::Completed,
		Canceled = Windows::Foundation::AsyncStatus::Canceled,
		Error = Windows::Foundation::AsyncStatus::Error,

		// Non-client visible internal states.
		Created,
		CancelPending,
		Closed,
		Undefined,
	};

	// This class describes an exception thrown by the M2AsyncGenerator in 
	// order to force the current task to cancel.
	class M2AsyncTaskCanceledException : public std::exception
	{
	public:
		explicit M2AsyncTaskCanceledException(
			_In_z_ const char * _Message) noexcept
			: std::exception(_Message)
		{}

		M2AsyncTaskCanceledException() noexcept
			: exception()
		{}
	};

	// Internal base class implementation for async operations (based on 
	// internal Windows representation for ABI level async operations)
	template<typename AsyncType>
	ref class M2AsyncBase abstract : AsyncType::BaseType, IM2AsyncController
	{
	internal:
		HRESULT volatile m_ErrorCode;
		uint32 volatile m_Id;
		M2AsyncStatusInternal volatile m_Status;
		typename AsyncType::CompletedHandlerType^ volatile m_CompletedHandler;
		long volatile m_CompletedHandlerAssigned;
		long volatile m_CallbackMade;

		// This method is used to check if calls to the AsyncInfo properties
		// (id, status, errorcode) are legal in the current state. It also
		// generates the appropriate error hr to return in the case of an
		// illegal call.
		inline void CheckValidStateForAsyncCall()
		{
			M2AsyncStatusInternal Current = this->m_Status;

			if (Current == M2AsyncStatusInternal::Closed)
			{
				M2ThrowPlatformException(E_ILLEGAL_METHOD_CALL);
			}
			else if (Current == M2AsyncStatusInternal::Created)
			{
				M2ThrowPlatformException(E_ASYNC_OPERATION_NOT_STARTED);
			}
		}

		// This method checks to see if the delegate properties can be
		// modified in the current state and generates the appropriate
		// error hr in the case of violation.
		inline void CheckValidStateForDelegateCall()
		{
			if (this->m_Status == M2AsyncStatusInternal::Closed)
			{
				M2ThrowPlatformException(E_ILLEGAL_METHOD_CALL);
			}
		}

		// This method checks to see if results can be collected in the
		// current state and generates the appropriate error hr in
		// the case of a violation.
		inline void CheckValidStateForResultsCall()
		{
			M2AsyncStatusInternal Current = this->m_Status;

			if (Current == M2AsyncStatusInternal::Error)
			{
				M2ThrowPlatformException(this->m_ErrorCode);
			}

			// Illegal before transition to Completed or Cancelled state
			if (Current != M2AsyncStatusInternal::Completed)
			{
				M2ThrowPlatformException(E_ILLEGAL_METHOD_CALL);
			}
		}

		inline bool IsTerminalState(M2AsyncStatusInternal status)
		{
			return (status == M2AsyncStatusInternal::Error ||
				status == M2AsyncStatusInternal::Canceled ||
				status == M2AsyncStatusInternal::Completed ||
				status == M2AsyncStatusInternal::Closed);
		}

		inline bool IsTerminalState()
		{
			return this->IsTerminalState(this->m_Status);
		}

		inline bool TransitionToState(const M2AsyncStatusInternal NewState)
		{
			M2AsyncStatusInternal Current = this->m_Status;

			// This enforces the valid state transitions of the asynchronous 
			// worker object state machine.
			switch (NewState)
			{
			case M2AsyncStatusInternal::Started:
				if (Current != M2AsyncStatusInternal::Created)
				{
					return false;
				}
				break;
			case M2AsyncStatusInternal::CancelPending:
				if (Current != M2AsyncStatusInternal::Started)
				{
					return false;
				}
				break;

			case M2AsyncStatusInternal::Closed:
				if (!this->IsTerminalState(Current))
				{
					return false;
				}
				break;
			case M2AsyncStatusInternal::Completed:
			case M2AsyncStatusInternal::Canceled:
			case M2AsyncStatusInternal::Error:
				if (Current != M2AsyncStatusInternal::Started &&
					Current != M2AsyncStatusInternal::CancelPending)
				{
					return false;
				}
				break;
			default:
				return false;
				break;
			}

			// attempt the transition to the new state
			// Note: if m_Status == Current, then there was no intervening 
			// write by the async work object and the swap succeeded.
			M2AsyncStatusInternal RetState =
				static_cast<M2AsyncStatusInternal>(_InterlockedCompareExchange(
					reinterpret_cast<volatile LONG*>(&this->m_Status),
					NewState,
					static_cast<LONG>(Current)));

			// ICE returns the former state, if the returned state and the
			// state we captured at the beginning of this method are the same,
			// the swap succeeded.
			return (RetState == Current);
		}

		bool TryTransitionToCompleted()
		{
			return this->TransitionToState(M2AsyncStatusInternal::Completed);
		}

		bool TryTransitionToCancelled()
		{
			return this->TransitionToState(M2AsyncStatusInternal::Canceled);
		}

		bool TryTransitionToError(const HRESULT error)
		{
			_InterlockedCompareExchange(
				reinterpret_cast<volatile LONG*>(&this->m_ErrorCode),
				error,
				S_OK);
			return this->TransitionToState(M2AsyncStatusInternal::Error);
		}

		// This method can be called by derived classes periodically to 
		// determine whether the asynchronous operation should continue 
		// processing or should be halted.
		inline bool ContinueAsyncOperation()
		{
			return (this->m_Status == M2AsyncStatusInternal::Started);
		}

		// Start - this is not externally visible since async operations "hot 
		// start" before returning to the caller.
		void Start()
		{
			if (this->TransitionToState(M2AsyncStatusInternal::Started))
			{
				this->OnStart();
			}
			else
			{
				M2ThrowPlatformException(E_ILLEGAL_STATE_CHANGE);
			}
		}

		void FireCompletion()
		{
			this->TryTransitionToCompleted();

			// we guarantee that completion can only ever be fired once
			if (this->m_CompletedHandler != nullptr &&
				InterlockedIncrement(&this->m_CallbackMade) == 1)
			{
				try
				{
					this->m_CompletedHandler(
						(typename AsyncType::BaseType^)this, this->Status);
					this->m_CompletedHandler = nullptr;
				}
				catch (...)
				{
					// Null out the delegate since something went wrong when 
					// calling it.
					this->m_CompletedHandler = nullptr;
					throw;
				}
			}
		}

		// These three methods are used to allow the async worker 
		// implementation do work on state transitions. No real "work" should 
		// be done in these methods. In other words they should not block for a
		// long time on UI timescales.
		virtual void OnStart() = 0;
		virtual void OnClose() = 0;
		virtual void OnCancel() = 0;

		virtual typename AsyncType::ProgressHandlerType^ GetOnProgress()
		{
			M2ThrowPlatformException(E_UNEXPECTED);
		}

		virtual void PutOnProgress(
			typename AsyncType::ProgressHandlerType^ ProgressHandler)
		{
			M2ThrowPlatformException(E_UNEXPECTED);
		}

		virtual void PutOnId(uint32 Id)
		{
			this->CheckValidStateForAsyncCall();

			if (Id == 0)
			{
				M2ThrowPlatformException(E_INVALIDARG);
			}
			else if (this->m_Status != M2AsyncStatusInternal::Created)
			{
				M2ThrowPlatformException(E_ILLEGAL_METHOD_CALL);
			}

			this->m_Id = Id;
		}

		M2AsyncBase() :
			m_ErrorCode(S_OK),
			m_Id(1),
			m_Status(M2AsyncStatusInternal::Created),
			m_CompletedHandler(nullptr),
			m_CompletedHandlerAssigned(0),
			m_CallbackMade(0)
		{

		}

	public:
		virtual property Windows::Foundation::HResult ErrorCode
		{
			Windows::Foundation::HResult get()
			{
				this->CheckValidStateForAsyncCall();

				Windows::Foundation::HResult HR;
				HR.Value = this->m_ErrorCode;
				return HR;
			}
		}

		virtual property uint32 Id
		{
			uint32 get()
			{
				this->CheckValidStateForAsyncCall();

				return this->m_Id;
			}
		}

		virtual property Windows::Foundation::AsyncStatus Status
		{
			Windows::Foundation::AsyncStatus get()
			{
				this->CheckValidStateForAsyncCall();

				M2AsyncStatusInternal Current = this->m_Status;

				// Map our internal cancel pending to cancelled. This way 
				// "pending cancelled" looks to the outside as "cancelled" but 
				// can still transition to "completed" if the operation 
				// completes without acknowledging the cancellation request.
				switch (Current)
				{
				case M2AsyncStatusInternal::CancelPending:
					Current = M2AsyncStatusInternal::Canceled;
					break;
				case M2AsyncStatusInternal::Created:
					Current = M2AsyncStatusInternal::Started;
					break;
				default:
					break;
				}

				return static_cast<Windows::Foundation::AsyncStatus>(Current);
			}
		}

		virtual void Cancel()
		{
			if (this->TransitionToState(M2AsyncStatusInternal::CancelPending))
			{
				this->OnCancel();
			}
		}

		virtual void Close()
		{
			if (this->TransitionToState(M2AsyncStatusInternal::Closed))
			{
				this->OnClose();
			}
			else
			{
				// Closed => Closed transition is just ignored
				if (this->m_Status != M2AsyncStatusInternal::Closed)
				{
					M2ThrowPlatformException(E_ILLEGAL_STATE_CHANGE);
				}
			}
		}

		virtual property typename AsyncType::CompletedHandlerType^ Completed
		{
			typename typename AsyncType::CompletedHandlerType^ get()
			{
				this->CheckValidStateForDelegateCall();
				return this->m_CompletedHandler;
			}

			void set(
				typename AsyncType::CompletedHandlerType^ CompletedHandler)
			{
				this->CheckValidStateForDelegateCall();

				// this delegate property is "write once"
				if (InterlockedIncrement(
					&this->m_CompletedHandlerAssigned) == 1)
				{
					this->m_CompletedHandler = CompletedHandler;
					// Guarantee that the write of m_CompletedHandler is 
					// ordered with respect to the read of state below as 
					// perceived from FireCompletion on another thread.
					MemoryBarrier();
					if (this->IsTerminalState())
					{
						this->FireCompletion();
					}
				}
				else
				{
					M2ThrowPlatformException(E_ILLEGAL_DELEGATE_ASSIGNMENT);
				}
			}
		}

		virtual property typename AsyncType::ProgressHandlerType^ Progress
		{
			typename typename AsyncType::ProgressHandlerType^ get()
			{
				return this->GetOnProgress();
			}

			void set(typename AsyncType::ProgressHandlerType^ _ProgressHandler)
			{
				this->PutOnProgress(_ProgressHandler);
			}
		}

		virtual typename AsyncType::ReturnType GetResults()
		{
			M2ThrowPlatformException(E_UNEXPECTED);
		}

		virtual void CancelCurrentTask()
		{
			throw M2AsyncTaskCanceledException();
		}

		virtual bool IsTaskCancellationRequested()
		{
			return (this->m_Status == M2AsyncStatusInternal::CancelPending);
		}
	};

	// __declspec(no_empty_identity_interface) is used to suppress generation 
	// of WinRT default functions (e.g. QueryInterface, AddRef, etc). 
	// M2AsyncProgressBase is never used directly, so generation of WinRT 
	// functions is not needed and unnecessarily increases generated code size.

	template<typename AsyncType, bool NeedProgressHandler>
	ref class __declspec(no_empty_identity_interface)
		M2AsyncProgressBase abstract :
	M2AsyncBase<AsyncType>
	{
	};

	template<typename AsyncType>
	ref class __declspec(no_empty_identity_interface)
		M2AsyncProgressBase<AsyncType, true> abstract :
		M2AsyncBase<AsyncType>,
		IM2AsyncControllerEx<typename AsyncType::ProgressType>
	{
	internal:
		using AsyncProgressHandlerType =
			typename AsyncType::ProgressHandlerType;
		using AsyncProgressType = typename AsyncType::ProgressType;

		typename AsyncProgressHandlerType^ m_ProgressHandler;

		virtual typename AsyncProgressHandlerType^ GetOnProgress() override
		{
			this->CheckValidStateForDelegateCall();
			return this->m_ProgressHandler;
		}

		virtual void PutOnProgress(
			typename AsyncProgressHandlerType^ ProgressHandler) override
		{
			this->CheckValidStateForDelegateCall();
			this->m_ProgressHandler = ProgressHandler;
		}

		void FireProgress(const typename AsyncProgressType& ProgressValue)
		{
			if (this->m_ProgressHandler != nullptr)
			{
				try
				{
					this->m_ProgressHandler(
						(typename AsyncType::BaseType^)this, ProgressValue);
				}
				catch (...)
				{
					// Null out the delegate since something went wrong when 
					// calling it.
					this->m_ProgressHandler = nullptr;
					throw;
				}
			}
		}

	public:
		M2AsyncProgressBase() :
			M2AsyncBase<AsyncType>(),
			m_ProgressHandler(nullptr)
		{
		}

		virtual void ReportProgress(
			const typename AsyncType::ProgressType& ProgressValue)
		{
			this->FireProgress(ProgressValue);
		}
	};

	template<typename TClass, typename TReturn, typename TArg1>
	TArg1 M2Arg1ClassHelperThunk(TReturn(TClass::*)(TArg1) const);

	template<typename TClass, typename TReturn, typename TArg1>
	TReturn M2ReturnTypeClassHelperThunk(TReturn(TClass::*)(TArg1) const);

	template<typename TProgress>
	struct M2AsyncProgressTypeTraits
	{
		static const bool NeedProgressHandler = false;
		typedef void ProgressType;
	};

	template<typename TProgress>
	struct M2AsyncProgressTypeTraits<IM2AsyncControllerEx<TProgress>^>
	{
		static const bool NeedProgressHandler = true;
		typedef typename TProgress ProgressType;
	};

	template<typename TFunction>
	struct M2AsyncLambdaType
	{
		using ReturnType =
			decltype(M2ReturnTypeClassHelperThunk(&(TFunction::operator())));

		using ControllerType =
			decltype(M2Arg1ClassHelperThunk(&(TFunction::operator())));
		using ProgressTypeTraits = M2AsyncProgressTypeTraits<ControllerType>;
		using ProgressType = typename ProgressTypeTraits::ProgressType;
		static const bool NeedProgressHandler = ProgressTypeTraits::NeedProgressHandler;

		using AsyncType =
			typename M2AsyncType<ProgressType, ReturnType, NeedProgressHandler>;
	};

	template<typename TFunction, typename TReturn>
	class M2AsyncFunction
	{
	private:
		typename TFunction m_Function;
		std::shared_ptr<typename TReturn> m_Return;

	public:
		M2AsyncFunction() : m_Function(nullptr)
		{

		}

		M2AsyncFunction(const TFunction& Function)
			: m_Function(Function)
		{

		}

		template<typename... TArg>
		void Run(const TArg&... Arg)
		{
			this->m_Return = std::make_shared<typename TReturn>(
				this->m_Function(Arg...));
		}

		TReturn Get()
		{
			return *this->m_Return;
		}
	};

	template<typename TFunction>
	class M2AsyncFunction<TFunction, void>
	{
	private:
		typename TFunction m_Function;

	public:
		M2AsyncFunction() : m_Function(nullptr)
		{

		}

		M2AsyncFunction(const TFunction& Function)
			: m_Function(Function)
		{

		}

		template<typename... TArg>
		void Run(const TArg&... Arg)
		{
			this->m_Function(Arg...);
		}

		void Get()
		{

		}
	};

	// Async Creation Layer
	template<typename TFunction>
	ref class M2AsyncGenerator sealed : M2AsyncProgressBase<
		typename M2AsyncLambdaType<TFunction>::AsyncType,
		M2AsyncLambdaType<TFunction>::AsyncType::NeedProgressHandler>
	{
	internal:
		using AsyncReturnType =
			typename M2AsyncLambdaType<TFunction>::AsyncType::ReturnType;

		M2AsyncFunction<TFunction, AsyncReturnType> m_Function;

		M2AsyncGenerator(const TFunction& Function) : m_Function(Function)
		{
			// Virtual call here is safe as the class is declared 'sealed'
			this->Start();
		}

	public:

		virtual typename AsyncReturnType GetResults() override
		{
			this->CheckValidStateForResultsCall();
			return this->m_Function.Get();
		}

	internal:

		// The only thing we must do different from the base class is we must 
		// spin the hot task on transition from Created->Started. Otherwise, 
		// let the base thunk handle everything.

		virtual void OnStart() override
		{
			// Call the appropriate task generator to actually produce a task 
			// of the expected type. This might adapt the user lambda for 
			// progress reports, wrap the return result in a task, or allow for
			// direct return of a task depending on the form of the lambda.

			M2::CThread([this]()
			{
				// Capture the state.
				try
				{
					// Run the worker function.
					this->m_Function.Run(this);

					this->FireCompletion();
				}
				catch (M2AsyncTaskCanceledException&)
				{
					this->TryTransitionToCancelled();
				}
				catch (...)
				{
					this->TryTransitionToError(
						M2ThrownPlatformExceptionToHResult());
				}
			});
		}

		virtual void OnCancel() override
		{

		}

		virtual void OnClose() override
		{
		}
	};
}

// Creates a Windows Runtime asynchronous construct based on a user supplied 
// lambda or function object. 
// Parameters:
//   Function: The lambda or function object from which to create a Windows 
//   Runtime asynchronous construct.
//   The lambda type is 
//     "[Capture]([ControllerType] AsyncController) -> ReturnType {}"
//   If you want to return an IAsyncAction^ or IAsyncOperation<TResult>^ type,
//   you need to set the ControllerType to IM2AsyncController^.
//   If you want to return an IAsyncActionWithProgress<TProgress>^ or 
//   IAsyncOperationWithProgress<TResult, TProgress>^ type, you need to set the
//   ControllerType to IM2AsyncControllerEx^.
// Return value:
//   An asynchronous construct represented by an IAsyncAction^, 
//   IAsyncActionWithProgress<TProgress>^, IAsyncOperation<TResult>^, or an
//   IAsyncOperationWithProgress<TResult, TProgress>^. The interface returned 
//   depends on the signature of the lambda passed into the function.
template<typename TFunction>
__declspec(noinline)
M2AsyncCreateInternal::M2AsyncGenerator<TFunction>^ M2AsyncCreate(
	const TFunction& Function)
{
	return ref new M2AsyncCreateInternal::M2AsyncGenerator<TFunction>(
		Function);
}

#endif

#endif // _M2_ASYNC_HELPERS_
