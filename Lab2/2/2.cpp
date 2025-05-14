
// 1. Входной язык содержит логические выражения, разделенные символом ; (точка с запятой).
// 2. Логические выражения состоят из идентификаторов, констант 0 и 1, 
// знака присваивания (:=), знаков операций or, xor, and, not и круглых скобок.
// 3. Наличие комментариев неограниченной длины во входном файле. 
// Форму организации комментариев выбрать самостоятельно.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
using namespace std;

// типы лексем
enum class lexemeType {
	ИДЕНТИФИКАТОР, // имена переменных
	КОНСТАНТА, // 0, 1
	КЛЮЧЕВОЕ_СЛОВО, // and, not, xor, or
	КОММЕНТАРИЙ, // #комментарий#
	ОПЕРАТОР_ПРИСВАИВАНИЯ, // :=
	РАЗДЕЛИТЕЛЬ, // ;
	СКОБКА, // ( )
	ОШИБКА // остальное
};

// лексема
struct Lexeme {
	lexemeType type;
	string value;
};

// является ли лексема ключевым словом
bool isKeyword(const string& str) {
	static const vector<string> keywords = { "and", "or", "not", "xor" };
	return find(keywords.begin(), keywords.end(), str) != keywords.end();
}

// определение типа лексемы
Lexeme classifyLexeme(const string& lexeme) {
	if (lexeme.size() > 32) {
		return { lexemeType::ОШИБКА, lexeme }; // контроль длины идентификаторов
	}
	if (lexeme == "#") {
		return { lexemeType::КОММЕНТАРИЙ, lexeme };
	}
	if (lexeme == ":=") {
		return { lexemeType::ОПЕРАТОР_ПРИСВАИВАНИЯ, lexeme };
	}
	if (lexeme == ";") {
		return { lexemeType::РАЗДЕЛИТЕЛЬ, lexeme };
	}
	if (lexeme == "(" || lexeme == ")") {
		return { lexemeType::СКОБКА, lexeme };
	}
	if (lexeme == "0" || lexeme == "1") {
		return { lexemeType::КОНСТАНТА, lexeme };
	}
	if (isKeyword(lexeme)) {
		return { lexemeType::КЛЮЧЕВОЕ_СЛОВО, lexeme };
	}
	regex regular("([a-zA-Z]+)"); // для идентификаторов
	if (regex_match(lexeme, regular)) {
		return { lexemeType::ИДЕНТИФИКАТОР, lexeme };
	}
	return { lexemeType::ОШИБКА, lexeme }; // если ни одно условие не сработало
}

// лексический анализ
vector<Lexeme> analyze(const string& line) {
	vector<Lexeme> lexemes;
	string buff;
	char symb;
	int comment = 0; // для комментариев

	// обработка
	for (size_t i = 0; i < line.size(); ++i) {
		symb = line[i]; // посимвольное считывание

		// обработка комментариев
		{
			if (symb == '#')
				comment++;
			if (comment == 1)
				continue;
			else if (comment == 2) {
				comment = 0;
				buff = symb;
				lexemes.push_back(classifyLexeme(buff));
				buff.clear();
				continue; // чтобы не обрабатывать окончание комментария (в конце цикла)
			}
		}

		// пропуск пробельных символов
		if (isspace(symb)) {
			if (!buff.empty()) {
				lexemes.push_back(classifyLexeme(buff));
				buff.clear();
			}
			continue;
		}

		// обработка оператора присваивания
		if (buff == ":=") {
			lexemes.push_back(classifyLexeme(buff));
			buff.clear();
			continue;
		}

		// обработка разделителя и скобок
		if (symb == ';' || symb == '(' || symb == ')') {
			if (!buff.empty()) {						 // если буффер не пустой
				lexemes.push_back(classifyLexeme(buff)); // то выгрузить буффер
				buff.clear();							 // и очистить
			}
			lexemes.push_back(classifyLexeme(string(1, symb))); // обработать символ
			continue;											// перейти к следующей итерации (чтобы не обрабатывать ещё раз)
		}

		buff += symb; // добавление текущего символа в буффер
	}

	// обработка последней лексемы (если нет ;)
	if (!buff.empty())
		lexemes.push_back(classifyLexeme(buff));

	return lexemes;
}

int main()
{
	setlocale(LC_ALL, "rus");
	cout << "\033[97m";

	// выбор файла
	string file_name;
	int choice;
	cout << "Выберите файл для анализа: \n1. file_1.txt\n2. file_2.txt\n3. file_3.txt\nВвод: ";
	cin >> choice;
	system("cls");
	switch (choice) {
	case 1:
		file_name = "file_1.txt";
		break;
	case 2:
		file_name = "file_2.txt";
		break;
	case 3:
		file_name = "file_3.txt";
		break;
	default:
		cout << "Такого варианта не существует.\n";
		return 0;
	}

	// чтение данных из файла
	ifstream in;
	in.open(file_name);
	string line = "", program;
	if (!in.is_open())
		cout << "Не удалось открыть файл.\n";
	while (getline(in, line)) {
		program += line + " "; // для правильной обработки переносов
	}
	in.close();

	vector<Lexeme> lexemes = analyze(program);	// получение результата (разбиение на лексемы и их анализ)
	vector<pair<Lexeme, int>> errors;			// массив ошибок
	int error_count = 0;						// счётчик ошибок

	// вывод результатов
	cout << "\033[92m" "\n№\tЛексема\t\t\tЗначение\n" "\033[97m";
	for (size_t i = 0; i < lexemes.size(); ++i) {
		cout << i + 1 << '\t';
		switch (lexemes[i].type) { // тип
		case lexemeType::ИДЕНТИФИКАТОР:
			cout << "ИДЕНТИФИКАТОР\t\t";
			break;
		case lexemeType::КОНСТАНТА:
			cout << "КОНСТАНТА\t\t";
			break;
		case lexemeType::ОПЕРАТОР_ПРИСВАИВАНИЯ:
			cout << "ОПЕРАТОР ПРИСВАИВАНИЯ\t";
			break;
		case lexemeType::КЛЮЧЕВОЕ_СЛОВО:
			cout << "КЛЮЧЕВОЕ СЛОВО\t\t";
			break;
		case lexemeType::КОММЕНТАРИЙ:
			cout << "КОММЕНТАРИЙ\t\t";
			break;
		case lexemeType::РАЗДЕЛИТЕЛЬ:
			cout << "РАЗДЕЛИТЕЛЬ\t\t";
			break;
		case lexemeType::СКОБКА:
			cout << "СКОБКА\t\t\t";
			break;
		default:
			cout << "\033[91m" "ОШИБКА\t\t\t" "\033[97m";
			error_count++;
			errors.push_back({ lexemes[i], i + 1 }); // запоминание ошибки
			break;
		}
		cout << lexemes[i].value << '\n'; // значение
	}

	// вывод ошибок 
	{
		if (error_count != 0) {
			cout << "\033[91m" "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" "\033[97m";
			cout << "\nВсего ошибок: " << error_count << '\n';
			for (size_t i = 0; i < errors.size(); ++i)
				cout << "error: \"" << errors[i].first.value << "\"\tin the " << errors[i].second << " index\n";
		}
		else {
			cout << "\033[92m" "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" "\033[97m";
			cout << "\nВсего ошибок: " << error_count << '\n';
		}
	}

	return 0;
}