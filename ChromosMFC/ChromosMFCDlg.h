#include "pch.h"
#include <locale>
#include "chromos-read-stg/chromosfile.h"
#include <map>
#include<iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <fstream>
#include <list>
// ChromosMFCDlg.h: файл заголовка
//



#pragma once

struct ChromatoGram
{
	double time;
	double height;
	double area;
	double concent;
	double molecularMass;
	double concentMass;
	string_t component;
};

// Диалоговое окно CChromosMFCDlg
class CChromosMFCDlg : public CDialogEx
{
	// Создание
public:
	CChromosMFCDlg(CWnd* pParent = nullptr);	// стандартный конструктор

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHROMOSMFC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV
private:
private:
	CListCtrl m_listCtrl;
	CString& filePath = CString();
	chromosfile_t stg_file{};
	std::list<ChromatoGram> result;

	// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void OnBnClickedButtonClose();
	void OnBnClickedButtonOpen();
	void LoadDataFromFile();
	void PopulateListControl();
	afx_msg void OnLvnItemchangedListResult(NMHDR* pNMHDR, LRESULT* pResult);
};