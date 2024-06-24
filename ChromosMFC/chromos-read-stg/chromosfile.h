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


// ������ ����� �������������
typedef std::vector<LATTICE_T> lattice_t;


// ����� ������������� ��������� ����������
typedef std::basic_string<TCHAR> string_t;


// ������ ����������
typedef std::vector<std::pair<string_t, string_t>> options_t;


// ������� �������������
struct passport_t
{
	int assayId;					// ID �������
	string_t filename;				// ������ ���� � ����� �������������
	string_t sample;				// �����
	string_t series;				// �����
	string_t detector;				// ��������
	string_t method;				// �����
	string_t eluent;				// ������
	string_t reagent;				// �������
	string_t lab_assistant;			// ��������
	string_t station;				// ����� ������
	string_t place;					// ����� ������
	int numInSeria;					// ����� � ����� ���������
	int graduation;					// �����������
	int viala;						// ����� �����
	double volume;					// ���� ����� / ����� �����
	double dilution;				// ����� ��������� / ����������
	double norm;					// ����� (100%)
	double duration;				// ����������������� ������� � �������
	string_t column;				// �������. ������������
	int column_num;					// �������. �����
	double column_size;				// �������. ������� ������/ ������ �������
	double column_diamiter;			// �������. ���������� �������
	double column_length;			// �������. �����
	double liquidPassportFlag;		// -1 ������� 1 ����������
	double eluentFlow;				// ������. �����
	double eluentTemperature;		// ������. �����������
	double eluentPressuer;			// ������. ��������
	double reagentFlow;				// �������. �����
	double reagentTemperature;		// �������. �����������
	double reagentPressuer;			// �������. ��������
	string_t extra_parameters;		// �������������� ���������
	time_t date;					// ���� ������ �����
	time_t andate;					// ���� �������
	string_t comment;				// �����������
	double noise;					// ���
	double drift;					// �����
};


// ��� � ������� ����� ������������� (������ ������)
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


// ��� � ������� ����� �������������
struct peak_t
{
	int number;
	double front_time;				// ����� ������ ����
	double back_time;				// ����� ����� ����
	double concent;					// ������������
	string_t comment;				// �������� ����������
	BOOL reference;					// ��������
	int left_link;					// ����� �������������� ���� �����, ��� (-1)
	int right_link;					// ����� �������������� ���� ������, ��� (-1)
	options_t extended_options;

	// ����������� ����

	double front_base_height;
	double back_base_height;
	double time;					// ����� ������
	double area;					// �������
	double height;					// ������ ����
};


// ������� ����� �������������
typedef std::vector<peak_t> peakstable_t;


// �������������
struct chromosfile_t
{
	double dt;						// ����� ����� ���������
	double t0;						// ����� ������ �������
	lattice_t data;					// ������� �������, �������� � ������ ����� (�����) � ���������� �����.
	passport_t passport;			// �������
	peakstable_t peaks;				// ������� ����� �������������
	options_t common_options;		// ������ �������������� ����������
	double graph_range[4];
};


//bool OptionsIsExist(const options_t& options, LPCTSTR name);
void OptionsRead(LPSTREAM pStream, int ver, options_t& options);
void PassportRead(LPSTREAM pStream, LPCTSTR lpszPathName, passport_t& ps);
void PeakRead(LPSTREAM pStream, int ver, int number, peak_t& peak);
void PeaksTableRead(LPSTREAM pStream, peakstable_t& peaks, options_t& options);


int ChromosFileRead(LPCTSTR lpszPathName, chromosfile_t& file);
