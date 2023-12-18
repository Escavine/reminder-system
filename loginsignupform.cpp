// C++ Login + Registration form  

// pre-requisite libraries
#include <iostream>
#include <fstream> 
#include <string>  
#include <sqlite3.h> // header library for SQL command utilisation
#include <stdlib.h> // standard library 


using namespace std;


// void hashingAlgorithm() {} // for further encrypting the password


void loginSession(bool loggedInConfo, int UID) 
{ 
    sqlite3* db;
    int rc;
    sqlite3_stmt* stmt;
    int choice;
    int numOfReminders;

    sqlite3_open("userdata.db", &db); 

    cout << "Welcome to my reminders\n";

    const char* query = "SELECT userReminders FROM reminders WHERE userID = ?";

    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        cout << "List has failed to load\n";
        sqlite3_finalize(stmt);
        sqlite3_close(db); // in the instance that user has failed to load, then the database will close
        exit(0);
    }

    rc = sqlite3_bind_int(stmt, 1, UID); // column for reminders

    if (rc != SQLITE_OK)
    {
        cerr << "Cannot find reminders \n";
    }
    else 
    {
        cout << "Reminders have been found \n";
    }


    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* reminder = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        cout << "Current reminder: " << reminder << endl;
    }


    cout << "Options \n" << endl;
    cout << endl;
    cout << "1. Add reminders\n";
    cout << "2. Log out \n";
    cin >> choice;

    if (choice == 1)
    {
        int numOfReminders;
        int i;
        cout << "How many reminders do you want?";
        cin >> numOfReminders;
        int count; // comparsion integer
        bool reminderAppend = false; // this will be used to ensure that numOfReminder has been satisfied

        while (!reminderAppend)
        {
            string reminderInput;
            rc = sqlite3_open("userdata.db", &db);

            if (rc != SQLITE_OK)
            {
                cerr << "Error loading database\n"; // testing measures, will be removed once fully working
                sqlite3_finalize(stmt);
                sqlite3_close(db);
            }
            else 
            {
                cout << "Database connection initiated! \n";
            }

            const char* addReminder = "INSERT INTO reminders (userReminders) VALUES (?);"; // prompt to add reminders to the reminders table
            cout << "Write the details of the given reminder" << "(No. " << i << ")"; // the value i will keep incrementing to indicate the number of reminder
            cin >> reminderInput;
            
            rc = sqlite3_prepare_v2(db, addReminder, -1, &stmt, nullptr); // ready the SQL statement 

            if (rc != SQLITE_OK)
            {
                cerr << "SQL statement initialization has failed!\n";
            }
            else 
            {
                cout << "SQL statement intialization success!\n"; // testing measures will be removed once fully working
            }


            rc = sqlite3_bind_text(stmt, 1, reminderInput.c_str(), -1, SQLITE_STATIC);

            if (rc != SQLITE_OK)
            {
                cerr << "Failed to append the reminder!\n";
            }
            else 
            {
                cout << "Successfully binded the reminder!\n"; // testing measures
            }

            int result = sqlite3_step(stmt);

            if (result == SQLITE_DONE)
            {
                cout << "Reminder has successfully appended to the database! \n"; // testing measures
                count++;

                if (count == numOfReminders)
                {
                    reminderAppend = true; // in the instance that the set amount of reminders is met, exit the loop and go to burkina faso
                    loginSession(loggedInConfo, UID) // return to login session to display updated reminders
                }
            }
            else
            {
                cout << "Reminder has failed to append to database \n" << sqlite_errcode(stmt); // display error code to explain failure (TESTING MEASURE)
            }

        }

    }
}

    else if (choice == 2)
    {
        cout << "Log out initiated, see you soon!\n" << endl;
        int i;
        int seconds = 5; // countdown

        for (i = seconds; i >=0; i--)
        {
            cout << "Countdown intiated: " << i << "seconds" << endl;
        }

        exit(0);
    }
    
    else 
    {
        cout << "Invalid input, try again";
        loginSession(loggedInConfo, UID); // recurse till correct info is inputted
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

} 


