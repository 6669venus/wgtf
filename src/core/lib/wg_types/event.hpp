#ifndef EVENT_HPP
#define EVENT_HPP

#include <cassert>
#include <vector>

/**
 * Basic Event type to support an event that multicasts to handlers
 * that take a Sender* and an EventArgs type as parameters. This class
 * was written as simply as possible to have low maintenance of 
 * templates and macros.
 * In future we would like to see this class reimplemented using C++11 and
 * variadic templates.
 *
 * Handler functions must have the following signature:
 *  void (XXXX::) ( Sender* pSender, EventArgs args )
 *
 * If you would like to avoid a copy of your arguments, be sure to define
 * const EventArgs& as your EventArgs type.
 */
template <typename Sender, typename EventArgs>
class Event
{
private:
	/**
	 * Traits class to help with definition of a member function pointer
	 * that occurs frequently in the rest of the class.
	 */
	template <typename TargetType>
	struct TargetTraits
	{
		typedef void(TargetType::*MemberFunctionPtr)( Sender *, EventArgs );
	};

	/**
	 * A basic and naive implementation of a C++ delegate based upon the 
	 * initial implementation from here: 
	 * http://www.codeproject.com/Articles/13287/Fast-C-Delegate
	 * Features of this particular delegate include:
	 *  - equality operator (not available in std::function)
	 *  - no heap allocations required (also not available in std::function)
	 * Unfortunately the nature of this delegate is such that it can only
	 * accept an explicit set of arguments unless we implement it using 
	 * variadic templates. Currently and in the past, this has been enough
	 * though.
	 */
	class EventDelegate
	{
	public:
		EventDelegate()
			: pTarget_( NULL )
			, pStubMethod_( NULL )
		{}


		/**
		 * Static templated helper function to generate the appropriate
		 * delegate given the target type and function pointer.
		 */
		template < class TargetType, 
			typename TargetTraits<TargetType>::MemberFunctionPtr MethodPtr>
		static EventDelegate fromMethod( TargetType * pTarget )
		{
			EventDelegate d;
			d.pTarget_ = pTarget;
			d.pStubMethod_ = &stub_method< TargetType, MethodPtr >::stub;

			return d;
		}


		/**
		 * Invocation operator to trigger the delegate.
		 */
		void operator()( Sender * pSender, EventArgs args ) const
		{
			return (*pStubMethod_)( pTarget_, pSender, args );
		}


		/**
		 * Equality operator to facilitate removal of registered 
		 * handlers.
		 */
		bool operator==( const EventDelegate & other )
		{
			return other.pStubMethod_ == pStubMethod_ &&
				other.pTarget_ == pTarget_;
		}

	private:
		typedef void (*StubMethodType)( void *, Sender *, EventArgs );

		void * pTarget_;
		StubMethodType pStubMethod_;

		template < class TargetType, 
			typename TargetTraits<TargetType>::MemberFunctionPtr MethodPtr >
		struct stub_method
		{
			static void stub( void * pVoidTarget, 
				Sender * pSender, EventArgs args )
			{
				TargetType * pTarget = 
					static_cast< TargetType * >(pVoidTarget);
				return (pTarget->*MethodPtr)( pSender, args );

			}
		};
	};

public:
	
	/**
	 * The list of delegates. Helper functions allow member function delegates
	 * to be specified without any heap allocations for each one.
	 */
	class EventDelegateList
	{
	public:
		

		template <typename TargetType, 
			typename TargetTraits<TargetType>::MemberFunctionPtr pMemberFunc>
			void add( TargetType * pTarget )
		{
			// If this assertion is hit, then the event list
			// is currently being iterated and is not safe 
			assert( !isIterating_ );

			EventDelegate del = 
				EventDelegate::template fromMethod<TargetType, pMemberFunc>( pTarget );
			delegates_.push_back( del );
		}

		template <typename TargetType,
			typename TargetTraits<TargetType>::MemberFunctionPtr pMemberFunc>
			void remove( TargetType * pTarget )
		{
			// If this assertion is hit, then the event list
			// is currently being iterated and is not safe 
			assert( !isIterating_ );

			EventDelegate del = 
				EventDelegate::template fromMethod<TargetType, pMemberFunc>( pTarget );
			delegates_.erase( 
				std::remove(
					delegates_.begin(), 
					delegates_.end(), 
					del ), 
				delegates_.end() );
		}

	private:
		friend Event;

		EventDelegateList() :
			isIterating_( false )
		{

		}

