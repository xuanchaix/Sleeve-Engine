#pragma once
#include <cstdint>
#include <vector>

class Clock {
public:
	Clock();
	explicit Clock( Clock* parentClock );
	Clock( Clock const& clock ) = delete;
	~Clock();;

	void Reset();

	void Pause();
	void UnPause();
	void TogglePause();

	double GetDeltaSeconds() const;
	double GetTotalSeconds() const;

	void SetTimeScale( float newTimeScale );
	double GetTimeScale() const;

	uint64_t GetFrameCount() const;

	static void TickSystemClock();
	static void SetSystemMaxDeltaSeconds( double maxDeltaSeconds );
	static double GetSystemMaxDeltaSeconds();
	static Clock* GetSystemClock();
protected:
	void Tick();
	void Advance( double deltaSeconds );

	void AddChild( Clock* clock );
	void RemoveChild( Clock* clock );
protected:
	Clock* m_parent = nullptr;
	std::vector<Clock*> m_children;

	double m_deltaSeconds = 0.0;
	double m_lastFrameTimeSeconds = 0.0;
	double m_totalSeconds = 0.0;
	uint64_t m_frameCount = 0;
	double m_maxDeltaSeconds = 0.1;
	float m_timeScale = 1.f;
	bool m_isPaused = false;

	static Clock* s_systemClock;
};