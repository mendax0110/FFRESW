/**
 * @file ptrUtils.h
 * @author Adrian Goessl
 * @brief Header only library implementation.
 * @version 0.1
 * @date 2024-01-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PTRUTILS_H
#define PTRUTILS_H

#include <Arduino.h>
#include <serialMenu.h>

/**
 * @brief Template function to safely delete a pointer and set it to nullptr.
 *
 * @tparam T -> The type of the pointer.
 * @param ptr -> The pointer to delete.
 */
template <typename T>
static inline void SafeDelete(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

/**
 * @brief Function to safely delete an array and set it to nullptr.
 *
 * @tparam T -> The type of the array.
 * @param ptr -> The array to delete.
 */
template <typename T>
static inline void SafeDeleteArray(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete[] ptr;
        ptr = nullptr;
    }
}

/**
 * @brief Template to verify a pointer is not nullptr.
 *
 * @tparam T -> The type of the pointer.
 * @param value -> The pointer to verify.
 * @param expected -> The expected value of the pointer.
 * @param errorMsg -> The error message to print if the verification fails.
 */
template <typename T>
static inline void Verify(const T& value, const T& expected, const char* errorMsg = nullptr)
{
    if (value != expected)
    {
        if (errorMsg)
        {
        	SerialMenu::printToSerial(errorMsg);
        }
        else
        {
        	String errStr;
        	errStr += "[ERROR] Verification failed: Value (";
        	errStr += value;
        	errStr += ") does not match expected (";
        	errStr += expected;
        	errStr += ").";
        	SerialMenu::printToSerial(errStr);
        }
        while (true); // Halt execution
    }
}

/**
 * @brief Template Specialization for pointers (Pointer vs Pointer)
 *
 * @tparam T -> The type of the pointer.
 * @param value -> The pointer to verify.
 * @param expected -> The expected value of the pointer.
 * @param errorMsg -> The error message to print if the verification fails.
 */
template <typename T>
static inline void Verify(T* value, T* expected, const char* errorMsg = nullptr)
{
    if (value != expected)
    {
        if (errorMsg)
        {
        	SerialMenu::printToSerial(errorMsg);
        }
        else
        {
        	String errStr;
        	errStr += "[ERROR] Verification failed: Pointer (";
        	errStr += (unsigned long)value, HEX;
        	errStr += ") does not match expected pointer (";
        	errStr += (unsigned long)expected, HEX;
        	errStr += ").";
        	SerialMenu::printToSerial(errStr);
        }
        while (true); // Halt execution
    }
}

/**
 * @brief Template Specialization for pointers (Pointer vs nullptr)
 *
 * @tparam T -> The type of the pointer.
 * @param value -> The pointer to verify.
 * @param errorMsg -> The error message to print if the verification fails.
 */
template <typename T>
static inline void Verify(T* value, const char* errorMsg = nullptr)
{
    if (value != nullptr)  // Directly compare with nullptr (no std::nullptr_t)
    {
        if (errorMsg)
        {
        	SerialMenu::printToSerial(errorMsg);
        }
        else
        {
        	String errStr;
        	errStr += "[ERROR] Verification failed: Pointer (";
        	errStr += (unsigned long)value, HEX;
        	errStr += ") is not null.";
        	SerialMenu::printToSerial(errStr);
        }
        while (true); // Halt execution
    }
}


/**
 * @brief Macro to safely delete a pointer and verify it is nullptr.
 *
 */
#define tryDeletePtr(ptr) 										\
	if (PtrUtils::IsValidPtr(ptr))								\
    	SafeDelete(ptr);        								\



/// @brief Utility class for pointer operations. \class PtrUtils
class PtrUtils
{
public:
    /**
     * Check if a pointer is nullptr.
     *
     * \param ptr Pointer to check.
     * \return true if the pointer is nullptr, false otherwise.
     */
    template <typename T>
    static inline bool IsNullPtr(T* ptr)
    {
        return ptr == nullptr;
    }

