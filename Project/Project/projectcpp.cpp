#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <functional>
#include <cctype> // Для функции isalpha()
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
    ofstream file(filename, std::ios::app);  // Открыть файл для записи в режиме добавления (append)
    if (!file) {
        file.open(filename);  // Создать новый файл, если файл не существует
        file.close();  // Закрыть файл после создания
        file.open(filename, std::ios::app);  // Открыть созданный файл для записи в режиме добавления
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
        cerr << "Ошибка открытия файла пользователей." << endl;
        return nullptr;
    }

    // Определяем количество пользователей в файле
    int userCount = 0;
    string line;
    while (getline(userFile, line)) {
        userCount++;
    }
    userFile.clear(); // Сбрасываем флаг ошибки
    userFile.seekg(0, ios::beg); // Возвращаемся в начало файла

    // Создаем массив пользователей
    User* users = new User[userCount + 1]; // +1 для завершающего элемента с id = -1

    // Считываем пользователей из файла в массив
    int i = 0;
    while (getline(userFile, line)) {
        users[i] = parseUser(line);
        i++;
    }

    // Добавляем завершающий элемент
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
        cerr << "Ошибка открытия файла " << filename << endl;
        return;
    }

    User* users = nullptr;
    int numUsers = 0;

    // Чтение пользователей из файла
    string line;
    while (getline(inputFile, line)) {
        User user = parseUser(line);

        // Увеличение размера массива пользователей
        User* temp = new User[numUsers + 1];
        for (int i = 0; i < numUsers; ++i) {
            temp[i] = users[i];
        }
        temp[numUsers++] = user;
        delete[] users;
        users = temp;
    }
    inputFile.close();

    // Быстрая сортировка пользователей по идентификатору
    quickSort(users, 0, numUsers - 1);

    // Перезапись отсортированных записей в файл
    ofstream outputFile(filename);
    if (!outputFile) {
        cerr << "Ошибка открытия файла " << filename << " для записи" << endl;
        delete[] users; // Очистка памяти перед выходом из функции
        return;
    }

    for (int i = 0; i < numUsers; ++i) {
        outputFile << serializeUser(users[i]) << endl;
    }
    outputFile.close();

    // Освобождение выделенной памяти
    delete[] users;
}

