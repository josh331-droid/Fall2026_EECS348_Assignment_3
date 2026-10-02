/* ============================================================================
 * Program Name: EECS 348 Assignment 3 - CEO Email Priority Queue
 * Description: An object-oriented C++ program that prioritizes and manages 
 *              incoming emails for a company CEO using an array-based MaxHeap 
 *              implemented from scratch. Emails are prioritized by sender 
 *              rank (Boss > Subordinate > Peer > Important Person > Other Person) 
 *              and secondary date sorting (newest first). Supports NEXT, READ, 
 *              and COUNT commands from standard input.
 * Inputs: Standard input stream (std::cin) or redirected test file containing 
 *         EMAIL commands, COUNT, NEXT, and READ instructions.
 * Output: Standard output (std::cout) displaying email counts and detailed 
 *         top-priority email records as requested by commands.
 * Collaborators: None.
 * Other Sources: 
 *   - Claude 3.5 Sonnet (Anthropic API via Chatbox AI)
 *   - Gemini 1.5 Pro (Google DeepMind web interface)
 * Author's Full Name: Joshua Assefa
 * Creation Date: 2026-10-01
 * Revision Date: 2026-10-01
 * Revisions: Integrated robust date parsing, custom dynamic list-based MaxHeap, 
 *            fixed edge cases for consecutive NEXT/READ calls on empty queues.
 * ============================================================================
 */

#include <iostream>  // Include standard input/output stream operations
#include <string>    // Include std::string functionality for text manipulation
#include <vector>    // Include std::vector for the internal list-based heap container
#include <sstream>   // Include std::stringstream for delimiter-based parsing

// ============================================================================
// Block Attribution: Class Date
// Author: Joshua Assefa (Refactored and improved from Claude 3.5 Sonnet output)
// Description: Represents calendar dates formatted as MM-DD-YYYY and provides
//              chronological comparison logic.
// ============================================================================
class Date {
private:
    int month; // Numeric month (1 - 12)
    int day;   // Numeric day of the month (1 - 31)
    int year;  // Numeric four-digit year

public:
    // Default constructor initializing fields to zero
    Date() : month(0), day(0), year(0) {}

    // Parameterized constructor initializing specific month, day, and year
    Date(int m, int d, int y) : month(m), day(d), year(y) {}

    // Static helper method to parse "MM-DD-YYYY" formatted string into a Date object
    static Date parseDate(const std::string& dateStr) {
        int m = 0, d = 0, y = 0; // Temporary integer variables for tokens
        char dash1 = '\0', dash2 = '\0'; // Character holders for hyphens
        std::stringstream ss(dateStr); // Wrap string in a stringstream for parsing
        ss >> m >> dash1 >> d >> dash2 >> y; // Extract tokens separated by dashes
        return Date(m, d, y); // Return instantiated Date object
    }

    // Overloaded greater-than operator to compare two dates (true if this date is newer)
    bool operator>(const Date& other) const {
        if (year != other.year) return year > other.year; // Compare years first
        if (month != other.month) return month > other.month; // Compare months if years match
        return day > other.day; // Compare days if month and year match
    }

    // Overloaded equivalence operator to test date equality
    bool operator==(const Date& other) const {
        return (year == other.year && month == other.month && day == other.day); // All fields identical
    }

    // Accessor for the formatted original date representation
    std::string toString() const {
        std::string mStr = (month < 10 ? "0" : "") + std::to_string(month); // Pad single-digit month
        std::string dStr = (day < 10 ? "0" : "") + std::to_string(day);     // Pad single-digit day
        std::string yStr = std::to_string(year);                            // Convert year to string
        while (yStr.length() < 4) yStr = "0" + yStr;                        // Pad year if needed
        return mStr + "-" + dStr + "-" + yStr;                              // Assemble formatted string
    }
};

