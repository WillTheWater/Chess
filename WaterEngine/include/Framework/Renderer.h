#pragma once
#include "Framework/Core.h"
#include <SFML/Graphics.hpp> 

namespace we
{
	class Renderer
	{
	public:
		explicit Renderer(sf::RenderWindow& Window);
		~Renderer() = default;

		void Clear();
		void Display();

		template<typename DrawableType>
		void Draw(const DrawableType& Drawable);

		sf::RenderWindow& GetRenderWindow() const { return Window; }
		sf::Vector2u GetViewportSize() const 
		{ 
			sf::Vector2f size = Window.getView().getSize();
			return sf::Vector2u(static_cast<unsigned int>(size.x), static_cast<unsigned int>(size.y));
		}

	private:
		sf::RenderWindow& Window;
	};
	template<typename DrawableType>
	inline void Renderer::Draw(const DrawableType& Drawable)
	{
		static_assert(std::is_base_of_v<sf::Drawable, DrawableType>,
			"Renderer::Draw only accepts SFML drawable types");
		Window.draw(Drawable);
	}
}