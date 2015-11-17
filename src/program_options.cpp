/**
 *	@file program_options.cpp
 *  @brief Implementation of \ref ProgramOptions class.
 *  @author Federico Conte (draxent)
 */

#include "program_options.hpp"

ProgramOptions::ProgramOptions( int argc, char** argv )
{
	assert( argc > 0 && argv != NULL );
	this->argc = argc;
	this->argv = argv;
}

bool ProgramOptions::exists( const std::string& option ) const
{
	char** end = this->argv + this->argc;
	return ( std::find( this->argv, end, option ) != end );
}

bool ProgramOptions::exists( const std::string& option1, const std::string& option2 ) const
{
	bool exists_op1 = this->exists( option1 );
	bool exists_op2 = this->exists( option2 );
	return ( exists_op1 || exists_op2 );
}

char* ProgramOptions::get( const std::string& option ) const
{
	char** end  = argv + argc;
	char** pOption = std::find( this->argv, end, option );

	if ( (pOption != end) && (++pOption != end) )
		return *pOption;
	else
		return NULL;
}

char* ProgramOptions::get( const std::string& option1, const std::string& option2 ) const
{
	char* value = this->get( option1 );
	if ( value == NULL )
		value = this->get( option2 );
	return value;
}

int ProgramOptions::get_int( const std::string& option, int default_value ) const
{
		char* s = this->get( option );
		return ( ( s != NULL ) ? std::atoi(s) : default_value );
}

int ProgramOptions::get_int( const std::string& option1, const std::string& option2, int default_value ) const
{
		char* s = this->get( option1, option2 );
		return ( ( s != NULL ) ? std::atoi(s) : default_value );
}
