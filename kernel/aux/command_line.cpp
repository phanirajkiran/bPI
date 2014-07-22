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

#include "command_line.hpp"
#include <kernel/utils.h>
#include <kernel/timer.h>
#include "vec3.hpp"

using namespace std;

CommandBase::CommandBase(const std::string& name,
		const std::string& description, CommandLine& command_line)
	: command_name(name), command_description(description),
	  m_command_line(command_line) {
}

void CommandBase::finishExecute() {
	m_command_line.commandFinished();
}


TestCommand::TestCommand(CommandLine& command_line, FuncOnExecute on_execute,
		const std::string& name, const std::string& description)
	: CommandBase(name, description, command_line), m_on_execute(on_execute) {
}

void TestCommand::startExecute(const std::vector<std::string>& arguments) {
	m_on_execute(arguments, m_command_line.inputOutput());
}


CommandLine::CommandLine(InputOutput& io, const std::string& prompt)
	: m_current_executing(NULL), m_prompt(prompt), m_io(io) {
	initCommands();
	commandFinished(); //show the prompt
}

CommandLine::~CommandLine() {
	for(auto& cmd : m_commands) {
		delete cmd.second;
	}
}

int CommandLine::handleData() {
	if(m_current_executing)
		return m_current_executing->handleData();
	
	int c = m_io.readByte();
	if(c == -E_WOULD_BLOCK) return 0;
	if(c < 0) return c;
	
	//c == 0x1b: escape character (eg for cursor up: <ESC> [ {COUNT} A
	//  (COUNT is an optional integer, default is 1)
	//see: http://www.termsys.demon.co.uk/vtansi.htm
	//     http://ascii-table.com/ansi-escape-sequences.php
	//TODO: implement history & cursor movements

	if(isEnter(c)) {
		printEnter();
	} else if(isBackspace(c) && !m_current_command_line.empty()) {
		printBackspace();
	} else if(c == '\t' && m_current_command_line.empty()) {
		printEnter();
	} else {
		m_io.writeByte(c); //echo back
	}
	
	bool has_command = false;
	if(c == '\t' && m_current_command_line.empty()) {
		//FIXME: better tab completion
		m_current_command_line = "help";
		has_command = true;
	}
	if(isEnter(c) || has_command) {
		//new command: get name & arguments
		vector<string> arguments;
		string cmd_name;
		auto sep = m_current_command_line.find(' ');
		
		if(sep == string::npos) {
			cmd_name = m_current_command_line;
		} else {
			cmd_name = m_current_command_line.substr(0, sep);
			split(m_current_command_line.substr(sep+1), ' ', arguments);
		}
		
		auto cmd = m_commands.find(cmd_name);
		if(cmd == m_commands.end()) {
			if(!cmd_name.empty())
				m_io.printf("Error: unknown command '%s'\n", cmd_name.c_str());
			commandFinished();
		} else {
			m_current_executing = cmd->second;
			m_current_executing->startExecute(arguments);
			m_current_executing->handleData();
		}
		return 0;
	}

	if(isBackspace(c)) {
		if(!m_current_command_line.empty())
			m_current_command_line.pop_back();
	} else {
		m_current_command_line += (char)c;
	}
	
	return 0;
}

bool CommandLine::isBackspace(char c) {
	return c == '\b' || c == 0x7f; /* backspace or del character */
}
bool CommandLine::isEnter(char c) {
	return c == '\r' || c == '\n';
}

void CommandLine::addTestCommand(TestCommand::FuncOnExecute on_execute,
		const std::string& name, const std::string& description) {
	addCommand(*new TestCommand(*this, on_execute, name, description));
}

void CommandLine::printEnter() {
	m_io.writeByte('\n');
}
void CommandLine::printBackspace() {
	m_io.writeByte('\b');
	m_io.writeByte(0x1b); m_io.writeByte('['); m_io.writeByte('K'); //erase line
}
void CommandLine::moveCursorUp() {
	m_io.writeByte(0x1b); m_io.writeByte('['); m_io.writeByte('A'); //move cursor up
}
void CommandLine::eraseFullLine() {
	m_io.writeByte(0x1b); m_io.writeByte('['); m_io.writeByte('2'); m_io.writeByte('K'); //erase entire line
}

void CommandLine::split(const std::string& s, char seperator, std::vector<std::string>& output) {

	std::string::size_type prev_pos = 0, pos = 0;
	while ((pos = s.find(seperator, pos)) != std::string::npos) {
		if(pos != prev_pos) {
			std::string substring(s.substr(prev_pos, pos - prev_pos));
			output.push_back(substring);
		}

		prev_pos = ++pos;
	}

	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
}

void CommandLine::initCommands() {
	/* initialize list with all known commands */
	CommandBase* cmds[] = { 
			new CommandHelp(*this),
			new CommandMemoryUsage(*this),
			new CommandLog(*this),
	};
	
	for(uint i=0; i<sizeof(cmds)/sizeof(cmds[0]); ++i) {
		m_commands[cmds[i]->command_name] = cmds[i];
	}
}

void CommandLine::commandFinished() {
	m_current_executing = NULL;
	m_current_command_line.clear();
	m_io.writeString(m_prompt);
}

