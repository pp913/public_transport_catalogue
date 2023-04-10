#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршрутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

#define _USE_MATH_DEFINES



bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}


using namespace std::literals;
using namespace svg;

//Пространство имён для отрисвки форм
namespace shapes {

//Форма - маршнут/ломаная линия
class Route : public svg::Drawable {
public:
    Route(const std::vector<svg::Point>& points, 
          std::string color_fill, 
          std::string& color_pallete_element, 
          double line_width){
            
    //svg::Polyline route;
        for (svg::Point p : points) {
            pline.AddPoint({p.x, p.y});
        }
        pline.SetFillColor(color_fill)//пар
            .SetStrokeColor(color_pallete_element)//пар
            .SetStrokeWidth(line_width)//пар
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND) ;
    }
    
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(pline);
    }
    
    private:
        svg::Polyline pline;

};
    //Форма - точка, обозначающая остановку
class StopDot : public svg::Drawable { 
    public:
        StopDot(svg::Point center, double radius){
        center_ = center;
        radius_ = radius;    

    }
    
    void Draw(svg::ObjectContainer& container) const override {
                container.Add(Circle()
                              .SetCenter(center_)
                              .SetRadius(radius_)
                              .SetFillColor("white"));
                              //.SetStrokeColor("black"s));

    }

    private:
        svg::Point center_ ;
        double radius_;
    
};
    
}//shapes


template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

void TestStrLineJoin() {
        std::cout << StrokeLineJoin::ARCS << std::endl;
        std::cout << StrokeLineJoin::BEVEL << std::endl;
        std::cout << StrokeLineJoin::MITER << std::endl;
        std::cout << StrokeLineJoin::MITER_CLIP << std::endl;
        std::cout << StrokeLineJoin::ROUND << std::endl;
}


//start
using namespace svg;
using namespace shapes;
using namespace std;

//Класс сферического проектора - проецирует координаты остановок на сферическую поверхность.
SphereProjector renderer::MapRenderer::ProjectorInit(const std::vector<tc::Route*>& routes){
        //Создаём массив координат
    std::vector < geo::Coordinates > for_sphere;
    for (auto& route: routes) {
        //std::cerr << "stop" << i->name <<  std::endl;
        for (auto& coord : route -> routes_stops) {
            for_sphere.push_back(coord -> xy);
        }
    }

    //инициализируем сферический проектор
    SphereProjector proj_ {
        for_sphere.begin(), for_sphere.end(), width_, height_, padding_
    };
    
   return proj_;
    
}

