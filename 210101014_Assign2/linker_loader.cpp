#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<string>
#include<algorithm>
#include<sstream>
#include<iomanip>
#include<set>


using namespace std;

// helper functions
string strip(string txt)
{
	txt.erase(remove(txt.begin(), txt.end(), ' '), txt.end());
	return txt;
}

string format_number(int number, int w, char padding = '0')
{
	stringstream ss;
	ss <<  std::hex << std::uppercase << std::setfill(padding) << std::setw(w) << number;
	return ss.str();
}

string format_string(string txt1, int w, char padding = ' ')
{
	stringstream ss;
	ss << std::left << std::setfill(padding) << std::setw(w) << txt1;
	return ss.str();
}

bool is_number(string s)
{
	auto it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}


int PROGADDR;
int LAST;
vector<string> memory((1 << 18), "xx");

map<string,int> ESTAB;

vector<pair<string,int> > get_symbols(string txtRcd)
{
	vector<pair<string, int> > VecSym;
	int n = txtRcd.length();
	for(int i = 1; i < n; i += 12)
	{
		string symbol = txtRcd.substr(i, 6);
		int value = stoi(txtRcd.substr(i + 6, 6), nullptr, 16);
		VecSym.push_back(make_pair(symbol, value));
	}
	return VecSym; 
}

string format_string2(string txt1, int w, char padding = ' ')
{
	stringstream ss;
	ss << std::left << std::setfill(padding) << std::setw(w) << txt1;
	return ss.str();
}

void pass_1(string input)
{
	fstream inPtrFile;
	
	// open input file
	inPtrFile.open(input, ios::in);

	if(!inPtrFile.is_open())
	{
		perror(input.c_str());
		exit(1);
	}

	// Take program address as input from the user in hex
	string progaddr;
	cout << "Enter PROGADDR for the program: ";
	cin >> progaddr;
	PROGADDR = stoi(progaddr, nullptr, 16);

	int CSADDR = PROGADDR;
	int CSLTH = 0;
	string txtRcd;

	while(inPtrFile.good())
	{
		// read txtRcd
		getline(inPtrFile, txtRcd);

		// end of input
		if(txtRcd.empty())
			break;

		if(txtRcd.front() == 'H')
		{
			// Update CSADDR
			CSADDR = CSADDR + CSLTH;

			// CSECT and CSLTH from the txtRcd
			string CSECT = txtRcd.substr(1, 6);
			CSLTH = stoi(txtRcd.substr(13, 6), nullptr, 16);

			// Enter the CSECT to ESTAB
			if(ESTAB.find(CSECT) == ESTAB.end())
				ESTAB.insert(make_pair(CSECT, CSADDR));
			else
			{
				perror("duplicate external symbol");
				exit(1);
			}
		}

		if(txtRcd.front() == 'D')
		{
			// get the symbols in the txtRcd
			for(pair<string,int> symbol : get_symbols(txtRcd))
			{
				if(ESTAB.find(symbol.first) == ESTAB.end())
					ESTAB.insert(make_pair(symbol.first, symbol.second + CSADDR));
				else
				{
					perror("duplicate external symbol");
					exit(1);
				}
			}
		}

	}
	LAST = CSADDR + CSLTH;
}

