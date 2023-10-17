#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <limits>

using namespace std;

const int MAX_EXPRESSION_SIZE = 65, LETTER_OFFSET_IN_ASCII = 32,
BORDER_LENGTH = (MAX_EXPRESSION_SIZE + 1) * 2 + 11;

enum Menu {
	PRINT_TO_FILE_AND_SCREEN = 1, QUIT = 2
};


//linked queue
struct Node {
	char letter;
	Node* link;
};

class LinkedQueue {
public:
	const LinkedQueue& operator=(const LinkedQueue& otherQueue);

	bool isEmptyQueue() const { return queueFront == nullptr; }

	bool isFullQueue() const { return false; }

	void initializeQueue();

	char front() const;

	char back() const;

	void addQueue(char);

	void deleteQueue();

	LinkedQueue();

	LinkedQueue(const LinkedQueue& otherQuue);

	~LinkedQueue() { initializeQueue(); }

	string ToString();
private:
	Node* queueFront;
	Node* queueRear;
};



class EmptyStack {};

class FullStack {};

class Stack {
public:
	Stack() { InitializeStack(); }

	void InitializeStack() { stackTop = 0; }

	bool IsEmpty() const { return (stackTop == 0); }

	bool IsFullStack() const { return (stackTop == MAX_EXPRESSION_SIZE); }

	void Push(char);

	char Peek() const;

	void Pop();

	string ToString();
private:
	char expression[MAX_EXPRESSION_SIZE];
	int stackTop;
};

void ClearInvalidInput(string errMsg); //clears cin, clears the keyboard buffer, prints an error message

//prints the stack in reverse order to the file and screen
void PrintToFileAndScreen(string inFileName, string outFileName, string border, string header);

string ProcessString(string); //removes all non alpha characters and apostrophes

string ToLower(string); //converts string to lowercase

void PopulateStack(Stack&, string); // creates a stack filled with the processed string

void LoadQueue(LinkedQueue&, string);

int main() {
	int userChoice;
	string inFileName = "Test_Data.txt", outFileName = "OutputFile.txt",
		border, header;
	stringstream headerSS, borderSS;

	borderSS << setfill('-') << setw(BORDER_LENGTH) << "\n";
	headerSS << left << setw(MAX_EXPRESSION_SIZE + 1) << "INPUT" << setw(MAX_EXPRESSION_SIZE) << "REVERSED" << "PALINDROME";

	border = borderSS.str();
	header = headerSS.str();

	cout << left;

	do {
		cout << "\n\nMenu:\n"
			"1. Print expressions to a file and screen\n"
			"2. Quit\n";
		cin >> userChoice;

		switch (userChoice) {
		case PRINT_TO_FILE_AND_SCREEN:
			try {
				PrintToFileAndScreen(inFileName, outFileName, border, header);
			}
			catch (const EmptyStack&) {
				cout << "Exception caught: Unable to pop or peek from an empty stack.\n";
			}
			catch (const FullStack&) {
				cout << "Exception caught: Unable to push onto a full stack.\n";
			}
			break;
		case QUIT:
			cout << "QUITTING ..." << endl;
			break;
		default:
			ClearInvalidInput("INVALID INPUT");
		}

	} while (userChoice != QUIT);

}


const LinkedQueue& LinkedQueue::operator=(const LinkedQueue& otherQueue) {
}

LinkedQueue::LinkedQueue(const LinkedQueue& otherQueue) {

}

void LinkedQueue::initializeQueue() {
	Node* temp;

	while (queueFront != nullptr) {
		temp = queueFront;
		queueFront = queueFront->link;
		delete temp;
		//temp = nullptr;
	}
	queueRear = nullptr;
}

void LinkedQueue::addQueue(char newLetter) {
	Node* newNode;
	newNode = new Node;
	newNode->letter = newLetter;
	newNode->link = nullptr;

	if (queueFront == nullptr) {
		queueFront = newNode;
		queueRear = newNode;
	}
	else {
		queueRear->link = newNode;
		queueRear = queueRear->link;
	}
}

char LinkedQueue::front() const {
	assert(queueFront != nullptr);
	return queueFront->letter;
}

char LinkedQueue::back() const {
	assert(queueRear != nullptr);
	return queueRear->letter;
}

void LinkedQueue::deleteQueue() {
	Node* temp;
	
	if (!isEmptyQueue()) {
		temp = queueFront;
		queueFront = queueFront->link;
		delete temp;
		//temp = nullptr;

		if (queueFront == nullptr) {
			queueRear = nullptr;
		}
	}
	else {
		//throw exception
	}
}

LinkedQueue::LinkedQueue() {
	queueFront = nullptr;
	queueRear = nullptr;
}

string LinkedQueue::ToString() {
	stringstream ss;
	while (!isEmptyQueue()) {
		ss << front();
		deleteQueue();
	}
	ss << setw(MAX_EXPRESSION_SIZE);
	return ss.str();
}

string Stack::ToString() {
	stringstream ss;
	int size = stackTop;
	for (int i = 0; i < size; i++) {
		ss << Peek();
		Pop();
	}
	ss << setw(MAX_EXPRESSION_SIZE);
	return ss.str();
}

