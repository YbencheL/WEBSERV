#include  <iostream>
#include  <fstream>
#include  <deque>

enum NodeType { 
    ROOT, 
    SERVER, 
    LOCATION 
};

class nodes
{
    private:
        nodes *parent;
        NodeType type;
        std::string name;
        std::deque<std::string> values;
        std::deque<nodes> children;
    public:
        nodes() : parent(nullptr) {}
        void setType(NodeType type)
        {
            this->type = type;
        }
        void setName(std::string name)
        {
            this->name = name;
        }
        void addChild(nodes& child)
        {
            children.push_back(child);
            children.back().parent = this; 
        }
        void addVal(std::string val)
        {
            values.push_back(val);
        }
        nodes* getPrevChild()
        {
            return parent;
        }
        nodes* getLastChild()
        {
            if (children.empty())
                return nullptr;
            return &children.back();
        }
        void printval()
        {
            for(size_t i = 0; i < values.size(); i++)
            {
                std::cout << name << std::endl;
                std::cout << values[i] << std::endl;
            }
        }
};

void    treeing(std::string line, nodes **current)
{
    int pos = 0;

    if ((pos = line.find("server")) != std::string::npos)
    {
        nodes server;
        server.setType(SERVER);
        server.setName("server");
        (*current)->addChild(server);
        *current = (*current)->getLastChild();
    }else if ((pos = line.find("location")) != std::string::npos)
    {
        nodes location;
        location.setType(LOCATION);
        location.setName("location");
        (*current)->addChild(location);
        *current = (*current)->getLastChild();
    }else if ((pos = line.find("}")) != std::string::npos)
    {
        if ((*current)->getPrevChild() != nullptr)
            *current = (*current)->getPrevChild();
    }else
    {
        (*current)->addVal(line);
    }
}

int main()
{
    std::ifstream infile("sub");
    if (infile.fail())
    {
        std::cout << "Error" << std::endl;
    }
    std::string line;
    nodes root;
    root.setType(ROOT);
    root.setName("root");
    nodes* current = &root;
    while(getline(infile, line))
    {
        treeing(line, &current);
        current->printval();
        std::cout << "-----------" << std::endl;
    }
}