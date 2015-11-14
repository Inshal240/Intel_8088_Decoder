void updateWidth(bool specialMove = false);
byte getReg(bool specialMove = false);

// Summary: Decodes instructions in the instructionBuffer
void decodeInstruction()
{
  // Check if the instruction is invalid
  if (!validInstruction)
  {
    printMessage("Invalid instruction.");
    return;
  }

  // Get the value of the first 4 bits
  byte header = (instructionBuffer[0] & 0xF0) >> 4;

  // Checks for move instruction
  if ( isInArray(header, moveInstrHeader, MOV_HDR_ARR_SZ) )
  {
    switch (header)
    {
      case 0b1000: // 0b1000

        if ( (instructionBuffer[0] & 0xFC) == 0b10001000 )
        {
          printMessage("Reg/Mem to/from Reg");

          // Get value of mod
          updateMod();

          // Get value of width (w)
          updateWidth();

          // Get value of direction (d)
          updateDirection();

          // Process the mod value
          processModValue();
        }
        else if (instructionBuffer[0] == 0b10001110)
        {
          printMessage("Reg/Mem to Seg Reg");
        }
        else if (instructionBuffer[0] == 0b10001100)
        {
          printMessage("Seg Reg to Reg/Mem");
        }

        break;

      case 0b1010: // 0b1010

        if ( (instructionBuffer[0] & 0xFE) == 0b10100000 )
        {
          printMessage("Mem to Acc");
        }
        else if ( (instructionBuffer[0] & 0xFE) == 0b10100010 )
        {
          printMessage("Acc to Mem");
        }

        break;

      case 0b1011: // 0b1011

        printMessage("Imm to Reg");

        break;

      case 0b1100: // 0b1100

        if ( (instructionBuffer[0] & 0xFE) == 0b11000110 )
        {
          printMessage("Imm to Reg/Mem");
        }

        break;
    }
  }
}

// Summary: Checks whether an item is in a given array or not
bool isInArray(byte item, const byte* arr, const byte len)
{
  for (int idx = 0; idx < len; idx++)
  {
    if (item == arr[idx])
    {
      return true;
    }
  }
  return false;
}

// Summary: Gets the mod value from the instruction
void updateMod()
{
  mod = (instructionBuffer[1] & 0xC0) >> 6;
}

// Summary: Gets the width bit from the instruction
// NOTE: Boolean is required due to special case in MOV
void updateWidth(bool specialMove)
{
  if (specialMove)
    width = (instructionBuffer[0] & 0x08) >> 3;
  else
    width = (instructionBuffer[0] & 0x01);
}

// Summary: Gets the direction bit from the instruction
void updateDirection()
{
  dir = (instructionBuffer[0] & 0x02) >> 1;
}


// Summary: Returns the Reg value. It can also be used for SR.
// NOTE: Boolean is required due to special case in MOV
byte getReg(bool specialMove)
{
  if (specialMove)
    return (instructionBuffer[0] & 0x07);
  else
    return (instructionBuffer[1] & 0x38) >> 3;
}

// Summary: Returns the R/M value
byte getRM()
{
  return (instructionBuffer[1] & 0x07);
}

// Summary: Returns the displacement.
// NOTE: Displacement is always the third and fourth byte
int updateDisplacement(bool disp)
{
  if (disp)
    displacement = (instructionBuffer[3] << 8) | instructionBuffer[2];
  else
    displacement = (instructionBuffer[2]);
}

// Summary: Looks up in the mod table and updates relevant variables
// NOTE: Boolean is required due to some cases where direction bit is present
void processModValue()
{
  // If Direction (D) bit is high, REG is destination
  if (dir)
  {
    destination = getReg();
    source = getRM();
  }
  // else REG is source
  else
  {
    source = getReg();
    destination = getRM();
  }
  
  switch (mod)
  {
    case 0b00:    // No displacement
      break;

    case 0b01:    // 8-bit displacement
      updateDisplacement(DISP8);
      break;

    case 0b10:    // 16-bit displacement
      updateDisplacement(DISP16);
      break;

    case 0b11:    // Register to Register
      break;
  }
}

