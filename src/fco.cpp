/**
 *	@file fco.cpp
 *  @brief Implementation of \ref FCO class.
 *  @author Federico Conte (draxent)
 */

#include "fco.hpp"

namespace fco
{
	FCO::FCO()
	{
		this->properties = NULL;
		this->init();
	}

	FCO::FCO( Style style )
	{
		this->properties = NULL;
		this->init();
		this->add( style );
	}

	FCO::FCO( std::initializer_list<Style> args )
	{
		this->properties = NULL;
		this->init();
		for ( Style elem : args )
			this->add( elem );
	}

	void FCO::add( Style style )
	{
		int index = FCO::get_index( style );
		this->properties[index] = style;
	}

	void FCO::add( std::initializer_list<Style> args )
	{
		for ( Style elem : args )
			this->add( elem );
	}

	void FCO::remove( Style style )
	{
		int index = FCO::get_index( style );
		this->properties[index] = -1;
	}

	void FCO::remove( std::initializer_list<Style> args )
	{
		for ( Style elem : args )
			this->remove( elem );
	}

	std::string FCO::apply( const char* text ) const
	{
		int num_properties = 0;
		std::stringstream ss;

		// Check if some properties is present.
		for ( int i = 0; i < FCO::max_num_properties; i++ )
			if ( this->properties[i] != -1 )
				num_properties++;

		if ( num_properties > 0 )
		{
			bool first = true;

			ss << "\033[";
			for ( int i = 0; i < FCO::max_num_properties; i++ )
			{
				if ( this->properties[i] != -1 )
				{
					if ( !first ) ss << ";";
					ss << this->properties[i];
					first = false;
				}
			}

			ss << "m";
			ss << text;
			ss << "\033[0m";
		}
		else
			ss << text;

		return ss.str();
	}

	FCO FCO::operator+(const FCO& fco) const
	{
		FCO result;
		for ( int i = 0; i < FCO::max_num_properties; i++ )
		{
			if ( this->properties[i] != -1 )
				result.properties[i] = this->properties[i];
			else
				result.properties[i] = fco.properties[i];
		}
		return result;
	}

	void FCO::init()
	{
		// Initialize properties
		this->properties = new int[7];
		for ( int i = 0; i < FCO::max_num_properties; i++ )
			this->properties[i] = -1;
	}

	int FCO::get_index( Style style )
	{
		switch ( style )
		{
			default: case BOLD: return 0;
			case ITALIC: return 1;
			case UNDERLINE: return 2;
			case SELECTED: return 3;
			case STRIKETHROUGH: return 4;
			case FG_BLACK: case FG_RED: case FG_GREEN: case FG_YELLOW: case FG_BLUE: case FG_MAGENTA:
			case FG_CYAN: case FG_GRAY: case FG_DARK_GRAY: case FG_LIGHT_RED: case FG_LIGHT_GREEN:
			case FG_LIGHT_YELLOW: case FG_LIGHT_BLUE: case FG_LIGHT_MAGENTA: case FG_LIGHT_CYAN: case FG_WHITE:
				return 5;
			case BG_BLACK: case BG_RED: case BG_GREEN: case BG_YELLOW: case BG_BLUE: case BG_MAGENTA:
			case BG_CYAN: case BG_GRAY: case BG_DARK_GRAY: case BG_LIGHT_RED: case BG_LIGHT_GREEN:
			case BG_LIGHT_YELLOW: case BG_LIGHT_BLUE: case BG_LIGHT_MAGENTA: case BG_LIGHT_CYAN: case BG_WHITE:
				return 6;
		}
	}

	FCO::~FCO()
	{
		delete[] this->properties;
		this->properties = NULL;
	}
}
