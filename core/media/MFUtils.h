#pragma once

#if defined(_WIN32)

#    include <Unknwn.h>
#    include <wrl/client.h>

namespace MFUtils
{

template <typename _Ty>
using TComPtr = Microsoft::WRL::ComPtr<_Ty>;

template <typename _Ty>
inline TComPtr<_Ty> MakeComPtr()
{
    TComPtr<_Ty> obj;
    _Ty** ppv = &obj;
    *ppv      = new _Ty();
    return obj;
}

template <typename _Ty, typename... _Types>
inline TComPtr<_Ty> MakeComPtr(_Types&&... args)
{
    TComPtr<_Ty> obj;
    _Ty** ppv = &obj;
    *ppv      = new _Ty(std::forward<_Types>(args)...);
    return obj;
}

template <typename _Ty>
inline TComPtr<_Ty> ReferencedPtrToComPtr(_Ty* ptr)
{
    TComPtr<_Ty> obj;
    _Ty** ppv = &obj;
    *ppv      = ptr;
    return obj;
}

HRESULT InitializeMFOnce();

}  // namespace MFUtils

namespace DX
{
// Helper class for COM exceptions
class com_exception : public std::exception
{
public:
    com_exception(HRESULT hr) noexcept : result(hr) {}

    const char* what() const override
    {
        static char s_str[64] = {};
        sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
        return s_str;
    }

private:
    HRESULT result;
};

// Helper utility converts D3D API failures into exceptions.
inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw com_exception(hr);
    }
}
}  // namespace DX

#endif
