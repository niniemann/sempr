#ifndef SEMPR_ENTITY_PERSON_H_
#define SEMPR_ENTITY_PERSON_H_

#include <sempr/entity/Entity.hpp>
#include <sempr/storage/History.hpp>
#include <sempr/entity/RDFEntity.hpp>

namespace sempr { namespace entity {

#pragma db object
class Person : public Entity {
public:
    using Ptr = std::shared_ptr<Person>;
    enum Gender { MALE, FEMALE, UNKNOWN };

    Person();
    virtual ~Person(){}
    void init();

    ENTITY_DEFAULT_EVENT_METHODS(Person, Entity);

    std::string getFirst(){ return first_; }
    void setFirst(std::string s){ first_ = s; }
    std::string getLast(){ return last_; }
    void setLast(std::string s){ last_ = s; }
    Gender getGender(){ return gender_; }
    void setGender(Gender g){ gender_ = g; }

protected:
    friend class odb::access;
    RDFEntity::Ptr rdfEntity_;

    unsigned int age_; // TODO replace with a timestamp "born"
    std::string first_, last_; /// name
    Gender gender_;
};

// enable history:
typedef storage::History<Person::Ptr> PersonHistory;
#pragma db value(PersonHistory)

}}

#endif /* end of include guard: SEMPR_ENTITY_PERSON_H_ */
