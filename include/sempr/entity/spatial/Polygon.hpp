#ifndef SEMPR_ENTITY_SPATIAL_POLYGON_HPP_
#define SEMPR_ENTITY_SPATIAL_POLYGON_HPP_

#include <sempr/entity/spatial/Geometry.hpp>
#include <geos/geom/Polygon.h>

namespace sempr { namespace entity {

namespace geom = geos::geom;

/**
    Container class for GEOM Polygon
*/
#pragma db object
class Polygon : public Geometry {
    SEMPR_ENTITY
public:
    using Ptr = std::shared_ptr<Polygon>;

    Polygon();
    Polygon(const core::IDGenBase*);
    virtual ~Polygon();

    const geom::Polygon* getGeometry() const override;

    void setGeometry(geom::Polygon* geometry);

    /**
     * @brief Set the shell coordindates. Will overwrite the polygon geometry (including the holes)!
     * 
     * @note the coordinates have to define a closed linear ring. so the first and last coordinates have to be equal.
     */
    void setCoordinates(const std::vector<geom::Coordinate>& coordinates);
    void setCoordinates(const geom::CoordinateSequence& seq);

    /**
        Get a new entity with the same geometry (copy) referring to the same instance of
        SpatialReference.
    */
    Polygon::Ptr clone() const;

protected:
    geom::Polygon* getGeometryMut() override;

private:
    friend class odb::access;

    #pragma db type("TEXT")
    geom::Polygon* geometry_;

    virtual Polygon* raw_clone() const override;
};

}}

#endif /* end of include guard SEMPR_ENTITY_SPATIAL_POLYGON_HPP_ */
