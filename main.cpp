#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <bitset>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <iomanip>

using namespace std;

struct Instruction
{
  string label;
  string type;
  string op;
  string rs;
  string rt;
  string rd;
  string shamt;
  string funct;
  string immediate;
  string wordAdress;
  unsigned int address = 0x00400000;
};

bool hasTextSection(string &line);
string parseLine(string line, int startPoint, int &lastPos);
string findOpcode(Instruction inst);
string findTypeOfInstruct(string input);
string findFunct(Instruction inst);
int findAdress(vector<Instruction> instructions, string label);
string binaryToHex(string binaryStr);
string decimalToBinary(string inputString);
string decimalToBinary16(string immediateValue);
string decimalToBinary24(string immediateValue);
string hexToBinary(string hexString);
string hexDigitToBinary(char hexDigit);
string intToHex(int address);
void printAllInstructions(vector<Instruction> instructions, ofstream &outputFile);

// Main function for MIPS assembly code conversion
int main()
{
  string filename = "mips.asm";
  fstream file(filename);

  // Vector to store parsed MIPS instructions
  vector<Instruction> instructions;

  // Check if the file is successfully opened
  if (file.is_open())
  {
    string line, tmp;
    bool textFlag = false; // Flag variable for finding ".text" section
    int lastPos = 0;       // Variable for char position in a line

    // Starting address for instructions in hexadecimal
    unsigned int adressCounter = 0x00400000;

    // Read each line in the MIPS assembly file
    while (getline(file, line))
    {
      // Check if the current line is within the ".text" section
      textFlag = hasTextSection(line);

      // Process lines within the ".text" section
      if (textFlag == true)
      {
        // Process each line within the ".text" section
        while (getline(file, line))
        {
          Instruction inst;

          // Parse the current line to identify the instruction type
          tmp = parseLine(line, lastPos, lastPos);

          // Check if the parsed line contains a label
          if (tmp[tmp.size() - 1] == ':')
          {
            inst.label = tmp;
            inst.type = parseLine(line, lastPos, lastPos);
          }
          else
          {
            inst.type = tmp;
          }

          // Based on the instruction type, parse and populate the Instruction struct
          if (findTypeOfInstruct(inst.type) == "R") // Handle R-type instructions
          {
            if (inst.type == "sll")
            {
              // Process 'sll' instruction
              inst.op = findOpcode(inst);
              inst.rd = decimalToBinary(parseLine(line, lastPos, lastPos));
              inst.rt = decimalToBinary(parseLine(line, lastPos, lastPos));
              inst.shamt = (parseLine(line, lastPos, lastPos));
              inst.rs = "00000";
              inst.funct = "000000";
              inst.address = adressCounter;
            }
            else
            {
              // Process other R-type instructions
              inst.op = findOpcode(inst);
              inst.rd = decimalToBinary(parseLine(line, lastPos, lastPos));
              inst.rs = decimalToBinary(parseLine(line, lastPos, lastPos));
              inst.rt = decimalToBinary(parseLine(line, lastPos, lastPos));
              inst.shamt = "00000";
              inst.funct = findFunct(inst);
              inst.address = adressCounter;
            }

            // Add the parsed instruction to the vector
            instructions.push_back(inst);

            // Increment the address counter for the next instruction
            adressCounter += 4;

            // Reset the last position marker for parsing the next line
            lastPos = 0;
          }
          else if (findTypeOfInstruct(inst.type) == "I")
          {
            // Handle I-type instructions
            inst.op = findOpcode(inst);
            inst.rt = parseLine(line, lastPos, lastPos);
            inst.rs = parseLine(line, lastPos, lastPos);
            inst.immediate = parseLine(line, lastPos, lastPos);
            inst.address = adressCounter;

            instructions.push_back(inst);
            adressCounter += 4;
            lastPos = 0;
          }
          else if (findTypeOfInstruct(inst.type) == "J")
          {
            // Handle J-type instructions
            inst.op = findOpcode(inst);
            inst.wordAdress = parseLine(line, lastPos, lastPos);
            inst.address = adressCounter;

            instructions.push_back(inst);
            adressCounter += 4;
            lastPos = 0;
          }
          else // Handle errors related to unsupported instruction types
          {
            cerr << "Instruction type error!" << endl;
          }

          // If the parsed instruction type is not empty, update its address
          if (inst.type != "")
          {
            inst.address = adressCounter;
          }
        }
      }
      else
      {
        // Skip lines that are not within the ".text" section
        continue;
      }
    }
  }
  else // Handle errors related to the inability to open the input file
  {
    cerr << "Error! Unable to open file" << endl;
    return -1;
  }

  // Open an output file for writing the object code
  ofstream outputFile("output.obj");

  if (outputFile.is_open())
  {
    printAllInstructions(instructions, outputFile);

    outputFile.close();
  }
  else // Handle errors related to the inability to open the output file
  {
    cerr << "Error! Unable to open output file" << endl;
    return -1;
  }

  return 0;
}

