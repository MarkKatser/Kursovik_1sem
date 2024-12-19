#include <iostream>
#include <cmath>

// ��������� ��� �������� ����� �� ���������
struct Point {
    double x, y;
};

// ������� ��� ���������� ���������� ����� ����� �������
double distance(const Point& p1, const Point& p2) {
    return std::sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

// ������� ��� ���������� ����� ������� ������������ (����������, �� ������� ������� ��� ������)
double crossProduct(const Point& p1, const Point& p2, const Point& p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

// ������� ��� ���������� ����� �� ������� ������� ������������ ������
void sortPoints(Point points[], Point center) {
    for (int i = 0; i < 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
            if (crossProduct(center, points[i], points[j]) < 0) {
                // ������ ������� �����
                Point temp = points[i];
                points[i] = points[j];
                points[j] = temp;
            }
        }
    }
}

// ������� ��� ��������, �������� �� ������������ ����������
bool isRegularPolygon(const Point points[]) {
    double sideLength = distance(points[0], points[1]);

    // �������� ���� ������
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

    // ���� ��������� ���� �����
    std::cout << "������� 5 ����� (x, y):\n";
    for (int i = 0; i < 5; ++i) {
        std::cin >> points[i].x >> points[i].y;
    }

    // ���������� ����������� ����� ��� ���������� �� ������� �������
    Point center = { 0, 0 };
    for (int i = 0; i < 5; ++i) {
        center.x += points[i].x;
        center.y += points[i].y;
    }
    center.x /= 5;
    center.y /= 5;

    // ���������� ����� �� ������� ������� ������������ ������
    sortPoints(points, center);

    // ����� ��������������� �����
    std::cout << "����� �� ������� �������:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << "(" << points[i].x << ", " << points[i].y << ")\n";
    }

    // ��������, �������� �� ������������ ����������
    if (isRegularPolygon(points)) {
        std::cout << "������������ ����������.\n";
    }
    else {
        std::cout << "������������ �� ����������.\n";
    }

    return 0;
}
