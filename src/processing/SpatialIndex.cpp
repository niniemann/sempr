#include <sempr/processing/SpatialIndex.hpp>
#include <sempr/core/EntityEvent.hpp>
#include <sempr/entity/spatial/filter/LocalTransformationFilter.hpp>
#include <iterator>

#include <geos/geom/Coordinate.h>

//#include <ogr_core.h>

namespace sempr { namespace processing {

SpatialIndex::SpatialIndex()
{
    // nothing to do
}


std::string SpatialIndex::type() const
{
    return "SpatialIndex";
}

void SpatialIndex::process(query::SpatialIndexQuery::Ptr query)
{
    // TODO: transfer reference geometry into the frame of the spatial index
    // (make a copy?)
    // query->refGeo_

    // create the AABB of the transformed query-volume.

    geos::geom::Coordinate min, max;
    query->refGeo()->findEnvelope(min, max);

    bBox region(
        bPoint(min.x, min.y, min.z),
        bPoint(max.x, max.y, max.z)
    );
    

    std::vector<bValue> tmpResults;

    typedef query::SpatialIndexQuery::QueryType QueryType;
    switch (query->mode()) {

        case QueryType::WITHIN:
            rtree_.query(bgi::within(region), std::back_inserter(tmpResults));
            break;
        case QueryType::NOTWITHIN:
            rtree_.query(!bgi::within(region), std::back_inserter(tmpResults));
            break;

        // TODO: contains is introduced in boost 1.55, but ros indigo needs 1.54.
        // maybe its time for me to upgrade to 16.04 and kinetic...
        case QueryType::CONTAINS:
            rtree_.query(bgi::contains(region), std::back_inserter(tmpResults));
            break;
        case QueryType::NOTCONTAINS:
            rtree_.query(!bgi::contains(region), std::back_inserter(tmpResults));
            break;

        case QueryType::INTERSECTS:
            rtree_.query(bgi::intersects(region), std::back_inserter(tmpResults));
            break;
        case QueryType::NOTINTERSECTS:
            rtree_.query(!bgi::intersects(region), std::back_inserter(tmpResults));
            break;
        
        default:
            std::cout << "SpatialIndex: Mode " << query->mode() << " not implemented." << '\n';
    }

    std::transform( tmpResults.begin(), tmpResults.end(),
                    std::inserter(query->results, query->results.end()),
         [](bValue tmp) { return tmp.second; }
    );
}


void SpatialIndex::process(core::EntityEvent<entity::Geometry>::Ptr geoEvent)
{
    typedef core::EntityEvent<entity::Geometry>::EventType EType;
    entity::Geometry::Ptr geo = geoEvent->getEntity();

    switch (geoEvent->what()) {
        case EType::CREATED:
        case EType::LOADED:
            insertGeo(geo);
            break;
        case EType::CHANGED:
            updateGeo(geo);
            break;
        case EType::REMOVED:
            removeGeo(geo);
            break;
    }
}

void SpatialIndex::process(core::EntityEvent<entity::SpatialReference>::Ptr refEvent)
{
    typedef core::EntityEvent<entity::SpatialReference>::EventType EType;
    switch (refEvent->what()) {
        case EType::CREATED:
        case EType::LOADED:
            // nothing to do.
            break;
        case EType::CHANGED:
            // check which geometries are affected and update them.
            processChangedCS(refEvent->getEntity());
            break;
        case EType::REMOVED:
            // well...
            // what happens if we remove a SpatialReference that some geometry is pointing to?
            // (why would be do that? how can we prevent that?)
            break;
    }
}


void SpatialIndex::processChangedCS(entity::SpatialReference::Ptr cs)
{
    // we need to check every geometry currently in the index, if it is affected.
    for (auto entry : geo2box_)
    {
        if (entry.first->getCS()->isChildOf(cs))
        {
            // well, in that case update it.
            updateGeo(entry.first);
        }
    }
}


SpatialIndex::bValue SpatialIndex::createEntry(entity::Geometry::Ptr geo)
{
    
    // get the 3d envelope of the geometry.
    geos::geom::Coordinate min, max;
    geo->findEnvelope(min, max);

    // this envelope is in the coordinate system of the geometry. But what we need is an envelope
    // that is axis aligned with the root reference system. We could transform the geometry to root,
    // take the envelope and transform it back, but that is just ridiculus. Instead: Create a
    // bounding-box-geometry (8 points, one for each corner), transform it, and take its envelope.
    // ---
    // create a geometry with the matching extends: 8 point, every corner must be checked!

    geos::geom::Coordinate coord;
    std::vector<geos::geom::Coordinate> cornerCoordinates;
    coord = geos::geom::Coordinate(min.x, min.y, min.z); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(min.x, min.y, max.z); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(min.x, max.y, min.z); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(min.x, max.y, max.z); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(max.x, min.y, min.z); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(max.x, min.y, max.z); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(max.x, max.y, min.z); cornerCoordinates.push_back(coord);
    coord = geos::geom::Coordinate(max.x, max.y, max.z); cornerCoordinates.push_back(coord);

    geos::geom::MultiPoint* mp = geos::geom::GeometryFactory::getDefaultInstance()->createMultiPoint(cornerCoordinates);

    // transform the geometry
    LocalTransformationFilter tf(geo->getCS()->transformationToRoot());
    mp->apply_rw(tf);

    // get the new envelope
    EnvelopeFilter ef;
    mp->apply_ro(ef);

    // destroy the multi point after there usage:
    geos::geom::GeometryFactory::getDefaultInstance()->destroyGeometry(mp);

    // create the bBox out of bPoints.
    bBox box(
        bPoint(ef.getMin().x, ef.getMin().y, ef.getMin().z),
        bPoint(ef.getMax().x, ef.getMax().y, ef.getMax().z)
    );

    return bValue(box, geo);
}


void SpatialIndex::insertGeo(entity::Geometry::Ptr geo)
{
    // sanity: it needs a geometry, and a spatial reference.
    if (!geo->getGeometry() || !geo->getCS()) return;

    // create a new entry
    bValue entry = createEntry(geo);
    // save it in our map
    geo2box_[geo] = entry;
    // and insert it into the RTree
    rtree_.insert(entry);


    // std::cout << "inserted " << geo->id() << " into spatial index. AABB: "
    //     << "(" << entry.first.min_corner().get<0>() << ", " <<
    //               entry.first.min_corner().get<1>() << ", " <<
    //               entry.first.min_corner().get<2>() << ")  --  (" <<
    //               entry.first.max_corner().get<0>() << ", " <<
    //               entry.first.max_corner().get<1>() << ", " <<
    //               entry.first.max_corner().get<2>() << ")" << '\n';
}

void SpatialIndex::updateGeo(entity::Geometry::Ptr geo)
{
    // update? lets remove the old one first.
    removeGeo(geo);

    // sanity: it needs a geometry, and a spatial reference.
    if (!geo->getGeometry() || !geo->getCS()) return;

    // and re-insert it with updated data.
    insertGeo(geo);
}

void SpatialIndex::removeGeo(entity::Geometry::Ptr geo)
{
    // find the map-entry
    auto it = geo2box_.find(geo);
    if (it != geo2box_.end())
    {
        // remove from rtree
        rtree_.remove(it->second);
        // and from the map
        geo2box_.erase(it);
    }
}

}}
