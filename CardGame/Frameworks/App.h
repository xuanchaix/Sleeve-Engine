#pragma once
#include "Engine/Core/EngineFwdMinor.h"

class App {
public:
	void Initialize();
	void Run();
	void Exit();

	void BeginFrame();
	void RunFrame();
	void EndFrame();

	void SetMaxFrameRate( int maxFrameRate );
protected:

public:
};