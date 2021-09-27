

add_library(CommandLine INTERFACE)

target_sources(CommandLine 
	INTERFACE 
		Src/CommandLine/Argument.h
		Src/CommandLine/ArgumentDescription.h
		Src/CommandLine/Command.h
		Src/CommandLine/CommandLineException.h
		Src/CommandLine/Option.h
		Src/CommandLine/OptionDescription.h
		Src/CommandLine/Parser.h
		Src/CommandLine/ValueConverter.h
)

target_include_directories(CommandLine INTERFACE Src)