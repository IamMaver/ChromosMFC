
// ChromosMFCDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "ChromosMFC.h"
#include "ChromosMFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CChromosMFCDlg



CChromosMFCDlg::CChromosMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHROMOSMFC_DIALOG, pParent)
{
	result.clear();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChromosMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CChromosMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CChromosMFCDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CChromosMFCDlg::OnBnClickedButtonClose)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RESULT, &CChromosMFCDlg::OnLvnItemchangedListResult)
END_MESSAGE_MAP()


// Обработчики сообщений CChromosMFCDlg

BOOL CChromosMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	//m_listCtrl.SubclassDlgItem(IDC_LIST_RESULT, this);
	//m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//// Добавление столбцов
	//m_listCtrl.InsertColumn(0, _T("Time"), LVCFMT_LEFT, 100);
	//m_listCtrl.InsertColumn(1, _T("Height"), LVCFMT_LEFT, 100);
	//m_listCtrl.InsertColumn(2, _T("Area"), LVCFMT_LEFT, 100);
	//m_listCtrl.InsertColumn(3, _T("Concent"), LVCFMT_LEFT, 100);
	//m_listCtrl.InsertColumn(4, _T("Molecular Mass"), LVCFMT_LEFT, 100);
	//m_listCtrl.InsertColumn(5, _T("Concent Mass"), LVCFMT_LEFT, 100);
	//m_listCtrl.InsertColumn(6, _T("Comment"), LVCFMT_LEFT, 200);

	// Инициализация List Control
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// Добавление столбцов
	m_listCtrl.InsertColumn(0, _T("#"), LVCFMT_LEFT, 20);
	m_listCtrl.InsertColumn(1, _T("Время (мин)"), LVCFMT_LEFT, 80);
	m_listCtrl.InsertColumn(2, _T("Высота (мВ)"), LVCFMT_LEFT, 80);
	m_listCtrl.InsertColumn(3, _T("Площадь (мВ*мин)"), LVCFMT_LEFT, 110);
	m_listCtrl.InsertColumn(4, _T("Концентрация (мол%)"), LVCFMT_LEFT, 125);
	m_listCtrl.InsertColumn(5, _T("Концентрация (масс%)"), LVCFMT_LEFT, 135);
	m_listCtrl.InsertColumn(6, _T("Компонент"), LVCFMT_LEFT, 120);



	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CChromosMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CChromosMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CChromosMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CChromosMFCDlg::OnBnClickedButtonClose()
{
	EndDialog(IDCLOSE);
}

void CChromosMFCDlg::OnBnClickedButtonOpen()
{
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		filePath = dlg.GetPathName();
		LoadDataFromFile();
		PopulateListControl();
	}
}

