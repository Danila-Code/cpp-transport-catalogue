# Транспортный справочник
Учебный проект в Яндекс.Практикуме

## Описание
Справочник хранит информацию об остановках и транспортных маршрутах (автобусных). Заполнение справочника и получение информации о маршрутах реализовано с помощью JSON-запросов.

## Технологии:
* C++
* STL
* JSON
* XML
* SVG
* RAII
* динамический полиморфизм
* умные указатели
* ООП
* наследование
* алгоритмы на графах
* CMake

## В транспртном справочнике реализованы:
* класс для хранения транспортной информации TransportCatalogue
* класс для поиска наикратчайшего пути Router
* класс для представления остановок и маршрутов в виде графа зависимостей Graph
* отдельная библиотека для чтения JSON документа из потока и записи в поток (json.h, json.cpp)
* класс Builder для конструирования JSON документа, который выявляет ошибки построения на этапе компиляции
* класс JsonReader для заполнения справочника и формирования JSON ответов на запросы 
* отдельная библиотека для формирования строки с SVG графикой в формате XML (svg.h, svg.cpp)
* класс для отрисовки маршрутов MapRenderer с помощью SVG библиотеки
* класс RequestHandler для управлением остальными классами (TransportCatalogue, Router, MapRenderer), представляющий фасад справочника

## Будущие изменения:
* графический интерфейс
* сериализация транспортной информации

## Особенности транспортного справочника:
* программа разбита на модули-классы, каждый из которых выполняет свои функции
* заполнение справочника и выдача транспортной информации осуществляется с помощью JSON-запросов
* эффективное хранение транспортной информации в контейнере std::deque, который не инвалидируется при изменении размеров
* для быстрого поиска остановок и маршрутов по названию используются "легковесные" std::unordered_map'ы с указателями
* заполнение матрицы с наиболее короткими маршрутами происходит заранее в конструкторе класса Router 
* поиск наикратчайшего маршрута реализован с помощью алгоритма Дейкстры

## Запуск проекта
1. Скачайте файлы из текущего репозитория.
2. Создайте в корневой папке проекта папку "release" для сборки проекта, перейдите в неё в командной строке и запустите cmake:\
	`cmake ../ -DCMAKE_BUILD_TYPE=Release`\
**если работаете с MinGw, укажите дополнительный параметр -G "MinGW Makefiles".* 
7. Запустите сборку проекта в командной строке:\
	`cmake --build .`\
*Проект собран.*
8. Чтобы работать с транспортным справочником нужно в командной строке (находясь в папке "release" проекта) набрать:\
	`./transport-catalogue.exe <"входной файл запросов JSON" >"выходной файл ответов"`

