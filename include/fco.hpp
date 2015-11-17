/*
 *	@file fco.hpp
 *  @brief Header of \ref FCO class.
 *  @author Federico Conte (draxent)
 *
 *	This implementation is inspired by : <br/>
 *  cout_col v1.0.2 <br/>
 *  https://github.com/tomveltmeijer/cout_col <br/>
 *  The MIT License (MIT) <br/>
 *  Copyright (c) 2015 Tom Veltmeijer <br/>
 *  http://opensource.org/licenses/MIT <br/>
 */

#ifndef INCLUDE_FCO_HPP_
#define INCLUDE_FCO_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <initializer_list>

namespace fco
{
	/// List of possible styles.
	enum Style
	{
		BOLD = 1, /**< bold style. */
		ITALIC = 3, /**< italic style. */
		UNDERLINE = 4, /**< underline style. */
		SELECTED = 7, /**< selected style. */
		STRIKETHROUGH = 9, /**< strikethrough style. */
		FG_BLACK = 30, /**< black foreground color. */
		FG_RED = 31, /**< red foreground color. */
		FG_GREEN = 32, /**< green foreground color. */
		FG_YELLOW = 33, /**< yellow foreground color. */
		FG_BLUE = 34, /**< blue foreground color. */
		FG_MAGENTA = 35, /**< magenta foreground color. */
		FG_CYAN = 36, /**< cyan foreground color. */
		FG_GRAY = 37, /**< gray foreground color. */
		FG_DARK_GRAY = 90, /**< dark gray foreground color. */
		FG_LIGHT_RED = 91, /**< light red foreground color. */
		FG_LIGHT_GREEN = 92, /**< light green foreground color. */
		FG_LIGHT_YELLOW = 93, /**< light yellow foreground color. */
		FG_LIGHT_BLUE = 94, /**< light blue foreground color. */
		FG_LIGHT_MAGENTA = 95, /**< light magenta foreground color. */
		FG_LIGHT_CYAN = 96, /**< light cyan foreground color. */
		FG_WHITE = 97, /**< white foreground color. */
		BG_BLACK = 40, /**< black background color. */
		BG_RED = 41, /**< red background color. */
		BG_GREEN = 42, /**< green background color. */
		BG_YELLOW = 43, /**< yellow background color. */
		BG_BLUE = 44, /**< blue background color. */
		BG_MAGENTA = 45, /**< magenta background color. */
		BG_CYAN = 46, /**< cyan background color. */
		BG_GRAY = 47, /**< grey background color. */
		BG_DARK_GRAY = 100, /**< dark grey background color. */
		BG_LIGHT_RED = 101, /**< light red background color. */
		BG_LIGHT_GREEN = 102, /**< light green background color. */
		BG_LIGHT_YELLOW = 103, /**< light yellow background color. */
		BG_LIGHT_BLUE = 104, /**< light blue background color. */
		BG_LIGHT_MAGENTA = 105, /**< light magenta background color. */
		BG_LIGHT_CYAN = 106, /**< light cyan background color. */
		BG_WHITE = 107 /**< white background color. */
	};

	/// This class allow the user to print formatted output on the console in a simple way.
	class FCO
	{
	public:

		///  Initializes a new instance of the \ref FCO (Formatted Console Output) class.
		FCO();

		/**
		* Initializes a new instance of the \ref FCO (Formatted Console Output) class.
		* @param style	style to add to the object.
		*/
		FCO( Style style );

		/**
		* Initializes a new instance of the \ref FCO (Formatted Console Output) class.
		* @param args	list of style elements to add to the object.
		*/
		FCO( std::initializer_list<Style> args );

		/**
		* Add a style to the object
		* @param style	the style to add.
		*/
		void add( Style style );

		/**
		* Add a list of style elements to the object
		* @param args	the styles list to add.
		*/
		void add( std::initializer_list<Style> args );

		/**
		* Remove a style from the object
		* @param style	the style to remove.
		*/
		void remove( Style style );

		/**
		* Remove a list of style elements from the object
		* @param args	the styles list to remove.
		*/
		void remove( std::initializer_list<Style> args );

		/**
		* Apply the chosen format to the given text.
		* @param text	text to which appply the chosen format.
		* @return		the formatted string ready to be output from the console.
		*/
		std::string apply( const char* text ) const;

		/**
		* Sum the stylistic properties of this object with the given object.
		* During the sum, this object has more priority than the given object.
		* @param fco	given \ref FCO object.
		* @return		a \ref FCO object with stylistic properties obtained from the sum explained above.
		*/
		FCO operator+(const FCO& fco) const;

		/// Destructor of the \ref FCO class.
		~FCO();

	private:
		int* properties;
		static const int max_num_properties = 7;

		// Init the <em>properties</em> array, allocating the appropriate space.
		void init();

		/**
		* Get the index of the <em>properties</em> array where to store this kind of <em>style</em>.
		* Each stylistic property has a predefined slot where it is stored.
		* In this way is easier verify if a property is already present and add/remove properties.
		* @param style	style we would like to store.
		* @return		the index of the <em>properties</em> array  where to store the style.
		*/
		static int get_index( Style style );
	};
}

#endif /* INCLUDE_FCO_HPP_ */
