#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

#define PI 3.14159265358979323846

// ��������� ��� ������������� �����
struct Point {
    double x, y;
};

// ������� ��� �������� ����� �� ���� ������������ ������
Point rotatePoint(const Point& p, double angle, const Point& center) {
    double cosAngle = cos(angle);
    double sinAngle = sin(angle);
    double x = cosAngle * (p.x - center.x) - sinAngle * (p.y - center.y) + center.x;
    double y = sinAngle * (p.x - center.x) + cosAngle * (p.y - center.y) + center.y;
    return { x, y };
}

// ������� ��� ��������� ����������� �������������
std::vector<Point> generateRegularPentagon(double radius, const Point& center, double rotationAngle) {
    std::vector<Point> vertices;
    for (int i = 0; i < 5; ++i) {
        double angle = 2 * PI * i / 5; // ���� �������
        Point vertex = {
            center.x + radius * cos(angle),
            center.y + radius * sin(angle)
        };
        // ������� �����
        vertex = rotatePoint(vertex, rotationAngle, center);
        vertices.push_back(vertex);
        std::cout << vertex.x << " " << vertex.y << "\n";
    }
    return vertices;
}

// ��������, ����� �� ����� ������ ������������� (����� ����)
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

// ������� ��� ��������� ��������� ����� ������ �������������
std::vector<Point> generatePointsInsidePentagon(const std::vector<Point>& pentagon, int numPoints) {
    std::vector<Point> points;
    double minX = pentagon[0].x, maxX = pentagon[0].x;
    double minY = pentagon[0].y, maxY = pentagon[0].y;

    // ����� ������� �������������
    for (const auto& vertex : pentagon) {
        if (vertex.x < minX) minX = vertex.x;
        if (vertex.x > maxX) maxX = vertex.x;
        if (vertex.y < minY) minY = vertex.y;
        if (vertex.y > maxY) maxY = vertex.y;
    }

    // ������������ ����� ������ �������������
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

// ������� ��� ������ ������ � ������� Desmos
void outputForDesmos(const std::vector<Point>& vertices) {
    std::cout << "\n��� Desmos:\n";
    for (const auto& vertex : vertices) {
        std::cout << "x=(" << vertex.x << ", " << vertex.y << ")\n";
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    srand(static_cast<unsigned>(time(0))); // ������������� ���������� ��������� �����

    double radius;
    char centerOption, orientationOption;
    Point center;
    double rotationAngle = 0.0;

    std::cout << "������� ������ �������������: ";
    std::cin >> radius;

    // ����� ������: ������ ������� ��� ��������
    std::cout << "�� ������ ������ ����� �������? (+/-): ";
    std::cin >> centerOption;

    if (centerOption == '+') {
        std::cout << "������� ���������� ������ (x y): ";
        std::cin >> center.x >> center.y;
    }
    else {
        center.x = (rand() % 200) - 100; // ��������� �������� � ��������� [-100, 100]
        center.y = (rand() % 200) - 100;
        std::cout << "��������� �����: (" << center.x << ", " << center.y << ")\n";
    }

    // ����� ����������: ��������� ����� ��� ��������� ��������
    std::cout << "�� ������ ��������� ������������ ��������? (+/-): ";
    std::cin >> orientationOption;

    if (orientationOption == '+') {
        rotationAngle = ((double)rand() / RAND_MAX) * 2 * PI; // ��������� ����
        std::cout << "��������� ���� ��������: " << rotationAngle * 180 / PI << " ��������\n";
    }
    else {
        std::cout << "������������ ����� ��������� �������� �����.\n";
    }

    // ��������� ����������� �������������
    std::cout << "���������� ������������:\n";
    auto vertices = generateRegularPentagon(radius, center, rotationAngle);

    // ����� ������ � ������� Desmos
    outputForDesmos(vertices);

    // ���������� ����� ������ �������������
    int numInnerPoints;
    std::cout << "������� ����� �������� ������ �������������? (0 ��� ����������): ";
    std::cin >> numInnerPoints;

    if (numInnerPoints > 0) {
        auto innerPoints = generatePointsInsidePentagon(vertices, numInnerPoints);
        std::cout << "\n��������� ����� ������ �������������:\n";
        for (const auto& point : innerPoints) {
            std::cout << point.x << " " << point.y << "\n";
        }

        std::cout << "\n��� Desmos (����� ������):\n";
        for (const auto& point : innerPoints) {
            std::cout << "x=(" << point.x << ", " << point.y << ")\n";
        }
    }

    return 0;
}
