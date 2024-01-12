#define NUMBER_OF_VARIABLES 20
#define MAX_VARIABLE_NAME_LENGTH 10

#define READING_START_CHAR '@'

// declared in Commands.cpp
extern byte diagnosticsOutputLevel;


enum parseOperandResult {
	INVALID_VARIABLE_NAME=1,
	NO_ROOM_FOR_VARIABLE=2,
	VARIABLE_NOT_FOUND=3,
	VARIABLE_NAME_TOO_LONG=4,
	VARIABLE_NAME_OK=5,
	OPERAND_OK=6,
	USING_UNASSIGNED_VARIABLE=7,
	INVALID_OPERAND=8,
	INVALID_HARDWARE_READING_NAME=9,
	NO_DIGITS_IN_VALUE=10,
	FIRST_VARIABLE_NOT_FOUND=11,
	FIRST_VARIABLE_USED_BEFORE_DEFINITION=12,
	INVALID_OPERATOR=13,
	SECOND_VARIABLE_NOT_FOUND=14,
	SECOND_VARIABLE_USED_BEFORE_DEFINITION=15,
};

//#define VAR_DEBUG

struct op
{
	char operatorCh;
	int(*evaluator) (int, int);
};

struct logicalOp
{
	char * operatorCh;
	bool(*evaluator) (int, int);
};
struct logicalOp * findLogicalOp(char * text);

struct reading {
	char * name;
	int(*reader)(void);
};
struct reading * getReading(char * text);

void clearVariables();
bool getValue(int * result);
bool testCondition(bool * result);
void setVariable();
void viewVariable();
void setupVariables();

bool isVariableNameStart(char * ch);
bool isVariableNameChar(char * ch);
bool variableSlotEmpty(int position);
parseOperandResult findVariable(char * name, int *position);
parseOperandResult createVariable(char * namePos, int * varPos);
int getVariableNameLength(int position);

bool validOperator(char ch);
int checkIdentifier(char * var);



