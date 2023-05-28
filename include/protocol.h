//
// Created by Nikita Kasinski
//

#pragma once

class Protocol
{
public:
    typedef std::size_t PROTOCOL;
    static constexpr PROTOCOL FAILURE = 0;
    static constexpr PROTOCOL SUCCESS = 1;
    static constexpr PROTOCOL FINISH = 2;
    static constexpr size_t SIZE = sizeof(PROTOCOL);
};