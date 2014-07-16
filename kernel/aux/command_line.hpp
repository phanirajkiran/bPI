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


/*! command line interface class: reads and executes commands, using polling */
class CommandLine {
public:
	/* IO functions: must be non-blocking */
	CommandLine(InputOutput& io, const std::string& prompt = "$> ");
	
	/* polling method: returns 0 on success */
	int handleData();

	InputOutput& inputOutput() { return m_io; }
	
	std::string& prompt() { return m_prompt; }
	
	static void split(const std::string& s, char seperator, std::vector<std::string>& output);
	
	void addCommand(CommandBase& cmd) { m_commands[cmd.command_name] = &cmd; };
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

class CommandHelp : public CommandBase {
public:
	CommandHelp(CommandLine& command_line);
	virtual void startExecute(const std::vector<std::string>& arguments);
private:
	void printUsage(CommandBase& cmd);
};

class CommandMemoryUsage : public CommandBase {
public:
	CommandMemoryUsage(CommandLine& command_line);
	virtual void startExecute(const std::vector<std::string>& arguments);
private:
};


#endif /* _COMMAND_LINE_HEADER_H_ */


