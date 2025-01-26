#ifndef PTRUTILS_H
#define PTRUTILS_H

#include <Arduino.h>

// Delete a pointer and set it to nullptr
template <typename T>
static inline void SafeDelete(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

// Delete an array pointer and set it to nullptr
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
 * Verify that a value matches an expected value. If not, trigger a failure.
 */
// General template for non-pointer types
template <typename T>
static inline void Verify(const T& value, const T& expected, const char* errorMsg = nullptr)
{
    if (value != expected)
    {
        if (errorMsg)
        {
            Serial.println(errorMsg);
        }
        else
        {
            Serial.print("Verification failed: Value (");
            Serial.print(value);
            Serial.print(") does not match expected (");
            Serial.print(expected);
            Serial.println(").");
        }
        while (true); // Halt execution
    }
}

// Specialization for pointers (Pointer vs Pointer)
template <typename T>
static inline void Verify(T* value, T* expected, const char* errorMsg = nullptr)
{
    if (value != expected)
    {
        if (errorMsg)
        {
            Serial.println(errorMsg);
        }
        else
        {
            Serial.print("Verification failed: Pointer (");
            Serial.print((unsigned long)value, HEX); // Print pointer address in HEX
            Serial.print(") does not match expected pointer (");
            Serial.print((unsigned long)expected, HEX);
            Serial.println(").");
        }
        while (true); // Halt execution
    }
}

// Specialization for nullptr (Pointer vs nullptr)
template <typename T>
static inline void Verify(T* value, const char* errorMsg = nullptr)
{
    if (value != nullptr)  // Directly compare with nullptr (no std::nullptr_t)
    {
        if (errorMsg)
        {
            Serial.println(errorMsg);
        }
        else
        {
            Serial.print("Verification failed: Pointer (");
            Serial.print((unsigned long)value, HEX); // Print pointer address in HEX
            Serial.println(") is not null.");
        }
        while (true); // Halt execution
    }
}


/**
 * Macro to delete a pointer and verify it's nullptr.
 */
#define tryDeletePtr(ptr) 										\
    SafeDelete(ptr);        									\
    //Verify(ptr, nullptr, "Pointer not null after deletion!"); 	\


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
 * Clear an array by setting all elements to their default value.
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
 * Print pointer information for debugging.
 */
template <typename T>
static inline void PrintPtrInfo(T* ptr, const char* ptrName = "Pointer")
{
    if (ptr == nullptr)
    {
        Serial.print(ptrName);
        Serial.println(" is nullptr.");
    }
    else
    {
        Serial.print(ptrName);
        Serial.print(" points to address: 0x");
        Serial.println((uintptr_t)ptr, HEX);
    }
}

/**
 * Scoped pointer class to manage single pointers in RAII fashion.
 */
template <typename T>
class ScopedPointer
{
private:
    T* ptr;

public:
    explicit ScopedPointer(T* p = nullptr) : ptr(p) {}
    ~ScopedPointer() { SafeDelete(ptr); }

    T* get() const { return ptr; }
    T* release()
    {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* p = nullptr)
    {
        SafeDelete(ptr);
        ptr = p;
    }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
};

/**
 * Pointer wrapper class for RAII-style management.
 */
template <typename T>
class PointerWrapper
{
private:
    T* ptr;

public:
    explicit PointerWrapper(T* p = nullptr) : ptr(p) {}
    ~PointerWrapper() { SafeDelete(ptr); }

    T* get() const { return ptr; }
    T* release()
    {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* p = nullptr)
    {
        SafeDelete(ptr);
        ptr = p;
    }

    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }
};

#endif // PTRUTILS_H
