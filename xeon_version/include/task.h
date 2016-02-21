/**
 *	@file task.h
 *	@brief Header of \see Task_t struct.
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
#ifndef GAMEOFLIFE_TASK_H
#define GAMEOFLIFE_TASK_H

#include <iostream>

// Task message passed between \see Master and \see Worker.
struct Task_t
{
	Task_t ( size_t start, size_t end) : start(start), end(end) { }
	const size_t start, end;
};

#endif //GAMEOFLIFE_TASK_H
