#pragma once
#include <Siv3D.hpp>
#include "LoginState.h"
#include "TimelineState.h"

class SceneManager
{
public:
	SceneManager()
	{
		m_stateMap.emplace(State::Login, std::make_unique<LoginState>());
		m_stateMap.emplace(State::Timeline, std::make_unique<TimelineState>());

		m_currentState = State::Login;
	}

	void Update()
	{
		auto& state = m_stateMap[m_currentState];
		state->Update();

		if (state->IsNext())
		{
			state->Reset();
			m_currentState = state->NextState();
		}
	}

private:
	std::map<State, std::unique_ptr<StateBase>> m_stateMap;
	State m_currentState;
};
