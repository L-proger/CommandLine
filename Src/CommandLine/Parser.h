#pragma once

#include "Command.h"

namespace CommandLine {

    class Parser{
    public:
        void parse(int argc, const char* const* argv, Command& rootCommand){
            _applicationPath = argv[0];
			_rootCommand = &rootCommand;
			_currentCommand = _rootCommand;

			for (int i = 1; i < argc; ++i) {
				auto str = std::string(argv[i]);

                if (OptionDescription::isValidOption(str)) {
                    if(!parseOption(str)){
                        return;
                    }
				}
				else {
					parseCommandOrArgument(str);
				}
			}

			//validate last command
			validateCommandArgs(_currentCommand);
            finalizeCurrentOption();
            if (_currentCommand->_handler == nullptr) {
                throw CommandLine::Exception("No handler for command " + addQuotes(_currentCommand->name()));
			}
            _currentCommand->_handler();
        }

        const std::string& applicationPath() const {
            return _applicationPath;
        }

        static std::vector<std::string> splitCommandLineString(const std::string& cmdLine){
            std::vector<std::string> list;
            std::string arg;
            bool escape = false;
            enum { Idle, Arg, QuotedArg } state = Idle;
            for (char c : cmdLine) {
                if (!escape && c == '\\') { escape = true; continue; }
                switch (state) {
                case Idle:
                    if (!escape && c == '"') state = QuotedArg;
                    else if (escape || !(c == ' ')) { arg += c; state = Arg; }
                    break;
                case Arg:
                    if (!escape && c == '"') state = QuotedArg;
                    else if (escape || !(c == ' ')) arg += c;
                    else { list.push_back(arg); arg.clear(); state = Idle; }
                    break;
                case QuotedArg:
                    if (!escape && c == '"') state = arg.empty() ? Idle : Arg;
                    else arg += c;
                    break;
                }
                escape = false;
            }
            if (!arg.empty()) list .push_back(arg);
            return list;
        }
    private:
        Command* _currentCommand = nullptr;
        Command* _rootCommand = nullptr;
        Option* _currentOption = nullptr;
        bool _currentOptionAssigned = false;
        size_t _currentArgId = 0;
		std::string _applicationPath;

        static std::string addQuotes(const std::string& value) {
            return "\"" + value + "\"";
        }

        void validateCommandArgs(Command* cmd) {
            auto & args = cmd->getArguments();
            for (const auto& arg : args) {
                if (!arg->description().canBeEmpty() && !arg->isSet()) {
                    throw CommandLine::Exception("Positional argument " + addQuotes(arg->description().name()) + " is not set for command " + addQuotes(cmd->name()));
                }
            }
        }

        void parseCommandOrArgument(const std::string& str) {
            if (_currentOption) {
                if (_currentOption->description().type() == OptionType::SingleValue && _currentOption->_values->size() == 1){
                    if(!_currentOptionAssigned){
                        throw CommandLine::Exception("Too many values for option " + addQuotes(_currentOption->description().names()[0]));
                    }
                    finalizeCurrentOption();
                }
            }

            if (_currentOption) {
                _currentOption->_values->push_back(str);
                 _currentOptionAssigned = true;
            }
            else {
                auto subcommand = _currentCommand->getSubCommand(str);
                if (subcommand != nullptr) {
                    validateCommandArgs(_currentCommand);
                    finalizeCurrentOption();
                    _currentCommand = subcommand.get();
                    _currentArgId = 0;
                }
                else {
                    if (_currentArgId >= _currentCommand->getArguments().size()) {
                        throw CommandLine::Exception("Too many arguments for command " + addQuotes(_currentCommand->name()));
                    }
                    else {
                        auto& arg = _currentCommand->getArguments()[_currentArgId];
                        if(arg->description().type() == ArgumentType::SingleValue || arg->description().type() == ArgumentType::SingleOrNoValue){
                            arg->_value.emplace({str});
                            ++_currentArgId;
                        }else{
                            if(!arg->isSet()){
                                arg->_value.emplace({str});
                            }else{
                                arg->_value->emplace_back(str);
                            }
                        }
                    }
                }
            }
        }

        void finalizeCurrentOption(){
            if (_currentOption != nullptr) {
                if (_currentOption->description().type() == OptionType::SingleValue) {
                    if (_currentOption->_values->empty()) {
                        throw CommandLine::Exception("Value not set for option: " + addQuotes(_currentOption->description().names()[0]) + " in command " + addQuotes(_currentCommand->name()));
                    }
                    else if (_currentOption->_values->size() > 1) {
                        throw CommandLine::Exception("Too many values for option: " + addQuotes(_currentOption->description().names()[0]) + " in command " + addQuotes(_currentCommand->name()));
                    }
                }
                else if (_currentOption->description().type() == OptionType::SingleOrNoValue ) {
                    if (_currentOption->_values->size() > 1) {
                        throw CommandLine::Exception("Too many values for option: " + addQuotes(_currentOption->description().names()[0]) + " in command " + addQuotes(_currentCommand->name()));
                    }
                }else if (_currentOption->description().type() == OptionType::NoValue ) {
                    if (!_currentOption->_values->empty()) {
                        throw CommandLine::Exception("Too many values for option: " + addQuotes(_currentOption->description().names()[0]) + " in command " + addQuotes(_currentCommand->name()));
                    }
                }
                _currentOption = nullptr;
                _currentOptionAssigned = false;
            }
        }

        bool parseOption(const std::string& str) {
            finalizeCurrentOption();

            auto option = _currentCommand->getOption(str);
            if (option == nullptr) {
                throw CommandLine::Exception("Unexpected option " + addQuotes(str) + " for command " + addQuotes(_currentCommand->name()));
            }
            else {
                if(_currentCommand->getHelpOptionDesc().match(str)){
                    _currentCommand->printHelp();
                    return false;
                }

                if(!option->_values.has_value()){
                    option->_values.emplace();
                }

                if (option->description().type() != OptionType::NoValue) {
                    _currentOption = option.get();

                }
            }
            return true;
        }
    };
}
