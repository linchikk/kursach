#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <functional>
#include <cctype> // ��� ������� isalpha()
#include <clocale>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <cstring>

using namespace std;

struct User {
    int id;
    string login;
    string password;
    string surname;
    string name;
    int role;
} MyUser, EditUser;

struct Route {
    int id;
    string routeNum;
    string stations;
    string times;
};

struct Train {
    int id;
    bool isActive;
    int routeId;
    int allSeats;
    int freeSeats;
    float cost;
    int day;
    int mon;
    int year;
};

struct Ticket {
    int id;
    bool isActive;
    int trainId;
    int userId;
    int seats;
    float price;
};

void logFile(const string& text) {
    string filename = "log.txt";
    ofstream file(filename, std::ios::app);  // ������� ���� ��� ������ � ������ ���������� (append)
    if (!file) {
        file.open(filename);  // ������� ����� ����, ���� ���� �� ����������
        file.close();  // ������� ���� ����� ��������
        file.open(filename, std::ios::app);  // ������� ��������� ���� ��� ������ � ������ ����������
    }
}
User parseUser(const string& line) {
    User user;
    stringstream ss(line);
    ss >> user.id >> user.login >> user.password >> user.surname >> user.name >> user.role;
    return user;
}

User* readUsersToArray() {
    ifstream userFile("users.txt");
    if (!userFile) {
        cerr << "������ �������� ����� �������������." << endl;
        return nullptr;
    }

    // ���������� ���������� ������������� � �����
    int userCount = 0;
    string line;
    while (getline(userFile, line)) {
        userCount++;
    }
    userFile.clear(); // ���������� ���� ������
    userFile.seekg(0, ios::beg); // ������������ � ������ �����

    // ������� ������ �������������
    User* users = new User[userCount + 1]; // +1 ��� ������������ �������� � id = -1

    // ��������� ������������� �� ����� � ������
    int i = 0;
    while (getline(userFile, line)) {
        users[i] = parseUser(line);
        i++;
    }

    // ��������� ����������� �������
    users[userCount].id = -1;

    userFile.close();
    return users;
}
string serializeUser(const User& user) {
    stringstream ss;
    ss << user.id << " " << user.login << " " << user.password << " " << user.surname << " " << user.name << " " << user.role;
    return ss.str();
}

int partition(User* users, int low, int high) {
    int pivot = users[high].id;
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (users[j].id < pivot) {
            ++i;
            swap(users[i], users[j]);
        }
    }
    swap(users[i + 1], users[high]);
    return i + 1;
}

void quickSort(User* users, int low, int high) {
    if (low < high) {
        int pivot = partition(users, low, high);
        quickSort(users, low, pivot - 1);
        quickSort(users, pivot + 1, high);
    }
}

void sortFileRecordsById(const string& filename) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "������ �������� ����� " << filename << endl;
        return;
    }

    User* users = nullptr;
    int numUsers = 0;

    // ������ ������������� �� �����
    string line;
    while (getline(inputFile, line)) {
        User user = parseUser(line);

        // ���������� ������� ������� �������������
        User* temp = new User[numUsers + 1];
        for (int i = 0; i < numUsers; ++i) {
            temp[i] = users[i];
        }
        temp[numUsers++] = user;
        delete[] users;
        users = temp;
    }
    inputFile.close();

    // ������� ���������� ������������� �� ��������������
    quickSort(users, 0, numUsers - 1);

    // ���������� ��������������� ������� � ����
    ofstream outputFile(filename);
    if (!outputFile) {
        cerr << "������ �������� ����� " << filename << " ��� ������" << endl;
        delete[] users; // ������� ������ ����� ������� �� �������
        return;
    }

    for (int i = 0; i < numUsers; ++i) {
        outputFile << serializeUser(users[i]) << endl;
    }
    outputFile.close();

    // ������������ ���������� ������
    delete[] users;
}

