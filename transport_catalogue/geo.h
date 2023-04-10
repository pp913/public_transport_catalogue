#pragma once

//Географицеские координаты
namespace geo {

struct Coordinates {
    double lat; // Широта
    double lng; // Долгота
    bool operator==(const Coordinates& other) const {
        return lat == other.lat && lng == other.lng;
    }
    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};

    //Расчёт расстояния между координатами
double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo