/*
 * @file matrixG.hpp
 * @brief Header of \ref MatrixG class.
 * @author Federico Conte (draxent)
 */

#ifndef INCLUDE_MATRIXG_HPP_
#define INCLUDE_MATRIXG_HPP_

#include <assert.h>
#include <X11/Xlib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "matrix.hpp"

// White color.
#define WHITE 255
// Block color.
#define BLACK 0
// Maximum time waited before showing the next frame.
#define MAX_DELAY_SCREEN 10000

/// Extension of \ref Matrix class that allows to print the boolean matrix in a graphical way, using OpenCV2
class MatrixG : public Matrix
{
public:
	/**
	 * Initializes a new instance of the \ref MatrixG class.
	 * Set up this matrix using random values.
	 * @param height		number of rows of the boolean matrix.
	 * @param width			number of columns of the boolean matrix.
	 * @param output_video	output path where to store the generated video.
	 * @param steps			number of Game of Life steps, useful to derive screen rate value.
	 */
	MatrixG( int height, int width, const char* output_video, int steps );

	/**
	 * Initializes a new instance of the \ref MatrixG class.
	 * Set up this matrix using the values written in a text file.
	 * @param input_path	the path of the input file.
	 * @param output_video	output path where to store the generated video.
	 * @param steps			number of Game of Life steps, useful to derive screen rate value.
	 */
	MatrixG( const char* input_path, const char* output_video, int steps );

	/**
	 * Set the value of the i-th row and j-th column of the boolean matrix and of the screen image.
	 * @param i			index of the row to set.
	 * @param j			index of the column to set.
	 * @param value		value that the element will take.
	 */
	void set( int i, int j, bool value ) override;

	/**
	 * Print the boolean matrix on OpenCV2 windows.
	 * If <em>write_video</em> is set to <code>true</code>,
	 * add the current image as a frame of the output video.
	 */
	void print() override;

	/// Destructor of the \ref MatrixG class.
	~MatrixG();

private:
	cv::Mat screen;
	int pixel_size, screen_rate;
	bool write_video;
	cv::VideoWriter video;

	/// Init all the private variables.
	void init( const char* output_video, int steps );
};

#endif /* INCLUDE_MATRIXG_HPP_ */