		void beginIteration() const
		{
			isIterating_ = true;
		}

		void endIteration() const
		{
			isIterating_ = false;
		}

		typedef std::vector<EventDelegate> DelegateList;
		DelegateList delegates_;
		mutable bool isIterating_;
	};

	/**
	 * Invoke all event handlers passing the sender and the argument
	 * to all handlers.
	 * NOTE: At this point it is not supported to modify the contents of 
	 * the delegate list during invocation. An assertion will be raised
	 * if this is attempted.
	 */
	void invoke( Sender * pSender, EventArgs args ) const
	{
		list_.beginIteration();

		for (auto iter = list_.delegates_.begin(); 
			iter != list_.delegates_.end(); ++iter)
		{
			const EventDelegate & target = *iter;
			target( pSender, args );
		}

		list_.endIteration();
	}

	/**
	 * Expose the list of delegates to the owner of the event. The owner
	 * may then choose to expose this list to its public interface. This
	 * allows public add/remove of delegates, but only the owner may actually
	 * call 'invoke'.
	 */
	EventDelegateList & delegates()
	{
		return list_;
	}

private:
	EventDelegateList list_;
};

/**
* The following macros exist to make declaring events as simple as possible.
* The macros will automatically define an EventArgs structure, a public accessor
* to the EventDelegateList and a notifyEvent function.
*
* Usage on of the following:
* PUBLIC_EVENT( ClassName, EventName, argType1, argName1, argType2, argName2... )
* PROTECTED_EVENT( ClassName, EventName, argType1, argName1, argType2, argName2... )
* PRIVATE_EVENT( ClassName, EventName, argType1, argName1, argType2, argName2... )
*
* The PUBLIC, PROTECTED, PRIVATE refers to the accessibility of the notifyFunction.
* The current macros support up to five arguments per event.
*/

#define EXPAND_( X ) X

	// support up to 5 event arguments
#define EVENT_MACRO_CHOOSER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
	EVENT_MACRO, ...) EVENT_MACRO

#define EVENT_SIGNATURE1(type, name) type name
#define EVENT_SIGNATURE2(type, name, ...) type name, EXPAND_(EVENT_SIGNATURE1(__VA_ARGS__))
#define EVENT_SIGNATURE3(type, name, ...) type name, EXPAND_(EVENT_SIGNATURE2(__VA_ARGS__))
#define EVENT_SIGNATURE4(type, name, ...) type name, EXPAND_(EVENT_SIGNATURE3(__VA_ARGS__))
#define EVENT_SIGNATURE5(type, name, ...) type name, EXPAND_(EVENT_SIGNATURE4(__VA_ARGS__))
#define EVENT_SIGNATURE(...) EXPAND_(EXPAND_(EVENT_MACRO_CHOOSER(__VA_ARGS__,\
	EVENT_SIGNATURE5, ERROR, \
	EVENT_SIGNATURE4, ERROR, \
	EVENT_SIGNATURE3, ERROR, \
	EVENT_SIGNATURE2, ERROR, \
	EVENT_SIGNATURE1, ERROR))(__VA_ARGS__))

#define EVENT_ARGS1(type, name) name
#define EVENT_ARGS2(type, name, ...) name, EXPAND_(EVENT_ARGS1(__VA_ARGS__))
#define EVENT_ARGS3(type, name, ...) name, EXPAND_(EVENT_ARGS2(__VA_ARGS__))
#define EVENT_ARGS4(type, name, ...) name, EXPAND_(EVENT_ARGS3(__VA_ARGS__))
#define EVENT_ARGS5(type, name, ...) name, EXPAND_(EVENT_ARGS4(__VA_ARGS__))
#define EVENT_ARGS(...) EXPAND_(EXPAND_(EVENT_MACRO_CHOOSER(__VA_ARGS__,\
	EVENT_ARGS5, ERROR, \
	EVENT_ARGS4, ERROR, \
	EVENT_ARGS3, ERROR, \
	EVENT_ARGS2, ERROR, \
	EVENT_ARGS1, ERROR))(__VA_ARGS__))

