#define INSTR_BUFF_MAX 12
#define MOV_HDR_ARR_SZ 4

// Displacement bits
#define DISP8 0
#define DISP16 1

byte instructionBuffer[INSTR_BUFF_MAX];    // Holds the instruction read from port
byte instructionLength = 0;                // Holds the length in bytes of the instruction read
bool validInstruction = false;

byte moveInstrHeader[] = { 0b1000, 0b1010, 0b1011, 0b1100 };

// All register names. Index using width, then REG or R/M
const String registerNames[2][8] = {{"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH"},
                                    {"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI"}};

// All address calculations. Index using RM
const String addressCalc[8] = {"BX+SI", "BX+DI", "BP+SI", "BP+DI",
                               "SI", "DI", "DIRECT ADDRESS", "BX"};

// TODO: These are dummy values. Get actual names.
// All segment register names. Index using SR or REG
const String segRegNames[4] = {"CS", "DS", "ES", "SS"};

int source = 0, destination = 0;            // Holds the source and destination of the instruction
int immediate = 0, displacement = 0;        // Holds immediate and displacement values
byte mod = 0;                               // Holds mod value
bool width = 0, dir = 0, dirAvailable = false, segAccess = false;

void printInstructionBuffer(int len = INSTR_BUFF_MAX);

void setup()
{
  Serial.begin(115200);
}

void loop()
{

}

// Runs after loop if there is data in the RX buffer
void serialEvent()
{
  fetchInstruction();
  // TODO: Implement decoding
  decodeInstruction();
}

// Summary: Clears the instruction buffer and resets all flags and values
void clearInstructionBuffer()
{
  // Clear the instruction array
  for (int idx = 0; idx < INSTR_BUFF_MAX; idx++)
    instructionBuffer[idx] = 0;

  // Reset buffer length and valid instruction boolean
  instructionLength = 0;
  validInstruction = false;

  // Clear all variables
  immediate = 0;
  displacement = 0;
  source = 0;
  destination = 0;
  mod = 0;
  dirAvailable = false;
  segAccess = false;
}

// Print functions

// Summary: Prints input string and waits until it has been transferred
void printMessage(String message)
{
  Serial.println(message);
  Serial.flush();
}

// Summary: Prints the instruction buffer
void printInstructionBuffer(int len)
{
  // Print it back for debugging
  for (int idx = 0; idx < len; idx++)
  {
    // Add a leading zero to single digit hex values
    if (instructionBuffer[idx] < 0x10) Serial.print("0");

    // Print byte in hex format
    Serial.print(instructionBuffer[idx], HEX);
  }

  Serial.println();
}

