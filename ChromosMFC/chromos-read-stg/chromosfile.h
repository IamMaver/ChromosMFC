#pragma once
#include <windows.h>
#include <cstdint>
#include <tchar.h>
#include <vector>
#include <string>

#define PASSPORT_SIGNATURE		0xE04561CC
#define PASSPORT_VERSION		3

#define PEAKSTABLE_SIGNATURE1	0x125FFE11
#define PEAKSTABLE_SIGNATURE2	0xA2D28C45
// ver 2 mainstream
// ver 3 add dop.parameters (beta) for load VERYYYY BIG files
// ver 4 mainstream
#define PEAKSTABLE_VERSION		4

//#ifndef LATTICE_T
#define LATTICE_T float
//#endif


// Массив точек хроматограммы
typedef std::vector<LATTICE_T> lattice_t;


// Класс представления строковых переменных
typedef std::basic_string<TCHAR> string_t;


// Массив параметров
typedef std::vector<std::pair<string_t, string_t>> options_t;


// Паспорт хроматограммы
struct passport_t
{
	int assayId;					// ID анализа
	string_t filename;				// Полный путь к файлу хроматограммы
	string_t sample;				// Проба
	string_t series;				// Серия
	string_t detector;				// Детектор
	string_t method;				// Метод
	string_t eluent;				// Элюент
	string_t reagent;				// Реагент
	string_t lab_assistant;			// Лаборант
	string_t station;				// Пункт отбора
	string_t place;					// Точка отбора
	int numInSeria;					// Номер в серии измерений
	int graduation;					// Градуировка
	int viala;						// Номер виалы
	double volume;					// Масс пробы / ОБъем пробы
	double dilution;				// Масса стандарта / Разведение
	double norm;					// Норма (100%)
	double duration;				// Продолжительность анализа в минутах
	string_t column;				// Колонка. Наименование
	int column_num;					// Колонка. Номер
	double column_size;				// Колонка. Толщина пленки/ Размер фракции
	double column_diamiter;			// Колонка. Внутренний диаметр
	double column_length;			// Колонка. Длина
	double liquidPassportFlag;		// -1 газовый 1 жидкостный
	double eluentFlow;				// Элюент. Поток
	double eluentTemperature;		// Элюент. Температура
	double eluentPressuer;			// Элюент. Давление
	double reagentFlow;				// Реагент. Поток
	double reagentTemperature;		// Реагент. Температура
	double reagentPressuer;			// Реагент. Давление
	string_t extra_parameters;		// Дополнительные параметры
	time_t date;					// Дата отбора пробы
	time_t andate;					// Дата анализа
	string_t comment;				// Комментарий
	double noise;					// Шум
	double drift;					// Дрейф
};


// Пик в таблице пиков хроматограммы (старая версия)
struct peek_buff_old_t
{
	enum { COMM_LENGTH = 32 };
	double back;
	double front;
	double concent;
	char comment[COMM_LENGTH];
	int right;
	uint8_t reserved[48];
};


// Пик в таблице пиков хроматограммы
struct peak_t
{
	int number;
	double front_time;				// время начала пика
	double back_time;				// время конца пика
	double concent;					// концентрация
	string_t comment;				// название компонента
	BOOL reference;					// реперный
	int left_link;					// номер неразделенного пика слева, или (-1)
	int right_link;					// номер неразделенного пика справа, или (-1)
	options_t extended_options;

	// Вычисляемые поля

	double front_base_height;
	double back_base_height;
	double time;					// время выхода
	double area;					// площадь
	double height;					// высота пика
};


// Таблица пиков хроматограммы
typedef std::vector<peak_t> peakstable_t;


// Хроматограмма
struct chromosfile_t
{
	double dt;						// время между выборками
	double t0;						// время первой выборки
	lattice_t data;					// профиль функции, заданный в наборе точек (узлов) с постоянным шагом.
	passport_t passport;			// паспорт
	peakstable_t peaks;				// таблица пиков хроматограммы
	options_t common_options;		// список дополнительных параметров
	double graph_range[4];
};


//bool OptionsIsExist(const options_t& options, LPCTSTR name);
void OptionsRead(LPSTREAM pStream, int ver, options_t& options);
void PassportRead(LPSTREAM pStream, LPCTSTR lpszPathName, passport_t& ps);
void PeakRead(LPSTREAM pStream, int ver, int number, peak_t& peak);
void PeaksTableRead(LPSTREAM pStream, peakstable_t& peaks, options_t& options);


int ChromosFileRead(LPCTSTR lpszPathName, chromosfile_t& file);
