#pragma once

#include "Argument.h"
#include "Option.h"
#include <functional>
#include <iostream>

namespace CommandLine {

class Command final {
public:
    friend class Parser;
    using Constructor = std::function<void(Command&)>;
    using Handler = std::function<void()>;

    Command(const std::string& name, const std::string& helpText, Constructor constructor) : _name(name), _helpText(helpText) {
        constructor(*this);
    }

    Command& command(const std::string& name, const std::string& helpText, Constructor constructor){
        if(getSubCommand(name) != nullptr){
            throw CommandLine::Exception("Command::command: subcommand " + name + " already exists in command " + _name);
        }
        auto result = std::make_shared<Command>(name, helpText, constructor);
        _subCommands.push_back(result);
        return *result;
    }

    void addHelpOption(){
        option(_helpOptionDesc);
    }

    std::string toHelpString(ArgumentType type){
        if(type == ArgumentType::SingleValue){
            return " <required>";
        }else  if(type == ArgumentType::MultipleValues){
            return " [...]";
        }else  if(type == ArgumentType::SingleOrNoValue){
            return " [optional]";
        }else{
            throw CommandLine::Exception("Command::toHelpString: Unexpected argument type");
        }
    }

    void printHelp(){
        std::string helpIdent = "  ";
        std::cout << name() << ": " << helpText() << std::endl << std::endl << "Usage:" << std::endl;

        if(!_options.empty()){
            if(_arguments.empty()){
                std::cout << helpIdent << name() << " [options]" << std::endl;
            }else{
                bool hasRequiredOptions = std::find_if(_arguments.begin(), _arguments.end(), [](std::shared_ptr<Argument> arg){ return arg->description().type() == ArgumentType::SingleValue; }) != _arguments.end();
                std::cout << helpIdent << name() << " [options]" << (hasRequiredOptions ? " <args>" : " [args]") << std::endl;
            }
        }
        if(!_subCommands.empty()){
            std::cout << helpIdent << name() << " <subcommand> [options] [args]" << std::endl;
        }
        std::cout << std::endl;

        if(!_options.empty()){
            std::cout << "Options:" << std::endl;
            for(auto& option : _options){
                //TODO: use join
                if(option->description().names().size() == 1){
                    std::cout << helpIdent << option->description().names()[0]  << " : " << option->description().helpText() << std::endl;
                }else{
                    std::cout << helpIdent << option->description().names()[0]  << " " << option->description().names()[1] << " : " << option->description().helpText() << std::endl;
                }
            }
            std::cout << std::endl;
        }

        if(!_arguments.empty()){
            std::cout << "Arguments:" << std::endl;
            for(auto& argument : _arguments){
                std::cout << helpIdent << argument->description().name() << toHelpString(argument->description().type()) <<  " : " << argument->description().helpText() << std::endl;
            }
            std::cout << std::endl;
        }

        if(!_subCommands.empty()){
            std::cout << "Subcommands:" << std::endl;
            for(auto& command : _subCommands){
                std::cout << helpIdent << command->name() << std::endl;
            }
            std::cout << std::endl;
        }
    }

    Argument& argument(const ArgumentDescription& description){
        if(getArgument(description.name()) != nullptr){
            throw CommandLine::Exception("Argument " + description.name() + " already exists");
        }

        if(description.canBeEmpty()){
            if(_hasPotentiallyEmptyArgs){
                throw CommandLine::Exception("The command can have one optional or variable length argument");
            }
            _hasPotentiallyEmptyArgs = true;
        }else{
            if(_hasPotentiallyEmptyArgs){
                throw CommandLine::Exception("Can not add argument \"" + description.name() + "\" after argument that can be empty");
            }
        }

        auto result = std::make_shared<Argument>(description);
        _arguments.push_back(result);
        return *result;
    }

    Option& option(const OptionDescription& description){
        if ( std::find_if(_options.begin(), _options.end(), [&description](std::shared_ptr<Option> o) { return o->description().match(description);}) != _options.end()) {
            throw CommandLine::Exception("Option with name " + description.names()[0] + " already exists in command " + _name);
        }
        auto result = std::make_shared<Option>(description);
        _options.push_back(result);
        return *result;
    }

    std::shared_ptr<Command> getSubCommand(const std::string& name) {
        auto result = std::find_if(_subCommands.begin(), _subCommands.end(), [&name](std::shared_ptr<Command> value) { return value->name() == name; });
        if (result != _subCommands.end()) {
            return *result;
        }
        return nullptr;
    }

    std::shared_ptr<Option> getOption(const std::string& name) {
        auto result = std::find_if(_options.begin(), _options.end(), [&name](std::shared_ptr<Option> value) { return value->description().match(name); });
        if (result != _options.end()) {
            return *result;
        }
        return nullptr;
    }

    std::shared_ptr<Argument> getArgument(const std::string& name) {
        auto result = std::find_if(_arguments.begin(), _arguments.end(), [&name](std::shared_ptr<Argument> value) { return value->description().name() == name; });
        if (result != _arguments.end()) {
            return *result;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<Argument>>& getArguments() {
        return _arguments;
    }
    const std::string& name() const { return _name; }
    const std::string& helpText() const { return _helpText; }

    const OptionDescription& getHelpOptionDesc() const {
        return _helpOptionDesc;
    }

    void run(int argc, const char* const* argv);

    void handler(Handler handler){
        _handler = handler;
    }
private:
    Handler _handler;
    OptionDescription _helpOptionDesc = OptionDescription("--help", "Print help", OptionType::NoValue).alias("-h");
    bool _hasPotentiallyEmptyArgs = false;
    std::vector<std::shared_ptr<Command>> _subCommands;
    std::vector<std::shared_ptr<Argument>> _arguments;
    std::vector<std::shared_ptr<Option>> _options;
    std::string _name;
    std::string _helpText;
};

}
