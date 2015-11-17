/*
 * @file program_options.hpp
 * @brief Header of \ref ProgramOptions class.
 * @author Federico Conte (draxent)
 */

#ifndef INCLUDE_PROGRAM_OPTIONS_HPP_
#define INCLUDE_PROGRAM_OPTIONS_HPP_

#include <iostream>
#include <assert.h>
#include <algorithm>

/// This class mangages the program options.
class ProgramOptions
{
public:
	/// Initializes a new instance of the \ref ProgramOptions class.
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
	int get_int( const std::string& option, int default_value ) const;

	/**
	 * Retrieve the integer value of one of the two options we want to retrieve.
	 * @param option1, option2	options to looking for and to retrieve.
	 * @param default_value		value to return if the option value is <code>NULL</code>.
	 * @return					the option integer value or the <em>default_value</em>.
	 */
	int get_int( const std::string& option1, const std::string& option2, int default_value ) const;

private:
	int argc;
	char** argv;
};

#endif /* INCLUDE_PROGRAM_OPTIONS_HPP_ */