int getLastUserId() {
    sortFileRecordsById("users.txt");
    ifstream userFile("users.txt");
    if (!userFile) {
        cerr << "������ �������� ����� �������������." << endl;
        return -1; // ���������� -1 � ������ ������
    }

    int maxId = 0;
    int storedId, storedRole;
    string storedLogin, storedPassword, storedSurname, storedName;
    while (userFile >> storedId >> storedLogin >> storedPassword >> storedSurname >> storedName >> storedRole) {
        if (storedId > maxId) {
            maxId = storedId;
        }
    }

    userFile.close();
    return maxId;
}
bool doesUserExist(const string& login) {
    ifstream userFile("users.txt");
    if (!userFile) {
        cerr << "������ �������� ����� �������������." << endl;
        return false;
    }

    int storedId, storedRole;
    string storedLogin, storedPassword, storedSurname, storedName;
    while (userFile >> storedId >> storedLogin >> storedPassword >> storedSurname >> storedName >> storedRole) {
        if (storedLogin == login) {
            userFile.close();
            return true;
        }
    }

    userFile.close();
    return false;
}

void SortById(User* users, int userCount) {
    quickSort(users, 0, userCount - 1);
}

void merge(User* users, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // ��������� ������� ��� �������� �������
    User* L = new User[n1];
    User* R = new User[n2];

    // ����������� ������ �� ��������� ������� L[] � R[]
    for (int i = 0; i < n1; i++)
        L[i] = users[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = users[mid + 1 + j];

    // ������� ��������� �������� ������� � users[left..right]
    int i = 0;
    int j = 0;
    int k = left;
    while (i < n1 && j < n2) {
        if (L[i].login <= R[j].login) {
            users[k] = L[i];
            i++;
        }
        else {
            users[k] = R[j];
            j++;
        }
        k++;
    }

    // ����������� ���������� ��������� L[], ���� ��� ����
    while (i < n1) {
        users[k] = L[i];
        i++;
        k++;
    }

    // ����������� ���������� ��������� R[], ���� ��� ����
    while (j < n2) {
        users[k] = R[j];
        j++;
        k++;
    }

    // ����������� ������, ���������� ��� ��������� ��������
    delete[] L;
    delete[] R;
}

void mergeSort(User* users, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(users, left, mid);
        mergeSort(users, mid + 1, right);
        merge(users, left, mid, right);
    }
}

void SortByLogin(User* users, int userCount) {
    mergeSort(users, 0, userCount - 1);
}

int compare(User a, User b) {
    if (a.surname != b.surname)
        return a.surname < b.surname;
    return a.name < b.name;
}

void heapify(User* users, int n, int i) {
    int largest = i; // Initialize largest as root
    int l = 2 * i + 1; // left = 2*i + 1
    int r = 2 * i + 2; // right = 2*i + 2

    // If left child is larger than root
    if (l < n && compare(users[l], users[largest]))
        largest = l;

    // If right child is larger than largest so far
    if (r < n&& compare(users[r], users[largest]))
        largest = r;

    // If largest is not root
    if (largest != i) {
        swap(users[i], users[largest]);

        // Recursively heapify the affected sub-tree
        heapify(users, n, largest);
    }
}

// main function to do heap sort
void heapSort(User* users, int n) {
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(users, n, i);

    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        // Move current root to end
        swap(users[0], users[i]);

        // call max heapify on the reduced heap
        heapify(users, i, 0);
    }
}

void SortBySurnameName(User* users, int userCount) {
    heapSort(users, userCount);
}
void createAdminAccount() {
    ofstream userFile("users.txt");
    if (!userFile) {
        cerr << "������ �������� ����� � �������� �������� �������������." << endl;
        return;
    }

    User admin;
    admin.id = 1; // ID ��������������
    admin.login = "admin";
    admin.password = "admin"; // �����, �������, ���������� ������������ ���������� �������� �����������
    // ����� ����� ���������� ������������ ���������� ����
    admin.surname = "���������";
    admin.name = "�������������";// ��������� ���� fullName � ���������
    admin.role = 1; // ���� ��������������
    // ������� � ���� ������ ��������������
    userFile << admin.id << " " << admin.login << " " << admin.password << " " << admin.surname << " " << admin.name << " " << admin.role << endl;
    userFile.close();
}

