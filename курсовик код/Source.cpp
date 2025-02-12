﻿#include <iostream>
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

struct BoundingBox {
    double minX, minY, maxX, maxY;
};

BoundingBox calculateBoundingBox(const Point polygon[], int size) {
    BoundingBox box = { polygon[0].x, polygon[0].y, polygon[0].x, polygon[0].y };
    for (int i = 1; i < size; ++i) {
        box.minX = min(box.minX, polygon[i].x);
        box.minY = min(box.minY, polygon[i].y);
        box.maxX = max(box.maxX, polygon[i].x);
        box.maxY = max(box.maxY, polygon[i].y);
    }
    return box;
}

bool isPointInBoundingBox(const BoundingBox& box, const Point& p) {
    return p.x >= box.minX && p.x <= box.maxX && p.y >= box.minY && p.y <= box.maxY;
}

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

// Функция для извлечения чисел из строки
int extractNumbers(const char* line, double numbers[], int maxCount) {
    const char* ptr = line;
    int count = 0;
    while (*ptr && count < maxCount) {
        double value;
        if (parseDouble(ptr, value)) {
            numbers[count++] = value;
        }
        else {
            while (*ptr && !isdigit(*ptr) && *ptr != '-' && *ptr != '+') {
                ptr++; // Пропускаем символы, которые не являются частью числа
            }
        }
    }
    return count;
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

inline double angle(const Point& a, const Point& b, const Point& c) {
    double abx = b.x - a.x;
    double aby = b.y - a.y;
    double cbx = b.x - c.x;
    double cby = b.y - c.y;
    double dotProduct = abx * cbx + aby * cby;
    double magnitudeAB = sqrt(abx * abx + aby * aby);
    double magnitudeCB = sqrt(cbx * cbx + cby * cby);
    return acos(dotProduct / (magnitudeAB * magnitudeCB));
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
    double angleValue = angle(points[size - 1], points[0], points[1]);
    for (int i = 1; i < size; ++i) {
        if (fabs(distance(points[i], points[(i + 1) % size]) - sideLength) > EPSILON ||
            fabs(angle(points[i - 1], points[i], points[(i + 1) % size]) - angleValue) > EPSILON)
            return false;
    }
    return true;
}

// Проверка принадлежности точки пятиугольнику через пересечение отрезков
bool isPointInsidePolygon(const Point polygon[], int size, const Point& p) {
    int count = 0;

    // Проверяем пересечения
    for (int i = 0; i < size; ++i) {
        int next = (i + 1) % size;
        Point a = polygon[i], b = polygon[next];

        // Проверяем, не совпадает ли точка p с одной из вершин
        if (fabs(p.x - a.x) < EPSILON && fabs(p.y - a.y) < EPSILON) {
            return false; // Игнорируем точки-вершины
        }

        // Проверка пересечения
        if ((a.y > p.y) != (b.y > p.y)) {
            double intersectionX = a.x + (p.y - a.y) * (b.x - a.x) / (b.y - a.y);
            if (p.x < intersectionX) {
                count++;
            }
        }
    }
    return count % 2 == 1; // Нечётное количество пересечений означает, что точка внутри
}

void generateCombinations(Point points[], int pointCount, ofstream& protocolFile, ofstream& outputFile) {
    if (pointCount < 5) {
        protocolFile << "Ошибка: недостаточно точек для формирования пятиугольника.\n";
        outputFile << "Недостаточно точек для формирования пятиугольника.\n";
        return;
    }

    Point combination[5];
    int maxInsideCount = 0;

    // Выделение памяти в куче
    auto maxPolygons = new Point[100][5]; // Хранение до 100 пятиугольников
    auto allInsidePoints = new Point[100][1000]; // Точки внутри пятиугольников
    auto insidePointCounts = new int[100](); // Количество точек внутри каждого пятиугольника
    int maxPolygonCount = 0;

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
                            int insideCount = 0;
                            auto insidePoints = new Point[1000]; // Выделение памяти для точек внутри
                            BoundingBox box = calculateBoundingBox(combination, 5);

                            for (int p = 0; p < pointCount; ++p) {
                                if (!isPointInBoundingBox(box, points[p])) {
                                    continue;
                                }

                                bool isVertex = false;
                                for (int v = 0; v < 5; ++v) {
                                    if (fabs(points[p].x - combination[v].x) < EPSILON &&
                                        fabs(points[p].y - combination[v].y) < EPSILON) {
                                        isVertex = true;
                                        break;
                                    }
                                }
                                if (!isVertex && isPointInsidePolygon(combination, 5, points[p])) {
                                    insidePoints[insideCount++] = points[p];
                                }
                            }

                            if (insideCount > maxInsideCount) {
                                maxInsideCount = insideCount;
                                maxPolygonCount = 0; // Сброс количества пятиугольников
                                std::copy(combination, combination + 5, maxPolygons[maxPolygonCount]);
                                std::copy(insidePoints, insidePoints + insideCount, allInsidePoints[maxPolygonCount]);
                                insidePointCounts[maxPolygonCount] = insideCount;
                                maxPolygonCount++;
                            }
                            else if (insideCount == maxInsideCount) {
                                if (maxPolygonCount < 100) {
                                    std::copy(combination, combination + 5, maxPolygons[maxPolygonCount]);
                                    std::copy(insidePoints, insidePoints + insideCount, allInsidePoints[maxPolygonCount]);
                                    insidePointCounts[maxPolygonCount] = insideCount;
                                    maxPolygonCount++;
                                }
                            }

                            delete[] insidePoints; // Очистка памяти
                            protocolFile << "- Правильный. Точек внутри: " << insideCount << ".\n";
                        }
                        else {
                            protocolFile << "- Неправильный.\n";
                        }
                    }
                }
            }
        }
    }

    if (maxPolygonCount > 0) {
        outputFile << "\nПятиугольники с максимальным количеством точек внутри (" << maxInsideCount << "):\n";
        for (int i = 0; i < maxPolygonCount; ++i) {
            outputFile << "Пятиугольник " << (i + 1) << ":\n";
            for (int j = 0; j < 5; ++j) {
                outputFile << "(" << maxPolygons[i][j].x << ", " << maxPolygons[i][j].y << ")\n";
            }
            outputFile << "Точки внутри:\n";
            for (int j = 0; j < insidePointCounts[i]; ++j) {
                outputFile << "(" << allInsidePoints[i][j].x << ", " << allInsidePoints[i][j].y << ")\n";
            }
            outputFile << "\n";
        }
    }

    // Очистка памяти
    delete[] maxPolygons;
    delete[] allInsidePoints;
    delete[] insidePointCounts;
}

// Преобразование double в строку
string doubleToString(double value) {
    ostringstream oss;
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

    // Проверка на пустой входной файл
    if (inputFile.peek() == ifstream::traits_type::eof()) {
        protocolFile << "Ошибка: входной файл пуст.\n";
        outputFile << "Ошибка: входной файл пуст.\n";
        return;
    }

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
        double numbers[2];
        int numCount = extractNumbers(buffer, numbers, 2);
        if (numCount == 2 && isUniquePoint(points, validPoints, numbers[0], numbers[1])) {
            points[validPoints++] = { numbers[0], numbers[1] };
            protocolFile << "Считана точка: (" << numbers[0] << "; " << numbers[1] << ").\n";
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
    cout << "Задание: Дано   N   произвольных   точек   на   плоскости.Найти   среди   них   точки, являющиеся вершинами(образующими) фигур, обладающих следующими"
         << "свойствами: с максимальным числом точек в области(внутренней или строго выделенной). Задание выполнить для правильного пятиугольника.\n"
         << "Автор: Кацер Марк Алексеевич Группа: 4353\n"
         << "Дата: 24.12.24\n";

    setlocale(LC_ALL, "ru");
    processFile("points.txt", "protocol.txt", "output.txt");
    system("output.txt");
}
