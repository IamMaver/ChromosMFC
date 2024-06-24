#include "pch.h"
#include "chromosfile.h"
#include <atlconv.h>


#define STREAMNAME_RAWDATA			_T("Raw_data")
#define STREAMNAME_TIME				_T("Time")
#define STREAMNAME_TABLE			_T("Table")
#define STREAMNAME_PASSPORT			_T("Passport")
#define STREAMNAME_AUTOGRAPH		_T("AutoGraph")

//
// Tools
//


ULONG AfxReadStringLength(LPSTREAM pStream)
{
	ULONG nLength;
	BYTE b = 0;
	HRESULT hr;

	hr = pStream->Read(&b, sizeof(b), NULL);
	if (FAILED(hr))
		return 0;

	if (b == 0xff)
	{
		WORD w = 0;
		hr = pStream->Read(&w, sizeof(w), NULL);
		if (FAILED(hr))
			return 0;

		if (w == 0xffff)
		{
			DWORD dw = 0;
			hr = pStream->Read(&dw, sizeof(dw), NULL);
			if (FAILED(hr))
				return 0;

			if (dw == 0xffffffff)
			{
				ULONGLONG ulong = 0;
				hr = pStream->Read(&ulong, sizeof(ulong), NULL);
				if (FAILED(hr))
					return 0;

				nLength = (ULONG)ulong;
			}
			else
			{
				nLength = dw;
			}
		}
		else
		{
			nLength = w;
		}
	}
	else
	{
		nLength = b;
	}
	return nLength;
}


bool ReadRawData(LPSTREAM pStream, void* pData, ULONG uSize)
{
	HRESULT hr = pStream->Read(&pData, sizeof(pData), &uSize);
	return SUCCEEDED(hr) && uSize != sizeof(pData);
}


bool ReadString(LPSTREAM pStream, string_t& str)
{
	USES_CONVERSION;
	ULONG strLength = AfxReadStringLength(pStream);

	LPSTR lpszBuff = new CHAR[strLength + 1];
	HRESULT hr = pStream->Read(lpszBuff, strLength, NULL);
	lpszBuff[strLength] = L'\0';
	str = A2T(lpszBuff);
	delete[] lpszBuff;
	return SUCCEEDED(hr);
}


bool ReadDouble(LPSTREAM pStream, double& val)
{
	HRESULT hr = pStream->Read(&val, sizeof(double), NULL);
	return SUCCEEDED(hr);
}


//bool ReadBytes(LPSTREAM pStream, LPVOID lpData, ULONG cb)
//{
//	HRESULT hr = pStream->Read(lpData, cb, NULL);
//	return SUCCEEDED(hr);
//}


bool ReadInt32(LPSTREAM pStream, INT32& val)
{
	HRESULT hr = pStream->Read(&val, sizeof(INT32), NULL);
	return SUCCEEDED(hr);
}


bool ReadUInt32(LPSTREAM pStream, UINT32& val)
{
	HRESULT hr = pStream->Read(&val, sizeof(UINT32), NULL);
	return SUCCEEDED(hr);
}


bool ReadTime(LPSTREAM pStream, time_t& tm)
{
	__time32_t t = 0;
	HRESULT hr = pStream->Read(&t, sizeof(__time32_t), NULL);
	tm = t;
	return SUCCEEDED(hr);
}


//
// Calc
//


double LatticeGetBaseLineY(double t, double t1, double t2, double y1, double y2)
{
	if (t2 == t1)
		return y1;
	return y1 + (t - t1) * (y2 - y1) / (t2 - t1);
}


double LatticeGetData(double t, const lattice_t& data, double dt, double t0)
{
	if (data.size() < 2)
		return 0;

	if (t < t0)
		return LatticeGetBaseLineY(t, t0, t0 + dt, data[0], data[1]);

	size_t n = size_t(floor((t - t0) / dt));
	if (n >= (data.size() - 1))
		return LatticeGetBaseLineY(t, t0 + dt * (data.size() - 2), t0 + dt * (data.size() - 1),
			data[data.size() - 2], data[data.size() - 1]);

	return LatticeGetBaseLineY(t, t0 + dt * n, t0 + dt * (n + 1), data[n], data[n + 1]);
}


void SetBaseLine(peak_t& peak, double t1, double t2, const lattice_t& data, double dt, double t0)
{
	double y1 = LatticeGetData(t1, data, dt, t0);
	double y2 = LatticeGetData(t2, data, dt, t0);

	peak.front_base_height = LatticeGetBaseLineY(peak.front_time, t1, t2, y1, y2);
	peak.back_base_height = LatticeGetBaseLineY(peak.back_time, t1, t2, y1, y2);
}


void CalcLibPeakProperties(int num_data, const LATTICE_T* data, double dt, double t0,
	double front_time, double back_time, double front_base_height, double back_base_height,
	double& height, double& area, double& time)
{
	if (front_time == back_time || dt == 0)
	{
		height = 0;
		area = 0;
		time = 0;

		return;
	}

	if (front_time > back_time)
	{
		double ttmp = front_time;
		front_time = back_time;
		back_time = ttmp;
	}

	int i1 = int(ceil((front_time - t0) / dt)); // ближайшая точка правее front_time 
	int	i2 = int(floor((back_time - t0) / dt)); // ближайшая точка левее back_time 

	if (i2 > num_data - 2)
	{
		i2 = num_data - 2; // правее i2 должна остаться ещё одна точна для построения линии
	}
	if (i1 < 1)
	{
		i1 = 1;          // левее  i1 --//--
	}

	if (i1 > i2)
	{
		// времена начала и конца пика внутри одного сегмента между выборками
		height = 0;
		area = 0;
		time = (front_time + back_time) / 2;

		return;
	}

	double ti1 = t0 + dt * i1; // время первой точки внутри пика
	double ti2 = t0 + dt * i2; // время последней точки внутри пика

	// уровени сигнала начала и конца пика (по линейной интерполяции между точками)
	double front_height = data[i1] + (data[i1] - data[i1 - 1]) * (front_time - ti1) / dt;
	double back_height = data[i2] + (data[i2 + 1] - data[i2]) * (back_time - ti2) / dt;

	// расчёт уравнения базовой линии
	double base_k = (back_base_height - front_base_height) / (back_time - front_time);
	double base_b = front_base_height - base_k * front_time;

	int i_time = 0;
	double d_height = -DBL_MAX;
	double d_area = 0;

	for (int i = i1; i <= i2; i++)
	{
		double y_for_hight = fabs(data[i] - (base_k * (t0 + i * dt) + base_b)); // вычитание базовой только для нахождения высоты

		if (d_height < y_for_hight)
		{
			i_time = i;
			d_height = y_for_hight;
		}
		d_area += data[i];
	}

	d_area -= (data[i1] + data[i2]) / 2; // метод трапеций

	d_area *= dt;

	// добавляем кусочки перед первой и после последней точки
	d_area += (front_height + data[i1]) / 2 * (ti1 - front_time);
	d_area += (data[i2] + back_height) / 2 * (back_time - ti2);

	// вычитание базовой
	d_area -= (front_base_height + back_base_height) * (back_time - front_time) / 2;

	height = d_height;
	area = d_area;
	time = t0 + i_time * dt;
}


void SetProperties(peak_t& peak, const lattice_t& data, double dt, double t0)
{
	if (peak.left_link < 0 && peak.right_link < 0)
	{
		// для неразделённых пиков зададим базовую линию по самому пику
		peak.front_base_height = LatticeGetData(peak.front_time, data, dt, t0);
		peak.back_base_height = LatticeGetData(peak.back_time, data, dt, t0);
	}

	CalcLibPeakProperties(data.size(), data.data(), dt, t0,
		peak.front_time, peak.back_time,
		peak.front_base_height, peak.back_base_height,
		peak.height, peak.area, peak.time);
}


