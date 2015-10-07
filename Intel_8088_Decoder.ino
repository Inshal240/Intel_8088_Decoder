#define INSTR_MAX 6

byte instruction[INSTR_MAX];    // Holds the instruction

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

  // TODO: Decode instruction
}

byte getHexValue(int ch)
{
  if (ch == 'a' || ch == 'A')
  {
    return (byte) 0x0A;
  }
  else if (ch == 'b' || ch == 'B')
  {
    return (byte) 0x0B;
  }
  else if (ch == 'c' || ch == 'C')
  {
    return (byte) 0x0C;
  }
  else if (ch == 'd' || ch == 'D')
  {
    return (byte) 0x0D;
  }
  else if (ch == 'e' || ch == 'E')
  {
    return (byte) 0x0E;
  }
  else if (ch == 'f' || ch == 'F')
  {
    return (byte) 0x0F;
  }
  else if (isDigit(ch))
  {
    return byte (ch - 48);
  }
  else
  {
    Serial.println("Error in conversion. Answer may not be correct!");
    return (byte) ch;
  }
}

void fetchInstruction()
{
  int ch;   // Holds a temporary byte
  int idx = 0;

  // Clear the instruction array
  for (; idx < INSTR_MAX; idx++)
    instruction[idx] = 0;

  // Reset the counter
  idx = -1;

  // Get instruction and decode it
  while (Serial.available() && idx++ < INSTR_MAX)
  {
    ch = Serial.read();
    
    // Exit on line break
    if (ch == '\n') return;

    // Get upper digit's hex value and assign it to the upper nibble
    instruction[idx] = (getHexValue(ch) & 0x0F);
    instruction[idx] = instruction[idx] << 4;

    // Get lower digit's hex value and assign it to the lower nibble
    ch = Serial.read();
    instruction[idx] |= (getHexValue(ch) & 0x0F);
  }

  // Print it back for debugging
  for (int idx = 0; idx < INSTR_MAX; idx++)
    Serial.print(instruction[idx], HEX);
}

void decodeInstruction()
{
  // TODO: Decode the instruction here
}

