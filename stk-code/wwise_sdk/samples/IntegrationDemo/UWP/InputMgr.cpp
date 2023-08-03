/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2020 Audiokinetic Inc.
*******************************************************************************/

// InputMgr.cpp
/// \file 
/// Defines the methods declared in InputMgr.h

#include "stdafx.h"

#include "InputMgr.h"

InputMgr::InputMgr()
	: m_pUInput( NULL )
{
}

InputMgr::~InputMgr()
{
}

bool InputMgr::Init(
	void* in_pParam,
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount
)
{
	
	m_pUInput = new UniversalInput;

	m_pUInput->AddDevice(0, UGDeviceType_KEYBOARD);

	// Register for gamepad added and removed events.
	Gamepad::GamepadAdded += ref new  Windows::Foundation::EventHandler<Gamepad^>([=](Platform::Object^, Gamepad^ args)
	{
		OnGamepadConnected(args);
	});
	Gamepad::GamepadRemoved += ref new  Windows::Foundation::EventHandler<Gamepad^>([=](Platform::Object^, Gamepad^ args)
	{
		OnGamepadDisconnected(args);
	});

	// Add all gamepads already connected
	for (unsigned int i = 0; i < Gamepad::Gamepads->Size; i++)
	{
		Gamepad^ ref = Gamepad::Gamepads->GetAt(i);
		OnGamepadConnected(ref);
	}

	return true;
}

UniversalInput* InputMgr::UniversalInputAdapter() const
{
	return m_pUInput;
}

void InputMgr::Update()
{
	// Keyboard index always 0.
	TranslateKeyboard( 0 );

	for ( int i = 0; i < MAX_INPUT; i++ )
	{
		TranslateInput( NULL, i );
	}
}

void InputMgr::Release()
{
	if ( m_pUInput )
	{
		delete m_pUInput;
		m_pUInput = NULL;
	}
}

void InputMgr::TranslateKeyboard( int in_iPlayerIndex )
{
	UGBtnState iState = 0;	// Returned button state value
	bool bConnected = true;
	UGStickState joysticks[2];
	memset(joysticks, 0, sizeof(joysticks));


	Windows::Devices::Input::KeyboardCapabilities ^keyboardCapabilities = ref new Windows::Devices::Input::KeyboardCapabilities();
	if (keyboardCapabilities->KeyboardPresent) // Checks if keyboard is connected.
	{
		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::Up)
			|| Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::W))
		{
			iState |= UG_DPAD_UP;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::Down)
			|| Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::S))
		{
			iState |= UG_DPAD_DOWN;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::Left)
			|| Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::A))
		{
			iState |= UG_DPAD_LEFT;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::Right)
			|| Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::D))
		{
			iState |= UG_DPAD_RIGHT;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::Enter))
		{
			iState |= UG_BUTTON1;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::Escape))
		{
			iState |= UG_BUTTON2;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::Q))
		{
			iState |= UG_BUTTON3;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::E))
		{
			iState |= UG_BUTTON4;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::LeftShift))
		{
			iState |= UG_BUTTON5;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::R))
		{
			iState |= UG_BUTTON6;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::F1))
		{
			iState |= UG_BUTTON7;
		}

		if (Windows::UI::Core::CoreVirtualKeyStates::Down == Windows::UI::Core::CoreWindow::GetForCurrentThread()->GetAsyncKeyState(Windows::System::VirtualKey::Menu))
		{
			// If Alt is pressed.
			iState |= UG_OS_BUTTON;
		}

		m_pUInput->SetState( (AkUInt16) in_iPlayerIndex, bConnected, iState, joysticks );
	}
}

