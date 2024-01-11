//#define DIAGNOSTICS_ACTIVE 

// Stored program management

#define STATEMENT_CONFIRMATION 1
#define LINE_NUMBERS 2
#define ECHO_DOWNLOADS 4
#define DUMP_DOWNLOADS 8

enum ProgramState
{
	PROGRAM_STOPPED,
	PROGRAM_PAUSED,
	PROGRAM_ACTIVE,
	PROGRAM_AWAITING_MOVE_COMPLETION,
	PROGRAM_AWAITING_DELAY_COMPLETION,
	SYSTEM_CONFIGURATION_CONNECTION // will never enter this state
};

enum DeviceState
{
	EXECUTE_IMMEDIATELY,
	STORE_PROGRAM
};

#define COMMAND_BUFFER_SIZE 60

// Set command terminator to CR

#define STATEMENT_TERMINATOR 0x0D

// Set program terminator to string end
// This is the EOT character
#define PROGRAM_TERMINATOR 0x00

void dumpProgramFromEEPROM(int EEPromStart);
void startProgramExecution(int programPosition);
void haltProgramExecution();
void pauseProgramExecution();
void resumeProgramExecution();
void clearStoredProgram();
void startDownloadingCode(int downloadPosition);
void endProgramReceive();
void resetCommand();
void setupRemoteControl();

void updateRobot();
bool commandsNeedFullSpeed();

void sendStringToRobot(String str);
void sendLineToRobot(String str);

extern char *decodePos;
extern char *decodeLimit;

