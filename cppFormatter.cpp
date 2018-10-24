#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Return a string that strip the spaces and TABs from the beginning and the end of the original string
string strip(const string &s)
{
	int head = 0, tail = s.length() - 1;
	string stripped;
	while (head < s.length() && (s[head] == ' ' || s[head] == '\t'))
		head++;
	if (head == s.length())		// The whole string is stripped
		return stripped;
	while (tail >= 0 && (s[tail] == ' ' || s[tail] == '\t'))
		tail--;
	for (int i = head; i <= tail; i++)
		stripped += s[i];
	return stripped;
}

// Return a string that repeats the string s n times
string operator * (const string &s, const int n)
{
	string tmp;
	if (n >= 0)
	{
		for (int i = 0; i < n; i++)
		{
			tmp += s;
		}
	}
	return tmp;
}

// Return the number of character c in string s
int count(const string &s, const char c)
{
	int count = 0;
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] == c)
			count++;
	}
	return count;
}

// To check if there is any comment in that line
int hasComment(const string &s)
{
	int pos = s.find("//");
	if (pos != string::npos)	// There is comment in that line
	{
		if (pos == 0)	// The whole line is comment
			return pos;
		else	// There are codes and comments in the same line
		{
			pos--;
			while (pos >= 0 && (s[pos] == ' ' || s[pos] == '\t'))
				pos--;
			return pos + 1;	// Return the index of the beginning of comment (Including TABs and spaces)
		}
	}
	else
		return -1;	// No comment
}

//	Seperate the comment from the original string and return the comment part (Including TABs and spaces)
string stripAndGetComment(string &s)
{
	string tmp, comment;
	int idx = hasComment(s);
	if (idx > 0)
	{
		for (int i = idx; i < s.length(); i++)
			comment += s[i];
		for (int i = 0; i < idx; i++)
			tmp += s[i];
		s = tmp;
	}
	return comment;
}

// Return true if there is any key word that doesn't have to have curly bracket
bool hasKeyWord(const string &s)
{
	if ((count(s, '{') == 0) && (count(s, '}') == 0))
	{
		if (s.length() > 0 && s.find("for") == 0 && s[s.length() - 1] != ';')
			return true;
		else if (s.length() > 0 && s.find("if") == 0 && s[s.length() - 1] != ';')
			return true;
		else if (s.length() > 0 && s.find("else if") == 0 && s[s.length() - 1] != ';')
			return true;
		else if (s.length() > 0 && s.find("else") == 0 && s[s.length() - 1] != ';')
			return true;
		else if (s.length() > 0 && s.find("while") == 0 && s[s.length() - 1] != ';')
			return true;
		else
			return false;
	}
	else
		return false;
}

// To check if there is a new scope
int isNewScope(const string &s)
{
	int lBracket = 0;		// Left curly bracket
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] == '{')
			lBracket++;
		if (s[i] == '}')
			lBracket--;		// '}' pairs one left curly bracket
	}
	if (lBracket > 0)	// There are more '{' than '}', means we're entering a new scope
		return 1;
	else if (lBracket == 0)	// No curly bracket at all or the numbers of them are the same
		return 0;
	else	// More '}' than '{', means we are getting out of a scope
		return -1;
}

int main()
{
	int numOfTabs = 0;		// Number of TABs
	int nested = 0;		// Used to count the number of TABs in nested statement without curly bracket
	bool nonBracketKeyWord = false;	// True if there is any "nonBracketKeyWord" in the previous non-empty line
	bool display = false;	// It will display everything in the console if it is true

	// Input the file name
	string fileInName, fileOutName;
	do
	{
		cout << "Input the file name you want to format: ";
		cin >> fileInName;
	} while (fileInName.empty());
	// fileInName = "TestFile.cpp";		// Test
	fileOutName = "Output_" + fileInName;

	// Openning files
	fstream fin(fileInName, ios::in);

	if (fin.is_open())
	{
		fstream fout(fileOutName, ios::out);
		if (fout.is_open())
		{
			while (!fin.eof())
			{
				string buffer;		// Used to storage the information that read from the files
				string indentation;	// Used to storage the new indentation
				string comment;		// Used to storage the comment (if any)
				string tab = "\t";	// TAB as string

				getline(fin, buffer);	// Read one line from the file
				buffer = strip(buffer);	// Get rid of the tabs and spaces

				if (hasComment(buffer) > 0)	// With codes and comments in the same line
					comment = stripAndGetComment(buffer);
				else if (hasComment(buffer) == 0)	// The whole line is comment
				{
					indentation = tab * numOfTabs;
					buffer = indentation + buffer;
					if (display)
						cout << buffer << endl;
					fout << buffer << endl;
					continue;
				}

				switch (isNewScope(buffer))	// To check if there is a new scope
				{
				case 1:		// Entering a new scope, add one '\t' AFTER writting this line
					{
						indentation = tab * numOfTabs;
						numOfTabs++;

						// Write it into the new file
						buffer = indentation + buffer + comment;
						if (display)
							cout << buffer << endl;
						fout << buffer << endl;
						break;
					}
				case -1:	// Getting out of a scope, omit one '\t' BEFORE writting this line
					{
						numOfTabs--;
						indentation = tab * numOfTabs;

						// Write it into the new file
						buffer = indentation + buffer + comment;
						if (display)
							cout << buffer << endl;
						fout << buffer << endl;
						break;
					}
				case 0:		// Doesn't have extra curly bracket that can indicate the existance of a new scope
					{
						if (nonBracketKeyWord)	// Has "nonBracketKeyWord" in the previous line
						{
							if (buffer.length() != 0)	// Not empty
							{
								nested++;
								indentation = tab * (numOfTabs + nested);
								if (!hasKeyWord(strip(buffer)))
									nested = 0;
								buffer = indentation + buffer + comment;
								if (display)
									cout << buffer << endl;
								fout << buffer << endl;
							}
							else	// Carriage return, no indentation is needed
							{
								if (display)
									cout << endl;
								fout << endl;
								continue;
							}
						}
						else	// No "nonBracketKeyWord" in the previous line
						{
							// Special cases for "case", "private" and "public"
							if (buffer.find("case") == 0)
								indentation = tab * (numOfTabs - 1);
							else if (buffer.find("private") == 0)
								indentation = tab * (numOfTabs - 1);
							else if (buffer.find("public") == 0)
								indentation = tab * (numOfTabs - 1);
							else
								indentation = tab * numOfTabs;

							buffer = indentation + buffer + comment;
							if (display)
								cout << buffer << endl;
							fout << buffer << endl;
						}
						break;
					}
				}
				stripAndGetComment(buffer);
				nonBracketKeyWord = hasKeyWord(strip(buffer));
			}
			fout.close();
			cout << "---------------------------------------------------------------" << endl;
			cout << "Saved as " + fileOutName << endl;
		}
		else
		{
			cout << "Cannot create file" << endl;
			system("pause");
			return 1;
		}
		fin.close();
	}
	else
	{
		cout << "Cannot open file" << endl;
		system("pause");
		return 1;
	}

	system("pause");
	return 0;
}