void PeaksTableUpdate(peakstable_t& peaks, const lattice_t& data, double dt, double t0)
{
	for (size_t i = 0; i < peaks.size(); i++)
	{
		int right = peaks[i].right_link;
		if (right >= 0 && right < (int)peaks.size())
		{
			peaks[right].left_link = i;
		}
		else
		{
			peaks[i].right_link = -1;
		}
	}

	// поиск цепочки пиков и установка одной базовой линии
	for (size_t i = 0; i < peaks.size(); i++)
	{
		if (peaks[i].right_link < 0 || peaks[i].left_link >= 0)
			continue;  // not start of chain
		int last = peaks[i].right_link;
		int j = last;
		while ((j = peaks[j].right_link) >= 0)
		{
			last = j;
		}
		double t1 = peaks[i].front_time;
		double t2 = peaks[last].back_time;
		j = i;
		do
		{
			SetBaseLine(peaks[j], t1, t2, data, dt, t0);
		}
		while ((j = peaks[j].right_link) >= 0);
	}

	// перевычисление площадей, высот, и времён выхода пиков
	for (size_t i = 0; i < peaks.size(); i++)
	{
		SetProperties(peaks[i], data, dt, t0);
	}

	// вычитание площадей наездников из площади основного пика
	for (size_t i = 0; i < peaks.size(); i++)
	{
		double front = peaks[i].front_time;
		double back = peaks[i].back_time;
		for (size_t j = 0; j < peaks.size(); j++)
		{
			if (i == j)
				continue;
			double front1 = peaks[j].front_time;
			double back1 = peaks[j].back_time;
			if (front1 >= front && back1 <= back)
			{
				peaks[i].area -= peaks[j].area;
			}
		}
	}

	// все площади делаем положительными
	for (size_t i = 0; i < peaks.size(); i++)
	{
		if (peaks[i].area < 0)
		{
			peaks[i].area = fabs(peaks[i].area);
		}
	}
}


//
// Read
//


//bool OptionsIsExist(const options_t& options, LPCTSTR name)
//{
//	auto iter = std::find_if(options.cbegin(), options.cend(),
//		[name](const auto& x) { return _tcscmp(x.first.c_str(), name); });
//	return iter != options.cend();
//}


void OptionsRead(LPSTREAM pStream, int ver, options_t& options)
{
	// Дополнительные параметры появивиль в файле версии 3
	if (ver < 3)
		return;

	unsigned int sz;
	ReadUInt32(pStream, sz);

	if (sz == 0)
	{
		options.clear();
	}
	else
	{
		if (ver == 3)
		{
			// сразу проверяем на совпадения имён и пустые имена
			for (unsigned int i = 0; i < sz; i++)
			{
				string_t name, value;
				ReadString(pStream, name);
				ReadString(pStream, value);

				// Наименование параметра не указано
				if (name.empty())
					continue;

				//if (OptionsIsExist(options, name.c_str()))
				//	continue;

				options.push_back(std::make_pair(name, value));
			}
		}
		else
		{
			for (unsigned int i = 0; i < sz; i++)
			{
				string_t name, value;
				ReadString(pStream, name);
				ReadString(pStream, value);

				options.push_back(std::make_pair(name, value));
			}
		}
	}
}


void PassportRead(LPSTREAM pStream, LPCTSTR lpszPathName, passport_t& ps)
{
	ps.filename = lpszPathName;

	int sign, ver;
	ReadInt32(pStream, sign);
	ReadInt32(pStream, ver);

	// Подпись не соответствует файлу
	if (sign != PASSPORT_SIGNATURE)
		return;

	// Файл создан в более поздней версии
	if (ver > PASSPORT_VERSION)
		return;

	ReadString(pStream, ps.sample);
	ReadString(pStream, ps.extra_parameters);
	ReadDouble(pStream, ps.volume);
	ReadDouble(pStream, ps.dilution);
	ReadDouble(pStream, ps.norm);
	ReadTime(pStream, ps.date);
	ReadString(pStream, ps.place);
	ReadString(pStream, ps.method);
	ReadDouble(pStream, ps.duration);
	ReadInt32(pStream, ps.viala);
	ReadInt32(pStream, ps.numInSeria);
	ReadInt32(pStream, ps.graduation);
	ReadString(pStream, ps.lab_assistant);
	ReadString(pStream, ps.detector);
	ReadString(pStream, ps.column);
	ReadInt32(pStream, ps.column_num);
	ReadDouble(pStream, ps.column_size);
	ReadDouble(pStream, ps.column_diamiter);
	ReadDouble(pStream, ps.column_length);
	ReadDouble(pStream, ps.liquidPassportFlag);
	ReadString(pStream, ps.eluent);
	ReadDouble(pStream, ps.eluentFlow);
	ReadDouble(pStream, ps.eluentPressuer);
	ReadDouble(pStream, ps.eluentTemperature);
	ReadString(pStream, ps.reagent);
	ReadDouble(pStream, ps.reagentFlow);
	ReadDouble(pStream, ps.reagentPressuer);
	ReadDouble(pStream, ps.reagentTemperature);
	ReadTime(pStream, ps.andate);
	ReadInt32(pStream, ps.assayId);
	ReadString(pStream, ps.series);
	ReadString(pStream, ps.comment);

	if (ver >= 2)
	{
		ReadString(pStream, ps.station);
	}

	if (ver >= 3)
	{
		ReadDouble(pStream, ps.noise);
		ReadDouble(pStream, ps.drift);
	}
	else
	{
		ps.noise = 0.;
		ps.drift = 0.;
	}
}


