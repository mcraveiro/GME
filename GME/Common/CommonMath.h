
#include <limits>
#include <string>

// return: true if str represents -Inf, Inf, or NaN
static bool ParseSpecialDouble(const wchar_t* str, double& out)
{
	const wchar_t* val = (*str == '-' && *str != '\0') ? str + 1 : str;
	if (_wcsnicmp(L"-Inf", str, 4) == 0 || wcsncmp(L"-1.#INF", str, 7) == 0)
	{
		out = -std::numeric_limits<double>::infinity();
		return true;
	}
	else if (_wcsnicmp(L"Inf", str, 3) == 0 || wcsncmp(L"1.#INF", str, 6) == 0)
	{
		out = std::numeric_limits<double>::infinity();
		return true;
	}
	else if (_wcsnicmp(L"NaN", val, 3) == 0 || wcsncmp(L"1.#IND", val, 7) == 0 || wcsncmp(L"1.#QNAN", val, 7) == 0 || wcsncmp(L"1.#SNAN", val, 7) == 0)
	{
		out = std::numeric_limits<double>::quiet_NaN();
		return true;
	}
	return false;
}
