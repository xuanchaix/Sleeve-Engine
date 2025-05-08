#include "Core/Clock.h"
#include "Core/Time.h"
#include "Math/MathUtils.h"

Clock::Clock()
{
	if (s_systemClock != nullptr) {
		s_systemClock->AddChild( this );
		m_parent = s_systemClock;
	}
}

Clock::Clock( Clock* parentClock )
{
	parentClock->AddChild( this );
	m_parent = parentClock;
}

Clock::~Clock()
{
	if (m_parent) {
		m_parent->RemoveChild( this );
	}
}

void Clock::Reset()
{
	m_totalSeconds = 0.0;
	m_deltaSeconds = 0.0;
	m_frameCount = 0;
	m_lastFrameTimeSeconds = 0.0;
	m_timeScale = 1.f;
}

void Clock::Pause()
{
	m_isPaused = true;
}

void Clock::UnPause()
{
	m_isPaused = false;
}

void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
}

double Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}

double Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}

void Clock::SetTimeScale( float newTimeScale )
{
	m_timeScale = newTimeScale;
}

double Clock::GetTimeScale() const
{
	return m_timeScale;
}

uint64_t Clock::GetFrameCount() const
{
	return m_frameCount;
}

void Clock::TickSystemClock()
{
	s_systemClock->Tick();
}

void Clock::SetSystemMaxDeltaSeconds( double maxDeltaSeconds )
{
	s_systemClock->m_maxDeltaSeconds = maxDeltaSeconds;
}

double Clock::GetSystemMaxDeltaSeconds()
{
	return s_systemClock->m_maxDeltaSeconds;
}

Clock* Clock::GetSystemClock()
{
	return s_systemClock;
}

void Clock::Tick()
{
	double curSeconds = GetCurrentTimeSeconds();
	double deltaSeconds = curSeconds - m_lastFrameTimeSeconds;
	m_lastFrameTimeSeconds = curSeconds;
	deltaSeconds = GetClamped( deltaSeconds, 0.0, m_maxDeltaSeconds );
	Advance( deltaSeconds );
}

void Clock::Advance( double deltaSeconds )
{
	if (m_isPaused) {
		deltaSeconds = 0.0;
	}
	m_deltaSeconds = deltaSeconds * (double)m_timeScale;
	m_totalSeconds += m_deltaSeconds;
	++m_frameCount;
	for (auto clock : m_children) {
		clock->Advance( m_deltaSeconds );
	}
}

void Clock::AddChild( Clock* clock )
{
	m_children.push_back( clock );
}

void Clock::RemoveChild( Clock* clock )
{
	for (size_t i = 0; i < m_children.size(); ++i) {
		if (m_children[i] == clock) {
			m_children.erase( m_children.begin() + i );
			return;
		}
	}
}

Clock* Clock::s_systemClock = new Clock();


