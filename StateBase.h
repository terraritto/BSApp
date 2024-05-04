#pragma once

enum class State
{
	Login,
	Timeline,

	Invalid
};

class StateBase
{
public:
	StateBase() : m_isNext(false){}

	virtual void Update() = 0;

	void Reset() { m_isNext = false; }

	bool IsNext() const { return m_isNext; }

	State NextState() const { return m_state; }

protected:
	bool m_isNext;
	State m_state;
};
