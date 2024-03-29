/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _COMMAND_LINE_HEADER_H_
#define _COMMAND_LINE_HEADER_H_

#include <kernel/io.hpp>
#include <kernel/types.h>
#include <kernel/timer.h>

#include <map>
#include <functional>
#include <string>
#include <vector>

class CommandLine;

/*! base class for a single command (with a unique name) */
class CommandBase {
public:
	CommandBase(const std::string& name, const std::string& description,
			CommandLine& command_line);
	
	virtual ~CommandBase() {}
	

	/** start executing a command. for blocking input use handleData below.
	 * do not call finishExecute in here. */
	virtual void startExecute(const std::vector<std::string>& arguments) = 0;
	
	/** polling, if command can read input data.
	 * @return 0 means the command successfully made progress. <0 for error
	 *         >0 success but don't have to call it immediately again
	 */
	virtual int handleData() {
		finishExecute();
		return 0;
	}

	const std::string command_name;
	const std::string command_description;
	
protected:
	/* call this when command finished executing */
	void finishExecute();
	
	/**
	 * convert a string to an integer
	 * @return true if success
	 */
	bool parseInt(const std::string& str, int& value);
	
	CommandLine& m_command_line;
};

/** user definable test command that uses a lambda expression */
class TestCommand : public CommandBase {
public:
	typedef std::function<void (const std::vector<std::string>& arguments, InputOutput& io)> FuncOnExecute;

	TestCommand(CommandLine& command_line, FuncOnExecute on_execute,
			const std::string& name, const std::string& description="");

	virtual void startExecute(const std::vector<std::string>& arguments);
private:
	FuncOnExecute m_on_execute;
};


/*! command line interface class: reads and executes commands, using polling.
 * each command is uniquely identified by its name. all added commands will
 * be deleted when CommandLine is deleted.
 */
class CommandLine {
public:
	/* IO functions: must be non-blocking */
	CommandLine(InputOutput& io, const std::string& prompt = "$> ");
	~CommandLine();
	
	/**
	 * start executing a command. this fails if there is already a command
	 * executing
	 * @param command
	 * @return true on success
	 */
	bool executeCommand(const std::string& command);
	
	/* polling method: returns 0 on success */
	int handleData();

	InputOutput& inputOutput() { return m_io; }
	
	std::string& prompt() { return m_prompt; }
	
	static void split(const std::string& s, char seperator, std::vector<std::string>& output);
	
	void addCommand(CommandBase& cmd) { m_commands[cmd.command_name] = &cmd; };
	void addTestCommand(TestCommand::FuncOnExecute on_execute, const std::string& name="test",
			const std::string& description="");

	void printBackspace();
	void printEnter();
	void moveCursorUp(); /** move cursor one line up */
	void eraseFullLine(); /** earse entire line */
	/**
	 * erase n last lines & move cursor up by n lines
	 */
	void eraseLines(int num);
private:
	void initCommands();
	
	void commandFinished();
	
	bool isBackspace(char c);
	bool isEnter(char c);

	/* list of all commands. key is the command_name */
	std::map<std::string, CommandBase*> m_commands;
	
	std::string m_current_command_line;
	CommandBase* m_current_executing;
	
	std::string m_prompt;
	InputOutput& m_io;
	
	friend class CommandBase;
	friend class CommandHelp;
};


/* list of commands */

/** help: print usage of all/some commands */
class CommandHelp : public CommandBase {
public:
	CommandHelp(CommandLine& command_line);
	virtual void startExecute(const std::vector<std::string>& arguments);
private:
	void printUsage(CommandBase& cmd);
};


/** command to print memory usage */
class CommandMemoryUsage : public CommandBase {
public:
	CommandMemoryUsage(CommandLine& command_line);
	virtual void startExecute(const std::vector<std::string>& arguments);
private:
};

/** command to print/change log level */
class CommandLog : public CommandBase {
public:
	CommandLog(CommandLine& command_line);
	virtual void startExecute(const std::vector<std::string>& arguments);
private:
};

/**
 * class to continuously print the value of some variable(s).
 * this command is not part of the default commands.
 * the command exists when 'q' is pressed.
 */
namespace Math {
template<typename T>
class Vec3;
}
class CommandWatchValues : public CommandBase {
public:
	/**
	 * @param command_line parent
	 * @param min_update_delay_ms minimum delay between 2 prints. should be >=100
	 *                            to avoid flickering
	 * @param clear_before_update whether to send terminal commands to clear
	 * the previous output before a new output.
	 */
	CommandWatchValues(const std::string& command_name, CommandLine& command_line,
			int min_update_delay_ms=100, bool clear_before_update=true, bool add_to_command_line=true);
	
	/**
	 * add values to print
	 * @param value reference to the value that changes over time
	 */
	void addValue(const std::string& name, const float& value);
	void addValue(const std::string& name, const Math::Vec3<float>& value);

	virtual void startExecute(const std::vector<std::string>& arguments);
	
	virtual int handleData();
private:
	struct Value {
		std::string name;
		std::vector<const float*> components;
	};

	void printValue(const Value& value);

	std::vector<Value> m_values;
	Timestamp m_next_update;
	bool m_clear_before_update;
	bool m_clear_before_update_applied;
	Timestamp m_min_update_delay_ms;
	int m_last_printed_lines = 0;
};

#endif /* _COMMAND_LINE_HEADER_H_ */


