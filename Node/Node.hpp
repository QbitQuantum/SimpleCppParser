
#ifndef NODE_HPP
#define NODE_HPP
#pragma once

#include <string>

class Node
{
protected:
    int Type = -1;
    Node() {};
public:
    virtual std::string print() = 0;
    virtual ~Node() = default;
};

#endif // NODE_HPP