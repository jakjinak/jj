#ifndef JJ_ID_GENERATOR_H
#define JJ_ID_GENERATOR_H


class idHolder_t
{
    int id_;
public:
    idHolder_t() : id_(-1) {}
    idHolder_t(int id) : id_(id) {}
    void set_id(int id) { id_ = id; }
    int get_id() const { return id_; }
};

class idGenerator_t
{
    int idgen_;
protected:
    idGenerator_t() : idgen_(0) {}
    idGenerator_t(const idGenerator_t&) : idgen_(0) {}
public:
    int get_an_id() { return ++idgen_; }
};

#endif // JJ_ID_GENERATOR_H
