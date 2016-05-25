#ifndef I_VIEW_CREATOR_HPP
#define I_VIEW_CREATOR_HPP

#include <memory>
#include <functional>

class IView;
class ObjectHandle;

namespace wgt
{

class IViewCreator
{
public:
	virtual void createView(
		const char * path,
		const ObjectHandle & context,
		std::unique_ptr< IView > & o_ResultView,
		const char * uniqueName = nullptr ) = 0;

	virtual void createView(
		const char * path,
		const ObjectHandle & context,
		std::function< void(std::unique_ptr< IView > &) >,
		const char * uniqueName = nullptr ) = 0;
};

}

#endif //I_VIEW_CREATOR_HPP