void InputMgr::TranslateInput( void * in_pad, int in_iPlayerIndex )
{
	GamepadReading reading;
	UGBtnState iState = 0;
	bool bConnected = false;

	UGStickState joysticks[2];
	memset( joysticks, 0, sizeof( joysticks ) );

	m_Lock.Lock();
	Gamepad^ gp = m_Players[in_iPlayerIndex];
	if (gp)
	{
		bConnected = true;

		const UniversalGamepad * ugp = m_pUInput->GetGamepad(in_iPlayerIndex + 1);
		AkDeviceID deviceId = AK::GetDeviceIDFromGamepad(gp);
		if (ugp && ugp->GetDeviceID() != deviceId)
		{
			m_pUInput->RemoveDevice(in_iPlayerIndex + 1);
			ugp = nullptr;
		}
		if (!ugp)
			m_pUInput->AddDevice(in_iPlayerIndex + 1, UGDeviceType_GAMEPAD, deviceId);

		reading = gp->GetCurrentReading();
	}

	m_Lock.Unlock();

	if (bConnected)
	{
		GamepadButtons wButtons = reading.Buttons;
		if (GamepadButtons::DPadUp == (wButtons & GamepadButtons::DPadUp))
		{
			iState |= UG_DPAD_UP;
		}
		if (GamepadButtons::DPadDown == (wButtons & GamepadButtons::DPadDown))
		{
			iState |= UG_DPAD_DOWN;
		}
		if (GamepadButtons::DPadLeft == (wButtons & GamepadButtons::DPadLeft))
		{
			iState |= UG_DPAD_LEFT;
		}
		if (GamepadButtons::DPadRight == (wButtons & GamepadButtons::DPadRight))
		{
			iState |= UG_DPAD_RIGHT;
		}
		if (GamepadButtons::A == (wButtons & GamepadButtons::A))
		{
			iState |= UG_BUTTON1;
		}
		if (GamepadButtons::B == (wButtons & GamepadButtons::B))
		{
			iState |= UG_BUTTON2;
		}
		if (GamepadButtons::X == (wButtons & GamepadButtons::X))
		{
			iState |= UG_BUTTON3;
		}
		if (GamepadButtons::Y == (wButtons & GamepadButtons::Y))
		{
			iState |= UG_BUTTON4;
		}
		if (GamepadButtons::LeftShoulder == (wButtons & GamepadButtons::LeftShoulder))
		{
			iState |= UG_BUTTON5;
		}
		if (GamepadButtons::RightShoulder == (wButtons & GamepadButtons::RightShoulder))
		{
			iState |= UG_BUTTON6;
		}
		if (GamepadButtons::Menu == (wButtons & GamepadButtons::Menu))
		{
			iState |= UG_BUTTON7;
		}
		if (GamepadButtons::View == (wButtons & GamepadButtons::View))
		{
			iState |= UG_BUTTON8;
		}
		if (GamepadButtons::LeftThumbstick == (wButtons & GamepadButtons::LeftThumbstick))
		{
			iState |= UG_BUTTON9;
		}
		if (GamepadButtons::RightThumbstick == (wButtons & GamepadButtons::RightThumbstick))
		{
			iState |= UG_BUTTON10;
		}

		joysticks[UG_STICKLEFT].x = reading.LeftThumbstickX;
		joysticks[UG_STICKLEFT].y = reading.LeftThumbstickY;
		joysticks[UG_STICKRIGHT].x = reading.RightThumbstickX;
		joysticks[UG_STICKRIGHT].y = reading.RightThumbstickY;
	}

	m_pUInput->SetState( in_iPlayerIndex + 1, bConnected, iState, joysticks );
}

void InputMgr::OnGamepadConnected(Gamepad^ gp)
{
	m_Lock.Lock();

	AkInt16 firstAvailableIndex = -1;
	bool bFound = false;
	for (auto i = 0; i < MAX_INPUT; i++)
	{
		if (m_Players[i] == gp)
		{
			bFound = true;
		}
		else if (m_Players[i] == nullptr && firstAvailableIndex < 0) {
			firstAvailableIndex = i;
		}
	}

	if (!bFound && firstAvailableIndex >= 0)
	{
		m_Players[firstAvailableIndex] = gp;
	}

	m_Lock.Unlock();
}

void InputMgr::OnGamepadDisconnected(Gamepad^ gp)
{
	m_Lock.Lock();

	for (auto i = 0; i < MAX_INPUT; i++)
	{
		if (m_Players[i] == gp)
		{
			m_Players[i] = nullptr;
		}
	}

	m_Lock.Unlock();
}