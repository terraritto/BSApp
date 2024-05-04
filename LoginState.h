#pragma once
#include <Windows.h>
#pragma comment( lib, "shell32.lib" )
#include "StateBase.h"
#include <Siv3D.hpp>
#include "GlobalInfo.h"

class LoginState : public StateBase
{
public:
	enum class LoginProcess
	{
		Input,
		Load,
		Loading,
		Next
	};

	LoginState() : StateBase() { m_state = State::Timeline; m_loginState = LoginProcess::Input; }

	void Update() override
	{

		switch (m_loginState)
		{
		case LoginProcess::Input:
			{
				m_font(U"Login画面").draw(50, 10, 10, Palette::Black);

				// Id
				m_font(U"Id:").draw(30, 10, 140, Palette::Black);
				SimpleGUI::TextBox(m_id, Vec2{ 100, 140 });

				// pass
				m_font(U"pass:").draw(30, 10, 200, Palette::Black);
				SimpleGUI::TextBox(m_pass, Vec2{ 100, 200 });

				if (SimpleGUI::Button(U"Login!!", Vec2{ 320, 200 }))
				{
					m_loginState = LoginProcess::Load;
				}

				break;
			}

		case LoginProcess::Load:
			{
				// 読み込み開始
				String loginCommand;
				auto dir = FileSystem::CurrentDirectory() + U"py/login.bat";
				loginCommand = U" " + m_id.text + U" " + m_pass.text;
				dir = U"/c" + dir + loginCommand; // コマンドと結合していく
				ShellExecute(NULL, L"open", L"cmd.exe", dir.toWstr().c_str(), L"", SW_HIDE);

				m_timer.set(5s); m_timer.start();
				m_loginState = LoginProcess::Loading;
				break;
			}

		case LoginProcess::Loading:
			{
				m_font(U"Loading...").draw(50, 10, 10, Palette::Black);

				if(!m_timer.reachedZero())
				{
					break;
				}

				const String loginFile = U"py/loginInfo.txt";
				m_loginState = FileSystem::Exists(loginFile) ? LoginProcess::Next : LoginProcess::Input;
				FileSystem::Remove(loginFile); // 削除しておく
				break;
			}

		case LoginProcess::Next:
			{
				// 登録
				ID = m_id.text;
				Pass = m_pass.text;

				m_isNext = true;
				break;
			}
		}
	}

protected:
	LoginProcess m_loginState;

	TextEditState m_id{U".bsky.social"};
	TextEditState m_pass;
	Font m_font{ FontMethod::MSDF, 48 };

	Timer m_timer;
};