void pass_2(string input)
{
	fstream inPtrFile;
	
	// open input file
	inPtrFile.open(input, ios::in);

	if(!inPtrFile.is_open())
	{
		perror(input.c_str());
		exit(1);
	}


	int CSADDR = PROGADDR;
	int EXECADDR = PROGADDR;
	int CSLTH = 0;

	string txtRcd;
	while(inPtrFile.good())
	{
		getline(inPtrFile, txtRcd);

		// if end of input
		if(txtRcd.empty())
			break;

		if(txtRcd.front() == 'H')
			CSLTH = stoi(txtRcd.substr(13, 6), nullptr, 16);
		
		if(txtRcd.front() == 'T')
		{
			// move the txtRcd to its appropriate memory location byte by byte
			int STADDR = stoi(txtRcd.substr(1, 6), nullptr, 16) + CSADDR;
			int INDEX = 0;
			for(int i = 9; i < (int)txtRcd.length(); i += 2)
				memory[STADDR + INDEX++] = txtRcd.substr(i, 2); 
		}

		else if(txtRcd.front() == 'M')
		{
			string symbol = txtRcd.substr(10, 6);
			if(ESTAB.find(symbol) != ESTAB.end())
			{
				// extract address to be modified
				int address = stoi(txtRcd.substr(1, 6), nullptr, 16) + CSADDR;
				int length = stoi(txtRcd.substr(7, 2), nullptr, 16);

				char halfByte;
				if(length % 2)
					halfByte = memory[address][0];

				string val = "";
				for(int i = 0; i < (length + 1)/ 2; i++)
					val += memory[address + i];
				int value = stoi(val, nullptr, 16);

				cout << "value          = " << val << '\n';
				
				// apply modification
				int modification = ESTAB[symbol];
				char sign = txtRcd[9];
				if(sign == '+')
					value += modification;
				else
					value -= modification;

				// apply mask	
				int mask = 0;
				for(int i = 0; i < (length + length % 2) * 4; i++)
				{
					mask |= 1;
					mask = mask << 1;
				}
				mask = mask >> 1;
				value = value & mask;
				
				// write back the modified value
				val = format_number(value, length + (length % 2));
				cout << "modification   = " << format_number(modification, 6) << '\n';
				cout << "modified value = " << val << '\n';
				for(int i = 0; i < length; i += 2)
					memory[address + i / 2] = val.substr(i, 2);

				if(length % 2)
					memory[address][0] = halfByte;

			}
			else
			{
				cout << symbol << '\n';
				perror("undefined symbol");
				exit(1);
			}
		}
		if(txtRcd.front() == 'q')
		{
			if(txtRcd != "q")
			{
				int SECOND = stoi(txtRcd.substr(1, 6), nullptr, 16);
				EXECADDR = CSADDR + SECOND;
			}
			CSADDR = CSLTH + CSADDR;
		}

		if(txtRcd.front() == 'E')
		{
			if(txtRcd != "E")
			{
				int FIRST = stoi(txtRcd.substr(1, 6), nullptr, 16);
				EXECADDR = CSADDR + FIRST;
			}
			CSADDR = CSLTH + CSADDR;
		}

	}
	
	cout << "Starting execution at: " << format_number(EXECADDR, 4) << '\n';

}

void memoryFormat()
{
	fstream outPtrFile;
	// open memory file
	outPtrFile.open("memory.txt", ios::out);
	int i1 = (PROGADDR / 16) * 16;
	int n1 = ((LAST + 16) / 16) * 16;
	while(i1 < n1) 
	{
		outPtrFile << format_number(i1, 4) << ' ';

		for(int j = 0; j < 4; j++)
		{
			for(int k = 0; k < 4; k++)
				outPtrFile << memory[i1++];
			outPtrFile << ' ';
		}
		outPtrFile << '\n';
	}

	// close the memory file
	outPtrFile.close();
}

void memLinePrint()
{
	fstream outPtrFile, inPtrFile;
	// open line by line file
	outPtrFile.open("line_by_line.txt", ios::out);


	// open listing file
	inPtrFile.open("listing.txt", ios::in);

	if(!inPtrFile.is_open())
	{
		perror("listing.txt");
		exit(1);
	}

	string txtLine;
	vector<string> instructions;
	while(inPtrFile.good())
	{
		getline(inPtrFile, txtLine);
		if(txtLine.empty())
			break;
		
		string obcode = strip(txtLine.substr(62, 10));
		if(!obcode.empty())
			instructions.push_back(obcode);
	}

	vector<string> answer, address;

	int j = 0, i = 0;
	while(i < (int)(memory.size()))
	{
		if(memory[i] != "xx")
		{
			string strr = "";
			address.push_back(format_number(i, 6));
			int size = instructions[j].length() / 2;
			for(int k = i; k < i + size; k++)
				strr += memory[k];
			answer.push_back(strr);
			j++; 
			i += size;
			
		}
		else
			i++;

	}	
	outPtrFile << format_string("address", 10) << format_string("instruction", 10) << '\n';
	for(int i = 0; i < (int)answer.size(); i++)
	{
		outPtrFile << format_string(address[i], 10) << format_string(answer[i], 10) << '\n';
	}

	// close the files
	outPtrFile.close();
	inPtrFile.close();
}
int main(int argc, char **argv)
{
	// check that the input is provided in the proper format
	if(argc != 2)
	{
		cout << "Usage: ./a.out {source file}\n";
		return 0;
	}

	// 2-pass assembler running
	string input = argv[1];
	pass_1(input);      // 1st pass function called
	for(auto x : ESTAB)
		cout << x.first << ' ' << format_number(x.second, 4) << '\n';
	pass_2(input);      // 2nd pass called 

	memoryFormat();
	memLinePrint();
	return 0;
}