void Stack::Push(char newChar) {
	if (!IsFullStack()) {
		expression[stackTop] = newChar;
		stackTop++;
	}
	else {
		throw FullStack();
	}
}

char Stack::Peek() const {
	if (!IsEmpty()) {
		return expression[stackTop - 1];
	}
	else {
		throw EmptyStack();
	}
}

void Stack::Pop() {
	if (!IsEmpty()) {
		stackTop--;
	}
	else {
		throw EmptyStack();
	}
}

void ClearInvalidInput(string errMsg) {
	cout << "\n" << errMsg << "\n";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void PrintToFileAndScreen(string inputFileName, string outputFileName, string border, string header) {
	ifstream inputFile(inputFileName);
	ofstream outFile(outputFileName);
	string tempString, reversedString, originalString, isPalindrome;
	Stack tempStack;
	LinkedQueue tempQueue, tempQueue1;

	if (!inputFile) {
		cout << "Input file not found. Exiting the program." << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	if (inputFile.peek() == EOF) {
		cout << "The input file is empty. Quitting the program." << endl;
		inputFile.close();
		system("pause");
		exit(EXIT_FAILURE);
	}

	outFile << left << header << "\n";
	cout << "\n" << header << "\n" << border;

	while (!inputFile.eof()) {
		getline(inputFile, tempString);
		if (tempString.length() <= MAX_EXPRESSION_SIZE) {
			PopulateStack(tempStack, tempString);
			LoadQueue(tempQueue, tempString);

			reversedString = tempStack.ToString();
			originalString = tempQueue.ToString();

			if (reversedString == originalString) {
				isPalindrome = "YES";
			}
			else {
				isPalindrome = "NO";
			}

			//prints to file
			outFile << setw(MAX_EXPRESSION_SIZE + 1) << tempString
				<< setw(MAX_EXPRESSION_SIZE) << reversedString << isPalindrome << "\n";

			//prints to screen
			cout << setw(MAX_EXPRESSION_SIZE + 1) << tempString
				<< setw(MAX_EXPRESSION_SIZE) << reversedString << isPalindrome <<"\n";
		}

	}

	cout << border;
	inputFile.close();
	outFile.close();
}

void PopulateStack(Stack& stack, string expression) {
	expression = ToLower(expression);
	expression = ProcessString(expression);

	int strLen = expression.length();

	for (int i = 0; i < strLen; i++) {
		stack.Push(expression[i]);
	}

}

void LoadQueue(LinkedQueue& queue, string expression) {
	expression = ToLower(expression);
	expression = ProcessString(expression);

	int strLen = expression.length();

	for (int i = 0; i < strLen; i++) {
		char ch = expression[i];
		queue.addQueue(ch);
	}
}

string ProcessString(string str) {
	string processedString = "";
	int strLen = str.length();
	for (int i = 0; i < strLen; i++) {
		char ch = str[i];
		if (((ch >= 'a') && (ch <= 'z'))) {
			processedString += ch;
		}
		else if (ch == '\'') { //skips letter after apostrophe
			i++;
		}
	}
	return processedString;
}

string ToLower(string str) {
	string lowerCaseStr = "";
	for (int i = 0; i < str.length(); i++) {
		char ch = str[i];
		if (((ch >= 'A') && (ch <= 'Z'))) {
			ch += LETTER_OFFSET_IN_ASCII; //converts char to lower case
		}
		lowerCaseStr += ch;
	}
	return lowerCaseStr;
}

//TEST 1
/*

Menu:
1. Print expressions to a file and screen
2. Quit
1

INPUT                                                             REVERSED                                                         PALINDROME
----------------------------------------------------------------------------------------------------------------------------------------------
YES!!! you can dance!                                             ecnadnacuoysey                                                   NO
Hello, I'm Abbas                                                  sabbaiolleh                                                      NO
z y x w v u t s r q p o n m l k j i h g f e d c b a               abcdefghijklmnopqrstuvwxyz                                       NO
!!!!!!!!!!!!!!!!!!!!!A!!!!!!!!!!!!!!!!!!!!!!!                     a                                                                YES
This expression is exactly 65 characters long and should print___ tnirpdluohsdnagnolsretcarahcyltcaxesinoisserpxesiht              NO
 Matching cover pAGE, header, and sidebar.                        rabedisdnaredaehegaprevocgnihctam                                NO
I love to Code!!!!                                                edocotevoli                                                      NO
CS 136 is really fun                                              nufyllaersisc                                                    NO
D.S. Malik - C++ Programming                                      gnimmargorpckilamsd                                              NO
Borrow or rob?                                                    borroworrob                                                      YES
----------------------------------------------------------------------------------------------------------------------------------------------


Menu:
1. Print expressions to a file and screen
2. Quit
2
QUITTING ...
*/


//TEST 2
/*

Menu:
1. Print expressions to a file and screen
2. Quit
sdfad

INVALID INPUT


Menu:
1. Print expressions to a file and screen
2. Quit
2
QUITTING ...
*/

//TEST 3
/*
Menu:
1. Print expressions to a file and screen
2. Quit
1
Input file not found. Exiting the program.
Press any key to continue . . .
*/