void CChromosMFCDlg::LoadDataFromFile()
{
	std::map<string_t, double> molecularMass = {
{_T("метан"), 16.04246}, {_T("этан"), 30.06904}, {_T("пропан"), 44.09562},
{_T("н-бутан"), 58.1222}, {_T("2-метилпропан"), 58.1222}, {_T("н-пентан"), 72.14878},
{_T("2-метилбутан"), 72.14878}, {_T("2,2-диметилпропан"), 72.14878}, {_T("н-гексан"), 86.17536},
{_T("с6"), 86.17536}, {_T("с6+"), 86.17536}, {_T("с7"), 100.20194}, {_T("с8"), 114.22852},
{_T("2-метилпентан"), 86.17536}, {_T("3-метилпентан"), 86.17536}, {_T("2,2-диметилбутан"), 86.17536},
{_T("2,3-диметилбутан"), 86.17536}, {_T("н-гептан"), 100.20194}, {_T("н-октан"), 114.22852},
{_T("н-нонан"), 128.2551}, {_T("н-декан"), 142.28168}, {_T("н-ундекан"), 156.30826},
{_T("н-додекан"), 170.33484}, {_T("н-тридекан"), 184.36142}, {_T("н-тетрадекан"), 198.388},
{_T("н-пентадекан"), 212.41458}, {_T("этилен"), 28.05316}, {_T("пропен"), 42.07974},
{_T("1-бутен"), 56.10632}, {_T("цис-2-бутен"), 56.10632}, {_T("транс-2-бутен"), 56.10632},
{_T("2-метилпропен"), 56.10632}, {_T("1-пентен"), 70.1329}, {_T("пропадиен"), 40.06386},
{_T("1,2-бутадиен"), 54.09044}, {_T("1,3-бутадиен"), 54.09044}, {_T("ацетилен"), 26.03728},
{_T("циклопентан"), 70.1329}, {_T("метилциклопентан"), 84.15948}, {_T("этилциклопентан"), 98.18606},
{_T("циклогексан"), 84.15948}, {_T("метилциклогексан"), 98.18606}, {_T("этилциклогексан"), 112.21264},
{_T("бензол"), 78.11184}, {_T("толуол"), 92.13842}, {_T("этилбензол"), 106.165}, {_T("о-ксилол"), 106.165},
{_T("метанол"), 32.04186}, {_T("метантиол"), 48.10746}, {_T("водород"), 2.01588}, {_T("вода"), 18.01528},
{_T("сероводород"), 34.08088}, {_T("аммиак"), 17.03052}, {_T("цианид водорода"), 27.02534},
{_T("монооксид углерода"), 28.0101}, {_T("карбонилсульфид"), 60.0751}, {_T("дисульфид углерода"), 76.1407},
{_T("гелий"), 4.002602}, {_T("неон"), 20.1797}, {_T("аргон"), 39.948}, {_T("азот"), 28.0134},
{_T("кислород"), 31.9988}, {_T("диоксид углерода"), 44.0095}, {_T("диоксид серы"), 64.0638},
{_T("воздух"), 28.96546}
	};

	chromosfile_t stg_file{};
	int err = ChromosFileRead(filePath, stg_file);

	if (err)
	{
		AfxMessageBox(_T("File read error occurred"));
		return;
	}

	result.clear();

	for (const auto& it : stg_file.peaks)
	{
		auto it_map = molecularMass.find(it.comment);

		if (it_map != molecularMass.end())
		{
			result.push_back(ChromatoGram{ it.time,it.height, it.area, it.concent, it_map->second, 0.0, it.comment });
		}
	}

	double sumMolar = 0;
	double sumMass = 0;

	for (auto& it : result)
	{
		sumMolar += it.concent;
		sumMass += it.concent * it.molecularMass;
	}

	for (auto& it : result)
	{
		it.concentMass = it.concent * it.molecularMass / sumMass;
		it.concent /= sumMolar;
		it.concent *= 100;
		it.concentMass *= 100;
	}
}

void CChromosMFCDlg::PopulateListControl()
{
	// Очистка предыдущих данных
	m_listCtrl.DeleteAllItems();

	// Итератор для списка result
	auto it = result.begin();
	int index = 0;

	// Заполнение List Control новыми данными
	while (it != result.end())
	{
		// Добавление новой строки
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = index;
		lvItem.iSubItem = 0;
		lvItem.pszText = const_cast<LPTSTR>(std::to_wstring(it->time).c_str());
		m_listCtrl.InsertItem(&lvItem);

		// Заполнение столбцов
		m_listCtrl.SetItemText(index, 0, std::to_wstring(index+1).c_str());
		m_listCtrl.SetItemText(index, 1, std::to_wstring(it->time).c_str());
		m_listCtrl.SetItemText(index, 2, std::to_wstring(it->height).c_str());
		m_listCtrl.SetItemText(index, 3, std::to_wstring(it->area).c_str());
		m_listCtrl.SetItemText(index, 4, std::to_wstring(it->concent).c_str());
		m_listCtrl.SetItemText(index, 5, std::to_wstring(it->concentMass).c_str());
		m_listCtrl.SetItemText(index, 6, it->component.c_str());

		++it;
		++index;
	}
}

void CChromosMFCDlg::OnLvnItemchangedListResult(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: добавьте свой код обработчика уведомлений
	*pResult = 0;
}
