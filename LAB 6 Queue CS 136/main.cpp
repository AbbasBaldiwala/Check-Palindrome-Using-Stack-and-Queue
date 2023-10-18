#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <limits>

using namespace std;

const int MAX_EXPRESSION_SIZE = 65, LETTER_OFFSET_IN_ASCII = 32,
BORDER_LENGTH = (MAX_EXPRESSION_SIZE + 1) * 2 + 10;

enum Menu {
	PRINT_TO_FILE_AND_SCREEN = 1, QUIT = 2
};


//LINKED QUEUE


class EmptyQueue {};

struct Node {
	char letter;
	Node* link;
};

class LinkedQueue {
public:
	const LinkedQueue& operator=(const LinkedQueue& otherQueue);

	bool IsEmptyQueue() const { return queueFront == nullptr; }

	bool IsFullQueue() const { return false; }

	void ClearQueue();

	char Front() const;

	char Back() const;

	void AddQueue(char);

	void DeQueue();

	LinkedQueue();

	LinkedQueue(const LinkedQueue& otherQueue);

	~LinkedQueue() { ClearQueue(); }

private:
	Node* queueFront = nullptr;
	Node* queueRear = nullptr;

	void copyQueue(const LinkedQueue& other);
};


//STACK


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

	int GetStackSize() { return stackTop; }
private:
	char expression[MAX_EXPRESSION_SIZE];
	int stackTop;
};


//FUNCTION DECLARATIONS


void ClearInvalidInput(string errMsg); //clears cin, clears the keyboard buffer, prints an error message

//prints the stack in reverse order to the file and screen
void PrintToFileAndScreen(string inFileName, string outFileName, string border, string header);

string ProcessString(string expression); //removes all non alpha characters and apostrophes

string ToLower(string expression); //converts string to lowercase

void PopulateStackAndQueue(Stack& stack, LinkedQueue& queue, string expression); // creates a stack filled with the processed string

bool IsPalindrome(LinkedQueue& queue, Stack& stack, string& reversedExpression); //checks if the expression is a palindrome


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
				std::cerr << "Exception caught: Unable to pop or peek from an empty stack.\n";
			}
			catch (const FullStack&) {
				std::cerr << "Exception caught: Unable to push onto a full stack.\n";
			}
			catch (const EmptyQueue&) {
				std::cerr << "Exception caught: Unable to delete from an empty queue.\n";
			}
			catch (const std::bad_alloc error) {
				std::cerr << "Exception caught, Could not allocate memory: " << error.what() << "\n";
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


//LINKED QUEUE CLASS DEFINITIONS


void LinkedQueue::copyQueue(const LinkedQueue& otherQueue) {
	Node* newNode;
	Node* current; 

	if (otherQueue.queueFront == nullptr){
		queueFront = nullptr;
		queueRear = nullptr;
	}
	else{
		ClearQueue();
		current = otherQueue.queueFront;
		queueFront = new Node; 
		queueFront->letter = current->letter;
		queueFront->link = nullptr;
		
		queueRear = queueFront; 
		
		current = current->link; 

		while (current != nullptr){
			newNode = new Node; 
			newNode->letter = current->letter; 
			newNode->link = nullptr; 
			
			queueRear->link = newNode; 
			queueRear = newNode; 
			
			current = current->link;
			
		}
	}
}

const LinkedQueue& LinkedQueue::operator=(const LinkedQueue& RHS) {
	if (this != &RHS)
	{
		copyQueue(RHS);
	}
	return *this;
}

LinkedQueue::LinkedQueue(const LinkedQueue& otherQueue) {
	copyQueue(otherQueue);
}

void LinkedQueue::ClearQueue() {
	Node* temp;

	while (queueFront != nullptr) {
		temp = queueFront;
		queueFront = queueFront->link;
		delete temp;
	}
	queueRear = nullptr;
}

void LinkedQueue::AddQueue(char newLetter) {
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

char LinkedQueue::Front() const {
	assert(queueFront != nullptr);
	return queueFront->letter;
}

char LinkedQueue::Back() const {
	assert(queueRear != nullptr);
	return queueRear->letter;
}

void LinkedQueue::DeQueue() {
	Node* temp;
	
	if (!IsEmptyQueue()) {
		temp = queueFront;
		queueFront = queueFront->link;
		delete temp;

		if (queueFront == nullptr) {
			queueRear = nullptr;
		}
	}
	else {
		throw EmptyQueue();
	}
}

LinkedQueue::LinkedQueue() {
	queueFront = nullptr;
	queueRear = nullptr;
}


//STACK CLASS DEFINITIONS


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


//MAIN DEFINITIONS


void ClearInvalidInput(string errMsg) {
	cout << "\n" << errMsg << "\n";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void PrintToFileAndScreen(string inputFileName, string outputFileName, string border, string header) {
	ifstream inputFile(inputFileName);
	ofstream outFile(outputFileName);
	string tempString, reversedString, originalString, yesOrNo = "";
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

			PopulateStackAndQueue(tempStack, tempQueue, tempString);

			if (IsPalindrome(tempQueue, tempStack, reversedString)) {
				yesOrNo = "YES";
			}
			else {
				yesOrNo = "NO";
			}
		
			//prints to file
			outFile << setw(MAX_EXPRESSION_SIZE + 1) << tempString
				<< setw(MAX_EXPRESSION_SIZE) << reversedString << yesOrNo << "\n";

			//prints to screen
			cout << setw(MAX_EXPRESSION_SIZE + 1) << tempString
				<< setw(MAX_EXPRESSION_SIZE) << reversedString << yesOrNo <<"\n";
		}

	}

	cout << border;
	inputFile.close();
	outFile.close();
}

void PopulateStackAndQueue(Stack& stack, LinkedQueue& queue, string expression) {
	expression = ToLower(expression);
	expression = ProcessString(expression);

	size_t strLen = expression.length();

	for (int i = 0; i < strLen; i++) {
		stack.Push(expression[i]);
		queue.AddQueue(expression[i]);
	}

}

bool IsPalindrome(LinkedQueue& queue, Stack& stack, string& reversedString) {
	int size = stack.GetStackSize();
	reversedString = "";
	char chFromStack, chFromQueue;
	bool isPalindrome = true;
	for (int i = 0; i < size; i++) {
		chFromStack = stack.Peek();
		stack.Pop();

		chFromQueue = queue.Front();
		queue.DeQueue();

		if (chFromStack != chFromQueue) {
			isPalindrome = false;
		}
		reversedString += chFromStack;
	}
	return isPalindrome;
}

string ProcessString(string str) {
	string processedString = "";
	size_t strLen = str.length();
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


// OUTPUT/TESTS
/***********************************************************************************************************************************************/


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
tA;co Cat                                                         tacocat                                                          YES
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


/***********************************************************************************************************************************************/