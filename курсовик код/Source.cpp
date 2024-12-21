#include <iostream> 
#include <fstream>
#include <cmath>
#include <algorithm>
using namespace std;

// Устанавливаем константу для значения числа π
const double PI = 3.141592653589793;
const double EPSILON = 0.01; // Допустимая погрешность для сравнения сторон

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

// Функция для вычисления расстояния между двумя точками
double distance(const Point& p1, const Point& p2) {
    return std::sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

// Функция для нахождения центра пятиугольника
Point findCenter(const Point points[], int size) {
    Point center = { 0, 0 };
    for (int i = 0; i < size; ++i) {
        center.x += points[i].x;
        center.y += points[i].y;
    }
    center.x /= size;
    center.y /= size;
    return center;
}

// Функция для сортировки точек по часовой стрелке относительно центра
void sortPointsClockwise(Point points[], int size, Point center) {
    std::sort(points, points + size, [&center](const Point& a, const Point& b) {
        double angleA = atan2(a.y - center.y, a.x - center.x);
        double angleB = atan2(b.y - center.y, b.x - center.x);
        return angleA < angleB;
        });
}

// Функция для проверки, является ли пятиугольник правильным
bool isRegularPolygon(const Point points[], int size) {
    double sideLength = distance(points[0], points[1]);
    for (int i = 1; i < size; ++i) {
        int next = (i + 1) % size;
        if (std::abs(distance(points[i], points[next]) - sideLength) > EPSILON) {
            return false;
        }
    }
    return true;
}

// Функция для генерации всех возможных комбинаций из пяти точек и проверки их правильности
void generateCombinations(Point points[], int pointCount, ofstream& protocolFile, ofstream& outputFile) {
    Point combination[5];
    for (int i = 0; i < pointCount - 4; ++i) {
        for (int j = i + 1; j < pointCount - 3; ++j) {
            for (int k = j + 1; k < pointCount - 2; ++k) {
                for (int l = k + 1; l < pointCount - 1; ++l) {
                    for (int m = l + 1; m < pointCount; ++m) {
                        combination[0] = points[i];
                        combination[1] = points[j];
                        combination[2] = points[k];
                        combination[3] = points[l];
                        combination[4] = points[m];

                        Point center = findCenter(combination, 5);
                        sortPointsClockwise(combination, 5, center);

                        protocolFile << "Комбинация: (" << combination[0].x << ", " << combination[0].y << "), "
                            << "(" << combination[1].x << ", " << combination[1].y << "), "
                            << "(" << combination[2].x << ", " << combination[2].y << "), "
                            << "(" << combination[3].x << ", " << combination[3].y << "), "
                            << "(" << combination[4].x << ", " << combination[4].y << ") - ";

                        if (isRegularPolygon(combination, 5)) {
                            protocolFile << "Правильный.\n";
                            outputFile << "Правильный пятиугольник: (" << combination[0].x << ", " << combination[0].y << "), "
                                << "(" << combination[1].x << ", " << combination[1].y << "), "
                                << "(" << combination[2].x << ", " << combination[2].y << "), "
                                << "(" << combination[3].x << ", " << combination[3].y << "), "
                                << "(" << combination[4].x << ", " << combination[4].y << ").\n";
                        }
                        else {
                            protocolFile << "Неправильный.\n";
                        }
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
    generateCombinations(points, validPoints, protocolFile, outputFile);

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
