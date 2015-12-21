/**
 *	@file program_options.cpp
 *  @brief Implementation of \see ProgramOptions class.
 *  @author Federico Conte (draxent)
 *  
 *	Copyright 2015 Federico Conte
 *	https://github.com/Draxent/ConnectedComponents
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