void checkAndCreateUserFile() {
    ifstream userFile("users.txt");
    if (!userFile.is_open()) {

        userFile.close();
        createAdminAccount();
        string logText = "���� � �������� �������� ������������� ������. �������� ������� �������������.";
        logFile(logText);
        cout << logText << endl;
    }
    else {
        // ���� ����������, ���������, ������ �� ��
        userFile.seekg(0, ios::end);
        if (userFile.tellg() == 0) {
            // ���� ����, ��������� ��������������
            userFile.close();
            createAdminAccount();
            string logText = "���� � �������� �������� ������������� ����. �������� ������� �������������.";
            logFile(logText);
            cout << logText << endl;
        }
    }
}
int authenticateUser(const string& login, const string& password) {
    ifstream userFile("users.txt");
    if (!userFile) {
        cerr << "���� � �������� �������� ������������� �� ����������." << endl;
        return 0;
    }

    int storedId, storedRole;
    string storedLogin, storedPassword, storedSurname, storedName;
    while (userFile >> storedId >> storedLogin >> storedPassword >> storedSurname >> storedName >> storedRole) {
        // ��������� ����� � ������
        if (storedLogin == login && storedPassword == password) {
            // ���� ����� � ������ ���������, ��������� ������ ������������ � ���������� ��� ID
            MyUser.id = storedId;
            MyUser.login = storedLogin;
            MyUser.password = storedPassword;
            MyUser.surname = storedSurname;
            MyUser.name = storedName;
            MyUser.role = storedRole;
            string logText = "����������� ������������: " + to_string(MyUser.id);
            logFile(logText);
            userFile.close();
            return storedId;
        }
    }



    userFile.close();
    return 0;
}

void registerUser() {
    ofstream userFile("users.txt", ios::app);
    if (!userFile) {
        cerr << "������ �������� ����� �������������." << endl;
        return;
    }

    User newUser;
    while (true) {
        cout << "������� �������� �����: ";
        cin >> newUser.login;
        if (!doesUserExist(newUser.login)) {
            break;
        }
        cerr << "������������ � ����� ������� ��� ����������. ���������� ������ �����." << endl;
    }

    cout << "������� �������� ������: ";
    cin >> newUser.password;
    cout << "������� ���� �������: ";
    cin >> newUser.surname;
    cout << "������� ���� ���: ";
    cin >> newUser.name;
    newUser.id = getLastUserId() + 1;
    newUser.role = 0; // ������������
    // ���� ����� � ������ ���������, ��������� ������ ������������ � ���������� ��� ID
    MyUser.id = newUser.id;
    MyUser.login = newUser.login;
    MyUser.password = newUser.password;
    MyUser.surname = newUser.surname;
    MyUser.name = newUser.name;
    MyUser.role = newUser.role;
    // ������� � ���� ������ ������ ������������
    userFile << newUser.id << " " << newUser.login << " " << newUser.password << " " << newUser.surname << " " << newUser.name << " " << newUser.role << endl;
    userFile.close();
    sortFileRecordsById("users.txt");
    string logText = "����������� ������� ���������. ������������: " + to_string(newUser.id);
    logFile(logText);
    cout << logText << endl;
}


int main() {
    string logText = "������ �������";
    logFile(logText);
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    // ��������� ������������� ����� � �������� �������� ������������� � ������� ��� ��� �������������
    checkAndCreateUserFile();

    string login, password;
    int userId, choice;;
    bool exitLoop = false;
    while (!exitLoop) {

        cout << "�������� ��������:" << endl;
        cout << "1. �����������" << endl;
        cout << "2. �����������" << endl;
        cout << "3. �����" << endl;
        cout << "��� �����: ";

        cin >> choice;
        system("cls");
        if (cin.fail()) {
            cout << "�������� ����. ����������, ������� �����." << endl;
            cin.clear(); // ����� ����� ������
            cin.ignore(10000, '\n'); // ������������� ������������� �����
        }
        else {
            switch (choice) {
            case 1: { // �����������
                cout << "������� �����: ";
                cin >> login;
                cout << "������� ������: ";
                cin >> password;

                userId = authenticateUser(login, password);
                if (userId != 0) {
                    switch (MyUser.role)
                    {
                    case 0: showMenuUser();
                        break;
                    case 1: showMenuAdmin();
                        break;
                    default:
                        break;
                    }
                }
                else {
                    system("cls");
                    cout << "�������� ����� ��� ������." << endl;

                }
            }
                  break;
            case 2: // �����������
                registerUser();
                if (MyUser.role == 0) { showMenuUser(); }

                break;
            case 3: // �����
                cout << "�� ��������!" << endl;
                exitLoop = true;
                break;
            default:
                cout << "�������� �����. ����������, ���������� ��� ���." << endl;
            }
        }
    }
    logText = "���������� �������";
    logFile(logText);
    return 0;
}
