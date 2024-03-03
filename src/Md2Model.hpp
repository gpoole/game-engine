#include <string>

namespace GameEngine {

class Md2Model {
protected:
    Md2Model();

public:
    static Md2Model load(std::string const& path);
    void dump_info();
    ~Md2Model() = default;
};

}
