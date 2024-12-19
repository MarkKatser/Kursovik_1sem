#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

#define PI 3.14159265358979323846

// Структура для представления точки
struct Point {
    double x, y;
};

// Функция для поворота точки на угол относительно центра
Point rotatePoint(const Point& p, double angle, const Point& center) {
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);
    double x = cosAngle * (p.x - center.x) - sinAngle * (p.y - center.y) + center.x;
    double y = sinAngle * (p.x - center.x) + cosAngle * (p.y - center.y) + center.y;
    return { x, y };
}

// Функция для генерации правильного пятиугольника
std::vector<Point> generateRegularPentagon(double radius, const Point& center, double rotationAngle) {
    std::vector<Point> vertices;
    for (int i = 0; i < 5; ++i) {
        double angle = 2 * PI * i / 5; // Угол вершины
        Point vertex = {
            center.x + radius * cos(angle),
            center.y + radius * sin(angle)
        };
        // Поворот точки
        vertex = rotatePoint(vertex, rotationAngle, center);
        vertices.push_back(vertex);
        std::cout << vertex.x << " " << vertex.y << "\n";
    }
    return vertices;
}

// Проверка, лежит ли точка внутри пятиугольника (метод луча)
bool isPointInsidePentagon(const std::vector<Point>& pentagon, const Point& p) {
    int intersections = 0;
    for (size_t i = 0; i < pentagon.size(); ++i) {
        Point v1 = pentagon[i];
        Point v2 = pentagon[(i + 1) % pentagon.size()];

        if ((p.y > v1.y) != (p.y > v2.y)) {
            double intersectX = v1.x + (v2.x - v1.x) * (p.y - v1.y) / (v2.y - v1.y);
            if (p.x < intersectX) {
                ++intersections;
            }
        }
    }
    return intersections % 2 == 1;
}

// Функция для генерации случайных точек внутри пятиугольника
std::vector<Point> generatePointsInsidePentagon(const std::vector<Point>& pentagon, int numPoints) {
    std::vector<Point> points;
    double minX = pentagon[0].x, maxX = pentagon[0].x;
    double minY = pentagon[0].y, maxY = pentagon[0].y;

    // Найти границы пятиугольника
    for (const auto& vertex : pentagon) {
        if (vertex.x < minX) minX = vertex.x;
        if (vertex.x > maxX) maxX = vertex.x;
        if (vertex.y < minY) minY = vertex.y;
        if (vertex.y > maxY) maxY = vertex.y;
    }

    // Генерировать точки внутри пятиугольника
    while (points.size() < static_cast<size_t>(numPoints)) {
        Point p;
        p.x = minX + (double(rand()) / RAND_MAX) * (maxX - minX);
        p.y = minY + (double(rand()) / RAND_MAX) * (maxY - minY);
        if (isPointInsidePentagon(pentagon, p)) {
            points.push_back(p);
        }
    }

    return points;
}

// Функция для вывода данных в формате Desmos
void outputForDesmos(const std::vector<Point>& vertices) {
    std::cout << "\nДля Desmos:\n";
    for (const auto& vertex : vertices) {
        std::cout << "x=(" << vertex.x << ", " << vertex.y << ")\n";
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел

    double radius;
    char centerOption, orientationOption;
    Point center;
    double rotationAngle = 0.0;

    std::cout << "Введите радиус пятиугольника: ";
    std::cin >> radius;

    // Выбор центра: задать вручную или случайно
    std::cout << "Вы хотите задать центр вручную? (+/-): ";
    std::cin >> centerOption;

    if (centerOption == '+') {
        std::cout << "Введите координаты центра (x y): ";
        std::cin >> center.x >> center.y;
    }
    else {
        center.x = (rand() % 200) - 100; // Случайное значение в диапазоне [-100, 100]
        center.y = (rand() % 200) - 100;
        std::cout << "Случайный центр: (" << center.x << ", " << center.y << ")\n";
    }

    // Выбор ориентации: направить вверх или повернуть случайно
    std::cout << "Вы хотите повернуть пятиугольник случайно? (+/-): ";
    std::cin >> orientationOption;

    if (orientationOption == '+') {
        rotationAngle = ((double)rand() / RAND_MAX) * 2 * PI; // Случайный угол
        std::cout << "Случайный угол поворота: " << rotationAngle * 180 / PI << " градусов\n";
    }
    else {
        std::cout << "Пятиугольник будет направлен вершиной вверх.\n";
    }

    // Генерация правильного пятиугольника
    std::cout << "Правильный пятиугольник:\n";
    auto vertices = generateRegularPentagon(radius, center, rotationAngle);

    // Вывод данных в формате Desmos
    outputForDesmos(vertices);

    // Добавление точек внутрь пятиугольника
    int numInnerPoints;
    std::cout << "Сколько точек добавить внутрь пятиугольника? (0 для завершения): ";
    std::cin >> numInnerPoints;

    if (numInnerPoints > 0) {
        auto innerPoints = generatePointsInsidePentagon(vertices, numInnerPoints);
        std::cout << "\nСлучайные точки внутри пятиугольника:\n";
        for (const auto& point : innerPoints) {
            std::cout << point.x << " " << point.y << "\n";
        }

        std::cout << "\nДля Desmos (точки внутри):\n";
        for (const auto& point : innerPoints) {
            std::cout << "x=(" << point.x << ", " << point.y << ")\n";
        }
    }

    return 0;
}
