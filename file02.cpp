#include <bits/stdc++.h>
using namespace std;

/*
    TASK MANAGER SYSTEM (Console Based)

    This project is designed to manage daily tasks using a simple
    command-line interface.

    Key goals of this project:
    - Keep the logic simple and readable
    - Enforce proper task lifecycle rules
    - Provide clear feedback to the user
    - Focus on usability, not complexity
*/

/* =====================================================
   GLOBAL DATA
   ===================================================== */

// Used to generate simple task IDs like T1, T2, T3.
// Incremental IDs are easier for users to remember and type.
int taskCounter = 0;

/* =====================================================
   DATA MODELS
   ===================================================== */

// Stores basic user information.
// Password is stored as a hash for basic safety (demo purpose).
struct User {
    string id;        // U1, U2, ...
    string name;
    string email;
    string passHash;
};

// Represents a single task.
// The 'done' flag controls task state.
struct Task {
    string id;        // T1, T2, ...
    string userId;    // Owner of the task
    string title;
    string desc;
    bool done = false; // false = Pending, true = Completed
};

/* =====================================================
   UI HELPER FUNCTIONS
   ===================================================== */

// Prints a strong header for menus or sections.
void header(const string &title) {
    cout << endl;
    cout << "============================================" << endl;
    cout << "  " << title << endl;
    cout << "============================================" << endl << endl;
}

// Soft divider for grouping output without clutter.
void divider() {
    cout << "............................................" << endl;
}

// Standard prompt symbol to guide user input.
void prompt() {
    cout << ">> ";
}

// Checks if a string is empty or contains only spaces.
// This prevents invalid inputs like "   ".
bool isEmpty(const string &s) {
    for (char c : s) {
        if (!isspace(c)) return false;
    }
    return true;
}

// Basic email format validation.
// Keeps it simple and interview-friendly.
bool isValidEmail(const string &email) {
    if (email.find(' ') != string::npos) return false;

    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);

    return atPos != string::npos &&
           dotPos != string::npos &&
           atPos < dotPos;
}

// Converts a string to uppercase.
// Used to make Task ID input case-insensitive.
string toUpper(string s) {
    for (char &c : s) c = toupper(c);
    return s;
}

/* =====================================================
   AUTHENTICATION SERVICE
   ===================================================== */

class AuthService {
    vector<User> users;       // Stores registered users
    User* current = nullptr;  // Points to logged-in user

    // Simple hash for demonstration purposes.
    // Not secure for real applications.
    string hashPassword(const string &p) {
        return to_string(hash<string>{}(p));
    }

public:
    // Handles user registration with proper validation.
    bool registerUser(const string &n, const string &e,
                      const string &p, string &reason) {

        // Validate basic input
        if (isEmpty(n) || isEmpty(e) || isEmpty(p)) {
            reason = "Name, email, or password cannot be empty";
            return false;
        }

        // Validate email format
        if (!isValidEmail(e)) {
            reason = "Invalid email format";
            return false;
        }

        // Enforce minimum password length
        if (p.length() < 6) {
            reason = "Password must be at least 6 characters long";
            return false;
        }

        // Prevent duplicate email registration
        for (auto &u : users) {
            if (u.email == e) {
                reason = "Email already registered";
                return false;
            }
        }

        // Create and store the user
        users.push_back({
            "U" + to_string(users.size() + 1),
            n, e, hashPassword(p)
        });

        return true;
    }

    // Handles login with clear failure reasons.
    bool login(const string &e, const string &p, string &reason) {

        if (isEmpty(e) || isEmpty(p)) {
            reason = "Email or password cannot be empty";
            return false;
        }

        for (auto &u : users) {
            if (u.email == e) {
                if (u.passHash == hashPassword(p)) {
                    current = &u;
                    return true;
                } else {
                    reason = "Incorrect password";
                    return false;
                }
            }
        }

        reason = "Email not found";
        return false;
    }

    // Logs out the current user.
    void logout() {
        current = nullptr;
    }

    // Returns pointer to logged-in user (nullptr if none).
    User* getCurrentUser() {
        return current;
    }
};

/* =====================================================
   TASK FILTER TYPE
   ===================================================== */

// Used while viewing tasks.
enum class TaskFilter {
    ALL,
    PENDING,
    COMPLETED
};

/* =====================================================
   TASK CONTROLLER
   ===================================================== */

class TaskController {
    vector<Task> tasks; // Stores all tasks

public:
    // Adds a new task for the logged-in user.
    void addTask(const string &uid, const string &t, const string &d) {
        string tid = "T" + to_string(++taskCounter);

        tasks.push_back({tid, uid, t, d, false});

        divider();
        cout << "Task created successfully" << endl;
        cout << "Task ID : " << tid << endl;
        divider();
    }

    // Displays a short summary for user awareness.
    void showSummary(const string &uid) {
        int pending = 0, completed = 0;

        for (auto &t : tasks) {
            if (t.userId == uid) {
                t.done ? completed++ : pending++;
            }
        }

        divider();
        cout << "Task Summary" << endl;
        cout << "Pending   : " << pending << endl;
        cout << "Completed : " << completed << endl;
        divider();
    }