## Системные требования
Компилятор С++, С++20, CMake 3.8
## Формат входного файла
Входной файл имеет формата JSON со следующей структурой:
```
{
  "base_requests": [ ... ],
  "render_settings": { ... },
  "routing_settings": { ... },
  "stat_requests": [ ... ]
}
``` 
где:\
`base_requests` - массив для заполнения справочника, который содержит описание транспортных маршрутов (автобусов) и остановок;\
`render_settings` - словарь с параметрами рендеринга карты маршрутов;\
`routing_settings` - словарь, в котором содержатся параметры движения транспорта;\
`stat_requests` - массив, который содержит запросы к справочнику.
### Структура base_requests
#### Описание транспортного маршрута:  
```
{
  "type": "Bus",
  "name": "1",
  "stops": [
    "Девяткино",
    "Гражданский проспект",
    "Академическая",
    "Политехническая"
  ],
  "is_roundtrip": true
} 
```
где:\
`type` - тип запроса, для автобуса равен "Bus";\
`name` - название маршрута;\
`stops` - массив с названиями остановок маршрута;\
`is_roundtrip` - значение типа bool, указывает кольцевой маршрут или нет.
#### Описание остановки:  
```
{
  "type": "Stop",
  "name": "Владимирская",
  "latitude": 60.402222,
  "longitude": 30.547778,
  "road_distances": {
    "Маяковская": 1000,
    "Садовая": 2100
  }
} 
```
где:\
`type` - тип запроса, для остановки равен "Stop";\
`name` - название остановки;\
`latitude` и `longitude` - координаты остановки (широта и долгота соответственно);\
`road_distances` - словарь, в котором указываются расстояния до соседних остановок в формате - "название остановки": расстояние в метрах.
### Структура render_settings:
```
{
  "width": 1000.0,
  "height": 1000.0,

  "padding": 50.0,

  "line_width": 10.0,
  "stop_radius": 4.0,

  "bus_label_font_size": 20,
  "bus_label_offset": [7.0, 14.0],

  "stop_label_font_size": 12,
  "stop_label_offset": [7.0, -4.0],

  "underlayer_color": [255, 255, 255, 0.85],
  "underlayer_width": 3.0,

  "color_palette": [
    "purple"
    [255, 0, 0],
  ]
} 
```
где:\
`width` и `height` - ширина и высота изображения в пикселях;\
`padding` - отступ от края SVG-документа;\
`line_width` - толщина линий, которыми рисуются автобусные маршруты;\
`stop_radius` - радиус окружностей, которыми обозначаются остановки;\
`bus_label_font_size` - размер текста названия автобусного маршрута;\
`bus_label_offset` - смещение текста названия маршрута относительно координат конечных остановок, задаётся в виде массива `dx` и `dy`;\
`stop_label_font_size` - размер текста названия остановки;\
`stop_label_offset` - смещение названия остановки относительно её координат на карте, задаётся в виде массива `dx` и `dy`;\
`underlayer_color` - цвет подложки под названиями остановок и маршрутов;\
`underlayer_width` - толщина подложки под названиями остановок и маршрутов;\
`color_palette` - цветовая палитра для обозначения маршрутов. Непустой массив.\
Цвет задаётся разными способами:
- строкой, например, "purple";
- в виде массива из трёх целых чисел от 0 до 255 (каждое число описывает соответствующую компоненту цвета RGB формата);
- в виде массива из четырёх целых чисел формат RGBA - четвёртое число в диапазоне от 0.0 до 1.0 обозначает прозрачность.
### Структура routing_settings
```
"routing_settings": {
      "bus_wait_time": 5,
      "bus_velocity": 60
} 
```
где:\
`bus_wait_time` - время ожидания автобуса на остановке, единое для всего справочника, в минутах;\
`bus_velocity` - средняя скорость автобуса, единое для всего справочника, в км/ч.
### Структура stat_requests
#### Запрос информации о транспортном маршруте или остановке:
```
{
  "id": 16756,
  "type": "Bus",
  "name": "1"
} 
```
где:\
`id` - уникальный номер запроса;\
'type' - тип запроса, для автобуса равен "Bus", для отановки - "Stop";\
`name` - название маршрута, о котором будет выведена информация (для автобуса - номер, для остановки - название, например "Маяковская").
#### Запрос изображения карты маршрута:
```
{
  "type": "Map",
  "id": 5475678
}
```
где:\
`id` - уникальный номер запроса;\
`type` - тип запроса, для построения карты равен "Map".
### Запрос построения кратчайшего маршрута между двумя остановками
```
{
      "id": 246436
      "type": "Route",
      "from": "Парнас",
      "to": "Ладожская"
}
```
где:\
`id` - уникальный номер запроса;\
'type' - тип запроса, для нахождения маршрута равен "Route";\
`from` — название начальной остановки;\
`to` — название конечной остановки.\
Остановки from и to должны находиться в базе справочника.
## Формат выходного файла
Программа выдаёт ответы на запросы в виде JSON-файла.
### Ответ на запрос информации о маршруте
```
{
  "curvature": 1.365,
  "request_id": 576,
  "route_length": 5600,
  "stop_count": 6,
  "unique_stop_count": 5
} 
```
где:\
`curvature` - отношению фактической длины маршрута (по дорогам) к прямолинейным географическому расстоянию, то есть извилистость;\
`request_id` - уникальный идентификатор запроса, соответствует id запроса "Bus" в stat_requests входного файла;\
`route_length` - длина маршрута, в м;\
`stop_count` - количество остановок в маршруте;\
`unique_stop_count` - количество уникальных остановок в маршруте (транспорт может проходить по одной и той же остановке несколько раз).  
### Ответ на запрос информации об отановке
```
{
  "buses": [
      "430", "17"
  ],
  "request_id": 43656
} 
```
где:\
`buses` - массив с названиями маршрутов, которые проходят через остановку;\
`request_id` - уникальный идентификатор запроса, соответствует id запроса "Stop" в stat_requests входного файла.
### Ответ на запрос изображения маршрутов
```
{
  "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n
          <svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">...\n</svg>",
  "request_id": 3435
} 
```
где:\
`map` - строка с изображением в формате SVG;\
`request_id` - уникальный идентификатор запроса, соответствует id запроса "Map" в stat_requests входного файла.\
Пример карты маршрутов в формате SVG:
<?xml version="1.0" encoding="UTF-8" ?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1">
  <polyline points="626.417,71.4162 566.363,109.383 511.137,165.297 449.011,174.962 438.658,197.052 382.744,234.328 391.026,268.152 411.735,306.807 421.399,334.421 423.472,367.555 391.717,376.528 346.847,394.476 318.544,404.14 271.604,426.921 210.168,442.798 177.725,495.951 177.034,526.324 193.602,564.979 152.874,589.83 193.602,564.979 177.034,526.324 177.725,495.951 210.168,442.798 271.604,426.921 318.544,404.14 346.847,394.476 391.717,376.528 423.472,367.555 421.399,334.421 411.735,306.807 391.026,268.152 382.744,234.328 438.658,197.052 449.011,174.962 511.137,165.297 566.363,109.383 626.417,71.4162" fill="none" stroke="red" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="357.202,30 353.059,68.6553 326.139,104.552 311.643,154.942 264.703,190.148 275.746,233.637 301.979,280.577 319.235,305.428 343.395,357.89 323.378,377.91 318.544,404.14 315.783,429.682 317.165,466.267 319.235,497.33 326.83,528.394 327.518,564.291 395.857,611.229 460.057,620.205 395.857,611.229 327.518,564.291 326.83,528.394 319.235,497.33 317.165,466.267 315.783,429.682 318.544,404.14 323.378,377.91 343.395,357.89 319.235,305.428 301.979,280.577 275.746,233.637 264.703,190.148 311.643,154.942 326.139,104.552 353.059,68.6553 357.202,30" fill="none" stroke="blue" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="30,228.115 53.4715,265.391 109.386,324.757 218.453,339.252 343.395,357.89 423.472,367.555 486.287,384.811 579.477,453.151 625.038,501.473 695.448,531.846 664.384,572.573 770,616.753 664.384,572.573 695.448,531.846 625.038,501.473 579.477,453.151 486.287,384.811 423.472,367.555 343.395,357.89 218.453,339.252 109.386,324.757 53.4715,265.391 30,228.115" fill="none" stroke="green" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="176.343,364.103 323.378,377.91 391.717,376.528 408.974,393.785 486.287,384.811 550.486,373.077 618.135,364.794 686.474,395.857 727.893,426.921 686.474,395.857 618.135,364.794 550.486,373.077 486.287,384.811 408.974,393.785 391.717,376.528 323.378,377.91 176.343,364.103" fill="none" stroke="orange" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <polyline points="170.131,175.653 161.157,223.282 172.201,266.77 252.966,293.694 243.304,319.924 310.262,355.82 323.378,377.91 346.847,394.476 395.857,408.974 416.568,455.223 443.489,484.905 469.03,518.729 508.377,551.865 549.104,594.663 602.948,643.674 549.104,594.663 508.377,551.865 469.03,518.729 443.489,484.905 416.568,455.223 395.857,408.974 346.847,394.476 323.378,377.91 310.262,355.82 243.304,319.924 252.966,293.694 172.201,266.77 161.157,223.282 170.131,175.653" fill="none" stroke="purple" stroke-width="10" stroke-linecap="round" stroke-linejoin="round"/>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="626.417" y="71.4162" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">1</text>
  <text fill="red" x="626.417" y="71.4162" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">1</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="152.874" y="589.83" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">1</text>
  <text fill="red" x="152.874" y="589.83" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">1</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="357.202" y="30" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">2</text>
  <text fill="blue" x="357.202" y="30" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">2</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="460.057" y="620.205" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">2</text>
  <text fill="blue" x="460.057" y="620.205" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">2</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="30" y="228.115" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">3</text>
  <text fill="green" x="30" y="228.115" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">3</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="770" y="616.753" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">3</text>
  <text fill="green" x="770" y="616.753" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">3</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="176.343" y="364.103" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">4</text>
  <text fill="orange" x="176.343" y="364.103" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">4</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="727.893" y="426.921" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">4</text>
  <text fill="orange" x="727.893" y="426.921" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">4</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="170.131" y="175.653" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">5</text>
  <text fill="purple" x="170.131" y="175.653" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">5</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="602.948" y="643.674" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">5</text>
  <text fill="purple" x="602.948" y="643.674" dx="7" dy="14" font-size="20" font-family="Verdana" font-weight="bold">5</text>
  <circle cx="177.034" cy="526.324" r="4" fill="white"/>
  <circle cx="310.262" cy="355.82" r="4" fill="white"/>
  <circle cx="511.137" cy="165.297" r="4" fill="white"/>
  <circle cx="271.604" cy="426.921" r="4" fill="white"/>
  <circle cx="30" cy="228.115" r="4" fill="white"/>
  <circle cx="443.489" cy="484.905" r="4" fill="white"/>
  <circle cx="218.453" cy="339.252" r="4" fill="white"/>
  <circle cx="391.717" cy="376.528" r="4" fill="white"/>
  <circle cx="416.568" cy="455.223" r="4" fill="white"/>
  <circle cx="391.026" cy="268.152" r="4" fill="white"/>
  <circle cx="176.343" cy="364.103" r="4" fill="white"/>
  <circle cx="319.235" cy="305.428" r="4" fill="white"/>
  <circle cx="566.363" cy="109.383" r="4" fill="white"/>
  <circle cx="626.417" cy="71.4162" r="4" fill="white"/>
  <circle cx="549.104" cy="594.663" r="4" fill="white"/>
  <circle cx="579.477" cy="453.151" r="4" fill="white"/>
  <circle cx="395.857" cy="611.229" r="4" fill="white"/>
  <circle cx="53.4715" cy="265.391" r="4" fill="white"/>
  <circle cx="177.725" cy="495.951" r="4" fill="white"/>
  <circle cx="170.131" cy="175.653" r="4" fill="white"/>
  <circle cx="172.201" cy="266.77" r="4" fill="white"/>
  <circle cx="460.057" cy="620.205" r="4" fill="white"/>
  <circle cx="618.135" cy="364.794" r="4" fill="white"/>
  <circle cx="193.602" cy="564.979" r="4" fill="white"/>
  <circle cx="382.744" cy="234.328" r="4" fill="white"/>
  <circle cx="408.974" cy="393.785" r="4" fill="white"/>
  <circle cx="625.038" cy="501.473" r="4" fill="white"/>
  <circle cx="423.472" cy="367.555" r="4" fill="white"/>
  <circle cx="469.03" cy="518.729" r="4" fill="white"/>
  <circle cx="327.518" cy="564.291" r="4" fill="white"/>
  <circle cx="317.165" cy="466.267" r="4" fill="white"/>
  <circle cx="210.168" cy="442.798" r="4" fill="white"/>
  <circle cx="343.395" cy="357.89" r="4" fill="white"/>
  <circle cx="550.486" cy="373.077" r="4" fill="white"/>
  <circle cx="395.857" cy="408.974" r="4" fill="white"/>
  <circle cx="664.384" cy="572.573" r="4" fill="white"/>
  <circle cx="326.139" cy="104.552" r="4" fill="white"/>
  <circle cx="326.83" cy="528.394" r="4" fill="white"/>
  <circle cx="357.202" cy="30" r="4" fill="white"/>
  <circle cx="301.979" cy="280.577" r="4" fill="white"/>
  <circle cx="264.703" cy="190.148" r="4" fill="white"/>
  <circle cx="486.287" cy="384.811" r="4" fill="white"/>
  <circle cx="411.735" cy="306.807" r="4" fill="white"/>
  <circle cx="438.658" cy="197.052" r="4" fill="white"/>
  <circle cx="449.011" cy="174.962" r="4" fill="white"/>
  <circle cx="109.386" cy="324.757" r="4" fill="white"/>
  <circle cx="695.448" cy="531.846" r="4" fill="white"/>
  <circle cx="686.474" cy="395.857" r="4" fill="white"/>
  <circle cx="152.874" cy="589.83" r="4" fill="white"/>
  <circle cx="353.059" cy="68.6553" r="4" fill="white"/>
  <circle cx="508.377" cy="551.865" r="4" fill="white"/>
  <circle cx="346.847" cy="394.476" r="4" fill="white"/>
  <circle cx="770" cy="616.753" r="4" fill="white"/>
  <circle cx="323.378" cy="377.91" r="4" fill="white"/>
  <circle cx="243.304" cy="319.924" r="4" fill="white"/>
  <circle cx="161.157" cy="223.282" r="4" fill="white"/>
  <circle cx="318.544" cy="404.14" r="4" fill="white"/>
  <circle cx="311.643" cy="154.942" r="4" fill="white"/>
  <circle cx="727.893" cy="426.921" r="4" fill="white"/>
  <circle cx="315.783" cy="429.682" r="4" fill="white"/>
  <circle cx="421.399" cy="334.421" r="4" fill="white"/>
  <circle cx="252.966" cy="293.694" r="4" fill="white"/>
  <circle cx="275.746" cy="233.637" r="4" fill="white"/>
  <circle cx="602.948" cy="643.674" r="4" fill="white"/>
  <circle cx="319.235" cy="497.33" r="4" fill="white"/>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="177.034" y="526.324" dx="7" dy="-5" font-size="10" font-family="Verdana">Автово</text>
  <text fill="black" x="177.034" y="526.324" dx="7" dy="-5" font-size="10" font-family="Verdana">Автово</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="310.262" y="355.82" dx="7" dy="-5" font-size="10" font-family="Verdana">Адмиралтейская</text>
  <text fill="black" x="310.262" y="355.82" dx="7" dy="-5" font-size="10" font-family="Verdana">Адмиралтейская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="511.137" y="165.297" dx="7" dy="-5" font-size="10" font-family="Verdana">Академическая</text>
  <text fill="black" x="511.137" y="165.297" dx="7" dy="-5" font-size="10" font-family="Verdana">Академическая</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="271.604" y="426.921" dx="7" dy="-5" font-size="10" font-family="Verdana">Балтийская</text>
  <text fill="black" x="271.604" y="426.921" dx="7" dy="-5" font-size="10" font-family="Verdana">Балтийская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="30" y="228.115" dx="7" dy="-5" font-size="10" font-family="Verdana">Беговая</text>
  <text fill="black" x="30" y="228.115" dx="7" dy="-5" font-size="10" font-family="Verdana">Беговая</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="443.489" y="484.905" dx="7" dy="-5" font-size="10" font-family="Verdana">Бухарестская</text>
  <text fill="black" x="443.489" y="484.905" dx="7" dy="-5" font-size="10" font-family="Verdana">Бухарестская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="218.453" y="339.252" dx="7" dy="-5" font-size="10" font-family="Verdana">Василеостровская</text>
  <text fill="black" x="218.453" y="339.252" dx="7" dy="-5" font-size="10" font-family="Verdana">Василеостровская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="391.717" y="376.528" dx="7" dy="-5" font-size="10" font-family="Verdana">Владимирская</text>
  <text fill="black" x="391.717" y="376.528" dx="7" dy="-5" font-size="10" font-family="Verdana">Владимирская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="416.568" y="455.223" dx="7" dy="-5" font-size="10" font-family="Verdana">Волковская</text>
  <text fill="black" x="416.568" y="455.223" dx="7" dy="-5" font-size="10" font-family="Verdana">Волковская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="391.026" y="268.152" dx="7" dy="-5" font-size="10" font-family="Verdana">Выборгская</text>
  <text fill="black" x="391.026" y="268.152" dx="7" dy="-5" font-size="10" font-family="Verdana">Выборгская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="176.343" y="364.103" dx="7" dy="-5" font-size="10" font-family="Verdana">Горный институт</text>
  <text fill="black" x="176.343" y="364.103" dx="7" dy="-5" font-size="10" font-family="Verdana">Горный институт</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="319.235" y="305.428" dx="7" dy="-5" font-size="10" font-family="Verdana">Горьковская</text>
  <text fill="black" x="319.235" y="305.428" dx="7" dy="-5" font-size="10" font-family="Verdana">Горьковская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="566.363" y="109.383" dx="7" dy="-5" font-size="10" font-family="Verdana">Гражданский проспект</text>
  <text fill="black" x="566.363" y="109.383" dx="7" dy="-5" font-size="10" font-family="Verdana">Гражданский проспект</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="626.417" y="71.4162" dx="7" dy="-5" font-size="10" font-family="Verdana">Девяткино</text>
  <text fill="black" x="626.417" y="71.4162" dx="7" dy="-5" font-size="10" font-family="Verdana">Девяткино</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="549.104" y="594.663" dx="7" dy="-5" font-size="10" font-family="Verdana">Дунайская</text>
  <text fill="black" x="549.104" y="594.663" dx="7" dy="-5" font-size="10" font-family="Verdana">Дунайская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="579.477" y="453.151" dx="7" dy="-5" font-size="10" font-family="Verdana">Елизаровская</text>
  <text fill="black" x="579.477" y="453.151" dx="7" dy="-5" font-size="10" font-family="Verdana">Елизаровская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="395.857" y="611.229" dx="7" dy="-5" font-size="10" font-family="Verdana">Звёздная</text>
  <text fill="black" x="395.857" y="611.229" dx="7" dy="-5" font-size="10" font-family="Verdana">Звёздная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="53.4715" y="265.391" dx="7" dy="-5" font-size="10" font-family="Verdana">Зенит</text>
  <text fill="black" x="53.4715" y="265.391" dx="7" dy="-5" font-size="10" font-family="Verdana">Зенит</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="177.725" y="495.951" dx="7" dy="-5" font-size="10" font-family="Verdana">Кировский завод</text>
  <text fill="black" x="177.725" y="495.951" dx="7" dy="-5" font-size="10" font-family="Verdana">Кировский завод</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="170.131" y="175.653" dx="7" dy="-5" font-size="10" font-family="Verdana">Комендантский проспект</text>
  <text fill="black" x="170.131" y="175.653" dx="7" dy="-5" font-size="10" font-family="Verdana">Комендантский проспект</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="172.201" y="266.77" dx="7" dy="-5" font-size="10" font-family="Verdana">Крестовский остров</text>
  <text fill="black" x="172.201" y="266.77" dx="7" dy="-5" font-size="10" font-family="Verdana">Крестовский остров</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="460.057" y="620.205" dx="7" dy="-5" font-size="10" font-family="Verdana">Купчино</text>
  <text fill="black" x="460.057" y="620.205" dx="7" dy="-5" font-size="10" font-family="Verdana">Купчино</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="618.135" y="364.794" dx="7" dy="-5" font-size="10" font-family="Verdana">Ладожская</text>
  <text fill="black" x="618.135" y="364.794" dx="7" dy="-5" font-size="10" font-family="Verdana">Ладожская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="193.602" y="564.979" dx="7" dy="-5" font-size="10" font-family="Verdana">Ленинский проспект</text>
  <text fill="black" x="193.602" y="564.979" dx="7" dy="-5" font-size="10" font-family="Verdana">Ленинский проспект</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="382.744" y="234.328" dx="7" dy="-5" font-size="10" font-family="Verdana">Лесная</text>
  <text fill="black" x="382.744" y="234.328" dx="7" dy="-5" font-size="10" font-family="Verdana">Лесная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="408.974" y="393.785" dx="7" dy="-5" font-size="10" font-family="Verdana">Лиговский проспект</text>
  <text fill="black" x="408.974" y="393.785" dx="7" dy="-5" font-size="10" font-family="Verdana">Лиговский проспект</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="625.038" y="501.473" dx="7" dy="-5" font-size="10" font-family="Verdana">Ломоносовская</text>
  <text fill="black" x="625.038" y="501.473" dx="7" dy="-5" font-size="10" font-family="Verdana">Ломоносовская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="423.472" y="367.555" dx="7" dy="-5" font-size="10" font-family="Verdana">Маяковская</text>
  <text fill="black" x="423.472" y="367.555" dx="7" dy="-5" font-size="10" font-family="Verdana">Маяковская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="469.03" y="518.729" dx="7" dy="-5" font-size="10" font-family="Verdana">Международная</text>
  <text fill="black" x="469.03" y="518.729" dx="7" dy="-5" font-size="10" font-family="Verdana">Международная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="327.518" y="564.291" dx="7" dy="-5" font-size="10" font-family="Verdana">Московская</text>
  <text fill="black" x="327.518" y="564.291" dx="7" dy="-5" font-size="10" font-family="Verdana">Московская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="317.165" y="466.267" dx="7" dy="-5" font-size="10" font-family="Verdana">Московские ворота</text>
  <text fill="black" x="317.165" y="466.267" dx="7" dy="-5" font-size="10" font-family="Verdana">Московские ворота</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="210.168" y="442.798" dx="7" dy="-5" font-size="10" font-family="Verdana">Нарвская</text>
  <text fill="black" x="210.168" y="442.798" dx="7" dy="-5" font-size="10" font-family="Verdana">Нарвская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="343.395" y="357.89" dx="7" dy="-5" font-size="10" font-family="Verdana">Невский проспект</text>
  <text fill="black" x="343.395" y="357.89" dx="7" dy="-5" font-size="10" font-family="Verdana">Невский проспект</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="550.486" y="373.077" dx="7" dy="-5" font-size="10" font-family="Verdana">Новочеркасская</text>
  <text fill="black" x="550.486" y="373.077" dx="7" dy="-5" font-size="10" font-family="Verdana">Новочеркасская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="395.857" y="408.974" dx="7" dy="-5" font-size="10" font-family="Verdana">Обводный канал</text>
  <text fill="black" x="395.857" y="408.974" dx="7" dy="-5" font-size="10" font-family="Verdana">Обводный канал</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="664.384" y="572.573" dx="7" dy="-5" font-size="10" font-family="Verdana">Обухово</text>
  <text fill="black" x="664.384" y="572.573" dx="7" dy="-5" font-size="10" font-family="Verdana">Обухово</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="326.139" y="104.552" dx="7" dy="-5" font-size="10" font-family="Verdana">Озерки</text>
  <text fill="black" x="326.139" y="104.552" dx="7" dy="-5" font-size="10" font-family="Verdana">Озерки</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="326.83" y="528.394" dx="7" dy="-5" font-size="10" font-family="Verdana">Парк Победы</text>
  <text fill="black" x="326.83" y="528.394" dx="7" dy="-5" font-size="10" font-family="Verdana">Парк Победы</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="357.202" y="30" dx="7" dy="-5" font-size="10" font-family="Verdana">Парнас</text>
  <text fill="black" x="357.202" y="30" dx="7" dy="-5" font-size="10" font-family="Verdana">Парнас</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="301.979" y="280.577" dx="7" dy="-5" font-size="10" font-family="Verdana">Петроградская</text>
  <text fill="black" x="301.979" y="280.577" dx="7" dy="-5" font-size="10" font-family="Verdana">Петроградская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="264.703" y="190.148" dx="7" dy="-5" font-size="10" font-family="Verdana">Пионерская</text>
  <text fill="black" x="264.703" y="190.148" dx="7" dy="-5" font-size="10" font-family="Verdana">Пионерская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="486.287" y="384.811" dx="7" dy="-5" font-size="10" font-family="Verdana">Площадь Александра Невского</text>
  <text fill="black" x="486.287" y="384.811" dx="7" dy="-5" font-size="10" font-family="Verdana">Площадь Александра Невского</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="411.735" y="306.807" dx="7" dy="-5" font-size="10" font-family="Verdana">Площадь Ленина</text>
  <text fill="black" x="411.735" y="306.807" dx="7" dy="-5" font-size="10" font-family="Verdana">Площадь Ленина</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="438.658" y="197.052" dx="7" dy="-5" font-size="10" font-family="Verdana">Площадь Мужества</text>
  <text fill="black" x="438.658" y="197.052" dx="7" dy="-5" font-size="10" font-family="Verdana">Площадь Мужества</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="449.011" y="174.962" dx="7" dy="-5" font-size="10" font-family="Verdana">Политехническая</text>
  <text fill="black" x="449.011" y="174.962" dx="7" dy="-5" font-size="10" font-family="Verdana">Политехническая</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="109.386" y="324.757" dx="7" dy="-5" font-size="10" font-family="Verdana">Приморская</text>
  <text fill="black" x="109.386" y="324.757" dx="7" dy="-5" font-size="10" font-family="Verdana">Приморская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="695.448" y="531.846" dx="7" dy="-5" font-size="10" font-family="Verdana">Пролетарская</text>
  <text fill="black" x="695.448" y="531.846" dx="7" dy="-5" font-size="10" font-family="Verdana">Пролетарская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="686.474" y="395.857" dx="7" dy="-5" font-size="10" font-family="Verdana">Проспект Большевиков</text>
  <text fill="black" x="686.474" y="395.857" dx="7" dy="-5" font-size="10" font-family="Verdana">Проспект Большевиков</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="152.874" y="589.83" dx="7" dy="-5" font-size="10" font-family="Verdana">Проспект Ветеранов</text>
  <text fill="black" x="152.874" y="589.83" dx="7" dy="-5" font-size="10" font-family="Verdana">Проспект Ветеранов</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="353.059" y="68.6553" dx="7" dy="-5" font-size="10" font-family="Verdana">Проспект Просвещения</text>
  <text fill="black" x="353.059" y="68.6553" dx="7" dy="-5" font-size="10" font-family="Verdana">Проспект Просвещения</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="508.377" y="551.865" dx="7" dy="-5" font-size="10" font-family="Verdana">Проспект Славы</text>
  <text fill="black" x="508.377" y="551.865" dx="7" dy="-5" font-size="10" font-family="Verdana">Проспект Славы</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="346.847" y="394.476" dx="7" dy="-5" font-size="10" font-family="Verdana">Пушкинская</text>
  <text fill="black" x="346.847" y="394.476" dx="7" dy="-5" font-size="10" font-family="Verdana">Пушкинская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="770" y="616.753" dx="7" dy="-5" font-size="10" font-family="Verdana">Рыбацкое</text>
  <text fill="black" x="770" y="616.753" dx="7" dy="-5" font-size="10" font-family="Verdana">Рыбацкое</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="323.378" y="377.91" dx="7" dy="-5" font-size="10" font-family="Verdana">Сенная площадь</text>
  <text fill="black" x="323.378" y="377.91" dx="7" dy="-5" font-size="10" font-family="Verdana">Сенная площадь</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="243.304" y="319.924" dx="7" dy="-5" font-size="10" font-family="Verdana">Спортивная</text>
  <text fill="black" x="243.304" y="319.924" dx="7" dy="-5" font-size="10" font-family="Verdana">Спортивная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="161.157" y="223.282" dx="7" dy="-5" font-size="10" font-family="Verdana">Старая Деревня</text>
  <text fill="black" x="161.157" y="223.282" dx="7" dy="-5" font-size="10" font-family="Verdana">Старая Деревня</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="318.544" y="404.14" dx="7" dy="-5" font-size="10" font-family="Verdana">Технологический институт</text>
  <text fill="black" x="318.544" y="404.14" dx="7" dy="-5" font-size="10" font-family="Verdana">Технологический институт</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="311.643" y="154.942" dx="7" dy="-5" font-size="10" font-family="Verdana">Удельная</text>
  <text fill="black" x="311.643" y="154.942" dx="7" dy="-5" font-size="10" font-family="Verdana">Удельная</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="727.893" y="426.921" dx="7" dy="-5" font-size="10" font-family="Verdana">Улица Дыбенко</text>
  <text fill="black" x="727.893" y="426.921" dx="7" dy="-5" font-size="10" font-family="Verdana">Улица Дыбенко</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="315.783" y="429.682" dx="7" dy="-5" font-size="10" font-family="Verdana">Фрунзенская</text>
  <text fill="black" x="315.783" y="429.682" dx="7" dy="-5" font-size="10" font-family="Verdana">Фрунзенская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="421.399" y="334.421" dx="7" dy="-5" font-size="10" font-family="Verdana">Чернышевская</text>
  <text fill="black" x="421.399" y="334.421" dx="7" dy="-5" font-size="10" font-family="Verdana">Чернышевская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="252.966" y="293.694" dx="7" dy="-5" font-size="10" font-family="Verdana">Чкаловская</text>
  <text fill="black" x="252.966" y="293.694" dx="7" dy="-5" font-size="10" font-family="Verdana">Чкаловская</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="275.746" y="233.637" dx="7" dy="-5" font-size="10" font-family="Verdana">Чёрная речка</text>
  <text fill="black" x="275.746" y="233.637" dx="7" dy="-5" font-size="10" font-family="Verdana">Чёрная речка</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="602.948" y="643.674" dx="7" dy="-5" font-size="10" font-family="Verdana">Шушары</text>
  <text fill="black" x="602.948" y="643.674" dx="7" dy="-5" font-size="10" font-family="Verdana">Шушары</text>
  <text fill="rgba(255,255,255,0.85)" stroke="rgba(255,255,255,0.85)" stroke-width="3" stroke-linecap="round" stroke-linejoin="round" x="319.235" y="497.33" dx="7" dy="-5" font-size="10" font-family="Verdana">Электросила</text>
  <text fill="black" x="319.235" y="497.33" dx="7" dy="-5" font-size="10" font-family="Verdana">Электросила</text>
</svg>
### Ответ на запрос кратчайшего маршрута между двумя остановками
В ответе перечислен порядок проезда на различных автобусах, ожидание на остановках: 
```
{
          "items": [
              {
                  "stop_name": "Горьковская",
                  "time": 4,
                  "type": "Wait"
              },
              {
                  "bus": "2",
                  "span_count": 2,
                  "time": 7.18,
                  "type": "Bus"
              },
              {
                  "stop_name": "Сенная",
                  "time": 4,
                  "type": "Wait"
              },
              {
                  "bus": "5",
                  "span_count": 1,
                  "time": 2.41,
                  "type": "Bus"
              }
          ],
          "request_id": 5457,
          "total_time": 17.59
      }
 ```
где:\
`type` - тип действия, "Wait" - ожидание транспорта на остановке, "Bus" - проезд на транспорте;\
`span_count` - количество остановок;\
`time` - затрачиваемое время, в мин;\
`request_id` - уникальный идентификатор запроса, соответствует id запроса "Route" в stat_requests входного файла;\
`total_time` - суммарное время в пути, в мин.
