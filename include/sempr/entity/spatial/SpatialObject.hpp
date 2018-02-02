#ifndef SEMPR_ENTITY_SPATIALOBJECT_HPP_
#define SEMPR_ENTITY_SPATIALOBJECT_HPP_

#include <sempr/entity/Entity.hpp>
#include <sempr/entity/RDFPropertyMap.hpp>
#include <sempr/entity/spatial/GeometryCollection.hpp>
#include <map>

namespace sempr { namespace entity {

/**
    This class represents a generic spatial object with no exactly known semantic type. Instead,
    it contains a list of possible types and a confidence value for each of them. The type with
    the highest confidence is assumed to be true and presented via RDF statements.
    It can store a variety of key/values by using an RDFPropertyMap and has a pointer to a generic
    geometry-entity that can be used to store a geometry that represents it.

    TODO: Having a generic geometry is a bit tricky. Who should own it? What happens on removal? What happens when a new one is assigned? For know, I'll just give it a GeometryCollection that cannot be replaced. But of course you can modify the OGRGeometryCollection directly and feed it different stuff.
*/
#pragma db object
class SpatialObject : public Entity {
private:
    GeometryCollection::Ptr geometry_;
    RDFPropertyMap::Ptr property_;
    std::map<std::string, double> types_;

    friend class odb::access;

public:
    SpatialObject();
    /**
        Access the GeometryCollection of this SpatialObject.
        If you make changes to it you should call
            this->geometry()->changed()
        to notify the processing modules.
    */
    GeometryCollection::Ptr geometry();

    /**
        Access the RDFPropertyMap.
        If you make changes to it you should call
            this->properties()->changed()
        to notify the processing modules.
    */
    RDFPropertyMap::Ptr properties();

    /** return the type with highest confidence, optionally return the confidence in the given pointer */
    std::string type(double* confidence = nullptr) const;

    /** set the confidence for a given type */
    void type(const std::string&, double confidence);

    // TODO implement

    ENTITY_DEFAULT_EVENT_METHODS(SpatialObject, Entity);
};

}}


#endif /* end of include guard SEMPR_ENTITY_SPATIALOBJECT_HPP_ */