    /**
     * Check if a pointer is valid (not nullptr).
     *
     * \param ptr Pointer to check.
     * \return true if the pointer is not nullptr, false otherwise.
     */
    template <typename T>
    static inline bool IsValidPtr(T* ptr)
    {
        return ptr != nullptr;
    }
};

/**
 * @brief Template to Clear an array of pointers.
 *
 * @tparam T -> The type of the pointer.
 * @param array -> The array of pointers.
 * @param size -> The size of the array.
 */
template <typename T>
static inline void ClearArray(T* array, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        array[i] = T();
    }
}

/**
 * @brief Template to print info about a pointer.
 *
 * @tparam T -> The type of the pointer.
 * @param ptr -> The pointer to print info about.
 * @param ptrName -> The name of the pointer.
 */
template <typename T>
static inline void PrintPtrInfo(T* ptr, const char* ptrName = "Pointer")
{
    if (ptr == nullptr)
    {
    	SerialMenu::printToSerial("[INFO] " + String(ptrName) + String("is nullptr"));
    }
    else
    {
    	SerialMenu::printToSerial("[INFO] " + String(ptrName) + String(" points to address: 0x") + (uintptr_t)ptr, HEX);
    }
}

/**
 * @brief Template class for a Scoped Pointer. \class ScopedPointer
 *
 * @tparam T -> The type of the pointer.
 */
template <typename T>
class ScopedPointer
{
private:
    T* ptr;

public:
    explicit ScopedPointer(T* p = nullptr) : ptr(p) {}
    ~ScopedPointer() { SafeDelete(ptr); }

    /**
     * @brief Function to get the pointer.
     *
     * @return T* -> The pointer.
     */
    T* get() const { return ptr; }

    /**
     * @brief Function to release the pointer.
     *
     * @return T* -> The released pointer.
     */
    T* release()
    {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    /**
     * @brief Function to reset the pointer.
     *
     * @param p -> The pointer to reset to.
     */
    void reset(T* p = nullptr)
    {
        SafeDelete(ptr);
        ptr = p;
    }

    /**
     * @brief Operator to dereference the pointer.
     *
     * @return T& -> The dereferenced pointer.
     */
    T& operator*() const { return *ptr; }

    /**
     * @brief Operator to access the pointer.
     *
     * @return T* -> The pointer.
     */
    T* operator->() const { return ptr; }
};

/**
 * @brief Tempalte class for wrapping a pointer. \class PointerWrapper
 *
 * @tparam T
 */
template <typename T>
class PointerWrapper
{
private:
    T* ptr;

public:
    explicit PointerWrapper(T* p = nullptr) : ptr(p) {}
    ~PointerWrapper() { SafeDelete(ptr); }

    /**
     * @brief Function to get the pointer.
     *
     * @return T* -> The pointer.
     */
    T* get() const { return ptr; }

    /**
     * @brief Function to release the pointer.
     *
     * @return T* -> The released pointer.
     */
    T* release()
    {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    /**
     * @brief Function to reset the pointer.
     *
     * @param p -> The pointer to reset to.
     */
    void reset(T* p = nullptr)
    {
        SafeDelete(ptr);
        ptr = p;
    }

    /**
     * @brief Operator to dereference the pointer.
     *
     * @return T& -> The dereferenced pointer.
     */
    T& operator*() { return *ptr; }

    /**
     * @brief Operator to access the pointer.
     *
     * @return T* -> The pointer.
     */
    T* operator->() { return ptr; }
};

#define TRACE_DEPTH 4

/// @brief Enum to declare the severity of the error. \enum ErrorSeverity
enum ErrorSeverity
{
    ERROR_NONE = 0,
    ERROR_SOFT = 1,
    ERROR_FATAL = 2
};

/// @brief Struct to defined the error settings. \struct ErrorSeverity
struct Error
{
    int code;
    ErrorSeverity severity;
    const char* msg;
    const char* traceFiles[TRACE_DEPTH];
    int traceLines[TRACE_DEPTH];
    int traceDepth;

