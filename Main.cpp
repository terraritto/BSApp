# include <Siv3D.hpp>
#include "SceneManager.h"

void Main()
{
	Scene::SetBackground(ColorF{ 0.8, 0.9, 1.0 });

	::SceneManager manager;

	while (System::Update())
	{
		manager.Update();
	}
}