int getLastUserId() {
    sortFileRecordsById("users.txt");
    ifstream userFile("users.txt");
    if (!userFile) {
        cerr << "Ошибка открытия файла пользователей." << endl;
        return -1; // Возвращаем -1 в случае ошибки
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
        cerr << "Ошибка открытия файла пользователей." << endl;
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

    // временные массивы для хранения половин
    User* L = new User[n1];
    User* R = new User[n2];

    // копирование данных во временные массивы L[] и R[]
    for (int i = 0; i < n1; i++)
        L[i] = users[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = users[mid + 1 + j];

    // слияние временных массивов обратно в users[left..right]
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

    // копирование оставшихся элементов L[], если они есть
    while (i < n1) {
        users[k] = L[i];
        i++;
        k++;
    }

    // копирование оставшихся элементов R[], если они есть
    while (j < n2) {
        users[k] = R[j];
        j++;
        k++;
    }

    // освобождаем память, выделенную для временных массивов
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
        cerr << "Ошибка создания файла с учетными записями пользователей." << endl;
        return;
    }

    User admin;
    admin.id = 1; // ID администратора
    admin.login = "admin";
    admin.password = "admin"; // Здесь, конечно, необходимо использовать безопасный механизм хеширования
    // Здесь также необходимо генерировать уникальную соль
    admin.surname = "Системный";
    admin.name = "Администратор";// Добавлено поле fullName с значением
    admin.role = 1; // Роль администратора
    // Выводим в файл данные администратора
    userFile << admin.id << " " << admin.login << " " << admin.password << " " << admin.surname << " " << admin.name << " " << admin.role << endl;
    userFile.close();
}

void checkAndCreateUserFile() {
    ifstream userFile("users.txt");
    if (!userFile.is_open()) {

        userFile.close();
        createAdminAccount();
        string logText = "Файл с учетными записями пользователей создан. Добавлен штатный администратор.";
        logFile(logText);
        cout << logText << endl;
    }
    else {
        // Файл существует, проверяем, пустой ли он
        userFile.seekg(0, ios::end);
        if (userFile.tellg() == 0) {
            // Файл пуст, добавляем администратора
            userFile.close();
            createAdminAccount();
            string logText = "Файл с учетными записями пользователей пуст. Добавлен штатный администратор.";
            logFile(logText);
            cout << logText << endl;
        }
    }
}
int authenticateUser(const string& login, const string& password) {
    ifstream userFile("users.txt");
    if (!userFile) {
        cerr << "Файл с учетными записями пользователей не существует." << endl;
        return 0;
    }

    int storedId, storedRole;
    string storedLogin, storedPassword, storedSurname, storedName;
    while (userFile >> storedId >> storedLogin >> storedPassword >> storedSurname >> storedName >> storedRole) {
        // Проверяем логин и пароль
        if (storedLogin == login && storedPassword == password) {
            // Если логин и пароль совпадают, заполняем данные пользователя и возвращаем его ID
            MyUser.id = storedId;
            MyUser.login = storedLogin;
            MyUser.password = storedPassword;
            MyUser.surname = storedSurname;
            MyUser.name = storedName;
            MyUser.role = storedRole;
            string logText = "Авторизация пользователя: " + to_string(MyUser.id);
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
        cerr << "Ошибка открытия файла пользователей." << endl;
        return;
    }

    User newUser;
    while (true) {
        cout << "Введите желаемый логин: ";
        cin >> newUser.login;
        if (!doesUserExist(newUser.login)) {
            break;
        }
        cerr << "Пользователь с таким логином уже существует. Попробуйте другой логин." << endl;
    }

    cout << "Введите желаемый пароль: ";
    cin >> newUser.password;
    cout << "Введите вашу фамилию: ";
    cin >> newUser.surname;
    cout << "Введите ваше имя: ";
    cin >> newUser.name;
    newUser.id = getLastUserId() + 1;
    newUser.role = 0; // Пользователь
    // Если логин и пароль совпадают, заполняем данные пользователя и возвращаем его ID
    MyUser.id = newUser.id;
    MyUser.login = newUser.login;
    MyUser.password = newUser.password;
    MyUser.surname = newUser.surname;
    MyUser.name = newUser.name;
    MyUser.role = newUser.role;
    // Выводим в файл данные нового пользователя
    userFile << newUser.id << " " << newUser.login << " " << newUser.password << " " << newUser.surname << " " << newUser.name << " " << newUser.role << endl;
    userFile.close();
    sortFileRecordsById("users.txt");
    string logText = "Регистрация успешно завершена. Пользователь: " + to_string(newUser.id);
    logFile(logText);
    cout << logText << endl;
}


int main() {
    string logText = "Запуск системы";
    logFile(logText);
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    // Проверяем существование файла с учетными записями пользователей и создаем его при необходимости
    checkAndCreateUserFile();

    string login, password;
    int userId, choice;;
    bool exitLoop = false;
    while (!exitLoop) {

        cout << "Выберите действие:" << endl;
        cout << "1. Авторизация" << endl;
        cout << "2. Регистрация" << endl;
        cout << "3. Выход" << endl;
        cout << "Ваш выбор: ";

        cin >> choice;
        system("cls");
        if (cin.fail()) {
            cout << "Неверный ввод. Пожалуйста, введите число." << endl;
            cin.clear(); // Сброс флага ошибки
            cin.ignore(10000, '\n'); // Игнорирование некорректного ввода
        }
        else {
            switch (choice) {
            case 1: { // Авторизация
                cout << "Введите логин: ";
                cin >> login;
                cout << "Введите пароль: ";
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
                    cout << "Неверный логин или пароль." << endl;

                }
            }
                  break;
            case 2: // Регистрация
                registerUser();
                if (MyUser.role == 0) { showMenuUser(); }

                break;
            case 3: // Выход
                cout << "До свидания!" << endl;
                exitLoop = true;
                break;
            default:
                cout << "Неверный выбор. Пожалуйста, попробуйте еще раз." << endl;
            }
        }
    }
    logText = "Выключение системы";
    logFile(logText);
    return 0;
}
