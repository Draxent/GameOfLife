/**
 *	@file matrixG.cpp
 *  @brief Implementation of \see MatrixG class.
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

#include "matrixG.hpp"

MatrixG::MatrixG( int height, int psidth, const char* output_video, int steps ) : Matrix( height, psidth )
{
	this->write_video = false;
	this->pixel_size = 0;
	this->screen_rate = 0;
	this->init( output_video, steps );
}

MatrixG::MatrixG( const char* input_path, const char* output_video, int steps ) : Matrix( input_path )
{
	this->write_video = false;
	this->pixel_size = 0;
	this->screen_rate = 0;
	this->init( output_video, steps );
}

void MatrixG::set( int i, int j, bool value )
{
	int ps = this->pixel_size;
	Matrix::set( i, j, value );
	this->screen( cv::Rect( j * ps, i * ps, ps, ps) ) = cv::Scalar::all( value ? WHITE : BLACK );
}

void MatrixG::print()
{
	cv::imshow( "result", this->screen );
	cv::waitKey( this->screen_rate );
	if ( this->write_video )
		this->video << this->screen;
}

void MatrixG::init( const char* output_video, int steps )
{
	Display* d = XOpenDisplay(NULL);
	Screen*  s = DefaultScreenOfDisplay(d);
	std::cout << "SCREEN : height = " << s->height << ", width = " << s->width << std::endl;
	assert ( (this->cols  <= s->width) && (this->rows  <= s->height) );
	this->pixel_size = std::min( ( s->width / this->cols ), ( s->height / this->rows ) );
	this->screen_rate = std::max( 10, MAX_DELAY_SCREEN / steps );
	int ps = this->pixel_size;
	// Initialize the matrix image
	this->screen = cv::Mat::zeros( this->rows * ps, this->cols * ps, CV_8UC3 );

	// Set up the image with initial matrix configuration
	for ( int i = 0; i < this->rows; i++ )
		for ( int j = 0; j < this->cols; j++ )
			this->screen( cv::Rect( j * ps, i * ps, ps, ps) ) = cv::Scalar::all( this->get(i, j) ? WHITE : BLACK );

	// Create and move the windows where the result is shown
	cv::namedWindow( "result", cv::WINDOW_AUTOSIZE );
	cv::moveWindow( "result", 0, 0 );

	// If the psrite_video, set up the video
	this->write_video = ( output_video != NULL );
	if ( this->write_video )
	{
		this->video = cv::VideoWriter(
			std::string( output_video ) + ".avi",
			CV_FOURCC( 'M','J','P','G' ),
			2, // FPS
			cv::Size( this->cols * ps, this->rows * ps ) );
	}
}

MatrixG::~MatrixG()
{
	this->screen.release();
	this->video.release();
}
