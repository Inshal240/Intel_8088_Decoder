// Summary: Fetches instruction from serial port and converts it into machine code
// NOTE: Adjusts value of "validInstruction" boolean
void fetchInstruction()
{
  printMessage("Fetching data...");

  clearInstructionBuffer();
  
  // Get instruction
  Serial.readBytesUntil('\n', (char *) instructionBuffer, INSTR_BUFF_MAX);

  instructionLength = getInstructionLength();

  // Check if instruction is valid (2 characters represent each byte)
  if (instructionLength % 2 == 0)
  {
    // Adjust the length and validity
    validInstruction = true;    
    instructionLength /= 2;
  }
  else return;

  convertToMachineCode(instructionLength);

  printInstructionBuffer(instructionLength);
}

// Summary: Converts the data in instruction buffer to machine code
// NOTE: Adjusts value of "validInstruction" boolean
void convertToMachineCode(int len)
{
  // Conversion from ASCII to machine code
  for (int idx = 0; (idx < len) && validInstruction; idx++)
  {
    // Convert and set upper nibble
    instructionBuffer[idx] = (getHexValue(instructionBuffer[idx * 2]) & 0x0F) << 4;
    // Convert and set lower nibble
    instructionBuffer[idx] |= getHexValue(instructionBuffer[idx * 2 + 1]) & 0x0F;
  }

  // Clearing the rest of the array
  for (int idx = len; idx < INSTR_BUFF_MAX; idx++)
  {
    instructionBuffer[idx] = 0;
  }
}

// Summary: Returns the amount of non-zero items in
int getInstructionLength()
{
  int len = -1;

  while (instructionBuffer[++len] != 0);

  return len;
}

// Summary: Converts input ASCII to HEX
// NOTE: Adjusts value of "validInstruction" boolean
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
    validInstruction = false;
    return (byte) ch;
  }
}

