#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

int main( const int argc, const char* argv[] )
{
	sf::RenderWindow window{ sf::VideoMode{ 800, 600 }, "0hgame 2015 by mrwonko", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings{ 0, 0, 0, 2, 0, 0 } };
	while( window.isOpen() )
	{
		// fill up render queue
		window.clear();

		// handle events
		sf::Event ev;
		while( window.pollEvent( ev ) )
		{
			switch( ev.type )
			{
			case sf::Event::Closed:
				window.close();
				break;
			default:
				break;
			}
		}
		if( !window.isOpen() )
		{
			break;
		}

		// handle gameplay

		// swap buffers
		window.display();
	}
	return 0;
}
