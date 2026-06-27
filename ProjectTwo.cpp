//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Ashley Pleasant
// Description : ABCU Course Advising Program
//============================================================================

//Instructions:
// Compile on Mac/Linux/Codio:
// g++ -std=c++11 ProjectTwo.cpp -o ProjectTwo
// Run:
// ./ProjectTwo
//
// Compile on Windows with g++:
// g++ -std=c++11 ProjectTwo.cpp -o ProjectTwo.exe
// Run:
// ProjectTwo.exe
//
// When prompted, enter the course input file name:
// CS 300 ABCU_Advising_Program_Input.csv




// Included necessary libraries for the program.

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


// Structure holds information for one course.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Trim spaces from the beginning and end of a string.
string trim(const string& value) {
    size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }

    size_t last = value.find_last_not_of(" \t\r\n")ABCU_course_input;
    return value.substr(first, last - first + 1);
}

// Convert course numbers to uppercase so searches are not case-sensitive.
string toUpperCase(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char ch) { return static_cast<char>(toupper(ch)); });
    return value;
}

// Split a line from the input file by commas.
vector<string> splitLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

//Binary Tree used which was recommended structure for this project.
// Binary search tree used to store and print courses in alphanumeric order.
class BinarySearchTree {
private:
    struct Node {
        Course course;
        Node* left;
        Node* right;

        Node(Course course) {
            this->course = course;
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root;

    void addNode(Node*& node, Course course) {
        if (node == nullptr) {
            node = new Node(course);
        } else if (course.courseNumber < node->course.courseNumber) {
            addNode(node->left, course);
        } else {
            addNode(node->right, course);
        }
    }

    void inOrder(Node* node) const {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            inOrder(node->right);
        }
    }

    Course* searchNode(Node* node, const string& courseNumber) const {
        if (node == nullptr) {
            return nullptr;
        }

        if (node->course.courseNumber == courseNumber) {
            return &(node->course);
        }

        if (courseNumber < node->course.courseNumber) {
            return searchNode(node->left, courseNumber);
        }

        return searchNode(node->right, courseNumber);
    }

    void deleteTree(Node* node) {
        if (node != nullptr) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    ~BinarySearchTree() {
        clear();
    }

    void clear() {
        deleteTree(root);
        root = nullptr;
    }

    void insert(Course course) {
        addNode(root, course);
    }

    Course* search(const string& courseNumber) const {
        return searchNode(root, courseNumber);
    }

    void printCourseList() const {
        inOrder(root);
    }

    bool isEmpty() const {
        return root == nullptr;
    }
};

// Print the menu options for the user.
void printMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

// Load courses from a file into the binary search tree.
bool loadCourses(const string& fileName, BinarySearchTree& courseTree) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: The file could not be opened. Please check the file name and try again." << endl;
        return false;
    }

    vector<Course> courses;
    vector<string> courseNumbers;
    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        ++lineNumber;
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        vector<string> tokens = splitLine(line);

        // Each line must have at least a course number and a course title.
        if (tokens.size() < 2 || tokens[0].empty() || tokens[1].empty()) {
            cout << "Error: Line " << lineNumber << " is not formatted correctly." << endl;
            return false;
        }

        Course course;
        course.courseNumber = toUpperCase(tokens[0]);
        course.courseTitle = tokens[1];

        for (size_t i = 2; i < tokens.size(); ++i) {
            string prerequisite = toUpperCase(trim(tokens[i]));

            if (!prerequisite.empty()) {
                course.prerequisites.push_back(prerequisite);
            }
        }

        courses.push_back(course);
        courseNumbers.push_back(course.courseNumber);
    }

    inputFile.close();

    // Validate that each prerequisite exists as its own course in the file.
    for (const Course& course : courses) {
        for (const string& prerequisite : course.prerequisites) {
            if (find(courseNumbers.begin(), courseNumbers.end(), prerequisite) == courseNumbers.end()) {
                cout << "Error: Prerequisite " << prerequisite << " for course "
                     << course.courseNumber << " does not exist in the file." << endl;
                cout << "Course data was not loaded." << endl;
                return false;
            }
        }
    }

    // Clear any previous data before loading new data.
    courseTree.clear();

    for (const Course& course : courses) {
        courseTree.insert(course);
    }

    cout << courses.size() << " courses loaded successfully." << endl;
    return true;
}

// Print one course and its prerequisite information.
void printCourseInformation(const BinarySearchTree& courseTree, const string& courseNumber) {
    Course* course = courseTree.search(courseNumber);

    if (course == nullptr) {
        cout << "Course not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites:" << endl;

    for (const string& prerequisiteNumber : course->prerequisites) {
        Course* prerequisite = courseTree.search(prerequisiteNumber);

        if (prerequisite != nullptr) {
            cout << prerequisite->courseNumber << ", " << prerequisite->courseTitle << endl;
        } else {
            // This should not happen if validation passed, but it protects output if data changes.
            cout << prerequisiteNumber << ", Title not found" << endl;
        }
    }
}

int main() {
    BinarySearchTree courseTree;
    bool dataLoaded = false;
    string userInput;

    cout << "Welcome to the course planner." << endl;

    while (true) {
        printMenu();
        cout << "What would you like to do? ";
        getline(cin, userInput);

        int choice;

        try {
            choice = stoi(userInput);
        } catch (...) {
            cout << userInput << " is not a valid option." << endl;
            continue;
        }

        if (choice == 1) {
            string fileName;
            cout << "Enter the course data file name: ";
            getline(cin, fileName);

            dataLoaded = loadCourses(fileName, courseTree);
        } else if (choice == 2) {
            if (!dataLoaded || courseTree.isEmpty()) {
                cout << "Please load the course data first." << endl;
            } else {
                cout << "Here is a sample schedule:" << endl;
                courseTree.printCourseList();
            }
        } else if (choice == 3) {
            if (!dataLoaded || courseTree.isEmpty()) {
                cout << "Please load the course data first." << endl;
            } else {
                string courseNumber;
                cout << "What course do you want to know about? ";
                getline(cin, courseNumber);
                printCourseInformation(courseTree, toUpperCase(trim(courseNumber)));
            }
        } else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
            break;
        } else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
