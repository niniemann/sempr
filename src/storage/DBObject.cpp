#include <sempr/storage/DBObject.h>

namespace sempr { namespace storage {

boost::uuids::random_generator DBObject::uuid_gen = boost::uuids::random_generator();

void DBObject::dbcallback(odb::callback_event e, odb::database &db)
{
    switch (e) {
        case odb::callback_event::pre_load:
            preLoad(db);
            break;
        case odb::callback_event::post_load:
            postLoad(db);
            break;
    }
}


void DBObject::dbcallback(odb::callback_event e, odb::database &db) const
{
    switch (e) {
        case odb::callback_event::pre_persist:
            // assumption: this is NOT created const.
            const_cast<DBObject*>(this)->id_ = DBObject::uuid_gen();
            prePersist(db);
            break;
        case odb::callback_event::post_persist:
            postPersist(db);
            break;
        case odb::callback_event::pre_update:
            preUpdate(db);
            break;
        case odb::callback_event::post_update:
            postUpdate(db);
            break;
        case odb::callback_event::pre_erase:
            preErase(db);
            break;
        case odb::callback_event::post_erase:
            postErase(db);
            break;
    }
}


void DBObject::prePersist(odb::database& db) const {}
void DBObject::postPersist(odb::database& db) const {}
void DBObject::preUpdate(odb::database& db) const {}
void DBObject::postUpdate(odb::database& db) const {}
void DBObject::preErase(odb::database& db) const {}
void DBObject::postErase(odb::database& db) const {}
void DBObject::preLoad(odb::database& db) {}
void DBObject::postLoad(odb::database& db) {}
    
} /* storage */
    
} /* sempr */