CommandHelp::CommandHelp(CommandLine& command_line)
	: CommandBase("help", "Print help for all commands or a single command",
	command_line) {
}

void CommandHelp::startExecute(const std::vector<std::string>& arguments) {
	map<string, CommandBase*>& commands = m_command_line.m_commands;
	if(arguments.empty()) {
		for(auto& cmd : commands)
			printUsage(*cmd.second);
	} else {
		for(auto& cmd_name : arguments) {
			auto cmd = commands.find(cmd_name);
			if(cmd != commands.end())
				printUsage(*cmd->second);
		}
	}
}

void CommandHelp::printUsage(CommandBase& cmd) {
	InputOutput& io = m_command_line.inputOutput();
	const size_t name_length = 16;

	io.writeString(cmd.command_name);
	for(size_t i=0; i<name_length - cmd.command_name.length(); ++i)
		io.writeByte(' ');

	io.writeString(cmd.command_description);
	io.writeByte('\n');
}


CommandMemoryUsage::CommandMemoryUsage(CommandLine& command_line)
	: CommandBase("free", "Show current memory usage of malloc'able memory",
	command_line) {
}

void CommandMemoryUsage::startExecute(
		const std::vector<std::string>& arguments) {
	/* print memory usage */
	m_command_line.inputOutput().printf(
			"Total Memory: %R\n"
			"Free  Memory: %R (%i%%)\n",
			(int)ktotalMallocSpace(), (int)kfreeMallocSpace(),
			(int)((kfreeMallocSpace()/1024)*100/(ktotalMallocSpace()/1024)));
}

CommandLog::CommandLog(CommandLine& command_line)
	: CommandBase("log", "Show (no arguments) or set console log level (levels: off=none, debug, info, warn, error, critical)",
	command_line) {
}
void CommandLog::startExecute(
		const std::vector<std::string>& arguments) {
	if(arguments.size() == 0) {
		const char* log_str;
		switch(g_log_level) {
		case LogLevel_debug: log_str = "debug";
			break;
		case LogLevel_info: log_str = "info";
			break;
		case LogLevel_warn: log_str = "warn";
			break;
		case LogLevel_error: log_str = "error";
			break;
		case LogLevel_critical: log_str = "critical";
			break;
		default: log_str = "none";
			break;
		}
		m_command_line.inputOutput().printf("current log level: %s\n", log_str);
	} else {
		const string& level = arguments[0];
		if(level == "debug") {
			g_log_level = LogLevel_debug;
		} else if(level == "info") {
			g_log_level = LogLevel_info;
		} else if(level == "warn") {
			g_log_level = LogLevel_warn;
		} else if(level == "error") {
			g_log_level = LogLevel_error;
		} else if(level == "critical") {
			g_log_level = LogLevel_critical;
		} else if(level == "none"|| level == "off") {
			g_log_level = LogLevel_none;
		} else {
			m_command_line.inputOutput().printf("Error: unknown log level '%s'\n", level.c_str());
		}
	}
}

CommandWatchValues::CommandWatchValues(const std::string& command_name,
		CommandLine& command_line, int min_update_delay_ms, bool clear_before_update)
	: CommandBase(command_name, "repeatedly print a/some variables to the output. exit with 'q'",
		command_line),
	  m_clear_before_update(clear_before_update),
	  m_min_update_delay_ms((uint)min_update_delay_ms) {
}

void CommandWatchValues::addValue(const std::string& name, const float& value) {
	Value v;
	v.name = name;
	v.components.push_back(&value);
	m_values.push_back(v);
}

void CommandWatchValues::addValue(const std::string& name,
		const Math::Vec3<float>& value) {
	Value v;
	v.name = name;
	v.components.push_back(&value.x);
	v.components.push_back(&value.y);
	v.components.push_back(&value.z);
	m_values.push_back(v);
}

void CommandWatchValues::startExecute(const std::vector<std::string>& arguments) {
	m_next_update = getTimestamp()-1; //update now
	m_last_printed_lines = 0;
}

int CommandWatchValues::handleData() {
	int c = m_command_line.inputOutput().readByte();
	if(c == 'q') { //quit
		finishExecute();
		return 0;
	}
	if(c < 0 && c != -E_WOULD_BLOCK) return c;

	uint cur_time = getTimestamp();
	if(time_after(cur_time, m_next_update)) {
		if(m_clear_before_update)
			clearOutput();
		for(size_t i=0; i<m_values.size(); ++i) {
			printValue(m_values[i]);
		}
		m_last_printed_lines = m_values.size();
		m_next_update = cur_time + m_min_update_delay_ms*1000;
	}
	
	return 0;
}

void CommandWatchValues::printValue(const Value& value) {
	m_command_line.inputOutput().writeString(value.name);
	for(size_t i=0; i<value.components.size(); ++i) {
		m_command_line.inputOutput().printf(", %.3f", *value.components[i]);
	}
	m_command_line.inputOutput().writeByte('\n');
}

void CommandWatchValues::clearOutput() {
	for(int i=0; i<m_last_printed_lines; ++i) {
		m_command_line.moveCursorUp();
		m_command_line.eraseFullLine();
	}
}
