/**
 *	@file program_options.hpp
 *	@brief Header of \see ProgramOptions class.
 *	@author Federico Conte (draxent)
 *
 *	Copyright 2015 Federico Conte
 *	https://github.com/Draxent/GameOfLife
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *	http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 */


#ifndef GAMEOFLIFE_PROGRAM_OPTIONS_H
#define GAMEOFLIFE_PROGRAM_OPTIONS_H

#include <iostream>
#include <assert.h>
#include <algorithm>

/// This class mangages the program options.
class ProgramOptions
{
public:
	/// Initializes a new instance of the \see ProgramOptions class.
	ProgramOptions( int argc, char** argv );

	/**
	 * Check if the option exists among the program options.
	 * @param option	option to looking for.
	 * @return			<code>true</code> if the option is found, <code>false</code> otherwise.
	 */
	bool exists( const std::string& option ) const;

	/**
	 * Check if one of the two options exists among the program options.
	 * @param option1, option2	options to looking for.
	 * @return					<code>true</code> if the option is found, <code>false</code> otherwise.
	 */
	bool exists( const std::string& option1, const std::string& option2 ) const;

	/**
	 * Retrieve the string value of the option we want to retrieve.
	 * @param option	option to looking for and to retrieve.
	 * @return			the option string value or <code>NULL</code> if the option is not found.
	 */
	char* get( const std::string& option ) const;

	/**
	 * Retrieve the string value of one of the two options we want to retrieve.
	 * @param option1, option2	options to looking for and to retrieve.
	 * @return					the option string value or <code>NULL</code> if the option is not found.
	 */
	char* get( const std::string& option1, const std::string& option2 ) const;

	/**
	 * Retrieve the integer value of the option we want to retrieve.
	 * @param option			option to looking for and to retrieve.
	 * @param default_value		value to return if the option value is <code>NULL</code>.
	 * @return					the option integer value or the <em>default_value</em>.
	 */
	size_t get_number( const std::string& option, size_t default_value ) const;

	/**
	 * Retrieve the integer value of one of the two options we want to retrieve.
	 * @param option1, option2	options to looking for and to retrieve.
	 * @param default_value		value to return if the option value is <code>NULL</code>.
	 * @return					the option integer value or the <em>default_value</em>.
	 */
	size_t get_number( const std::string& option1, const std::string& option2, size_t default_value ) const;

private:
	int argc;
	char** argv;
};

#endif //GAMEOFLIFE_PROGRAM_OPTIONS_H
