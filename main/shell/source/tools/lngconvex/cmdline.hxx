/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/



#ifndef _CMDLINE_HXX_
#define _CMDLINE_HXX_

#include "defs.hxx"

//---------------------------------
/** Simple command line abstraction
*/

class CommandLine
{
public:

	//################################
	// Creation
	//################################


	CommandLine(size_t argc, char* argv[], const std::string& ArgPrefix = std::string("-"));


	//################################
	// Query
	//################################


	/** Return the argument count
	*/
	size_t get_arg_count() const;

	/** Return an argument by index
		This method doesn't skip argument
		names if any, so if the second
		argument is an argument name the
		function nevertheless returns it.

		@precond	0 <= Index < GetArgumentCount

		@throws std::out_of_range exception
		if the given index is to high
	*/
	std::string get_arg(size_t Index) const;

	/** Returns all argument name found in the
		command line. An argument will be identified
		by a specified prefix. The standard prefix
		is '-'.
		If there are no argument names the returned
		container is empty.
	*/
	StringListPtr_t get_arg_names() const;

	/** Returns an argument by name. If there are
		duplicate argument names in the command line,
		the first one wins.
		Argument name and the argument value must be separated
		by spaces. If the argument value starts with an
		argument prefix use quotes else the return value is
		an empty string because the value will be interpreted
		as the next argument name.
		If an argument value contains spaces use quotes.

		@precond	GetArgumentNames() -> has element ArgumentName

		@throws std::invalid_argument exception
		if the specified argument could not be
		found
	*/
	std::string get_arg(const std::string& ArgumentName) const;


	//################################
	// Command
	//################################


	/** Set the prefix used to identify arguments in
		the command line.

		@precond	prefix is not empty

		@throws std::invalid_argument exception if
		the prefix is empty
	*/
	void set_arg_prefix(const std::string& Prefix);

private:

	/** Returns whether a given argument is an argument name
	*/
	bool is_arg_name(const std::string& Argument) const;

private:
	size_t		m_argc;
	char**		m_argv;
	std::string m_argprefix;

// prevent copy and assignment
private:
	CommandLine(const CommandLine&);
	CommandLine& operator=(const CommandLine&);
};

#endif

/* vim: set noet sw=4 ts=4: */
