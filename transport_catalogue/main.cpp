#include "json_builder.h"
#include "json_reader.h"
#include "svg.h"
#include "request_handler.h"
#include "map_renderer.h"

using namespace json;
using namespace std::literals;

int main() {
    
    //Создаем транспортный справочник
    tc::TransportСatalogue catalogue;
    //Создаем отрисовщик карты
    renderer::MapRenderer map_renderer;
    //Создаем обработчик JSON запросов
    JSONReader reader(catalogue, map_renderer);
    //Считываем входной JSON файл
    reader.ReadJSON(std::cin);
    
    reader.SetCatalogue();
    reader.SetRenderer();
    reader.ReadStats();

}
