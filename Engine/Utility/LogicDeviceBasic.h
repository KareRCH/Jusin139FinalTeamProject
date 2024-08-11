#pragma once

#include "Engine_Defines.h"


BEGIN(Engine)

#pragma region ������

/// <summary>
/// fCur, fMax���� �����ϰ�, ���� ����� ���� ��ü
/// </summary>
class FGauge
{
public:
	FGauge() : fMax(_float()), fCur(_float()), fPrevCur(fCur) {}
	FGauge(_float _fMax, _bool bMax = false) 
		: fMax(_fMax), fCur(_float(_float(bMax)* _float(_fMax))) {}
	FGauge(const FGauge& rhs) : fMax(rhs.fMax), fCur(rhs.fCur), fPrevCur(rhs.fPrevCur) {}
	~FGauge() = default;

public:
	_float fMax = { 0.f }, fCur = { 0.f };
private:
	_float fPrevCur = { 0.f };

	// ���� Increase �� Cur���� �ʱ�ȭ
	_bool	bReserveReset = { false };


public:
	// Readjust ������ �����ε�
	FGauge operator =(_float fValue)
	{
		Readjust(fValue);
		return (*this);
	}
	 
public:
	// �� ������Ʈ �� �ƽ��� ���޽� ��ȯ
	_bool Increase(const _float&& increase) { return Increase(increase); }
	_bool Increase(const _float& increase)
	{
		// ��������
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur += increase;
		if (fCur >= fMax)
		{
			fCur = fMax;
			return true;
		}
		else if (fCur < 0.f)
			fCur = 0.f;

		return false;
	}

	// �� ������Ʈ �� �ƽ��� ���޽� ��ȯ
	_bool Increase_MaxOnce(const _float&& increase) { return Increase_MaxOnce(increase); }
	_bool Increase_MaxOnce(const _float& increase)
	{
		// ��������
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur += increase;
		if (fCur >= fMax)
		{
			fCur = fMax;
			return IsMax_Once();
		}
		else if (fCur < 0.f)
			fCur = 0.f;

		return false;
	}

	// �� ���� ������Ʈ
	_bool Decrease(const _float&& increase) { return Decrease(increase); }
	_bool Decrease(const _float& increase)
	{
		// ��������
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur -= increase;
		if (fCur <= 0.f)
		{
			fCur = 0.f;
			return true;
		}
		else if (fCur >= fMax)
			fCur = fMax;

		return false;
	}

	// Ư�� ����Ʈ(����)�� �Ѿ�� ��ȯ
	_bool Increase_UpToPoint(const _float&& increase, const _float&& point) 
	{ return Increase_UpToPoint(increase, point); }
	_bool Increase_UpToPoint(const _float& increase, const _float& point)
	{
		// ��������
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur += increase;
		if (fCur >= point)
		{
			fCur = point;
			return true;
		}
		else if (fCur <= 0.f)
			fCur = 0.f;

		return false;
	}

	// Ư�� ����Ʈ(����)�� �Ѿ�� ��ȯ, �ѹ� �����ص� ������ �����Ѵ�.
	_bool Increase_UpToPointOnce(_float increase, _float point)
	{
		// ��������
		if (AssignReserveReset())
			fCur = 0.f;

		fPrevCur = fCur;
		fCur += increase;
		if ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f))
		{
			if (fCur >= fMax)
			{
				fCur = fMax;
			}
			return true;
		}
		else if (fCur <= 0.f)
			fCur = 0.f;

		return false;
	}

	// ���簪 �ʱ�ȭ
	void Reset()
	{
		fCur = _float();
		fPrevCur = fCur;
	}

	void Reset(_float vResetValue)
	{
		fCur = vResetValue;
		fPrevCur = fCur;
	}

	void ReserveReset()
	{
		bReserveReset = true;
	}

	void Set_Max()
	{
		Reset(fMax);
	}

	// ���簪�� ������
	void Reverse()
	{
		fCur = (fMax - fCur);
		fPrevCur = fCur;
	}

	// fMax �� �缳�� �� ���簪 �ʱ�ȭ
	void Readjust(_float _fMax)
	{
		fMax = _fMax;
		fCur = _float();
	}

	// Ư�� ����Ʈ(����)�� �Ѿ�� ��� Ʈ��
	_bool IsUpTo(_float point) const
	{
		return (fCur >= point);
	}

	// �������� üũ�ϰ��� �ϴ� ������ �ѹ��� ������ ���� üũ�մϴ�.
	_bool IsUpTo_Once(_float point, _float increase) const
	{
		return ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f));
	}

	// �ƽ��� ��� ��� Ʈ��
	_bool IsMax() const
	{
		return (fCur >= fMax);
	}

	// �ƽ��� ��� �ѹ��� Ʈ��
	_bool IsMax_Once() const
	{
		return (fCur >= fMax && fPrevCur != fCur);
	}

	_bool IsZero_Once() const
	{
		return (fCur <= fMax && fPrevCur != fCur);
	}

	// �ۼ�Ʈ �� ��ȯ
	_float Get_Percent() const
	{
		return (Cast<_float>(fCur) / Cast<_float>(fMax));
	}