    Error()
    	: code(0)
    	, severity(ERROR_NONE)
    	, msg("")
    	, traceDepth(0)
    {

    }

    Error(int c, const char* m, ErrorSeverity s, const char* file, int line)
        : code(c)
    	, severity(s)
    	, msg(m)
    	, traceDepth(0)
    {
        addTrace(file, line);
    }

    void addTrace(const char* file, int line)
    {
        if (traceDepth < TRACE_DEPTH)
        {
            traceFiles[traceDepth] = file;
            traceLines[traceDepth] = line;
            traceDepth++;
        }
    }

    bool hasError() const
    {
        return code != 0;
    }

    bool isFatal() const
    {
        return severity == ERROR_FATAL;
    }

    void log() const
    {
        Serial.print(F("[ERROR] "));
        Serial.print(code);
        Serial.print(F(": "));
        Serial.print(msg);
        Serial.print(F(" | Severity: "));
        Serial.println(severity == ERROR_FATAL ? F("FATAL") : F("SOFT"));

        for (int i = 0; i < traceDepth; ++i)
        {
            Serial.print(F(" -> at "));
            Serial.print(traceFiles[i]);
            Serial.print(F(":"));
            Serial.println(traceLines[i]);
        }
    }
};

/**
 * @brief Various macros for errorhandling.
 */
#define UNIQUE_NAME(base) __UNIQUE_NAME(base, __LINE__)
#define __UNIQUE_NAME(base, line) base##line

static Error* __SAFE_ERR_PTR__ = nullptr;

#define TRY																										\
    for (Error UNIQUE_NAME(_safetry_err_) = {}, *prev = __SAFE_ERR_PTR__, *err = &UNIQUE_NAME(_safetry_err_);	\
         __SAFE_ERR_PTR__ = err, err != nullptr;																\
         __SAFE_ERR_PTR__ = prev, err = nullptr)

#define CATCH \
    else if (__SAFE_ERR_PTR__ && __SAFE_ERR_PTR__->hasError())

#define FINALLY \
    else

#define THROW(code, msg)															\
	do {																			\
		if (__SAFE_ERR_PTR__)														\
		{																			\
			*__SAFE_ERR_PTR__ = Error(code, msg, ERROR_FATAL, __FILE__, __LINE__);	\
			break;																	\
		}																			\
	} while (0)

#define THROW_SOFT(code, msg)														\
	do {																			\
		if (__SAFE_ERR_PTR__)														\
		{																			\
			*__SAFE_ERR_PTR__ = Error(code, msg, ERROR_SOFT, __FILE__, __LINE__);	\
			break;																	\
		}																			\
	} while (0)

#define RETHROW(code, msg)									\
	do {													\
		if (__SAFE_ERR_PTR__)								\
		{													\
			__SAFE_ERR_PTR__->addTrace(__FILE__, __LINE__); \
			__SAFE_ERR_PTR__->code = code;					\
			__SAFE_ERR_PTR__->msg = msg;					\
			break;											\
		}													\
	} while (0)

#define THROW_IF_ERR(err)									\
	do {													\
		if ((err).hasError() && __SAFE_ERR_PTR__)			\
		{													\
			*__SAFE_ERR_PTR__ = err;						\
			__SAFE_ERR_PTR__->addTrace(__FILE__, __LINE__); \
			break;											\
		}													\
	} while (0)

#define THROW_IF_NULL(ptr, code, msg)				\
	do {											\
		if ((ptr) == nullptr) { THROW(code, msg); } \
	} while (0)

#define THROW_IF_FALSE(cond, code, msg)		\
	do {									\
		if (!(cond)) { THROW(code, msg); }	\
	} while (0)

#define RETURN_IF_ERR(expr)				\
	do {								\
		Error _e = (expr);				\
		if (_e.hasError()) return _e;	\
	} while (0)


#endif // PTRUTILS_H
