#pragma once

enum class TimeState
{
	Stopped,
	Running,
	Paused
};

template <typename T>
class Timer
{
public:
	Timer() : startTicks(0), pauseTicks(0), state(TimeState::Stopped) { };

	// Controls
	void Start();
	void Pause();
	void Resume();
	void Stop();
	void Restart();

	T GetMilliseconds() const;
	T GetSeconds() const { return GetMilliseconds() / 1000; }

	bool IsPaused() const { return state == TimeState::Paused; }
	bool IsStarted() const { return state == TimeState::Running || state == TimeState::Paused; }
	bool IsRunning() const { return state == TimeState::Running; }

private:
	T startTicks;
	T pauseTicks;

	TimeState state;
};

using TimerF = Timer<float>;

template <typename T>
inline void Timer<T>::Start() 
{
	state = TimeState::Running;

	startTicks = static_cast<T>(SDL_GetTicks());
}


template <typename T>
inline void Timer<T>::Pause()
{
	if(state == TimeState::Running)
	{
		state = TimeState::Paused;

		pauseTicks = SDL_GetTicks() - startTicks;
		startTicks = 0;
	}
}


template <typename T>
inline void Timer<T>::Resume()
{
	if(state == TimeState::Paused)
	{
		state = TimeState::Running;

		startTicks = SDL_GetTicks() - pauseTicks;
		pauseTicks = 0;
	}
}


template <typename T>
inline void Timer<T>::Stop()
{
	if(state == TimeState::Running || state == TimeState::Paused)
	{
		state = TimeState::Stopped;

		startTicks = 0; 
		pauseTicks = 0;
	}
}


template <typename T>
inline void Timer<T>::Restart()
{ 
	state = TimeState::Running;

	pauseTicks = 0;
	startTicks = static_cast<T>(SDL_GetTicks());
}


template <typename T>
inline T Timer<T>::GetMilliseconds() const
{
	if (state == TimeState::Running)
		return SDL_GetTicks() - startTicks;

	else if (state == TimeState::Paused)
		return pauseTicks;

	else
		return 0;
}