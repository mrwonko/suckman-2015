#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

#include <array>
#include <string>
#include <random>
#include <algorithm>

std::string getMapName()
{
	// originally customizable using mapname.txt
	return "maps/default.png";
}

sf::Vector2f constrain( sf::Vector2f vec, const sf::FloatRect& bounds )
{
	if( vec.x < bounds.left )
	{
		vec.x = bounds.left;
	}
	else if( vec.x > bounds.left + bounds.width )
	{
		vec.x = bounds.left + bounds.width;
	}
	if( vec.y < bounds.top )
	{
		vec.y = bounds.top;
	}
	else if( vec.y > bounds.top + bounds.height )
	{
		vec.y = bounds.top + bounds.height;
	}
	return vec;
}

int main( const int argc, const char* argv[] )
{

	sf::RenderWindow window{ sf::VideoMode{ 800, 600 }, "Suckman 2015 by mrwonko", sf::Style::Close | sf::Style::Titlebar, sf::ContextSettings{ 0, 0, 0, 2, 0, 0 } };
	sf::Clock clock;

	std::random_device random_device;
	std::mt19937 random_engine{ random_device() };

	constexpr int cellSize = 50;
	constexpr int width = 14;
	constexpr int height = 10;
	const sf::FloatRect bounds{ 0, 0, width - 1, height - 1 };
	constexpr float itemSize = .2f;
	constexpr float playerSize = .8f;
	constexpr float playerSpeed = 5; // units per second
	auto randomLocation = [ width, height, &random_engine ]() -> sf::Vector2f
	{
		std::uniform_int_distribution<> x_distribution{ 0, width - 1 };
		std::uniform_int_distribution<> y_distribution{ 0, height - 1 };
		return{ 
			static_cast< float >( x_distribution( random_engine ) ),
			static_cast< float >( y_distribution( random_engine ) )
		};
	};
	std::array< sf::RectangleShape, 2 > items{
		sf::RectangleShape{ { itemSize, itemSize } },
		sf::RectangleShape{ { itemSize, itemSize } }
	};
	// new random location; not one of an existing item
	auto randomItemLocation = [ &items, &randomLocation ]() -> sf::Vector2f
	{
		sf::Vector2f position;
		// FIXME: HACKYish
		do
		{
			position = randomLocation();
		}
		while( std::find_if( items.begin(), items.end(), [ &position ]( const sf::RectangleShape& rect )
		{
			return rect.getPosition() == position;
		} ) != items.end() );
		return position;
	};
	for( auto& item : items )
	{
		item.setPosition( randomItemLocation() );
		item.setFillColor( sf::Color::Red );
		item.setOrigin( -( 1 - itemSize ) / 2, -( 1 - itemSize ) / 2 );
	}

	sf::RectangleShape player{ { playerSize, playerSize } };
	player.setFillColor( sf::Color::Blue );
	player.setOrigin( -( 1 - playerSize ) / 2, -( 1 - playerSize ) / 2 );

	sf::Texture backgroundImage;
	backgroundImage.loadFromFile( getMapName() );

	sf::Sprite background{ backgroundImage };
	{
		const auto bgBounds = background.getLocalBounds();
		background.setScale( width / bgBounds.width, height / bgBounds.height );
	}

	std::size_t points{ 0 };
	sf::View fieldView{ { 0, 0, static_cast< float >( width ), static_cast< float >( height ) } };
	//fieldView.move( -50, -50 );

	while( window.isOpen() )
	{
		// fill up render queue
		window.clear();
		{
			window.setView( fieldView );
			window.draw( background );
			for( const auto& item : items )
			{
				window.draw( item );
			}
			window.draw( player );
		}

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
		auto deltaT = clock.getElapsedTime().asSeconds();
		clock.restart();
		{
			// handle player movement
			{
				auto playerPos = player.getPosition();
				if( sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) )
				{
					playerPos.x -= playerSpeed * deltaT;
				}
				if( sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) )
				{
					playerPos.x += playerSpeed * deltaT;
				}
				if( sf::Keyboard::isKeyPressed( sf::Keyboard::Up ) )
				{
					playerPos.y -= playerSpeed * deltaT;
				}
				if( sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) )
				{
					playerPos.y += playerSpeed * deltaT;
				}
				player.setPosition( constrain( playerPos, bounds ) );
			}
			// collision checks
			for( auto& item : items )
			{
				if( static_cast< int >( item.getPosition().x ) == static_cast< int >( player.getPosition().x + .5f )
					&& static_cast< int >( item.getPosition().y ) == static_cast< int >( player.getPosition().y + .5f ) )
				{
					item.setPosition( randomItemLocation() );
					// never 2 items at the same location
					break;
				}
			}
		}

		// swap buffers
		window.display();
	}
	return 0;
}
