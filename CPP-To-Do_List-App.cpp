#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <list>
#include <ctime>
#include <fstream>
#include <sstream>
using namespace std;

class to_do_item {
private:
    int id;
    string description,priority;
    bool done;
    tm deadline;

public:
    to_do_item() : id(0), description(""), done(false) { 
        deadline = { 0 };
    }
    ~to_do_item() = default;

    bool create(string new_description, string new_priority, tm new_deadline) {
        description = new_description;
        priority = new_priority;
        deadline = new_deadline;
        done = false;
        return true;
    }

    void setid(int new_id) { id = new_id; }
    int getid() const { return id; }
    string getdescription() const { return description; }
    string getpriority() const { return priority; }
    bool isdone() const { return done; }
    tm getdeadline() const { return deadline; }

    void markdone(bool val) { done = val; }

    string time_left() const {
        time_t now = time(0); //current in seconds
        tm* ltm = localtime(&now); //current local time
        time_t current_time = mktime(ltm); //convert tm to time_t
        time_t deadline_time = mktime(const_cast<tm*>(&deadline)); //convert tm to time_t 

        double diff = difftime(deadline_time, current_time);

        int days_left = diff / (60 * 60 * 24);
        int hours_left = (diff - days_left * (60 * 60 * 24)) / (60 * 60);
        int minutes_left = (diff - days_left * (60 * 60 * 24) - hours_left * (60 * 60)) / 60;

        if (diff < 0) {
            double overdue_diff = -diff; 
            int overdue_days = overdue_diff / (60 * 60 * 24);
            int overdue_hours = (overdue_diff - overdue_days * (60 * 60 * 24)) / (60 * 60);
            int overdue_minutes = (overdue_diff - overdue_days * (60 * 60 * 24) - overdue_hours * (60 * 60)) / 60;

            return "Task is overdue! By: " + to_string(overdue_days) + " days, " + to_string(overdue_hours) + " hours, " + to_string(overdue_minutes) + " minutes.";
        }
        else {
            return to_string(days_left) + " days, " + to_string(hours_left) + " hours, " + to_string(minutes_left) + " minutes left.";
        }
    }
    
    string deadline_to_string() const {
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &deadline);
        return string(buffer);
    }

    bool string_to_deadline(const string& deadline_str) {
        int year, month, day, hour, minute;
        if (sscanf(deadline_str.c_str(), "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute) == 5) {
            deadline.tm_year = year - 1900;
            deadline.tm_mon = month - 1;
            deadline.tm_mday = day;
            deadline.tm_hour = hour;
            deadline.tm_min = minute;
            deadline.tm_sec = 0;
            return true;
        }
        return false;
    }
};

int generateid(const list<to_do_item>& todo) {
    int new_id;
    bool id_exists;

    do {
        new_id = rand() % 1000 + 1;
        id_exists = false;

        for (const auto& item : todo) {
            if (item.getid() == new_id) {
                id_exists = true;
                break;
            }
        }
    } while (id_exists); 

    return new_id;
}

void savetasks(const list<to_do_item>& todo) {
    ofstream file("tasks.txt");

    for (const auto& item : todo) {
        file << item.getid() << "," 
            << item.getdescription() << "," 
            << item.getpriority() << "," 
            << item.deadline_to_string() << ","
            << (item.isdone() ? "true" : "false") << endl;
    }

    file.close();
    cout << endl << "Tasks saved successfully!" << endl;
}

void loadtasks(list<to_do_item>& todo) {
    ifstream file("tasks.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string id,description,priority,done,deadline_str;
        tm deadline{};

        getline(ss, id, ',');
        getline(ss, description, ',');
        getline(ss, priority, ',');
        getline(ss, deadline_str, ',');
        getline(ss, done, ',');

        to_do_item item;
        item.setid(stoi(id));
        item.create(description, priority, deadline);
        item.markdone(done == "true");

        if (item.string_to_deadline(deadline_str)) {
            todo.push_back(item);
        }
    }

    file.close();
    cout << "Tasks loaded successfully!" << endl;
}

