/*
 * Copyright 2011 Nate Koenig
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef _SDF_CONSOLE_HH_
#define _SDF_CONSOLE_HH_

#include <iostream>
#include <fstream>
#include <string>

namespace sdf
{
  /// \addtogroup sdf SDF
  /// \{

  /// \brief Output a message
  #define sdfmsg (sdf::Console::Instance()->ColorMsg("Msg", 32))

  /// \brief Output a debug message
  #define sdfdbg (sdf::Console::Instance()->ColorMsg("Dbg", 36))

  /// \brief Output a warning message
  #define sdfwarn (sdf::Console::Instance()->ColorErr("Warning", \
        __FILE__, __LINE__, 33))

  /// \brief Output an error message
  #define sdferr (sdf::Console::Instance()->ColorErr("Error", \
        __FILE__, __LINE__, 31))

  /// \brief Log a message
  #define sdflog (sdf::Console::Instance()->Log() << "[" <<\
      __FILE__ << ":" << __LINE__ << "] ")

  /// start marker
  #define sdfclr_start(clr) "\033[1;33m"
  /// end marker
  #define sdfclr_end "\033[0m"

  /// \brief Message, error, warning, and logging functionality
  class Console
  {
    /// \brief Default constructor
    private: Console();

    /// \brief Destructor
    private: virtual ~Console();

    /// \brief Return an instance to this class
    public: static Console *Instance();

    /// \brief Load the message parameters
    public: void Load();

    /// \brief Set quiet output
    /// \param[in] q True to prevent warning
    public: void SetQuiet(bool _q);

    /// \brief Use this to output a colored message to the terminal
    /// \param[in] _lbl Text label
    /// \param[in] _color Color to make the label
    /// \return Reference to an output stream
    public: std::ostream &ColorMsg(const std::string &_lbl, int _color);

    /// \brief Use this to output an error to the terminal
    /// \param[in] _lbl Text label
    /// \param[in] _file File containing the error
    /// \param[in] _line Line containing the error
    /// \param[in] _color Color to make the label
    /// \return Reference to an output stream
    public: std::ostream &ColorErr(const std::string &_lbl,
                const std::string &_file, unsigned int _line, int _color);

    /// \brief Use this to output a message to a log file
    /// \return Reference to output stream
    public: std::ofstream &Log();

    /// \brief True if logging data
    private: bool logData;

    /// \brief A stream that does not output anywhere
    private: class NullStream : public std::ostream
             {
               /// \brief constructor
               public: NullStream() : std::ios(0), std::ostream(0) {}
             };

    /// \brief null stream
    private: NullStream nullStream;

    /// \brief message stream
    private: std::ostream *msgStream;

    /// \brief error stream
    private: std::ostream *errStream;

    /// \brief log stream
    private: std::ofstream logStream;

    /// Pointer to myself
    private: static Console *myself;
  };
  /// \}
}
#endif