#pragma once
#include "svg.h"
#include "geo.h"
#include "domain.h"

#include <string>
#include <vector>

#include <cmath>

#include <set>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>


// Отрисовщик, отвечающий визуализацию карты маршрутов в формате SVG.


//Компаратор для вывода остановок в алфавитном порядке
   struct SortedByName {
        bool operator()(tc::Stop* lhs, tc::Stop* rhs) const {
            return lhs->name < rhs->name;
        }
    };

inline const double EPSILON = 1e-6;

bool IsZero(double value);


class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_ = 0;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};


namespace renderer{
    
class MapRenderer{
public:
    
    SphereProjector ProjectorInit(const std::vector<tc::Route*>& routes);
    
    void Render(const std::vector<tc::Route*>& routes, std::ostream& out);

    
    void SetWidth(double width);
    void SetHeight(double height);
    void SetPadding(double padding);
    void SetLineWidth(double line_width);
    void SetStopRadius(double stop_radius);
    void SetUnderlayerWidth(double underlayer_width);
    
    void SetBusFontSize(double bus_label_font_size);
    void SetBusLabeldx(double bus_label_offset_dx);
    void SetBusLabeldy(double bus_label_offset_dy);
    
    void SetStopFontSize(double stop_label_font_size);
    void SetStopLabeldx(double stop_label_offset_dx);
    void SetStopLabeldy(double stop_label_offset_dy);
    
    void SetUnderlayerColor(const std::string& underlayer_color);
    void AddToPallete(const std::string& color);

private:

    double width_ ;
    double height_ ;
    double padding_ ;
    double line_width_ ;
    double stop_radius_ ;
    double underlayer_width_ ;
    
    double bus_label_font_size_;
    double bus_label_offset_dx_ ;
    double bus_label_offset_dy_ ;
    
    double stop_label_font_size_ ;
    double stop_label_offset_dx_;    
    double stop_label_offset_dy_;  
    
    std::string underlayer_color_ ;
    std::vector<std::string> color_pallete_ ;
    
    svg::Document doc;
    
    

};
    
} //namespace renderer end