bool isLoggedIn(int choice, int retryAttempts) {
    string un, pw;
    int rc; // return code 
    bool loggedInConfo = false; // local variables
    sqlite3_stmt *stmt; 
    sqlite3* db;

    rc = sqlite3_open("userdata.db", &db);

    const char* sql = "SELECT * FROM users WHERE username = ? AND password = ?";
    ;rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr); 

    if (rc != SQLITE_OK)
    {
        cerr << "Error preparing statement\n";
        sqlite3_close(db);
        return false;
    }
 

    while (!loggedInConfo) {
        cout << "Enter your username:\n";
        cin >> un;

        cout << "Enter your password:\n ";
        cin >> pw;

        cout << "Logging in...\n";

        rc = sqlite3_bind_text(stmt, 1, un.c_str(), -1, SQLITE_STATIC);

        if (rc != SQLITE_OK)
        {
            cerr << "Error binding username\n";
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            break;
        }
        else
        {
            cout << "Username has been initialized\n";
        }


        rc = sqlite3_bind_text(stmt, 2, pw.c_str(), -1, SQLITE_STATIC);

        if (rc != SQLITE_OK)
        {
            cerr << "Password has not been initalized\n";
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            break;
        }
        else
        {
            cout << "Password has been intialized\n";

        }


        int result = sqlite3_step(stmt);


        if (result == SQLITE_ROW)
        {
            int UID = sqlite3_column_int(stmt, 0); // this will figure out userID of person
            cout << "User ID: " << UID << endl;
            cout << "User authenticated.\n" << endl;
            loggedInConfo = true;
            loginSession(loggedInConfo, UID);
        }
        else 
        {
            cerr << "Details are incorrect, please try again\n" << endl;

            if (retryAttempts > 0)
            {
                isLoggedIn(choice, retryAttempts - 1); // recurse to allow user to input their details again till they are correct.
            }
            else
            {
                cerr << "Too many attempts made, terminating... ";
                exit(0);
            }
        }

    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return loggedInConfo;
}


bool signUp(int choice, int retryAttempts) 
{
    string un, pw;
    bool signUpSuccess = false; // local variables
    sqlite3_stmt* stmt;
    int rc;
    sqlite3* db;

    rc = sqlite3_open("userdata.db", &db);

    const char* sql = "INSERT INTO users (username, password) VALUES (?, ?)";
    ;rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr); 

    if (rc != SQLITE_OK)
    {
        cerr << "Error preparing statement\n" << endl;
        return false;
    }
    else 
    {
        cout << "Sucessfully prepared statement\n" << endl;
    }

    while (!signUpSuccess) 
    {
        cout << "Enter a new username:\n ";
        cin >> un;
    
        cout << "Enter a new password:\n "; 
        cin >> pw;
    
        cout << "Signing up... \n";

        rc = sqlite3_bind_text(stmt, 1, un.c_str(), -1, SQLITE_STATIC);

        if (rc != SQLITE_OK) {
            cerr << "Error binding username\n" << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            break;
        }
        else 
        {
            cout << "Sucessfully binded username\n" << endl;
        }


        rc = sqlite3_bind_text(stmt, 2, pw.c_str(), -1, SQLITE_STATIC); 


        if (rc != SQLITE_OK)
        { 
            cerr << "Error binding password\n" << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            break;
        }
        else 
        {
            cout << "Sucessfully binded password\n" << endl;
        }

        int result = sqlite3_step(stmt); // insert the new user to the database once compilation is completed


        if (result != SQLITE_DONE) 
        {
            cerr << "Sign up has failed to insert to the database " << endl;
        
            if (retryAttempts > 0) // mitigating infinite recursion to prevent stack overflow errors
            {
                signUp(choice, retryAttempts - 1);
            }
            else
            {
                cerr << "Too many attempts, program terminating...";
                exit(0);
            }
        }
        else 
        {
            cout << "Sign up has been successfully initiated! " << endl; // in the instance that it is a success, then it will be outputted, or otherwise it has failed to append
            signUpSuccess = true;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    }
    return signUpSuccess; // confirms output, other measures will be made to ensure that the passwords are secure (i.e. encryption techniques such as a hash salt)    
}



void choiceFunction()
{
    int choice;
    int retryAttempts = 3; // user can only input details 3 times before termination to prevent infinite recursion
    
    cout << "Reminder System Main Menu\n" << endl;
    cout << endl;
    cout << "------- Main Menu -------\n";
    
    
    cout << "1. Sign up\n";
    cout << "2. Login\n";    
    cin >> choice;
    
    
    if (choice == 1)
    {
        cout << "You will be redirected to the sign-up form... " << endl;
        bool signUpProcess = signUp(choice, retryAttempts);
    }
    else if (choice == 2)
    {
        cout << "You will be redirected to the login form... " << endl;
        bool loginResult = isLoggedIn(choice, retryAttempts);
    }
    else
    {
        cerr << "Input not recognized, please use a sensible input" << endl;
        choiceFunction(); // keep recusing till right input is made 
    }
}


int main()
{
    sqlite3* db;
    int rc; // return code

    rc = sqlite3_open("userdata.db", &db);

    if (rc != SQLITE_OK) 
    {
        cerr << "Users database hasn't been initialized\n";
    }
    else 
    {
        cout << "Users database has been intialized\n"; // test
    }

    choiceFunction(); // once database connection is established, proceed to go to the sign-in/login page

    sqlite3_close(db);
    return 0;
}
