    // Lists tasks based on selected filter.
    void listTasks(const string &uid, TaskFilter filter) {
        bool found = false;

        for (auto &t : tasks) {
            if (t.userId != uid) continue;
            if (filter == TaskFilter::PENDING && t.done) continue;
            if (filter == TaskFilter::COMPLETED && !t.done) continue;

            found = true;
            divider();
            cout << "Task ID     : " << t.id << endl;
            cout << "Title       : " << t.title << endl;
            cout << "Description : " << t.desc << endl;
            cout << "Status      : "
                 << (t.done ? "Completed" : "Pending") << endl;
        }

        if (!found) {
            divider();
            cout << "No tasks found for this section" << endl;
        }
        divider();
    }

    // Marks a task as completed.
    // Only pending tasks can be completed.
    void markDone(string tid, const string &uid) {
        tid = toUpper(tid);

        for (auto &t : tasks) {
            if (t.id == tid && t.userId == uid) {

                if (t.done) {
                    divider();
                    cout << "Task is already completed" << endl;
                    divider();
                    return;
                }

                t.done = true;
                divider();
                cout << "Task marked as completed" << endl;
                divider();
                return;
            }
        }

        divider();
        cout << "Task not found. Tip: Use ID like T1" << endl;
        divider();
    }

    // Deletes a task only if it is completed.
    void deleteTask(string tid, const string &uid) {
        tid = toUpper(tid);

        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->id == tid && it->userId == uid) {

                if (!it->done) {
                    divider();
                    cout << "Complete the task before deleting it" << endl;
                    divider();
                    return;
                }

                cout << "Are you sure you want to delete this task? (y/n): ";
                char ch;
                cin >> ch;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (tolower(ch) != 'y') {
                    divider();
                    cout << "Delete cancelled" << endl;
                    divider();
                    return;
                }

                tasks.erase(it);
                divider();
                cout << "Task deleted successfully" << endl;
                divider();
                return;
            }
        }

        divider();
        cout << "Task not found or already deleted" << endl;
        divider();
    }
};

/* =====================================================
   MAIN FUNCTION
   ===================================================== */

int main() {
    ios::sync_with_stdio(true);
    cin.tie(nullptr);

    AuthService auth;
    TaskController task;

    while (true) {

        // If no user is logged in
        if (!auth.getCurrentUser()) {
            header("TASK MANAGER SYSTEM");

            cout << "1. Register" << endl;
            cout << "2. Login" << endl;
            cout << "3. Exit" << endl << endl;

            prompt();
            int ch;
            cin >> ch;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (ch == 1) {
                string n, e, p, reason;
                cout << "Name     : "; getline(cin, n);
                cout << "Email    : "; getline(cin, e);
                cout << "Password : "; getline(cin, p);

                if (auth.registerUser(n, e, p, reason)) {
                    divider();
                    cout << "Registration successful" << endl;
                    divider();
                } else {
                    divider();
                    cout << "Registration failed" << endl;
                    cout << "Reason : " << reason << endl;
                    divider();
                }
            }
            else if (ch == 2) {
                string e, p, reason;
                cout << "Email    : "; getline(cin, e);
                cout << "Password : "; getline(cin, p);

                if (auth.login(e, p, reason)) {
                    divider();
                    cout << "Login successful" << endl;
                    divider();
                } else {
                    divider();
                    cout << "Login failed" << endl;
                    cout << "Reason : " << reason << endl;
                    divider();
                }
            }
            else if (ch == 3) {
                divider();
                cout << "Thank you for using the Task Manager System" << endl;
                cout << "Have a great day!" << endl;
                divider();
                break;
            }
        }
        // If user is logged in
        else {
            User* u = auth.getCurrentUser();
            header("Welcome, " + u->name);

            task.showSummary(u->id);

            cout << "1. Add Task" << endl;
            cout << "2. View All Tasks" << endl;
            cout << "3. View Pending Tasks" << endl;
            cout << "4. View Completed Tasks" << endl;
            cout << "5. Mark Task as Completed" << endl;
            cout << "6. Delete Task" << endl;
            cout << "7. Logout" << endl << endl;

            prompt();
            int ch;
            cin >> ch;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (ch == 1) {
                string t, d;
                cout << "Title       : "; getline(cin, t);
                cout << "Description : "; getline(cin, d);
                task.addTask(u->id, t, d);
            }
            else if (ch == 2) task.listTasks(u->id, TaskFilter::ALL);
            else if (ch == 3) task.listTasks(u->id, TaskFilter::PENDING);
            else if (ch == 4) task.listTasks(u->id, TaskFilter::COMPLETED);
            else if (ch == 5) {
                string tid;
                cout << "Enter Task ID: ";
                getline(cin, tid);
                task.markDone(tid, u->id);
            }
            else if (ch == 6) {
                string tid;
                cout << "Enter Task ID: ";
                getline(cin, tid);
                task.deleteTask(tid, u->id);
            }
            else if (ch == 7) {
                auth.logout();
                divider();
                cout << "You have been logged out successfully" << endl;
                divider();
            }
        }
    }

    return 0;
}