private:
	_bool AssignReserveReset()
	{
		if (bReserveReset)
		{
			bReserveReset = !bReserveReset;
			return true;
		}

		return false;
	}
};

using FDelay = FGauge;

#pragma endregion


#pragma region �պ� ������

/// <summary>
/// fCur, fMin, fMax���� �����ϰ�
/// �� ���̸� fCur�� �պ��ϴ� ������ Ŭ�����̴�.
/// </summary>
class FRoundTripGauge
{
public:
	explicit FRoundTripGauge() {}
	explicit FRoundTripGauge(_float _fMin, _float _fMax) 
		: fMax(_fMax), fMin(_fMin) {}
	FRoundTripGauge(const FRoundTripGauge& rhs) 
		: fMax(rhs.fMax), fCur(rhs.fCur), fPrevCur(rhs.fPrevCur) 
		, fMin(rhs.fMin), bIsLoop(rhs.bIsLoop)
	{}
	~FRoundTripGauge() = default;

public:
	_float fMin = { 0.f }, fMax = { 0.f }, fCur = { 0.f };

private:
	_float	fPrevCur = { 0.f };			// ������
	_byte	bSign = { 1 };				// ���⼺
	_bool	bIsLoop = { false };		// �ݺ� ����
	_bool	bIsEnd = { false };			// ����

public:
	// �� ������Ʈ, Max����, Min �������� �ѹ��� True�� ��ȯ��
	_bool Update(_float value)
	{
		fPrevCur = fCur;
		if (!bIsEnd)
		{
			// ���⼺�� ���� ���� ������Ʈ
			fCur += value * _float(bSign);
			if (fCur >= fMax && bSign == 1)
			{
				fCur = fMax;
				bSign = -1;

				return true;
			}
			else if (fCur <= fMin && bSign == -1)
			{
				fCur = fMin;
				bSign = 1;

				// ���� ������ ���������� ������.
				if (!bIsLoop)
					bIsEnd = true;

				return true;
			}
		}

		return false;
	}

	void Loop(_bool bLoop)
	{
		bIsEnd = false;
		bIsLoop = bLoop;
	}

	// ���簪 �ʱ�ȭ
	void Reset()
	{
		bIsEnd = false;
		fCur = _float();
		fPrevCur = fCur;
	}

	// fMax �� �缳�� �� ���簪 �ʱ�ȭ
	void Readjust(_float _fMin, _float _fMax)
	{
		fMin = _fMin;
		fMax = _fMax;
		fCur = _float();
	}

	// Ư�� ����Ʈ(����)�� �Ѿ�� ��� Ʈ��
	_bool IsUpTo(_float point)
	{
		return (fCur >= point);
	}

	// �������� üũ�ϰ��� �ϴ� ������ �ѹ��� ������ ���� üũ�մϴ�.
	_bool IsUpTo_Once(_float point, _float increase)
	{
		return ((fCur >= point - increase * 0.5f) && (fCur < point + increase * 0.5f));
	}

	_bool IsFinished()
	{
		return bIsEnd;
	}

	// �ƽ��� ��� ��� Ʈ��
	_bool IsMax()
	{
		return (fCur >= fMax);
	}

	// �ƽ��� ��� �ѹ��� Ʈ��
	_bool IsMax_Once()
	{
		return (fCur >= fMax && fPrevCur != fCur);
	}

	// �ۼ�Ʈ �� ��ȯ
	_float Percent() const
	{
		return (Cast<_float>(fCur) / Cast<_float>(fMax));
	}

	// 1�� �������� �ۼ�Ʈ�� ����, Max�� ����
	_float SaturatedPercent() const
	{
		return max(0.f, min(Cast<_float>(fCur), 1.f));
	}

	// Value�� �������� �ۼ�Ʈ�� ����, Max�� ����
	_float ClampPercent(const _float fValue) const
	{
		return max(0.f, min(Cast<_float>(fCur) / fValue, 1.f));
	}
};

#pragma endregion


#pragma region �ð� üĿ

/// <summary>
/// �ð��� �������� üŷ�ϴ� ����
/// </summary>
class FTimeChecker
{
public:
	FTimeChecker() {}
	FTimeChecker(_float fInterval) 
		: fMax(fInterval) {}
	FTimeChecker(const FTimeChecker& rhs) 
		: fTime(rhs.fTime), fMax(rhs.fMax) {}
	~FTimeChecker() {}

private:
	_float	fTime = { 0.f };
	_float	fMax = { 0.f };
	_bool	bTicked = { false };

public:
	FTimeChecker operator =(_float fValue)
	{
		fMax = fValue;
		fTime = 0.f;
		bTicked = false;

		return (*this);
	}

public:
	_bool Update(_cref_time fTimeDelta)
	{
		if (bTicked)
			bTicked = false;

		_bool bIsChecked = false;
		fTime += fTimeDelta;
		while (fTime >= fMax)
		{
			fTime -= fMax;
			bIsChecked = true;
		}

		return bTicked = bIsChecked;
	}

	_bool IsTicked()
	{
		return bTicked;
	}

};

#pragma endregion


END