/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#ifndef TIMER_H
#define TIMER_H

#include <ctime>

/**
 * @brief The Timer class is the mainly responsible for all Timer related functions.
 */
class Timer
{
public:
    Timer(); /*!< Default constructor */

    // Timer control
    void start(); /*!< Start method, set _time1 to the current system timespecc */
    void stop(); /*!< Stop method, set _time2 to the current system timespec */

    // Getters
    double getSeconds(); /*!< @return The difference in seconds from _time2 and _time1 */
    double getMiliSeconds(); /*!< @return The difference in miliseconds from _time2 and _time1 */
    double getMicroSeconds(); /*!< @return The difference in microseconds from _time2 and _time1 */
    double getNanoSeconds(); /*!< @return The difference in nanoseconds from _time2 and _time1 */

private:
    timespec _time1; /*!< Store the start() timespec */
    timespec _time2; /*!< Store the stop() timespec */
};

#endif // TIMER_H