// Function to check if a line contains the ".text" section
bool hasTextSection(string &line)
{
  int foundText = line.find(".text");
  return (foundText != string::npos);
}

string parseLine(string line, int startPoint, int &lastPos)
{
  string result = "";

  // If starting position and last position are equal, assign the last position
  if (lastPos == startPoint && startPoint > 0)
  {
    startPoint = lastPos + 1;
  }

  // Iterate through the line characters
  for (int i = startPoint; i < line.size(); i++)
  {
    // Skip whole line if '#' is encountered
    if (line[i] == '#')
    {
      lastPos = i;
      return result;
    }

    // Skip whitespaces, tabs, and commas
    if (line[i] == ' ' || line[i] == '\t' || line[i] == ',')
    {
      lastPos = i;
      continue;
    }

    // Skip dollar sign
    if (line[i] == '$')
    {
      lastPos = i;
      continue;
    }
    else
    {
      // Append the character to the result string
      result += line[i];
      lastPos = i;

      // If the next character is whitespace, tab, newline, or comma, store the last position
      if (line[i + 1] == ' ' || line[i + 1] == '\t' || line[i + 1] == '\n' || line[i + 1] == ',')
      {
        lastPos = i;
        return result;
      }
    }
  }

  // If the result is an empty string, return "null"
  if (result == "")
  {
    return "null";
  }

  // If the end of the line is reached, store the last position
  lastPos = 0;
  return result;
}

string findTypeOfInstruct(string type)
{
  if (type == "add" || type == "sub" || type == "and" || type == "or" || type == "sll")
  {
    return "R";
  }
  if (type == "addi" || type == "andi" || type == "beq" || type == "bne")
  {
    return "I";
  }
  if (type == "j")
  {
    return "J";
  }
  return "Type Error!";
}

void printAllInstructions(vector<Instruction> instructions, ofstream &outputFile)
{
  string tmp;
  stringstream ss;
  ss << "ADDRESS\t    CODE\n";
  for (Instruction inst : instructions)
  {
    string input;
    if (inst.type == "add" || inst.type == "sub" || inst.type == "and" || inst.type == "or")
    {
      input = inst.op + inst.rs + inst.rt + inst.rd + inst.shamt + inst.funct;
      ss << intToHex(inst.address) << "\t" << binaryToHex(input) << endl;
    }
    else if (inst.type == "addi" || inst.type == "andi")
    {
      input = inst.op + decimalToBinary(inst.rs) + decimalToBinary(inst.rt) +
              decimalToBinary16(inst.immediate);
      ss << intToHex(inst.address) << "\t" << binaryToHex(input) << endl;
    }
    else if (inst.type == "beq" || inst.type == "bne")
    {
      for (Instruction inst_ : instructions)
      {
        if (!inst_.label.empty() && inst_.label.find(':') != string::npos && inst_.label.substr(0, inst_.label.find(':')) == inst.immediate)
        {
          tmp = to_string((inst_.address - inst.address - 4) / 4);
          input = inst.op + decimalToBinary(inst.rt) + decimalToBinary(inst.rs) + decimalToBinary16(tmp);
          ss << intToHex(inst.address) << "\t" << binaryToHex(input) << endl;
        }
      }
    }
    else if (inst.type == "sll")
    {
      input = inst.op + "00000" + inst.rt + inst.rd + decimalToBinary(inst.shamt) + inst.funct;
      ss << intToHex(inst.address) << "\t" << binaryToHex(input) << endl;
    }
    else if (inst.type == "j")
    {
      for (Instruction inst_ : instructions)
      {
        if (!inst_.label.empty() && inst_.label.find(':') != string::npos && inst_.label.substr(0, inst_.label.find(':')) == inst.wordAdress)
        {
          tmp = intToHex(inst_.address).substr(3, -1);
          tmp = hexToBinary(tmp);
          input = inst.op + tmp;
          ss << intToHex(inst.address) << "\t" << binaryToHex(input) << endl;
        }
      }
    }
    else
    {
      cerr << "Type Error!";
    }
  }

  outputFile << ss.str();
}

int findAdress(vector<Instruction> instructions, string label)
{
  for (Instruction inst : instructions)
  {
    if (inst.label == label)
    {
      return inst.address;
    }
  }
  return 0;
}