void renderer::MapRenderer::Render(const std::vector <tc::Route*>& routes, std::ostream& out) {

    SphereProjector proj = ProjectorInit(routes);
    
    //Создаём массив координат
    std::vector < geo::Coordinates > for_sphere;
    for (auto& route: routes) {
        //std::cerr << "stop" << i->name <<  std::endl;
        for (auto& coord : route -> routes_stops) {
            for_sphere.push_back(coord -> xy);
        }
    }



    size_t cycle = 0;

    //1 линии маршрутов
    //строим маршруты по точкам
    for (auto& route: routes) {

        if (route -> routes_stops.empty()) continue;

        std::vector < svg::Point > points;
        std::vector < unique_ptr < svg::Drawable >> picture;

        if (route -> is_ringed == true) { //некольцевой
            //туда
            for (auto& coord: route -> routes_stops) {
                points.push_back(proj(coord -> xy)); //проекция 
            }

        } else { //кольцевой

            for (auto it = route -> routes_stops.begin(); it != route -> routes_stops.end(); ++it) {
                points.push_back(proj(( * it) -> xy));
            }
            //обратно
            for (auto it = std::next(route -> routes_stops.rbegin()); it != route -> routes_stops.rend(); ++it) {
                points.push_back(proj(( * it) -> xy));
            }
        }

        picture.emplace_back(make_unique < Route > (points, "none", color_pallete_[cycle], line_width_));
        if (cycle >= color_pallete_.size() - 1 || color_pallete_.size() == 1) cycle = 0;
        else cycle++;
        DrawPicture(picture, doc);

    } //for : routes end1

    
    
    
    //2
    //названия маршрутов
    //проходим второй раз
    //text
    cycle = 0;
    for (auto & route: routes) {
        if (route -> routes_stops.empty()) continue;

        const Text first_stop =
            Text()
            .SetFontFamily("Verdana"s)
            .SetFontWeight("bold"s)
            .SetFontSize(bus_label_font_size_)
            .SetOffset(svg::Point {bus_label_offset_dx_,bus_label_offset_dy_})
            .SetPosition({proj(route -> routes_stops.front() -> xy).x, proj(route -> routes_stops.front() -> xy).y})
            .SetData(route -> name);
        doc.Add(Text {first_stop}
            .SetStrokeColor(underlayer_color_)
            .SetFillColor(underlayer_color_)
            .SetStrokeLineJoin(StrokeLineJoin::ROUND)
            .SetStrokeLineCap(StrokeLineCap::ROUND)
            .SetStrokeWidth(underlayer_width_));
        doc.Add(Text {first_stop}
            .SetFillColor(color_pallete_[cycle]));

        if (route -> is_ringed == false && route -> routes_stops.front() != route -> routes_stops.back()) { //некольцевой
            const Text last_stop =
                Text()
                .SetFontFamily("Verdana"s)
                .SetFontWeight("bold"s)
                .SetFontSize(bus_label_font_size_)
                .SetOffset(svg::Point {bus_label_offset_dx_, bus_label_offset_dy_ })
                .SetPosition({proj(route -> routes_stops.back() -> xy).x, proj(route -> routes_stops.back() -> xy).y})
                .SetData(route -> name);
            doc.Add(Text {last_stop}
                .SetStrokeColor(underlayer_color_)
                .SetFillColor(underlayer_color_)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeWidth(underlayer_width_));
            doc.Add(Text {
                last_stop
            }.SetFillColor(color_pallete_[cycle]));

        }

        if (cycle >= color_pallete_.size() - 1 || color_pallete_.size() == 1) cycle = 0;
        else cycle++;

    } //for : routes end 2

    
    
    
    //3
    //точки остановок
    std::set < tc::Stop * , SortedByName > stops_set; // отсортированные остановки
    for (auto & route: routes) {
        for (auto & stop : route -> routes_stops) {
            stops_set.emplace(stop);
        }
    }

    for (auto & stop: stops_set) {
        std::vector < unique_ptr < svg::Drawable >> picture2;
        picture2.emplace_back(make_unique < StopDot > (Point {
            proj(stop -> xy).x, proj(stop -> xy).y
        }, stop_radius_));
        DrawPicture(picture2, doc);
    }

    //Названия остановок.
    for (auto & stop: stops_set) {
        const Text first_stop =
            Text()
            .SetFontFamily("Verdana"s)
            .SetFontSize(stop_label_font_size_)
            .SetOffset(svg::Point {stop_label_offset_dx_, stop_label_offset_dy_ })
            .SetPosition({proj(stop -> xy).x, proj(stop -> xy).y})
            .SetData(stop -> name);
        doc.Add(Text {first_stop}
            .SetStrokeColor(underlayer_color_)
            .SetFillColor(underlayer_color_)
            .SetStrokeLineJoin(StrokeLineJoin::ROUND)
            .SetStrokeLineCap(StrokeLineCap::ROUND)
            .SetStrokeWidth(underlayer_width_));
        doc.Add(Text {first_stop}
            .SetFillColor("black"));
    }

    //Вывод изображения
    doc.Render(out);

}

//Сеттеры, задающие параметры отрисовки
void renderer::MapRenderer::SetWidth(double width){
    width_ = width;
}
void renderer::MapRenderer::SetHeight(double height){
    height_ = height;
}
void renderer::MapRenderer::SetPadding(double padding){
        padding_ = padding;
    }
void renderer::MapRenderer::SetLineWidth(double line_width){
    line_width_ = line_width;
} 
void renderer::MapRenderer::SetStopRadius(double stop_radius){
    stop_radius_ = stop_radius;
} 
void renderer::MapRenderer::SetUnderlayerWidth(double underlayer_width){
    underlayer_width_ = underlayer_width;
}    
    
void renderer::MapRenderer::SetBusFontSize(double bus_label_font_size){
    bus_label_font_size_ = bus_label_font_size;
}
    
void renderer::MapRenderer::SetBusLabeldx(double bus_label_offset_dx){
    bus_label_offset_dx_ = bus_label_offset_dx;
}
    
void renderer::MapRenderer::SetBusLabeldy(double bus_label_offset_dy){
    bus_label_offset_dy_ = bus_label_offset_dy;
}
    
void renderer::MapRenderer::SetStopFontSize(double stop_label_font_size){
    stop_label_font_size_ = stop_label_font_size;
}
void renderer::MapRenderer::SetStopLabeldx(double stop_label_offset_dx){
    stop_label_offset_dx_ = stop_label_offset_dx;
}
    
void renderer::MapRenderer::SetStopLabeldy(double stop_label_offset_dy){
    stop_label_offset_dy_ = stop_label_offset_dy;
}
    
void renderer::MapRenderer::SetUnderlayerColor(const std::string& underlayer_color){
    underlayer_color_ = underlayer_color;
}

void renderer::MapRenderer::AddToPallete(const std::string& color){
    color_pallete_.push_back(color);
}





