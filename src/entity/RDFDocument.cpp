#include <sempr/entity/RDFDocument.hpp>
#include <Soprano/Soprano>
#include <Soprano/Parser>

#include <RDFDocument_odb.h>

namespace sempr { namespace entity {

SEMPR_ENTITY_SOURCE(RDFDocument)

RDFDocument::RDFDocument()
    : RDFVector(new core::IDGen<RDFDocument>())
{
}

RDFDocument::~RDFDocument()
{
}

std::string RDFDocument::filename() const {
    return filename_;
}

RDFDocument::Ptr RDFDocument::FromFile(const std::string &file)
{
    RDFDocument::Ptr doc(new RDFDocument());
    doc->filename_ = file;

    //
    // const Soprano::Parser* p = Soprano::PluginManager::instance()->discoverParserForSerialization( Soprano::SerializationRdfXml );
    // Soprano::Ser

    Soprano::RdfSerialization options[] = {
        Soprano::SerializationRdfXml,
        Soprano::SerializationTurtle,
        Soprano::SerializationN3,
        Soprano::SerializationNQuads,
        Soprano::SerializationNTriples
    };

    // try different formats for parsing
    for (auto option : options) {
        const Soprano::Parser* p = Soprano::PluginManager::instance()->discoverParserForSerialization( option );

        if (!p) {
            std::cout << "no parser for " << option << '\n';
            continue;
        }

        auto statements = p->parseFile(QString::fromStdString(file), QUrl(), option);
        if (p->lastError() == Soprano::Error::ErrorNone)
        {
            while(statements.next())
            {
                // qDebug() << *statements;
                doc->addTriple(
                    RDFValueTriple( (*statements).subject().toN3().toStdString(),
                            (*statements).predicate().toN3().toStdString(),
                            (*statements).object().toN3().toStdString() )
                );
            }

            break;
        } else {
            qDebug() << p->lastError().message();
        }
    }

    return doc;
}


}}
