#pragma once
#ifndef PYTHON_INTERPRETER_HPP
#define PYTHON_INTERPRETER_HPP

#include "interfaces/core_python_script/i_interpreter.hpp"

class Python27Interpreter
	: public Implements< IPythonInterpreter > // Always implement latest version
{
public:
	bool init();
	void fini();

	bool import( const char* name ) override;
};


#endif // PYTHON_INTERPRETER_HPP

