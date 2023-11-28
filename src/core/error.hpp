#pragma once

#include <iostream>

class coreError : public std::exception
{
public:
    coreError(const char* msg) : msg(msg) {}
    const char* what() const noexcept { return msg; }
private:
    const char* msg;
};