void PeakRead(LPSTREAM pStream, int ver, int number, peak_t& peak)
{
	peak.number = number;
	if (ver == 0)
	{
		USES_CONVERSION;
		peek_buff_old_t buff;
		ReadRawData(pStream, &buff, sizeof(buff));
		peak.front_time = buff.front;
		peak.back_time = buff.back;
		peak.right_link = buff.right;
		peak.left_link = -1;
		peak.concent = buff.concent;
		peak.comment = A2T(buff.comment);
		peak.reference = false;
	}
	else
	{
		ReadDouble(pStream, peak.front_time);
		ReadDouble(pStream, peak.back_time);
		ReadDouble(pStream, peak.concent);
		ReadString(pStream, peak.comment);
		ReadInt32(pStream, peak.right_link);
		peak.left_link = -1;
		if (ver >= 2)
		{
			ReadInt32(pStream, peak.reference);
		}
		else
		{
			peak.reference = false;
		}
	}

	OptionsRead(pStream, ver, peak.extended_options);
}


void PeaksTableRead(LPSTREAM pStream, peakstable_t& peaks, options_t& options)
{
	HRESULT hr;
	if (NULL == pStream)
		return;

	ULARGE_INTEGER libNewPosition = { 0, 0 };
	LARGE_INTEGER startPosition = { 0, 0 };

	hr = pStream->Seek(startPosition, STREAM_SEEK_END, &libNewPosition);
	if (S_OK != hr)
		return;

	hr = pStream->Seek(startPosition, STREAM_SEEK_SET, NULL);
	if (S_OK != hr)
		return;

	int stream_size = libNewPosition.LowPart;
	
	if (stream_size < 2 * sizeof(int))
	{
		peaks.clear();
		return;
	}

	int sign1 = 0;
	int sign2 = 0;
	int ver = 0;
	int num = 0;

	hr = pStream->Read(&sign1, sizeof(sign1), NULL);
	if (FAILED(hr))
		return;

	hr = pStream->Read(&sign2, sizeof(sign2), NULL);
	if (FAILED(hr))
		return;

	bool bNewStyle = (sign1 == PEAKSTABLE_SIGNATURE1 && sign2 == PEAKSTABLE_SIGNATURE2);
	if (!bNewStyle)
	{
		// return to the begining
		LARGE_INTEGER startPosition = { 0, 0 };
		hr = pStream->Seek(startPosition, STREAM_SEEK_SET, NULL);
		if (S_OK != hr)
			return;
	}

	if (bNewStyle)
	{
		hr = pStream->Read(&ver, sizeof(ver), NULL);
		if (FAILED(hr))
			return;
		
		if (ver > PEAKSTABLE_VERSION)
			return;

		hr = pStream->Read(&num, sizeof(num), NULL);
		if (FAILED(hr))
			return;
	}
	else
	{
		num = stream_size / sizeof(peek_buff_old_t);
	}

	peaks.resize(num);
	for (size_t i = 0; i < peaks.size(); i++)
	{
		PeakRead(pStream, ver, i, peaks[i]);
	}

	OptionsRead(pStream, ver, options);
}


