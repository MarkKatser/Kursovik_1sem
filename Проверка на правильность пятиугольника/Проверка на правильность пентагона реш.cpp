#include <iostream>
#include <cmath>

// Структура для хранения точки на плоскости
struct Point {
    double x, y;
};

// Функция для вычисления расстояния между двумя точками
double distance(const Point& p1, const Point& p2) {
    return std::sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

// Функция для вычисления знака площади треугольника (определяет, по часовой стрелке или против)
double crossProduct(const Point& p1, const Point& p2, const Point& p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

// Функция для сортировки точек по часовой стрелке относительно центра
void sortPoints(Point points[], Point center) {
    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
            if (crossProduct(center, points[i], points[j]) < 0) {
                // Меняем местами точки
                Point temp = points[i];
                points[i] = points[j];
                points[j] = temp;
            }
        }
    }
}

// Функция для проверки, является ли пятиугольник правильным
bool isRegularPolygon(const Point points[]) {
    double sideLength = distance(points[0], points[1]);

    // Проверка всех сторон
    for (int i = 1; i < 5; ++i) {
        int next = (i + 1) % 5;
        if (std::abs(distance(points[i], points[next]) - sideLength) > 0.01) {
            return false;
        }
    }

    return true;
}

int main() {
    setlocale(LC_ALL, "ru");

    Point points[5];

    // Ввод координат пяти точек
    std::cout << "Введите 5 точек (x, y):\n";
    for (int i = 0; i < 5; ++i) {
        std::cin >> points[i].x >> points[i].y;
    }

    // Нахождение центральной точки для сортировки по часовой стрелке
    Point center = { 0, 0 };
    for (int i = 0; i < 5; ++i) {
        center.x += points[i].x;
        center.y += points[i].y;
    }
    center.x /= 5;
    center.y /= 5;

    // Сортировка точек по часовой стрелке относительно центра
    sortPoints(points, center);

    // Вывод отсортированных точек
    std::cout << "Точки по часовой стрелке:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << "(" << points[i].x << ", " << points[i].y << ")\n";
    }

    // Проверка, является ли пятиугольник правильным
    if (isRegularPolygon(points)) {
        std::cout << "Пятиугольник правильный.\n";
    }
    else {
        std::cout << "Пятиугольник не правильный.\n";
    }

    return 0;
}
