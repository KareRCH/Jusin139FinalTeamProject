#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

using namespace rapidjson;

/// <summary>
/// RapidJson�� ���� ���� �Ľ� Ŭ����
/// ���� ������ �ٸ��� �پ��� ������ ������ ǥ���� �� �ֵ��� ���۵Ǿ���.
/// 
/// �ܺ� ������ ���� �� �ε� ���
/// 1�� ���� ���ã�� ���
/// ����� Object �ֱ� ���
/// �迭�� Object �ֱ� ���
/// �б� ���
/// 
/// [���ó]
/// ���� ������Ʈ ���� ���հ� �迭�� �߰� ��
/// RapidJson�� ���� �ӵ��� Ȱ���Ͽ� �ӵ��� �ʿ��� �κ�.
/// �˻��� Ȱ���Ͽ� JSON�� ���������� �����ؾ��ϴ� �κ��� �ʿ��� �� FRapidJson�� ���.
/// </summary>
class ENGINE_DLL FRapidJson
{
public:
	FRapidJson();
	FRapidJson(const FRapidJson& rhs);
	~FRapidJson() {}

public:
	static FRapidJson Open_Json(const wstring& strPath);

public:
	HRESULT Output_Json(const wstring& strPath);
	HRESULT Input_Json(const wstring& strPath);
	void Print_Json();

public:
	// ����� �ܼ� ������ �߰�, String
	HRESULT Write_StringData(const string& strMemberName, const string& strValue);
	// ����� �ܼ� ������ �߰�, Wstring
	HRESULT Write_StringData(const string& strMemberName, const wstring& strValue);
	// ����� �ܼ� ������ �߰�, Bool
	HRESULT Write_Data(const string& strMemberName, const _bool bValue);
	// ����� �ܼ� ������ �߰�, Int
	HRESULT Write_Data(const string& strMemberName, const _int iValue);
	// ����� �ܼ� ������ �߰�, Int
	HRESULT Write_Data(const string& strMemberName, const _uint iValue);
	// ����� �ܼ� ������ �߰�, Float
	HRESULT Write_Data(const string& strMemberName, const _float fValue);
	// ����� �ܼ� ������ �߰�, Float3
	HRESULT Write_Data(const string& strMemberName, const _float3 fValue);
	// ����� �ܼ� ������ �߰�, Float4
	HRESULT Write_Data(const string& strMemberName, const _float4 fValue);
	// ����� ������Ʈ ������ �߰�
	HRESULT Write_ObjectData(const string& strMemberName, FRapidJson& SerialData);
	// �迭�� �ٸ� Document �߰�
	HRESULT Pushback_ObjectToArray(const string& strMemberName, FRapidJson& SerialData);
	// �迭�� String �߰�
	HRESULT Pushback_StringToArray(const string& strMemberName, const string& strValue);
	// �迭�� String �߰�
	HRESULT Pushback_StringToArray(const string& strMemberName, const wstring& strValue)
	{ return Pushback_StringToArray(strMemberName, ConvertToString(strValue)); }

public:
	HRESULT Read_Data(const string& strMemberName, _bool& tValue) const;
	HRESULT Read_Data(const string& strMemberName, string& tValue) const;
	HRESULT Read_Data(const string& strMemberName, wstring& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _int& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _uint& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _float& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _float3& tValue) const;
	HRESULT Read_Data(const string& strMemberName, _float4& tValue) const;
	HRESULT Read_ObjectData(const string& strMemberName, FRapidJson& Data);
	HRESULT Read_ObjectFromArray(const string& strMemberName, _uint iIndex, FRapidJson& DstData);
	HRESULT Read_DataFromArray(const string& strMemberName, _uint iIndex, string& strValue);
	HRESULT Read_DataFromArray(const string& strMemberName, _uint iIndex, wstring& strValue);

public:
	_uint Read_ArraySize(const string& strMemberName);
	
public:
	Document	m_Doc;

};

END