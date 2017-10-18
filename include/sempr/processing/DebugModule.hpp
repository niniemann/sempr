#ifndef SEMPR_PROCESSING_DEBUGMODULE_H_
#define SEMPR_PROCESSING_DEBUGMODULE_H_

#include <sempr/processing/Module.hpp>
#include <sempr/entity/Entity.hpp>

namespace sempr { namespace processing {
    
class DebugModule : public Module {
public:
    using Ptr = std::shared_ptr<DebugModule>;
    DebugModule();
    virtual ~DebugModule();
    
    virtual std::string type() const override;
    
    virtual void process(core::Event::Ptr e) override;
    void process(entity::Entity::Event::Ptr e);
};
    
} /* processing */
    
} /* sempr */

#endif /* end of include guard: SEMPR_PROCESSING_MODULE_H_ */