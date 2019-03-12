#ifndef NODE_H
#define NODE_H

class Node
{
private:
    /* data */
public:
    double phi_bu;
    double phi;
    Node(/* args */);
    ~Node();
};

Node::Node(/* args */): phi_bu{0.0}, phi{0.0}
{
}

Node::~Node()
{
}







#endif