int ChromosFileRead(LPCTSTR lpszPathName, chromosfile_t& file)
{
	USES_CONVERSION;
	LPSTORAGE pStgRoot = NULL;
	LPSTREAM pStream = NULL;
	ULONG nLength = 0ul;
	HRESULT hr;

	hr = StgOpenStorage(T2COLE(lpszPathName), NULL,
		STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &pStgRoot);
	if (FAILED(hr))
		return -1;

	// read Data
	hr = pStgRoot->OpenStream(T2COLE(STREAMNAME_RAWDATA), NULL,
		STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
	if (FAILED(hr))
	{
		pStgRoot->Release();
		return -1;
	}

	ULARGE_INTEGER libNewPosition = { 0, 0 };
	LARGE_INTEGER startPosition = { 0, 0 };
	hr = pStream->Seek(startPosition, STREAM_SEEK_END, &libNewPosition);
	if (S_OK != hr)
	{
		pStream->Release();
		pStgRoot->Release();
		return -1;
	}

	DWORD nDataLength = libNewPosition.LowPart / sizeof(LATTICE_T);
	hr = pStream->Seek(startPosition, STREAM_SEEK_SET, NULL);
	if (S_OK != hr)
	{
		pStream->Release();
		pStgRoot->Release();
		return -1;
	}

	file.data.resize(nDataLength);
	hr = pStream->Read(file.data.data(), libNewPosition.LowPart, &nLength);
	if (FAILED(hr))
	{
		pStream->Release();
		pStgRoot->Release();
		return -1;
	}
	pStream->Release();

	// read Time
	pStream = NULL;
	hr = pStgRoot->OpenStream(T2COLE(STREAMNAME_TIME), NULL,
		STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
	if (FAILED(hr))
	{
		pStgRoot->Release();
		return -1;
	}

	hr = pStream->Seek(startPosition, STREAM_SEEK_END, &libNewPosition);
	if (FAILED(hr))
	{
		pStream->Release();
		pStgRoot->Release();
		return -1;
	}
	float Time[3];
	if (libNewPosition.LowPart < sizeof(Time))
	{
		file.dt = 1;
		file.t0 = 0;
	}
	else
	{
		hr = pStream->Seek(startPosition, STREAM_SEEK_SET, NULL);
		if (S_OK != hr)
		{
			pStream->Release();
			pStgRoot->Release();
			return -1;
		}
		hr = pStream->Read(Time, sizeof(Time), &nLength);
		if (S_OK != hr)
		{
			pStream->Release();
			pStgRoot->Release();
			return -1;
		}
		file.dt = Time[1] - Time[0];
		file.t0 = Time[0];
	}
	pStream->Release();

	// read Peaks table
	pStream = NULL;
	hr = pStgRoot->OpenStream(T2COLE(STREAMNAME_TABLE), NULL,
		STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
	if (FAILED(hr))
	{
		pStgRoot->Release();
		return -1;
	}

	PeaksTableRead(pStream, file.peaks, file.common_options);
	pStream->Release();

	// read Passport
	pStream = NULL;
	hr = pStgRoot->OpenStream(T2COLE(STREAMNAME_PASSPORT), NULL,
		STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
	if (FAILED(hr))
	{
		pStgRoot->Release();
		return -1;
	}

	PassportRead(pStream, lpszPathName, file.passport);
	pStream->Release();

	// read AutoGraph
	pStream = NULL;
	hr = pStgRoot->OpenStream(T2COLE(STREAMNAME_AUTOGRAPH), NULL,
		STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
	if (FAILED(hr))
	{
		pStgRoot->Release();
		return -1;
	}

	hr = pStream->Read(&file.graph_range, sizeof(file.graph_range), &nLength);
	if (FAILED(hr))
	{
		pStream->Release();
		pStgRoot->Release();
		return -1;
	}

	pStream->Release();

	pStgRoot->Release();
	
	PeaksTableUpdate(file.peaks, file.data, file.dt, file.t0);

	return 0;
}
