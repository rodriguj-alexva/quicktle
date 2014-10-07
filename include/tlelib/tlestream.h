/*-----------------------------------------------------------------------------+
 | TLELib                                                                      |
 | Copyright 2011-2014 Sergei Fundaev                                          |
 +-----------------------------------------------------------------------------+
 | This file is part of TLELib.                                                |
 |                                                                             |
 | TLELib is free software: you can redistribute it and/or modify              |
 | it under the terms of the GNU Lesser General Public License as published by |
 | the Free Software Foundation, either version 3 of the License, or           |
 | (at your option) any later version.                                         |
 |                                                                             |
 | TLELib is distributed in the hope that it will be useful,                   |
 | but WITHOUT ANY WARRANTY; without even the implied warranty of              |
 | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               |
 | GNU Lesser General Public License for more details.                         |
 |                                                                             |
 | You should have received a copy of the GNU Lesser General Public License    |
 | along with TLELib. If not, see <http://www.gnu.org/licenses/>.              |
 +----------------------------------------------------------------------------*/
/*!
    \file tlestream.h
    \brief File contains the definition of tle_stream object.
*/

#ifndef TLESTREAM_H
#define TLESTREAM_H

#include <iostream>
#include <tlelib/tlenode.h>

namespace tlelib
{

/*!
    \brief Class for operations with an input stream.
*/
class tle_stream
{
public:
    /*!
        \brief Constructor.
        \param source - input stream
        \param file_type - TLE file type (2- or 3-lines)
    */
    tle_stream(std::istream &source, const tle_file_type file_type = two_lines);
    /*!
        \brief Extract the tle_node object from the input stream.
        \param node - the tle_node object
        \return Reference to input stream
    */
    std::istream &operator>>(tle_node &node);
    /*!
        \brief Operator bool()
        \return True if the input stream can be read further.
    */
    operator bool();
    /*!
        \brief Set the parsing mode
        \param parsingMode - parsing mode: true means, that the node object,
                             obtained by >> operator, shoud parse the lines immediately,
                             false - should parse the lines when it is required (lazy initalization).
        \return Previous value of parsing mode.
    */
    bool enforce_parsing(bool parsingMode);

private:
    tle_stream() {} //!< Default constructor is unavailable.

    std::istream *m_source; //!< Pointer to input stream
    tle_file_type m_file_type; //!< File type: 2- or 3-lines.
    bool m_enforce_parsing; //!< Define whether the tle_node objects, obtained by >> operator, should immediately parse the lines.
};
//------------------------------------------------------------------------------

} // namespace tlelib

#endif // TLESTREAM_H
