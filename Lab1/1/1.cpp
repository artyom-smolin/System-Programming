#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;
 
#define MAX_ID_LENGTH 32 // максимальная длина идентификатора
#define TABLE_SIZE 257 // размер хэш-таблицы
#define PRIME_NUMBER 251 // простое число близкое к 256 в меньшую сторону

// хэш-функция 
int hashFunction(const string& identifier) {
    int hashValue = 0;
    for (char c : identifier) {
        hashValue += c; // суммирование ASCII-кодов символов идентификатора
    }
    hashValue %= TABLE_SIZE; // обеспечивает хранение 256 идентификаторов
    return hashValue;
}

// рехэширование с помощью произведения
int rehashFunction(int hashValue, int i) { 
    return (hashValue * TABLE_SIZE * i) % PRIME_NUMBER; 
}

// класс Хэш-таблица
class HashTable {
    vector<string> table;
public:
    HashTable():
        table(TABLE_SIZE, "") {}

    // вставка и поиск
    bool insert(const string&);
    bool search(const string&, int&);

    // вывод
    void print();
};

// вставка в хэш-таблицу
bool HashTable::insert(const string& identifier)
{
    int index = hashFunction(identifier);
    int attempts = 0;
    while (!table[index].empty()) {
        attempts++;
        index = rehashFunction(index, attempts);
        if (attempts > TABLE_SIZE) 
            return false; // таблица переполнена
    }
    table[index] = identifier;
    return true;
}

// поиск в хэш-таблице
bool HashTable::search(const string& identifier, int& comparisons) {
    int index = hashFunction(identifier);
    comparisons = 0;
    while (!table[index].empty()) {
        comparisons++;
        if (table[index] == identifier)
            return true;
        index = rehashFunction(index, comparisons);
    }
    return false;
}

// вывод
void HashTable::print() {
    cout << "Индекс\tИдентификатор\n";
    for (int i = 0; i < TABLE_SIZE; ++i) 
        if (!table[i].empty()) 
            cout << i << "\t" << table[i] << '\n';
}

// поиск в простом списке (последовательные сравнения)
bool searchInSimpleList(const vector<string>& simpleList, const string& identifier, int& comparisons) {
    comparisons = 0;
    for (const auto& id : simpleList) {
        comparisons++;
        if (id == identifier)
            return true;
    }
    return false;
}

int main() {
    setlocale(LC_ALL, "rus");

    // чтение идентификаторов из файла
    string filename = "file1.txt";
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла!\n";
        return 1;
    }

    vector<string> identifiers; // массив идентификаторов
    string line;
    while (getline(file, line)) {
        if (line.length() <= MAX_ID_LENGTH)
            identifiers.push_back(line);
    }
    file.close();

    if (identifiers.empty()) {
        cerr << "Файл пуст.\n";
        return 1;
    }

    // создание хэш-таблицы
    HashTable hashTable;
    for (const string& id : identifiers)
        hashTable.insert(id);

    // создание простого списка 
    vector<string> simpleList;
    for (const string& id : identifiers)
        simpleList.push_back(id);
   
    // циклические переменные
    int rehashComparisons, listComparisons, rehashComparisons_all = 0, listComparisons_all = 0;
    bool foundRehash, foundList;
    int cycle_counter = 0;

    // поиск идентификаторов
    for (;;) {
        cout << "Для остановки введите \"#\"\n";
        cout << "Введите идентификатор для поиска: ";
        string searchIdentifier;
        getline(cin, searchIdentifier);
        if (searchIdentifier == "#") {
            system("cls");
            break;
        }
        cout << '\n';
        cycle_counter++;

        // поиск в хэш-таблице с рехэшированием
        rehashComparisons = 0;
        foundRehash = hashTable.search(searchIdentifier, rehashComparisons);
        cout << "Результат поиска в хэш-таблице с рехэшированием: " << (foundRehash ? "Найден" : "Не найден");
        cout << "\nЧисло сравнений: " << rehashComparisons << "\n\n";
        rehashComparisons_all += rehashComparisons;

        // поиск в простом списке
        listComparisons = 0;
        foundList = searchInSimpleList(simpleList, searchIdentifier, listComparisons);
        cout << "Результат поиска в простом списке: " << (foundList ? "Найден" : "Не найден");
        cout << "\nЧисло сравнений: " << listComparisons << "\n\n";
        listComparisons_all += listComparisons;
        
        system("pause");
        system("cls");
    }

    // результаты
    if (cycle_counter == 0)
        cout << "Результатов нет.\n";
    else {
        cout << "Среднее число сравнений в хэш-таблице с рехэшированием методом произведений: " <<
            1.0 * rehashComparisons_all / cycle_counter << '\n';
        cout << "Среднее число сравнений в неупорядоченном списке: " <<
            1.0 * listComparisons_all / cycle_counter << '\n';
    }
    
    cout << '\n';
    system("pause");
    system("cls");

    cout << "Хэш-Таблица:\n";
    hashTable.print();

    cout << "\nПростой список:\n";
    cout << "Индекс\tИдентификатор\n";
    for (int i = 0; i < simpleList.size(); ++i)
        cout << i << "\t" << simpleList[i] << '\n';
    cout << '\n';
    system("pause");

    return 0;
}