/**
 *	@file matrixG.cpp
 *  @brief Implementation of \ref MatrixG class.
 *  @author Federico Conte (draxent)
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
