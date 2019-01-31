#ifndef JJ_ID_GENERATOR_H
#define JJ_ID_GENERATOR_H

/*! A simple wrapper over an integer id. */
class idHolder_t
{
    int id_; //!< the id
public:
    /*! Ctor - defaults id to -1 = not set */
    idHolder_t() : id_(-1) {}
    /*! Ctor - initializes to given id */
    idHolder_t(int id) : id_(id) {}
    /*! Changes the id to given one */
    void set_id(int id) { id_ = id; }
    /*! Returns the stored id. */
    int get_id() const { return id_; }
};

/*! A simple generator of unique integer ids.
Only an instance guarantees "uniqueness". */
class idGenerator_t
{
    int idgen_; //!< counter of "uniqueness"
protected:
    /*! Ctor */
    idGenerator_t() : idgen_(0) {}
    /*! Copy ctor - copies do not share state and may generate same ids */
    idGenerator_t(const idGenerator_t&) : idgen_(0) {}
public:
    /*! Generates unique id. */
    int get_an_id() { return ++idgen_; }
};

#endif // JJ_ID_GENERATOR_H
