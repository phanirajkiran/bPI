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
	
	/* polling, if command can read input data */
	virtual int handleData() {
		finishExecute();
		return 0;
	}

	const std::string command_name;
	const std::string command_description;
	
protected:
	/* call this when command finished executing */
	void finishExecute();
	
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
	
	/* polling method: returns 0 on success */
	int handleData();

	InputOutput& inputOutput() { return m_io; }
	
	std::string& prompt() { return m_prompt; }
	
	static void split(const std::string& s, char seperator, std::vector<std::string>& output);
	
	void addCommand(CommandBase& cmd) { m_commands[cmd.command_name] = &cmd; };
	void addTestCommand(TestCommand::FuncOnExecute on_execute, const std::string& name="test",
			const std::string& description="");
private:
	void initCommands();
	
	void commandFinished();
	
	bool isBackspace(char c);
	bool isEnter(char c);
	void printBackspace();
	void printEnter();

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


#endif /* _COMMAND_LINE_HEADER_H_ */