#define INIT_EVENT_ARGS1(type, name) name##_(name)
#define INIT_EVENT_ARGS2(type, name, ...) name##_(name), EXPAND_(INIT_EVENT_ARGS1(__VA_ARGS__))
#define INIT_EVENT_ARGS3(type, name, ...) name##_(name), EXPAND_(INIT_EVENT_ARGS2(__VA_ARGS__))
#define INIT_EVENT_ARGS4(type, name, ...) name##_(name), EXPAND_(INIT_EVENT_ARGS3(__VA_ARGS__))
#define INIT_EVENT_ARGS5(type, name, ...) name##_(name), EXPAND_(INIT_EVENT_ARGS4(__VA_ARGS__))
#define INIT_EVENT_ARGS(...) EXPAND_(EXPAND_(EVENT_MACRO_CHOOSER(__VA_ARGS__,\
	: INIT_EVENT_ARGS5, ERROR, \
	: INIT_EVENT_ARGS4, ERROR, \
	: INIT_EVENT_ARGS3, ERROR, \
	: INIT_EVENT_ARGS2, ERROR, \
	: INIT_EVENT_ARGS1, ERROR))(__VA_ARGS__))

#define DECLARE_EVENT_ARGS1(type, name)  type name##_;
#define DECLARE_EVENT_ARGS2(type, name, ...) type name##_; EXPAND_(DECLARE_EVENT_ARGS1(__VA_ARGS__))
#define DECLARE_EVENT_ARGS3(type, name, ...) type name##_; EXPAND_(DECLARE_EVENT_ARGS2(__VA_ARGS__))
#define DECLARE_EVENT_ARGS4(type, name, ...) type name##_; EXPAND_(DECLARE_EVENT_ARGS3(__VA_ARGS__))
#define DECLARE_EVENT_ARGS5(type, name, ...) type name##_; EXPAND_(DECLARE_EVENT_ARGS4(__VA_ARGS__))
#define DECLARE_EVENT_ARGS(...) EXPAND_(EXPAND_(EVENT_MACRO_CHOOSER(__VA_ARGS__,\
	DECLARE_EVENT_ARGS5, ERROR, \
	DECLARE_EVENT_ARGS4, ERROR, \
	DECLARE_EVENT_ARGS3, ERROR, \
	DECLARE_EVENT_ARGS2, ERROR, \
	DECLARE_EVENT_ARGS1, ERROR))(__VA_ARGS__))

#define DECLARE_EVENT_WITH_ARGS(modifier, sender, event, ...)					\
public:																			\
	struct event##Args {														\
		event##Args( EVENT_SIGNATURE(__VA_ARGS__) )								\
			INIT_EVENT_ARGS(__VA_ARGS__) {}										\
		DECLARE_EVENT_ARGS(__VA_ARGS__)											\
	};																			\
	typedef Event< const sender, const struct event##Args & > event##Event;		\
	class event##Event::EventDelegateList & on##event() {						\
		return on##event##_.delegates();										\
	}																			\
modifier##:																		\
	void notify##event( EVENT_SIGNATURE(__VA_ARGS__) ) const {					\
		event##Args eventArgs( EVENT_ARGS(__VA_ARGS__) );						\
		on##event##_.invoke( this, eventArgs );									\
	}																			\
private:																		\
	event##Event on##event##_;

#define DECLARE_EVENT_WITHOUT_ARGS(modifier, sender, event, ...)				\
public:																			\
	struct event##Args {};														\
	typedef Event< const sender, const struct event##Args & > event##Event;		\
	class event##Event::EventDelegateList & on##event() {						\
		return on##event##_.delegates();										\
	}																			\
modifier##:																		\
	void notify##event() const {												\
		event##Args eventArgs;													\
		on##event##_.invoke( this, eventArgs );									\
	}																			\
private:																		\
	event##Event on##event##_;

#define DECLARE_EVENT(modifier, sender, event, ...) EXPAND_(EXPAND_(EVENT_MACRO_CHOOSER(__VA_ARGS__,\
	DECLARE_EVENT_WITH_ARGS, ERROR,	\
	DECLARE_EVENT_WITH_ARGS, ERROR,	\
	DECLARE_EVENT_WITH_ARGS, ERROR,	\
	DECLARE_EVENT_WITH_ARGS, ERROR, \
	DECLARE_EVENT_WITH_ARGS, DECLARE_EVENT_WITHOUT_ARGS))(modifier, sender, event, __VA_ARGS__))

#define PUBLIC_EVENT(sender, event, ...) DECLARE_EVENT(public, sender, event, __VA_ARGS__)
#define PROTECTED_EVENT(sender, event, ...) DECLARE_EVENT(protected, sender, event, __VA_ARGS__)
#define PRIVATE_EVENT(sender, event, ...) DECLARE_EVENT(private, sender, event, __VA_ARGS__)

#endif // EVENT_HPP
