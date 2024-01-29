# include <Siv3D.hpp>
# include "Editor/NotificationAddon.hpp"
# include "Editor/Editor.hpp"

void Main()
{
	Window::Resize(1280, 720);

	Editor editor;
	editor.init();

	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	while (System::Update())
	{
		if (SimpleGUI::Button(U"normal", Vec2{ 600, 40 }, 160))
		{
			NotificationAddon::Show(U"normal");
		}

		if (SimpleGUI::Button(U"information", Vec2{ 600, 80 }, 160))
		{
			NotificationAddon::Show(U"information", NotificationAddon::Type::Information);
		}

		if (SimpleGUI::Button(U"question", Vec2{ 600, 120 }, 160))
		{
			NotificationAddon::Show(U"question", NotificationAddon::Type::Question);
		}

		if (SimpleGUI::Button(U"success", Vec2{ 600, 160 }, 160))
		{
			NotificationAddon::Show(U"success", NotificationAddon::Type::Success);
		}

		if (SimpleGUI::Button(U"warning", Vec2{ 600, 200 }, 160))
		{
			NotificationAddon::Show(U"warning", NotificationAddon::Type::Warning);
		}

		if (SimpleGUI::Button(U"failure", Vec2{ 600, 240 }, 160))
		{
			NotificationAddon::Show(U"failure", NotificationAddon::Type::Failure);
		}
	}
}