// ============================================================================
// Block Attribution: Class Email
// Author: Joshua Assefa (Refactored and improved from Claude 3.5 Sonnet output)
// Description: Stores individual email attributes and implements strict 
//              priority comparison operators.
// ============================================================================
class Email {
private:
    std::string sender;   // Sender category label
    std::string subject;  // Subject line string
    Date date;            // Date object indicating when email arrived
    std::string rawDate;  // Raw date string for preserved output printing

    // Internal mapping of sender category strings to integer priority weights
    int getSenderPriority() const {
        if (sender == "Boss") return 5;             // Boss read first
        if (sender == "Subordinate") return 4;     // Subordinate read next
        if (sender == "Peer") return 3;            // Peer read next
        if (sender == "Important Person") return 2;// Important Person read next
        if (sender == "Other Person") return 1;    // Other Person read last
        return 0;                                  // Fallback for unknown categories
    }

public:
    // Default constructor
    Email() : sender(""), subject(""), date(), rawDate("") {}

    // Parameterized constructor
    Email(const std::string& snd, const std::string& sub, const std::string& dt)
        : sender(snd), subject(sub), date(Date::parseDate(dt)), rawDate(dt) {}

    // Getters for email properties
    std::string getSender() const { return sender; }     // Returns sender category
    std::string getSubject() const { return subject; }   // Returns subject line
    std::string getDate() const { return rawDate; }      // Returns original date string

    // Priority comparison: Returns true if this email has HIGHER priority than other
    bool operator>(const Email& other) const {
        int p1 = getSenderPriority();       // Current email sender priority
        int p2 = other.getSenderPriority(); // Comparison email sender priority
        if (p1 != p2) {
            return p1 > p2;                 // Higher sender rank takes precedence
        }
        return date > other.date;           // Same sender: newest date read first
    }

    // Priority comparison: Returns true if this email has LOWER priority than other
    bool operator<(const Email& other) const {
        int p1 = getSenderPriority();       // Current email sender priority
        int p2 = other.getSenderPriority(); // Comparison email sender priority
        if (p1 != p2) {
            return p1 < p2;                 // Lower sender rank loses
        }
        return other.date > date;           // Same sender: older date loses
    }
};

// ============================================================================
// Block Attribution: Class MaxHeap
// Author: Joshua Assefa (Refactored and improved from Claude 3.5 Sonnet output)
// Description: Custom list-based Max-Heap data structure implemented from 
//              scratch without using external heap libraries.
// ============================================================================
class MaxHeap {
private:
    std::vector<Email> heapList; // Underlying list container storing heap elements

    // Sift an element upward to restore heap properties after insertion
    void heapifyUp(int index) {
        while (index > 0) {                                  // Continue until root is reached
            int parentIndex = (index - 1) / 2;               // Calculate parent node index
            if (heapList[index] > heapList[parentIndex]) {   // Check if child has higher priority
                std::swap(heapList[index], heapList[parentIndex]); // Swap child and parent
                index = parentIndex;                         // Move up to parent's position
            } else {
                break;                                       // Heap invariant satisfied
            }
        }
    }

    // Sift an element downward to restore heap properties after deletion
    void heapifyDown(int index) {
        int size = heapList.size(); // Current count of elements in heap
        while (true) {
            int leftChild = 2 * index + 1;   // Calculate left child index
            int rightChild = 2 * index + 2;  // Calculate right child index
            int largest = index;             // Assume current node has highest priority

            // Check if left child exists and has higher priority
            if (leftChild < size && heapList[leftChild] > heapList[largest]) {
                largest = leftChild;
            }
            // Check if right child exists and has higher priority than current largest
            if (rightChild < size && heapList[rightChild] > heapList[largest]) {
                largest = rightChild;
            }
            // If largest is not the current index, swap and continue sifting
            if (largest != index) {
                std::swap(heapList[index], heapList[largest]); // Swap elements
                index = largest;                               // Continue downward
            } else {
                break;                                         // Heap invariant satisfied
            }
        }
    }

public:
    // Insert an email into the priority heap
    void insert(const Email& email) {
        heapList.push_back(email);          // Append email to end of dynamic list
        heapifyUp(heapList.size() - 1);     // Restore heap order by bubbling up
    }

