#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>
using namespace std;

const double PI = 3.141592653589793;
const double EPSILON = 0.01;

struct Point {
    double x, y;
};

// Функция для преобразования строки в число
bool parseDouble(const char*& ptr, double& value) {
    value = 0.0;
    double fraction = 0.0, divisor = 1.0;
    bool negative = false;

    while (isspace(*ptr)) ptr++;
    if (*ptr == '-' || *ptr == '+') negative = (*ptr++ == '-');

    if (!isdigit(*ptr)) return false;
    while (isdigit(*ptr)) value = value * 10 + (*ptr++ - '0');

    if (*ptr == '.') {
        ptr++;
        while (isdigit(*ptr)) {
            fraction = fraction * 10 + (*ptr++ - '0');
            divisor *= 10;
        }
        value += fraction / divisor;
    }

    if ((*ptr == 'e' || *ptr == 'E') && isdigit(*(ptr + 1))) {
        int exponent = 0;
        bool expNegative = (*++ptr == '-') && ++ptr;
        while (isdigit(*ptr)) exponent = exponent * 10 + (*ptr++ - '0');
        value *= pow(10, expNegative ? -exponent : exponent);
    }

    value = negative ? -value : value;
    return true;
}

// Функция для проверки строки и извлечения координат
bool parseLine(const char* line, double& x, double& y) {
    const char* ptr = line;
    return parseDouble(ptr, x) && parseDouble(ptr, y) && *ptr == '\0';
}

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
    return i > 0;
}

bool isUniquePoint(const Point points[], int pointCount, double x, double y) {
    for (int i = 0; i < pointCount; ++i)
        if (points[i].x == x && points[i].y == y) return false;
    return true;
}

inline double distance(const Point& p1, const Point& p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

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

void sortPointsClockwise(Point points[], int size, Point center) {
    sort(points, points + size, [&center](const Point& a, const Point& b) {
        return atan2(a.y - center.y, a.x - center.x) < atan2(b.y - center.y, b.x - center.x);
        });
}

bool isRegularPolygon(const Point points[], int size) {
    double sideLength = distance(points[0], points[1]);
    for (int i = 1; i < size; ++i)
        if (fabs(distance(points[i], points[(i + 1) % size]) - sideLength) > EPSILON)
            return false;
    return true;
}

void generateCombinations(Point points[], int pointCount, ofstream& protocolFile, ofstream& outputFile) {
    Point combination[5];
    for (int i = 0; i < pointCount - 4; ++i) {
        for (int j = i + 1; j < pointCount - 3; ++j) {
            for (int k = j + 1; k < pointCount - 2; ++k) {
                for (int l = k + 1; l < pointCount - 1; ++l) {
                    for (int m = l + 1; m < pointCount; ++m) {
                        combination[0] = points[i]; combination[1] = points[j];
                        combination[2] = points[k]; combination[3] = points[l];
                        combination[4] = points[m];

                        Point center = findCenter(combination, 5);
                        sortPointsClockwise(combination, 5, center);

                        protocolFile << "Комбинация: ";
                        for (int n = 0; n < 5; ++n)
                            protocolFile << "(" << combination[n].x << ", " << combination[n].y << ") ";

                        if (isRegularPolygon(combination, 5)) {
                            protocolFile << "- Правильный.\n";
                            outputFile << "Правильный пятиугольник: ";
                            for (int n = 0; n < 5; ++n)
                                outputFile << "(" << combination[n].x << ", " << combination[n].y << ") ";
                            outputFile << "\n";
                        }
                        else {
                            protocolFile << "- Неправильный.\n";
                        }
                    }
                }
            }
        }
    }
}

// Пользовательская функция преобразования double в строку
std::string doubleToString(double value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void processFile(const char* inputFileName, const char* protocolFileName, const char* outputFileName) {
    ifstream inputFile(inputFileName);
    ofstream protocolFile(protocolFileName), outputFile(outputFileName);

    if (!inputFile || !protocolFile || !outputFile) {
        cout << "Ошибка открытия файлов!" << endl;
        return;
    }

    protocolFile << "Начало обработки файла.\n";
    int pointCount = 0;
    inputFile >> pointCount;
    inputFile.ignore();

    if (pointCount <= 0) {
        protocolFile << "Ошибка: некорректное число точек.\n";
        return;
    }
    protocolFile << "Ожидается " << pointCount << " точек.\n";

    Point points[1000];
    char buffer[256];
    double x, y;
    int validPoints = 0;

    while (customReadLine(inputFile, buffer, sizeof(buffer)) && validPoints < pointCount) {
        if (buffer[0] == '\0') {
            protocolFile << "Пропущена пустая строка.\n";
            continue;
        }
        if (parseLine(buffer, x, y) && isUniquePoint(points, validPoints, x, y)) {
            points[validPoints++] = { x, y };
            protocolFile << "Считана точка: (" << x << "; " << y << ").\n";
        }
        else {
            protocolFile << "Пропущена строка: " << buffer << "\n";
        }
    }

    // Проверяем, все ли точки считаны
    protocolFile << (validPoints < pointCount ?
        "Предупреждение: Считано " + doubleToString(validPoints) + " точек.\n" :
        "Все точки успешно считаны.\n");

    // Записываем считанные точки в output
    outputFile << "Считанные точки (" << validPoints << "):\n";
    for (int i = 0; i < validPoints; ++i) {
        outputFile << "(" << points[i].x << ", " << points[i].y << ")\n";
    }
    outputFile << "\n";

    // Генерируем комбинации
    generateCombinations(points, validPoints, protocolFile, outputFile);

    protocolFile << "Обработка файла завершена.\n";
}

int main() {
    setlocale(LC_ALL, "ru");
    processFile("points.txt", "protocol.txt", "output.txt");
    system("output.txt");
    return 0;
}
