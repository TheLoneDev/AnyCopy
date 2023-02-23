#include <iostream>
#include "Window.h"
int main()
{
	auto& window = AnyCopy::Window::GetWindow();

	if (!window.Init())
	{
		AnyCopy::Window::ShowMsg("Error initializing window");
		return -1;
	}

	window.Show();
	window.Run();

	return 0;
}