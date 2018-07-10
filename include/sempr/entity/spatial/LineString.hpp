#ifndef SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_
#define SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <geos/geom/LineString.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Container class for GEOM LineStrin
*/
#pragma db object
class LineString : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<LineString>;

    LineString();
    LineString(const core::IDGenBase*);
    virtual ~LineString();

    geom::LineString* geometry() override;

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    LineString::Ptr clone() const;

private:
    friend class odb::access;

    #pragma db type("TEXT")
    geom::LineString* geometry_;

    virtual LineString* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_LINESTRING_HPP_ */
