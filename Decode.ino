void updateWidth(bool specialMove = false);
byte getReg(bool specialMove = false);
void printDecodedInstruction(String source, String dest, String disp = "", bool dispSrc = true);

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
        else if (instructionBuffer[0] == 0b10001110
                 || instructionBuffer[0] == 0b10001100)
        {
          printMessage("Reg/Mem to/from Seg Reg");

          // Segment registers are being accessed
          segAccess = true;

          // Get value of direction (d)
          updateDirection();

          // Get mod value
          updateMod();

          processModValue();
        }

        break;

      case 0b1010: // 0b1010

        if ( (instructionBuffer[0] & 0xFC) == 0b10100000 )
        {
          printMessage("Mem to/from Acc");

          // Get the direction bit
          updateDirection();

          // Get value of width (w)
          updateWidth();

          // Use immediate to store address
          updateImmediate(1);

          if (dir)
            printMessage("MOV from " + String(immediate, HEX) + "H"
                         + " to " + registerNames[width][destination]);
          else
            printMessage("MOV from " + registerNames[width][destination]
                         + " to " + String(immediate, HEX) + "H");
        }

        break;

      case 0b1011: // 0b1011

        printMessage("Imm to Reg");

        // Get value of width (w) (special case)
        updateWidth(true);

        // Get destination register
        destination = getReg(true);

        // Get immediate value
        updateImmediate(1);

        // Print decoded result
        printMessage("MOV " + String(immediate, HEX) + "H"
                     + " to " + registerNames[width][destination]);
        break;

      case 0b1100: // 0b1100

        if ( (instructionBuffer[0] & 0xFE) == 0b11000110 )
        {
          printMessage("Imm to Reg/Mem");

          // Get value of width (w) (special case)
          updateWidth();

          // Get destination register
          destination = getRM();

          // Get mod value
          updateMod();

          processModValueImmediate();
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
  dirAvailable = true;
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
int updateDisplacement(bool disp16)
{
  if (disp16)
    displacement = (instructionBuffer[3] << 8) | instructionBuffer[2];
  else
    displacement = (instructionBuffer[2]);
}

// Summary: Returns the immediate value.
int updateImmediate(int start)
{
  if (width)
    immediate = (instructionBuffer[start + 1] << 8) | instructionBuffer[start];
  else
    immediate = (instructionBuffer[start]);
}

// Summary: Selects destination and source
// NOTE: Do NOT use this function with IMMEDIATE instructions
void selectSrcDest()
{
  // If Direction (D) bit is high, REG is destination
  if (dirAvailable && !dir)
  {
    source = getReg();
    destination = getRM();
  }
  // else REG is source
  else
  {
    destination = getReg();
    source = getRM();
  }
}

// Summary: Looks up in the mod table and updates relevant variables
// NOTE: Boolean is required due to some cases where direction bit is present
void processModValue()
{
  selectSrcDest();

  switch (mod)
  {
    case 0b00:    // No displacement

      if (!segAccess)
      {
        if (dirAvailable && !dir)
        {
          printMessage("MOV from " + addressCalc[source]
                       + " to " + registerNames[width][destination]);
        }
        else
        {
          printMessage("MOV from " + registerNames[width][source]
                       + " to " + addressCalc[destination]);
        }
      }
      else
      {
        if (dirAvailable && !dir)
        {
          printMessage("MOV from " + addressCalc[source]
                       + " to " + segRegNames[destination]);
        }
        else
        {
          printMessage("MOV from " + segRegNames[source]
                       + " to " + addressCalc[destination]);
        }
      }

      break;

    case 0b01:    // 8-bit displacement
      updateDisplacement(DISP8);

      if (!segAccess)
      {
        if (dirAvailable && !dir)
        {
          printMessage("MOV from " + addressCalc[source]
                       + "+" + String(displacement, HEX) + "H"
                       + " to " + registerNames[width][destination]);
        }
        else
        {
          printMessage("MOV from " + registerNames[width][source]
                       + " to " + addressCalc[destination]
                       + "+" + String(displacement, HEX) + "H");
        }
      }
      else
      {
        if (dirAvailable && !dir)
        {
          printMessage("MOV from " + addressCalc[source]
                       + "+" + String(displacement, HEX) + "H"
                       + " to " + segRegNames[destination]);
        }
        else
        {
          printMessage("MOV from " + segRegNames[source]
                       + " to " + addressCalc[destination]
                       + "+" + String(displacement, HEX) + "H");
        }
      }
      break;

    case 0b10:    // 16-bit displacement
      updateDisplacement(DISP16);

      if (!segAccess)
      {
        if (dirAvailable && !dir)
        {
          printMessage("MOV from " + addressCalc[source]
                       + "+" + String(displacement, HEX) + "H"
                       + " to " + registerNames[width][destination]);
        }
        else
        {
          printMessage("MOV from " + registerNames[width][source]
                       + " to " + addressCalc[destination]
                       + "+" + String(displacement, HEX) + "H");
        }
      }
      else
      {
        if (dirAvailable && !dir)
        {
          printMessage("MOV from " + addressCalc[source]
                       + "+" + String(displacement, HEX) + "H"
                       + " to " + segRegNames[destination]);
        }
        else
        {
          printMessage("MOV from " + segRegNames[source]
                       + " to " + addressCalc[destination]
                       + "+" + String(displacement, HEX) + "H");
        }
      }
      break;

    case 0b11:    // Register to Register
      if (!segAccess)
      {
        printMessage("MOV from " + registerNames[width][source]
                     + " to " + registerNames[width][destination]);
      }
      else
      {
        if (dirAvailable && !dir)
        {
          // Width is hardcoded to one for 16 bit transactions only
          printMessage("MOV from " + registerNames[1][source]
                       + " to " + segRegNames[destination]);
        }
        else
        {
          printMessage("MOV from " + segRegNames[source]
                       + " to " + registerNames[1][destination]);
        }
      }
      break;
  }
}

// Summary: Looks up in the mod table and updates relevant variables 
//          for immediate instructions
void processModValueImmediate()
{
  switch (mod)
  {
    case 0b00:    // No displacement
      updateImmediate(2);
      
      printMessage("MOV " + String(immediate, HEX) + "H"
                   + " to " + addressCalc[destination]);

      break;

    case 0b01:    // 8-bit displacement
      updateDisplacement(DISP8);
      updateImmediate(3);
      
      printMessage("MOV " + String(immediate, HEX) + "H"
                   + " to " + addressCalc[destination]
                   + "+" + String(displacement, HEX) + "H");

      break;

    case 0b10:    // 16-bit displacement
      updateDisplacement(DISP16);
      updateImmediate(4);
      
      printMessage("MOV " + String(immediate, HEX) + "H"
                   + " to " + addressCalc[destination]
                   + "+" + String(displacement, HEX) + "H");

      break;

    case 0b11:    // Register to Register
      updateImmediate(2);
      
      printMessage("MOV " + String(immediate, HEX) + "H"
                   + " to " + registerNames[width][destination]);

      break;
  }
}

// Summary: Prints the decoded instruction using given paramters
void printDecodedInstruction(String source, String dest, String disp, bool dispSrc)
{
  if (disp != "")
  {
    if (dispSrc)
    {
      Serial.print("MOV from " + source + "+" + String(displacement, HEX) + "H" + " to " + dest);
    }
    else
    {
      Serial.print("MOV from " + source  + "H" + " to " + dest + "+" + String(displacement, HEX));
    }
  }
  else
  {
    Serial.print("MOV from " + source + "H" + " to " + dest);
  }
}