    // Return the highest-priority email without deleting it
    bool peek(Email& topEmail) const {
        if (heapList.empty()) return false; // Return false if heap is empty
        topEmail = heapList[0];             // Root contains the highest-priority email
        return true;                        // Indicate success
    }

    // Remove the highest-priority email from the heap
    bool removeTop() {
        if (heapList.empty()) return false;         // Cannot pop from empty heap
        heapList[0] = heapList.back();              // Overwrite root with the last element
        heapList.pop_back();                        // Remove duplicate last element
        if (!heapList.empty()) {
            heapifyDown(0);                         // Restore heap order by bubbling down
        }
        return true;                                // Indicate success
    }

    // Return current count of elements in the heap
    int size() const {
        return heapList.size(); // Return size of underlying vector
    }

    // Check whether the heap contains any elements
    bool isEmpty() const {
        return heapList.empty(); // True if size is 0
    }
};

// ============================================================================
// Block Attribution: Helper Utility Functions
// Author: Joshua Assefa
// Description: String cleaning and whitespace trimming functions.
// ============================================================================
static inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n"); // Find first non-whitespace
    if (first == std::string::npos) return "";      // String is entirely whitespace
    size_t last = str.find_last_not_of(" \t\r\n");   // Find last non-whitespace
    return str.substr(first, (last - first + 1));    // Extract and return trimmed substring
}

// ============================================================================
// Block Attribution: Main Driver Routine
// Author: Joshua Assefa (Refactored and improved from Claude 3.5 Sonnet output)
// Description: Reads instructions line-by-line and executes priority queue actions.
// ============================================================================
int main() {
    MaxHeap emailQueue; // Instantiate custom MaxHeap priority queue
    std::string line;   // Buffer string for input line reading

    // Read instructions line-by-line from standard input until EOF
    while (std::getline(std::cin, line)) {
        line = trim(line); // Remove extraneous boundary whitespace
        if (line.empty()) continue; // Skip empty input lines

        // Case 1: Parse EMAIL command
        if (line.rfind("EMAIL", 0) == 0) {
            std::string content = line.substr(5); // Strip off leading "EMAIL" command
            std::stringstream ss(content);        // Parse remainder by comma delimiters
            std::string sender, subject, dateStr; // Storage strings for components

            std::getline(ss, sender, ',');   // Extract sender category
            std::getline(ss, subject, ',');  // Extract subject line
            std::getline(ss, dateStr);       // Extract date string

            sender = trim(sender);    // Trim whitespace around sender
            subject = trim(subject);  // Trim whitespace around subject
            dateStr = trim(dateStr);  // Trim whitespace around date string

            // Insert newly constructed email object into the priority queue
            emailQueue.insert(Email(sender, subject, dateStr));
        }
        // Case 2: Output unread email count
        else if (line == "COUNT") {
            std::cout << "There are " << emailQueue.size() << " emails to read." << std::endl;
        }
        // Case 3: Display next highest priority email
        else if (line == "NEXT") {
            Email nextEmail; // Object container for top email
            if (emailQueue.peek(nextEmail)) {
                std::cout << "Next email:" << std::endl;
                std::cout << "Sender: " << nextEmail.getSender() << std::endl;
                std::cout << "Subject: " << nextEmail.getSubject() << std::endl;
                std::cout << "Date: " << nextEmail.getDate() << std::endl;
            }
        }
        // Case 4: Mark top email as read and remove from priority queue
        else if (line == "READ") {
            emailQueue.removeTop(); // Remove highest-priority email without terminal output
        }
    }

    return 0; // Return zero indicating normal program execution
}