int main()
{
E:
    string version = "v0.1.0";
    list<to_do_item> todo;
    list<to_do_item>::iterator it;

    srand(time(NULL));

    loadtasks(todo);

    while (1) {
        system("cls");
        cout << "To Do List Maker - " << version << endl;
        cout << endl << endl;

        for (it = todo.begin(); it != todo.end(); it++) {
            string completed = it->isdone() ? "Done" : "Not Done";

            cout << it->getid() << " | "
                << it->getdescription() << " | "
                << it->getpriority() << " | ";

            if (completed == "Done") {
                cout << completed << endl;
            }
            else {
                cout << "Time Left: " << it->time_left() << " | " << completed << endl;
            }

            cout << endl;
        }

        cout << "[a]dd a new task" << endl;
        cout << "[c]omplete a task" << endl;
        cout << "[e]dit a task" << endl;
        cout << "[q]uit program" << endl;

        cout << endl << "Choice: ";
        char option;
        int input_id;
        string input_description, input_priority, input_deadline;
        bool found = false;

        cin >> option;
        cin.ignore();

        if (option == 'q') {
            savetasks(todo);
            cout << endl << "We will call it a day then! Bye!" << endl;
            break;
        }
        else if (option == 'c') {
            if (todo.empty()) {
                cout << "No tasks Left! Add your first task!" << endl;
                goto A;
            }
            else {
                cout << endl << "Enter Id of the task: ";
                bool task_completed = false;

                while (!task_completed) {
                    cin >> input_id;
                    cin.ignore();
                    found = false;

                    for (it = todo.begin(); it != todo.end(); it++) {
                        if (input_id == it->getid()) {
                            it->markdone(true);
                            found = true;
                            task_completed = true;
                            break;
                        }
                    }

                    if (!found) {
                        cout << endl << "Wrong Id! Please try again: ";
                    }
                }
            }
        }
        else if (option == 'a') {
        A:
            cout << endl << "Add description: ";
            getline(cin, input_description);

            cout << endl << "Set Priority(High,Medium,Low): ";
            getline(cin, input_priority);
            
        C:
            cout << endl << "Enter deadline (YYYY-MM-DD HH:MM): ";
            getline(cin, input_deadline);

            to_do_item new_item;
            tm deadline{};
            if (new_item.string_to_deadline(input_deadline)) {
                new_item.create(input_description, input_priority, new_item.getdeadline());
                new_item.setid(generateid(todo));
                todo.push_back(new_item);
            }
            else {
                cout << "Invalid deadline format. Please use YYYY-MM-DD HH:MM format." << endl;
                goto C;
            }


        }
        else if (option == 'e') {
            if (todo.empty()) {
                cout << "No tasks Left! Add your first task!" << endl;
                goto A;
            }
            else {
                cout << endl << "Enter Id of the task: ";
                cin >> input_id;
                cin.ignore();
                found = false;

                for (it = todo.begin(); it != todo.end(); it++) {
                    if (input_id == it->getid()) {
                        it->markdone(true);
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    cout << endl << "Wrong Id! Please try again: ";
                }
                else {
                D:
                    cout << "Enter what do you want to edit (1-description;2-priority;3-deadline;4-status;) : ";
                    int a;
                    cin >> a;
                    cin.ignore();
                    
                    if (a == 1) {
                        cout << "Enter new Description: ";
                        string new_description;
                        getline(cin, new_description);

                        it->create(new_description, it->getpriority(), it->getdeadline());
                        cout << "Description updated successfully!" << endl;

                    }
                    else if (a == 3) {
                        B:
                        cout << "Enter new Deadline (YYYY-MM-DD HH:MM): ";
                        string new_deadline_str;
                        getline(cin, new_deadline_str);

                        if (it->string_to_deadline(new_deadline_str)) {
                            cout << "Deadline updated successfully!" << endl;
                        }
                        else {
                            cout << "Invalid deadline format. Please use YYYY-MM-DD HH:MM format." << endl;
                            goto B;
                        }
                    }
                    else if (a == 2) {
                        cout << "Enter new Priority (High, Medium, Low): ";
                        string new_priority;
                        getline(cin, new_priority);

                        it->create(it->getdescription(), new_priority, it->getdeadline());
                        cout << "Priority updated successfully!" << endl;
                    }
                    else if(a == 4) {
                        cout << "Enter new Status (1 for Done, 0 for Not Done): ";
                        int new_status;
                        cin >> new_status;
                        cin.ignore();

                        it->markdone(new_status == 1);
                        cout << "Status updated successfully!" << endl;
                    }
                    else {
                        cout << "Invalid input!" << endl;
                        goto D;
                    }

                }
            }
        }
        else {
            cout << "Invalid input!" << endl;
            goto E;
        }
    }
    return 0;
}