string findFunct(Instruction inst)
{
  string local_funct = "?????";

  if (inst.type == "add")
  {
    local_funct = "100000";
  }
  else if (inst.type == "sub")
  {
    local_funct = "100010";
  }
  else if (inst.type == "and")
  {
    local_funct = "100100";
  }
  else if (inst.type == "or")
  {
    local_funct = "100101";
  }
  else if (inst.type == "sll")
  {
    local_funct = "000000";
  }

  return local_funct;
}

string findOpcode(Instruction inst)
{
  if (inst.type == "add" || inst.type == "sub" || inst.type == "and" || inst.type == "or" || inst.type == "sll")
  {
    return "000000";
  }
  else if (inst.type == "addi")
  {
    return "001000";
  }
  else if (inst.type == "andi")
  {
    return "001100";
  }
  else if (inst.type == "beq")
  {
    return "000100";
  }
  else if (inst.type == "bne")
  {
    return "000101";
  }
  else if (inst.type == "j")
  {
    return "000010";
  }
  else
  {
    return "Type Error";
  }
}

string decimalToBinary(string input)
{
  int decimal;
  try
  {
    decimal = stoi(input);
  }
  catch (const invalid_argument &e)
  {
    cerr << "Invalid input. Please provide a valid number.";
    return "?????";
  }
  catch (const out_of_range &e)
  {
    cerr << "Input out of range. Number should be between 0 and 31 (inclusive).";
    return "?????";
  }

  if (decimal <= 0 || decimal >= 31)
  {
    cerr << "Input out of range. Number should be between 0 and 31 (inclusive).";
    return "?????";
  }

  return bitset<5>(decimal).to_string();
}

string binaryToHex(string binaryStr)
{
  string hexOStr;
  string tmp = "0000";

  for (int j = 0; j < binaryStr.size(); j += 4)
  {
    tmp = binaryStr.substr(j, 4);
    if (tmp == "0000")
      hexOStr += "0";
    else if (tmp == "0001")
      hexOStr += "1";
    else if (tmp == "0010")
      hexOStr += "2";
    else if (tmp == "0011")
      hexOStr += "3";
    else if (tmp == "0100")
      hexOStr += "4";
    else if (tmp == "0101")
      hexOStr += "5";
    else if (tmp == "0110")
      hexOStr += "6";
    else if (tmp == "0111")
      hexOStr += "7";
    else if (tmp == "1000")
      hexOStr += "8";
    else if (tmp == "1001")
      hexOStr += "9";
    else if (tmp == "1010")
      hexOStr += "a";
    else if (tmp == "1011")
      hexOStr += "b";
    else if (tmp == "1100")
      hexOStr += "c";
    else if (tmp == "1101")
      hexOStr += "d";
    else if (tmp == "1110")
      hexOStr += "e";
    else if (tmp == "1111")
      hexOStr += "f";
    else
      continue;
  }

  return "0x" + hexOStr;
}

string decimalToBinary16(string inputString)
{
  int decimalValue = stoi(inputString);
  string binaryResult;

  if (decimalValue < 0)
  {
    // Handle negative numbers using 2's complement
    decimalValue &= 0xFFFF; // Ensure that it fits within 16 bits
    bitset<16> binaryBits(decimalValue);
    binaryResult = binaryBits.to_string();
  }
  else
  {
    // Handle positive numbers
    bitset<16> binaryBits(decimalValue);
    binaryResult = binaryBits.to_string();
  }

  return binaryResult;
}

string decimalToBinary24(string inputString)
{
  int decimalValue = stoi(inputString);
  string binaryResult;

  if (decimalValue < 0)
  {
    // Handle negative numbers using 2's complement
    decimalValue &= 0xFFFF; // Ensure that it fits within 16 bits
    bitset<16> binaryBits(decimalValue);
    binaryResult = binaryBits.to_string();
  }
  else
  {
    // Handle positive numbers
    bitset<24> binaryBits(decimalValue);
    binaryResult = binaryBits.to_string();
  }

  return binaryResult;
}

string hexDigitToBinary(char hexDigit)
{
  static unordered_map<char, string> hexToBinary = {
      {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"}, {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"}, {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"}, {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"}};

  hexDigit = toupper(hexDigit);
  return hexToBinary[hexDigit];
}

string hexToBinary(string hexString)
{
  string binaryString;

  for (char hexDigit : hexString)
  {
    if (hexDigit == 'x' || hexDigit == 'X')
    {
      continue;
    }
    binaryString += hexDigitToBinary(hexDigit);
  }

  return binaryString;
}

string intToHex(int address)
{
  stringstream stream;
  stream << "0x" << hex << setw(8) << setfill('0') << address;
  return stream.str();
}