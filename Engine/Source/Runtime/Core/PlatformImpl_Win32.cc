//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename>
// Date : <Date>
// Comments :
//
//---------------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "PlatformImpl.h"



DirectX::XMFLOAT3 dmax(const DirectX::XMFLOAT3& a,const DirectX::XMFLOAT3& b)
{
	DirectX::XMFLOAT3 r;

	r.x = std::max(a.x,b.x);
	r.y = std::max(a.y,b.y);
	r.z = std::max(a.z,b.z);

	return r;
}

DirectX::XMFLOAT3 dmin(const DirectX::XMFLOAT3& a,const DirectX::XMFLOAT3& b)
{
	DirectX::XMFLOAT3 r;

	r.x = std::min(a.x,b.x);
	r.y = std::min(a.y,b.y);
	r.z = std::min(a.z,b.z);

	return r;
}

bool dequal(const DirectX::XMFLOAT4& a,const DirectX::XMFLOAT4& b)
{
	auto aa = DirectX::XMLoadFloat4(&a);
	auto bb = DirectX::XMLoadFloat4(&b);

	return DirectX::XMVector4Equal(aa,bb);
}

YumeEngine::YumeString XMVectorToString(const DirectX::XMVECTOR& v)
{
	YumeEngine::YumeString str;
	str.AppendWithFormat("%f %f %f %f",v.m128_f32[0],v.m128_f32[1],v.m128_f32[2],v.m128_f32[3]);
	return str;
}

int GetWndMouseModifiers(WPARAM wparam) {
	int modifiers = 0;
	if(wparam & MK_CONTROL)
		modifiers |= EVENTFLAG_CONTROL_DOWN;
	if(wparam & MK_SHIFT)
		modifiers |= EVENTFLAG_SHIFT_DOWN;
	if(IsKeyDown(VK_MENU))
		modifiers |= EVENTFLAG_ALT_DOWN;
	if(wparam & MK_LBUTTON)
		modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
	if(wparam & MK_MBUTTON)
		modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
	if(wparam & MK_RBUTTON)
		modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

	// Low bit set from GetKeyState indicates "toggled".
	if(::GetKeyState(VK_NUMLOCK) & 1)
		modifiers |= EVENTFLAG_NUM_LOCK_ON;
	if(::GetKeyState(VK_CAPITAL) & 1)
		modifiers |= EVENTFLAG_CAPS_LOCK_ON;
	return modifiers;
}

int GetWndKeyboardModifiers(WPARAM wparam,LPARAM lparam) {
	int modifiers = 0;
	if(IsKeyDown(VK_SHIFT))
		modifiers |= EVENTFLAG_SHIFT_DOWN;
	if(IsKeyDown(VK_CONTROL))
		modifiers |= EVENTFLAG_CONTROL_DOWN;
	if(IsKeyDown(VK_MENU))
		modifiers |= EVENTFLAG_ALT_DOWN;

	// Low bit set from GetKeyState indicates "toggled".
	if(::GetKeyState(VK_NUMLOCK) & 1)
		modifiers |= EVENTFLAG_NUM_LOCK_ON;
	if(::GetKeyState(VK_CAPITAL) & 1)
		modifiers |= EVENTFLAG_CAPS_LOCK_ON;

	switch(wparam) {
	case VK_RETURN:
		if((lparam >> 16) & KF_EXTENDED)
			modifiers |= EVENTFLAG_IS_KEY_PAD;
		break;
	case VK_INSERT:
	case VK_DELETE:
	case VK_HOME:
	case VK_END:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
		if(!((lparam >> 16) & KF_EXTENDED))
			modifiers |= EVENTFLAG_IS_KEY_PAD;
		break;
	case VK_NUMLOCK:
	case VK_NUMPAD0:
	case VK_NUMPAD1:
	case VK_NUMPAD2:
	case VK_NUMPAD3:
	case VK_NUMPAD4:
	case VK_NUMPAD5:
	case VK_NUMPAD6:
	case VK_NUMPAD7:
	case VK_NUMPAD8:
	case VK_NUMPAD9:
	case VK_DIVIDE:
	case VK_MULTIPLY:
	case VK_SUBTRACT:
	case VK_ADD:
	case VK_DECIMAL:
	case VK_CLEAR:
		modifiers |= EVENTFLAG_IS_KEY_PAD;
		break;
	case VK_SHIFT:
		if(IsKeyDown(VK_LSHIFT))
			modifiers |= EVENTFLAG_IS_LEFT;
		else if(IsKeyDown(VK_RSHIFT))
			modifiers |= EVENTFLAG_IS_RIGHT;
		break;
	case VK_CONTROL:
		if(IsKeyDown(VK_LCONTROL))
			modifiers |= EVENTFLAG_IS_LEFT;
		else if(IsKeyDown(VK_RCONTROL))
			modifiers |= EVENTFLAG_IS_RIGHT;
		break;
	case VK_MENU:
		if(IsKeyDown(VK_LMENU))
			modifiers |= EVENTFLAG_IS_LEFT;
		else if(IsKeyDown(VK_RMENU))
			modifiers |= EVENTFLAG_IS_RIGHT;
		break;
	case VK_LWIN:
		modifiers |= EVENTFLAG_IS_LEFT;
		break;
	case VK_RWIN:
		modifiers |= EVENTFLAG_IS_RIGHT;
		break;
	}
	return modifiers;
}


bool IsKeyDown(WPARAM wparam) {
	return (GetKeyState(wparam) & 0x8000) != 0;
}

float GetDeviceScaleFactor() {
	static float scale_factor = 1.0;
	static bool initialized = false;

	if(!initialized) {
		// This value is safe to cache for the life time of the app since the user
		// must logout to change the DPI setting. This value also applies to all
		// screens.
		HDC screen_dc = ::GetDC(NULL);
		int dpi_x = GetDeviceCaps(screen_dc,LOGPIXELSX);
		scale_factor = static_cast<float>(dpi_x) / 96.0f;
		::ReleaseDC(NULL,screen_dc);
		initialized = true;
	}

	return scale_factor;
}