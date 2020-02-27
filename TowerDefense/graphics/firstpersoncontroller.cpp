#include "pch.h"
#include "firstpersoncontroller.h"

namespace TowerDefense
{
	namespace gfx
	{
		FirstPersonController::FirstPersonController(mth::Positionf& target) :
			m_target(target),
			m_speed(1.0f),
			m_fastSpeed(5.0f),
			m_sensitivity(0.005f),
			m_inputFlags(0) {}
		void FirstPersonController::MouseDown(mth::float2 cursor)
		{
			m_inputFlags |= FLAG_MOUSE;
			m_cursor = cursor;
		}
		void FirstPersonController::MouseMove(mth::float2 cursor)
		{
			if (m_inputFlags & FLAG_MOUSE)
			{
				mth::float2 delta = (cursor - m_cursor) * m_sensitivity;
				m_cursor = cursor;
				m_target.rotation.x += delta.y;
				m_target.rotation.y += delta.x;
				if (m_target.rotation.x < -mth::pi * 0.5f)
					m_target.rotation.x = -mth::pi * 0.5f;
				if (m_target.rotation.x > mth::pi * 0.5f)
					m_target.rotation.x = mth::pi * 0.5f;
				m_target.rotation.y = std::fmodf(m_target.rotation.y + mth::pi, 2.0f * mth::pi) - mth::pi;
			}
		}
		void FirstPersonController::MouseUp()
		{
			m_inputFlags &= ~FLAG_MOUSE;
		}
		void FirstPersonController::KeyDown(Windows::System::VirtualKey key)
		{
			switch (key)
			{
			case Windows::System::VirtualKey::W:
				m_inputFlags |= FLAG_W;
				break;
			case Windows::System::VirtualKey::A:
				m_inputFlags |= FLAG_A;
				break;
			case Windows::System::VirtualKey::S:
				m_inputFlags |= FLAG_S;
				break;
			case Windows::System::VirtualKey::D:
				m_inputFlags |= FLAG_D;
				break;
			case Windows::System::VirtualKey::Space:
				m_inputFlags |= FLAG_SPACE;
				break;
			case Windows::System::VirtualKey::Control:
				m_inputFlags |= FLAG_CTRL;
				break;
			case Windows::System::VirtualKey::Shift:
				m_inputFlags |= FLAG_SHIFT;
				break;
			}
		}
		void FirstPersonController::KeyUp(Windows::System::VirtualKey key)
		{
			switch (key)
			{
			case Windows::System::VirtualKey::W:
				m_inputFlags &= ~FLAG_W;
				break;
			case Windows::System::VirtualKey::A:
				m_inputFlags &= ~FLAG_A;
				break;
			case Windows::System::VirtualKey::S:
				m_inputFlags &= ~FLAG_S;
				break;
			case Windows::System::VirtualKey::D:
				m_inputFlags &= ~FLAG_D;
				break;
			case Windows::System::VirtualKey::Space:
				m_inputFlags &= ~FLAG_SPACE;
				break;
			case Windows::System::VirtualKey::Control:
				m_inputFlags &= ~FLAG_CTRL;
				break;
			case Windows::System::VirtualKey::Shift:
				m_inputFlags &= ~FLAG_SHIFT;
				break;
			}
		}
		void FirstPersonController::Update(float deltaTime)
		{
			float speed = ((m_inputFlags & FLAG_SHIFT) ? m_fastSpeed : m_speed) * deltaTime;

			if (m_inputFlags & FLAG_W)
				m_target.MoveForward(speed);
			if (m_inputFlags & FLAG_S)
				m_target.MoveBackward(speed);
			if (m_inputFlags & FLAG_A)
				m_target.MoveLeft(speed);
			if (m_inputFlags & FLAG_D)
				m_target.MoveRight(speed);
			if (m_inputFlags & FLAG_SPACE)
				m_target.MoveUp(speed);
			if (m_inputFlags & FLAG_CTRL)
				m_target.MoveDown(speed);
		}
	}
}