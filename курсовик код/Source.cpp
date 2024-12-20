#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

// Устанавливаем константу для значения числа π
const double PI = 3.141592653589793;

// Функция для преобразования строки в число
bool parseDouble(const char*& ptr, double& value) {
    bool negative = false;
    value = 0.0;
    double fraction = 0.0;
    double divisor = 1.0;

    // Пропускаем пробелы
    while (isspace(*ptr)) ptr++;

    // Проверяем знак числа
    if (*ptr == '-') {
        negative = true;
        ptr++;
    }
    else if (*ptr == '+') {
        ptr++;
    }

    // Обрабатываем целую часть числа
    if (!isdigit(*ptr)) return false;
    while (isdigit(*ptr)) {
        value = value * 10 + (*ptr - '0');
        ptr++;
    }

    // Обрабатываем дробную часть числа, если есть
    if (*ptr == '.') {
        ptr++;
        while (isdigit(*ptr)) {
            fraction = fraction * 10 + (*ptr - '0');
            divisor *= 10;
            ptr++;
        }
        value += fraction / divisor;
    }

    // Обрабатываем экспоненциальную часть числа, если есть
    if (*ptr == 'e' || *ptr == 'E') {
        ptr++;
        bool expNegative = false;
        int exponent = 0;

        if (*ptr == '-') {
            expNegative = true;
            ptr++;
        }
        else if (*ptr == '+') {
            ptr++;
        }

        if (!isdigit(*ptr)) return false;
        while (isdigit(*ptr)) {
            exponent = exponent * 10 + (*ptr - '0');
            ptr++;
        }

        double expValue = pow(10, expNegative ? -exponent : exponent);
        value *= expValue;
    }

    if (negative) value = -value;
    return true;
}

// Функция для проверки строки и извлечения координат
bool parseLine(const char* line, double& x, double& y) {
    int count = 0;
    const char* ptr = line;

    while (*ptr != '\0' && count < 2) {
        // Пропускаем пробелы
        while (isspace(*ptr)) ptr++;

        double value;
        if (parseDouble(ptr, value)) {
            if (count == 0) x = value;
            else y = value;
            count++;
        }
        else {
            return false; // Если не удалось считать число, строка некорректна
        }
    }

    // Пропускаем оставшиеся символы, если они есть
    while (*ptr != '\0') {
        if (!isspace(*ptr)) return false; // Если не пробелы, строка некорректна
        ptr++;
    }

    return count == 2; // Условие: строка корректна, если удалось считать 2 числа
}

// Функция для считывания строки вручную
bool customReadLine(ifstream& inputFile, char* buffer, size_t bufferSize) {
    size_t i = 0;
    char ch;
    while (inputFile.get(ch)) {
        if (ch == '\n' || i == bufferSize - 1) {
            buffer[i] = '\0';
            return true;
        }
        buffer[i++] = ch;
    }
    buffer[i] = '\0';
    return i > 0; // Возвращаем true, если что-то было считано
}

// Структура для хранения точки
struct Point {
    double x, y;
};

// Функция для проверки уникальности точки
bool isUniquePoint(const Point points[], int pointCount, double x, double y) {
    for (int i = 0; i < pointCount; ++i) {
        if (points[i].x == x && points[i].y == y) {
            return false; // Точка уже существует
        }
    }
    return true;
}

// Функция для генерации всех возможных комбинаций из пяти точек
void generateCombinations(Point points[], int pointCount, ofstream& protocolFile) {
    for (int i = 0; i < pointCount - 4; ++i) {
        for (int j = i + 1; j < pointCount - 3; ++j) {
            for (int k = j + 1; k < pointCount - 2; ++k) {
                for (int l = k + 1; l < pointCount - 1; ++l) {
                    for (int m = l + 1; m < pointCount; ++m) {
                        protocolFile << "Комбинация: (" << points[i].x << ", " << points[i].y << "), "
                            << "(" << points[j].x << ", " << points[j].y << "), "
                            << "(" << points[k].x << ", " << points[k].y << "), "
                            << "(" << points[l].x << ", " << points[l].y << "), "
                            << "(" << points[m].x << ", " << points[m].y << ").\n";
                    }
                }
            }
        }
    }
}

// Основная функция обработки файла
void processFile(const char* inputFileName, const char* protocolFileName, const char* outputFileName) {
    ifstream inputFile(inputFileName);
    ofstream protocolFile(protocolFileName);
    ofstream outputFile(outputFileName);

    if (!inputFile.is_open() || !protocolFile.is_open() || !outputFile.is_open()) {
        cout << "Ошибка открытия файлов!" << endl;
        return;
    }

    protocolFile << "Начало обработки файла.\n";

    int pointCount = 0; // Количество ожидаемых точек
    inputFile >> pointCount;
    inputFile.ignore(); // Пропуск первого символа новой строки

    if (pointCount <= 0) {
        protocolFile << "Ошибка: некорректное число точек.\n";
        return;
    }
    protocolFile << "Ожидается " << pointCount << " точек.\n";

    double x, y;
    int validPoints = 0;
    char buffer[256];

    Point points[1000]; // Массив для хранения уникальных точек (предполагаем, что не более 1000 точек)

    while (customReadLine(inputFile, buffer, sizeof(buffer)) && validPoints < pointCount) {
        if (buffer[0] == '\0') {
            protocolFile << "Пропущена пустая строка.\n";
            continue;
        }
        if (parseLine(buffer, x, y)) {
            // Проверяем, уникальна ли точка
            if (isUniquePoint(points, validPoints, x, y)) {
                protocolFile << "Считана точка: (" << x << "; " << y << ").\n";
                outputFile << x << " " << y << endl;
                points[validPoints].x = x;
                points[validPoints].y = y;
                validPoints++;
            }
            else {
                protocolFile << "Пропущена повторяющаяся точка: (" << x << "; " << y << ").\n";
            }
        }
        else {
            protocolFile << "Пропущена некорректная строка: \"" << buffer << "\".\n";
        }
    }

    if (validPoints < pointCount) {
        protocolFile << "Предупреждение: считано только " << validPoints << " точек из " << pointCount << ".\n";
    }
    else {
        protocolFile << "Все " << pointCount << " точек успешно считаны.\n";
    }

    // Вызываем функцию для генерации комбинаций из пяти точек
    generateCombinations(points, validPoints, protocolFile);

    protocolFile << "Обработка файла завершена.\n";
    inputFile.close();
    protocolFile.close();
    outputFile.close();
}

int main() {
    setlocale(LC_ALL, "ru");

    // Имена файлов
    const char* inputFileName = "points.txt";
    const char* protocolFileName = "protocol.txt";
    const char* outputFileName = "output.txt";

    // Запуск обработки файла
    processFile(inputFileName, protocolFileName, outputFileName);

    // Открываем файл вывода для просмотра
    system(outputFileName);

    return 0;
}
